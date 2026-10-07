#!/usr/bin/env python3
"""Cluster test harness for Gled: starts headless Saturns, drives them through
phases with cluster_node.C, then compares their dumps.

Scenarios:
  basic  sun <- m1 <- m2 and sun <- m1b; relinks, renames, removal, on-demand
         mirroring, a fire-space lens (the original 4-Saturn test).
  scale  sun with --fanout moons w01.. and a chain c1 <- c2 <- ... of --chain
         Saturns. Chain members except the last own a mandatory queen. Every
         moon instantiates a lens in the sun's queen Shared, all at once, and
         overwrites the title of the lens 'hot'; chain members also
         instantiate a lens in their master's queen.

Usage: run_cluster.py [--scenario basic|scale] [--fanout N] [--chain N]
                      [--rundir DIR] [--port BASE] [--settle SEC]
                      [--hostpool H1,H2,...] [--remote-base DIR]
                      [--compare-only DIR]
With --hostpool the Saturns are spread round-robin over the hosts (the sun on
the first one) and run inside an apptainer image, through the site wrapper
<remote-base>/run-in-container.sh <command> (see test/README.md).
They are started detached through an existing ssh master connection,
/tmp/claude-$UID/<host>.sock. The run directory then lives under
<remote-base>/runs and is copied back to --rundir for the comparison.
Exit status 0 means every check passed.
"""

import argparse, json, os, re, shlex, signal, socket, subprocess, sys, time

HERE = os.path.dirname(os.path.abspath(__file__))
MACRO = os.path.join(HERE, "cluster_node.C")


class Fail(Exception):
    pass


# ------------------------------------------------------------------------------
# Topologies
# ------------------------------------------------------------------------------
# A topology is a list of start groups; each group is a list of node dicts
# {role, master, allow_moons, env}. A group starts once the previous group has
# finished its build phase.

def node(role, master=None, allow_moons=False, env=None):
    return {"role": role, "master": master, "allow_moons": allow_moons, "env": env or {}}


def basic_topology():
    return [[node("sun", None, True)],
            [node("m1", "sun", True), node("m1b", "sun")],
            [node("m2", "m1")]]


def scale_topology(fanout, chain):
    sc = {"CLUSTER_SCENARIO": "scale"}
    groups = [[node("sun", None, True, dict(sc))]]
    prev = "sun"
    for k in range(1, chain + 1):
        env = dict(sc)
        if k < chain:
            env["CLUSTER_OWNQUEEN"] = "1"
        if prev != "sun":
            env["CLUSTER_MASTERQUEEN"] = "Q-" + prev
        groups.append([node("c%d" % k, prev, k < chain, env)])
        prev = "c%d" % k
    wide = [node("w%02d" % i, "sun", False, dict(sc)) for i in range(1, fanout + 1)]
    if len(groups) > 1:
        groups[1].extend(wide)
    else:
        groups.append(wide)
    return groups


def flatten(groups):
    return [n for g in groups for n in g]


def depths(nodes):
    by_role = {n["role"]: n for n in nodes}
    d = {}
    for n in nodes:
        k, m = 0, n["master"]
        while m:
            k += 1
            m = by_role[m]["master"]
        d[n["role"]] = k
    return d


# ------------------------------------------------------------------------------
# Running the cluster
# ------------------------------------------------------------------------------

def ssh_cmd(host):
    return ["ssh", "-o", "ControlPath=/tmp/claude-%d/%s.sock" % (os.getuid(), host), host]


class Cluster:
    def __init__(self, groups, rundir, base_port, hostpool=None, remote_base=None):
        self.groups = groups
        self.nodes = {n["role"]: n for n in flatten(groups)}
        self.roles = [n["role"] for n in flatten(groups)]
        self.depth = depths(flatten(groups))
        self.rundir = rundir
        self.port = {r: base_port + i for i, r in enumerate(self.roles)}
        self.procs = {}
        self.started = {}
        self.remote = bool(hostpool)
        if self.remote:
            self.host = {r: hostpool[i % len(hostpool)] for i, r in enumerate(self.roles)}
            self.coord = hostpool[0]
            self.wrapper = os.path.join(remote_base, "run-in-container.sh")

    def ssh(self, host, script, check=True):
        r = subprocess.run(ssh_cmd(host) + [script], capture_output=True, text=True)
        if check and r.returncode != 0:
            raise Fail("ssh %s failed: %s" % (host, r.stderr.strip()))
        return r.stdout

    def path(self, name):
        return os.path.join(self.rundir, name)

    def master_address(self, role):
        if not self.remote:
            return "localhost"
        # ROOT resolves the first address, which is IPv6 for some uafs.
        return socket.getaddrinfo(self.host[role] + ".t2.ucsd.edu", None, socket.AF_INET)[0][4][0]

    def start(self, role):
        n = self.nodes[role]
        cmd = ["saturn", "--logflush", "--noprompt", "--run", "-l",
               "--name", role, "--port", str(self.port[role]),
               "--pidfile", self.path(role + ".pid"),
               "--outerr", self.path(role + ".out")]
        if n["allow_moons"]:
            cmd.append("--allowmoons")
        if n["master"]:
            cmd += ["--master", "%s:%d" % (self.master_address(n["master"]), self.port[n["master"]])]
        cmd.append(MACRO)
        env = {"CLUSTER_ROLE": role, "CLUSTER_RUNDIR": self.rundir,
               "CLUSTER_ALLOW_MOONS": "1" if n["allow_moons"] else "0"}
        env.update(n["env"])
        self.started[role] = time.time()
        if self.remote:
            # The wrapper runs apptainer with --cleanenv; APPTAINERENV_ variables still pass.
            envs = " ".join("APPTAINERENV_%s=%s" % (k, shlex.quote(v)) for k, v in env.items())
            # Braces: '&' must apply to the saturn only, so that the remote shell
            # exits at once and the ssh session closes.
            script = "cd %s && { %s nohup %s %s > /dev/null 2>&1 < /dev/null & }" % (
                shlex.quote(self.rundir), envs, self.wrapper, " ".join(shlex.quote(a) for a in cmd))
            self.ssh(self.host[role], script)
        else:
            self.procs[role] = subprocess.Popen(cmd, env=dict(os.environ, **env), cwd=self.rundir,
                                                stdin=subprocess.DEVNULL)

    def dead(self, roles):
        """Returns the roles whose process has exited."""
        if not self.remote:
            return {r for r in roles if self.procs[r].poll() is not None}
        out = set()
        by_host = {}
        for r in roles:
            by_host.setdefault(self.host[r], []).append(r)
        for h, rs in by_host.items():
            script = "; ".join(
                "p=$(cat %s 2>/dev/null); if [ -n \"$p\" ]; then kill -0 $p 2>/dev/null || echo %s; else echo nopid:%s; fi"
                % (shlex.quote(self.path(r + ".pid")), r, r) for r in rs)
            for w in self.ssh(h, script, check=False).split():
                if w.startswith("nopid:"):
                    r = w[6:]
                    if time.time() - self.started[r] > 120:
                        out.add(r)
                else:
                    out.add(w)
        return out

    def go(self, phase):
        if self.remote:
            self.ssh(self.coord, "echo go > %s" % shlex.quote(self.path("go." + phase)))
        else:
            open(self.path("go." + phase), "w").write("go\n")

    def listing(self):
        if self.remote:
            return set(self.ssh(self.coord, "ls -1 %s" % shlex.quote(self.rundir)).split())
        return set(os.listdir(self.rundir))

    def read(self, name):
        if self.remote:
            return self.ssh(self.coord, "cat %s" % shlex.quote(self.path(name)))
        return open(self.path(name)).read()

    def wait_done(self, roles, phase, timeout=600):
        t0 = last_alive_check = time.time()
        pending = set(roles)
        while pending:
            files = self.listing()
            for r in sorted(pending):
                fail = "fail.%s.%s" % (r, phase)
                if fail in files:
                    raise Fail("%s failed in phase %s: %s" % (r, phase, self.read(fail).strip()))
                if "done.%s.%s" % (r, phase) in files:
                    pending.discard(r)
            if pending and (not self.remote or time.time() - last_alive_check > 5):
                last_alive_check = time.time()
                gone = self.dead(pending)
                if gone:
                    raise Fail("%s exited during phase %s" % (sorted(gone), phase))
            if time.time() - t0 > timeout:
                raise Fail("timeout in phase %s, waiting for %s" % (phase, sorted(pending)))
            time.sleep(0.5 if self.remote else 0.1)

    def signal_roles(self, roles, sig):
        if not self.remote:
            for r in roles:
                if self.procs[r].poll() is None:
                    self.procs[r].send_signal(sig)
            return
        by_host = {}
        for r in roles:
            by_host.setdefault(self.host[r], []).append(r)
        for h, rs in by_host.items():
            pids = " ".join("$(cat %s 2>/dev/null)" % shlex.quote(self.path(r + ".pid")) for r in rs)
            self.ssh(h, "kill -%d %s 2>/dev/null; true" % (int(sig), pids), check=False)

    def save_stacks(self, roles):
        """Writes <role>.stack with eu-stack output for hung Saturns, if eu-stack exists."""
        for r in roles:
            pidfile = shlex.quote(self.path(r + ".pid"))
            stack = shlex.quote(self.path(r + ".stack"))
            script = ("command -v eu-stack > /dev/null && eu-stack -p $(cat %s) > %s 2>&1; true"
                      % (pidfile, stack))
            if self.remote:
                self.ssh(self.host[r], script, check=False)
            else:
                subprocess.run(["bash", "-c", script])

    def stop(self):
        """Stops the Saturns level by level, deepest first; returns roles that needed SIGKILL."""
        # Nodes still waiting for a phase leave their macro on go.quit, so that
        # the ROOT main loop can handle SIGTERM.
        self.go("quit")
        time.sleep(1.0)
        started = [r for r in self.roles if r in self.started]
        killed = []
        # Leaves first: a moon whose master disappears aborts on purpose
        # (Saturn::socket_closed asserts).
        for level in sorted({self.depth[r] for r in started}, reverse=True):
            rs = [r for r in started if self.depth[r] == level]
            self.signal_roles(rs, signal.SIGTERM)
            t0 = time.time()
            alive = set(rs)
            while alive and time.time() - t0 < 30:
                time.sleep(1.0)
                alive -= self.dead(alive) if self.remote else {r for r in alive if self.procs[r].poll() is not None}
            if alive:
                self.save_stacks(sorted(alive))
                self.signal_roles(sorted(alive), signal.SIGKILL)
                killed += sorted(alive)
        if not self.remote:
            for p in self.procs.values():
                try:
                    p.wait(timeout=10)
                except subprocess.TimeoutExpired:
                    pass
        return killed


def run(groups, rundir, base_port, settle, hostpool=None, remote_base=None):
    local_rundir = rundir
    if hostpool:
        rundir = os.path.join(remote_base, "runs", os.path.basename(rundir.rstrip("/")))
        q = shlex.quote(rundir)
        subprocess.run(ssh_cmd(hostpool[0]) + ["mkdir -p %s && cd %s && rm -f go.* done.* fail.* dump.* *.out *.pid"
                                               % (q, q)], check=True)
    else:
        os.makedirs(rundir, exist_ok=True)
        for f in os.listdir(rundir):
            if re.match(r"(go|done|fail|dump)\.", f) or f.endswith((".out", ".pid")):
                os.remove(os.path.join(rundir, f))

    c = Cluster(groups, rundir, base_port, hostpool, remote_base)
    errors, notes = [], []
    t_start = time.time()
    try:
        c.go("build")
        for g in groups:
            for n in g:
                c.start(n["role"])
            c.wait_done([n["role"] for n in g], "build")
        notes.append("all %d Saturns built and connected after %.0f s" % (len(c.roles), time.time() - t_start))
        for phase in ("mirror", "modify"):
            c.go(phase)
            c.wait_done(c.roles, phase)
            time.sleep(settle)
        c.go("dump")
        c.wait_done(c.roles, "dump")
        notes.append("all phases done after %.0f s" % (time.time() - t_start))
    except Fail as e:
        errors.append(str(e))
    finally:
        killed = c.stop()

    if hostpool:
        os.makedirs(local_rundir, exist_ok=True)
        subprocess.run(["rsync", "-a", "--delete", "-e", " ".join(ssh_cmd(hostpool[0])[:-1]),
                        "%s:%s/" % (hostpool[0], rundir), local_rundir + "/"], check=True)
        notes.append("run directory %s:%s copied to %s" % (hostpool[0], rundir, local_rundir))
        notes.append("hosts: " + ", ".join("%s=%d" % (h, list(c.host.values()).count(h))
                                           for h in sorted(set(c.host.values()))))

    for r in killed:
        errors.append("%s did not exit within 30 s of SIGTERM" % r)
    err_lines = {}
    for r in c.roles:
        out = os.path.join(local_rundir, r + ".out")
        if not os.path.exists(out):
            continue
        text = open(out, errors="replace").read()
        for pat in ("SIGSEGV", "Terminal signal", "*** Break ***", "SIGABRT",
                    "Assertion", "corrupted", "inconsistency"):
            if pat in text:
                errors.append("%s log contains '%s'" % (r, pat))
        if r not in killed and "Saturn::Shutdown done" not in text:
            errors.append("%s log has no clean shutdown" % r)
        errs = [l for l in text.splitlines() if l.startswith("ERR:")]
        if errs:
            err_lines[r] = errs
    for r, errs in err_lines.items():
        notes.append("%s log: %d ERR line(s), first: %s" % (r, len(errs), errs[0]))
    return errors, notes


# ------------------------------------------------------------------------------
# Comparing the dumps
# ------------------------------------------------------------------------------

def parse_dump(fname):
    """Returns (ruled: {qkey: [lens line]}, header: {qkey: str}, shells: set)."""
    ruled, header, shells = {}, {}, set()
    cur = None
    for line in open(fname):
        line = line.rstrip("\n")
        if line.startswith("queen "):
            cur = line.split()[1]
            ruled[cur] = []
            header[cur] = line
        elif line.startswith("lens "):
            ruled[cur].append(line)
        elif line.startswith("shell "):
            shells.add(line.split()[1])
            cur = None
        else:
            cur = None
    return ruled, header, shells


def split_lens(line):
    """Splits a lens line into (structure, hash, id)."""
    parts = line.split(" | ")
    ident = parts.pop() if parts[-1].startswith("id=") else ""
    hsh = parts.pop() if parts[-1].startswith("hash=") else ""
    return " | ".join(parts), hsh, ident


def expect_basic(dumps, rulers, roles, expect):
    def rulers_of(q):
        return sorted(rulers.get(q, []))

    expect(rulers_of("sun/SunQueen") == sorted(roles), "SunQueen ruled everywhere, got %s" % rulers_of("sun/SunQueen"))
    expect(rulers_of("sun/Shared") == sorted(roles), "Shared ruled everywhere, got %s" % rulers_of("sun/Shared"))
    expect(rulers_of("sun/OnDemand") == ["m1b", "sun"], "OnDemand ruled on sun and m1b, got %s" % rulers_of("sun/OnDemand"))
    expect(rulers_of("m1/M1Shared") == ["m1", "m2"], "M1Shared ruled on m1 and m2, got %s" % rulers_of("m1/M1Shared"))
    for r in ("m1", "m2"):
        expect("sun/OnDemand" in dumps[r][2], "OnDemand is a shell on %s" % r)
    expect(all("m1/M1Shared" not in dumps[r][0] and "m1/M1Shared" not in dumps[r][2]
               for r in ("sun", "m1b")), "M1Shared unknown on sun and m1b")
    for r in roles:
        fq = "%s/FireQueen" % r
        expect(rulers_of(fq) == [r], "%s ruled only on %s, got %s" % (fq, r, rulers_of(fq)))
    expect(any("| fire-local |" in l for l in dumps["m1"][0].get("m1/FireQueen", [])),
           "fire-local lens in m1's fire queen")

    sh = "\n".join(dumps["sun"][0].get("sun/Shared", []))
    expect("| n1-renamed-by-m2 |" in sh, "n1 renamed by m2 (flare up two levels)")
    expect("| born-on-m1b |" in sh, "lens instantiated by m1b in Shared")
    expect("| doomed |" not in sh, "lens 'doomed' removed by sun")
    m = re.search(r"lens (\S+) \| gled::ZNodeLink \| nl \|.*?@Lens=(\S+)", sh)
    n1 = re.search(r"lens (\S+) \| gled::ZNode \| n1-renamed-by-m2 \|", sh)
    expect(m and n1 and m.group(2) == n1.group(1), "nl links to n1 after relink by m2")
    m1s = "\n".join(dumps["m1"][0].get("m1/M1Shared", []))
    expect("| m1-top | touched-by-m2 |" in m1s, "m1-top title set by m2 (flare to m1)")
    od = "\n".join(dumps["sun"][0].get("sun/OnDemand", []))
    expect("| odn-renamed-by-m1b |" in od, "odn renamed by m1b in a queen mirrored on request")


def expect_scale(dumps, rulers, groups, expect, notes):
    nodes = flatten(groups)
    roles = [n["role"] for n in nodes]

    def rulers_of(q):
        return sorted(rulers.get(q, []))

    expect(rulers_of("sun/Shared") == sorted(roles), "Shared ruled on all %d Saturns, got %d"
           % (len(roles), len(rulers_of("sun/Shared"))))
    expect(rulers_of("sun/SunQueen") == sorted(roles), "SunQueen ruled on all Saturns")

    sh = dumps["sun"][0].get("sun/Shared", [])
    names = {l.split(" | ")[2] for l in sh}
    missing = sorted(r for r in roles if r != "sun" and "from-" + r not in names)
    expect(not missing, "a lens from every moon in Shared; missing: %s" % missing)
    hot = [l.split(" | ")[3] for l in sh if l.split(" | ")[1:3] == ["gled::ZNode", "hot"]]
    expect(len(hot) == 1 and hot[0].startswith("hot-by-"), "hot has a title set by a moon, got %s" % hot)
    if hot:
        notes.append("concurrent title writes to 'hot': %s won on every Saturn" % hot[0])

    chain = [n for n in nodes if re.match(r"c\d+$", n["role"])]
    for i, n in enumerate(chain):
        if n["env"].get("CLUSTER_OWNQUEEN") != "1":
            continue
        q = "%s/Q-%s" % (n["role"], n["role"])
        want = sorted(m["role"] for m in chain[i:])
        expect(rulers_of(q) == want, "%s ruled on %s, got %s" % (q, want, rulers_of(q)))
        child = chain[i + 1]["role"]
        qnames = {l.split(" | ")[2] for l in dumps[n["role"]][0].get(q, [])}
        expect("from-" + child in qnames, "lens from %s in %s" % (child, q))


def compare(rundir, scenario, groups):
    roles = [n["role"] for n in flatten(groups)]
    errors, notes = [], []
    dumps = {}
    for r in roles:
        f = os.path.join(rundir, "dump.%s.txt" % r)
        if not os.path.exists(f):
            errors.append("missing dump for %s" % r)
            continue
        dumps[r] = parse_dump(f)
    if errors:
        return errors, notes

    rulers = {}
    for r, (ruled, _, _) in dumps.items():
        for q in ruled:
            rulers.setdefault(q, []).append(r)

    # 1. Replicas of each queen must agree: structure, then content hash, then IDs.
    for q, rs in sorted(rulers.items()):
        ref = rs[0]
        ref_lines = [split_lens(l) for l in dumps[ref][0][q]]
        for r in rs[1:]:
            lines = [split_lens(l) for l in dumps[r][0][q]]
            for what, idx in (("structure", 0), ("content hash", 1), ("id", 2)):
                a = [x[idx] for x in ref_lines]
                b = [x[idx] for x in lines]
                if a != b:
                    diff = [(i, x, y) for i, (x, y) in enumerate(zip(a, b)) if x != y]
                    detail = diff[0] if diff else ("length", len(a), len(b))
                    errors.append("%s differs between %s and %s in %s; first: %s"
                                  % (q, ref, r, what, detail))
                    break

    # 2. Scenario expectations.
    def expect(cond, msg):
        if not cond:
            errors.append("expectation failed: " + msg)

    if scenario == "basic":
        expect_basic(dumps, rulers, roles, expect)
    else:
        expect_scale(dumps, rulers, groups, expect, notes)

    shared = sorted((q, len(rs)) for q, rs in rulers.items() if len(rs) > 1)
    notes.append("queens compared across Saturns: " + ", ".join("%s on %d" % x for x in shared))
    return errors, notes


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--scenario", choices=["basic", "scale"], default="basic")
    ap.add_argument("--fanout", type=int, default=8, help="scale: moons directly under the sun")
    ap.add_argument("--chain", type=int, default=4, help="scale: length of the chain under the sun")
    ap.add_argument("--rundir", default=os.path.join(os.getcwd(), "cluster-run"))
    ap.add_argument("--port", type=int, default=19200)
    ap.add_argument("--settle", type=float, default=2.0,
                    help="seconds to wait after each phase for MIRs to propagate")
    ap.add_argument("--hostpool", metavar="H1,H2,...",
                    help="spread the Saturns round-robin over these hosts, e.g. uaf-4,uaf-2,uaf-3,uaf-9")
    ap.add_argument("--remote-base", default=os.environ.get("GLED_T2_BASE"),
                    help="--hostpool: directory holding run-in-container.sh, the image,\n"
                         "a copy of the build tree and runs/ (default: $GLED_T2_BASE)")
    ap.add_argument("--compare-only", metavar="DIR")
    a = ap.parse_args()
    if a.hostpool and not a.remote_base:
        ap.error("--hostpool needs --remote-base or GLED_T2_BASE")

    rundir = os.path.abspath(a.compare_only or a.rundir)
    topo_file = os.path.join(rundir, "topology.json")
    if a.compare_only:
        saved = json.load(open(topo_file))
        scenario, groups = saved["scenario"], saved["groups"]
    else:
        scenario = a.scenario
        groups = basic_topology() if scenario == "basic" else scale_topology(a.fanout, a.chain)

    errors, notes = [], []
    if not a.compare_only:
        hostpool = a.hostpool.split(",") if a.hostpool else None
        errors, notes = run(groups, rundir, a.port, a.settle, hostpool, a.remote_base)
        os.makedirs(rundir, exist_ok=True)
        json.dump({"scenario": scenario, "groups": groups}, open(topo_file, "w"), indent=1)

    cerr, cnotes = compare(rundir, scenario, groups)
    errors += cerr
    for n in notes + cnotes:
        print("note:", n)
    for e in errors:
        print("FAIL:", e)
    print("RESULT:", "PASS" if not errors else "FAIL (%d)" % len(errors), "rundir=" + rundir)
    sys.exit(0 if not errors else 1)


if __name__ == "__main__":
    main()

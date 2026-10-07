#!/usr/bin/env python3
"""Type Ctrl-C into gled running on a pseudo-terminal (TRint prompt).

usage: ctrlc_pty.py <log> <wait-pattern> <gled args...>

Starts gled on a pty in this directory, waits until <wait-pattern> appears in its output, waits
2 s more, writes Ctrl-C (\\003) to the terminal, waits 10 s, writes '.q', and
gives gled 30 s to exit.  Everything gled prints goes to <log>, with time
stamps for the keystrokes.  Exit status: gled's, or 99 if it had to be killed.
"""
import os, pty, select, signal, sys, time

log_path, pattern, args = os.path.abspath(sys.argv[1]), sys.argv[2].encode(), sys.argv[3:]
os.chdir(os.path.dirname(os.path.abspath(__file__)))
pid, fd = pty.fork()
if pid == 0:
    os.execvp("gled", ["gled"] + args)

t0 = time.time()
buf = b""
log = open(log_path, "wb")

def note(msg):
    log.write(f"\n### {time.time() - t0:6.1f} {msg}\n".encode()); log.flush()

def pump(seconds, until=None):
    global buf
    end = time.time() + seconds
    while time.time() < end:
        r, _, _ = select.select([fd], [], [], 0.2)
        if r:
            try:
                data = os.read(fd, 4096)
            except OSError:
                return False          # gled closed the terminal
            if not data:
                return False
            buf += data; log.write(data); log.flush()
            if until and until in buf:
                return True
        if os.waitpid(pid, os.WNOHANG)[0]:
            return False
    return None

pump(120, pattern)
pump(2)
note("Ctrl-C"); os.write(fd, b"\x03")
pump(10)
note(".q");
try:
    os.write(fd, b".q\n")
except OSError:
    pass
pump(30)

# The terminal closes before the process is gone; give it the rest of 30 s.
end = time.time() + 30
while True:
    done, status = os.waitpid(pid, os.WNOHANG)
    if done or time.time() > end:
        break
    time.sleep(0.2)
if not done:
    note("still running, SIGKILL"); os.kill(pid, signal.SIGKILL); os.waitpid(pid, 0)
    sys.exit(99)
note(f"exit status {os.waitstatus_to_exitcode(status)}")
sys.exit(os.waitstatus_to_exitcode(status) & 0xff)

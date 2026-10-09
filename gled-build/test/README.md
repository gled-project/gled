# Gled tests

Checks to run against a built gled tree. A clean build proves very little:
dictionaries can go stale silently, and the worst bug found so far passed the
build and the class checksums while corrupting the heap. Run, in this order:

1. `make` — clean.
2. After a change to `gled_mk_dict_gen.pl` or `gled_mk_dict_dep.pl`,
   regenerate the dictionaries (remove `libsets/*/dict/*`,
   `libsets/*/make_dict.inc` and `libsets/*/lib/*.pcm`) and rebuild. Make
   regenerates a libset's dictionary and C++ module when a gled header it
   includes, a `LinkDef.h`, `glass.list`, ROOT's `modules.idx` or the
   module of a libset it requires changes, but it does not track the
   generators.
3. `regress/` — dictionary selection and class checksums.
4. `demos/run_demos.sh` — the demos, including how they shut down.
5. `cluster/run_cluster.py` — when MIR routing, Saturn or queen code changed.
6. `signals/` — when signal handling, start-up or shutdown changed.

Everything needs the gled environment (`source build_env.sh`, which sets
`GLEDSYS`). Tests that start `gled` open windows and need `DISPLAY`; an
off-screen X server keeps them off the desktop:

```sh
Xvnc :77 -geometry 1600x1200 -SecurityTypes None &   # once
export DISPLAY=:77
```

`timeout N gled ...` exiting with 124 means gled ran until timeout's SIGTERM
and then exited. That is not enough on its own: timeout also returns 124 when
gled aborts while shutting down after the SIGTERM. Check the output too
(`run_demos.sh` does), and `coredumpctl list`.

## regress/

**`dictsig.pl`** prints, from the generated rootcling sources, every
`TGenericClassInfo` (name, version expression, `Set*` calls), the
`classesHeaders` names and the `AddClassAlternate` calls, sorted. The
dictionaries are C++ modules, which leave `classesHeaders` empty. Diff it
against the baseline to show that a change kept the dictionary selection:

```sh
perl regress/dictsig.pl $GLEDSYS/../libsets/{GledCore,Numerica,Audio1,Geom1,GledGTS,Var1}/dict/*_Dict.cc \
  | diff regress/dictsig.baseline -
```

The baseline covers the six base libsets (426 lines, 2026-10-09).

**`ckdump.C`** writes the class version and checksum of the 148 glasses of
the six base libsets to `CKDUMP.txt` in the current directory:

```sh
cd <scratch dir>
gled --run --rnr GL --noprompt $GLEDSYS/test/regress/ckdump.C
diff $GLEDSYS/test/regress/cksum.baseline CKDUMP.txt
```

Checksums cover persistent members only. A transient (`//!`) member can appear
or vanish, shifting every offset behind it, without moving a checksum — so
checksums plus demos, never checksums alone. When a change legitimately alters
a class, copy `CKDUMP.txt` over `cksum.baseline` after the demos pass, and say
why in the ChangeLog.

**`deptest.C`** asserts `Var1`, which has the deepest dependency chain, and
reports per libset whether the base, `_View` and `_Rnr_GL` init symbols
resolve. All six should say `yes yes yes`:

```sh
gled --run --rnr GL --noprompt $GLEDSYS/test/regress/deptest.C
```

**`noassert.C`** checks in a Geom1 and a Var1 lens without `AssertLibSet`
and spawns an Eye. The rootmaps load the libraries; `ZQueen::CheckIn` must
set up the libsets. The six libsets should say `yes yes yes` (`yes no no`
under `saturn`, which loads no View or renderer libraries), and `gled` must
keep running until the timeout:

```sh
cd $GLEDSYS/demos/GledCore
timeout 22 gled --rnr GL --noprompt $GLEDSYS/test/regress/noassert.C
timeout 22 saturn --noprompt $GLEDSYS/test/regress/noassert.C < /dev/null
```

## demos/

```sh
demos/run_demos.sh <outdir> [<LibSet>/<macro>.C ...]
```

Runs each demo for 22 s and checks the exit status (124), the log (no crash,
heap-corruption, cling or ROOT error lines) and that the GUI message loop
exited. A macro that does not exist fails. The demos are taken from
`$DEMOS_DIR/<LibSet>`, `$GLEDSYS/demos/<LibSet>` by default.
Without a list it runs the 14 base demos of GledCore and Geom1. The exit status
is the number of failures; each log is kept in `<outdir>`.

## cluster/

Every demo runs in one process. `run_cluster.py` starts headless `saturn`
processes on localhost — a sun with moons, and moons of moons — drives them
through phases with `cluster_node.C` (build, mirror, modify, dump), and checks
that every queen ruled on several Saturns has the same structure, content and
IDs on all of them, and that every modification arrived. Exit status 0 means
PASS.

```sh
cluster/run_cluster.py --rundir <scratch dir>/basic                 # ~40 s
cluster/run_cluster.py --scenario scale --fanout 3 --chain 2 --rundir <scratch dir>/scale
cluster/run_cluster.py --compare-only <rundir>                      # re-check dumps
```

`scale` runs a sun with `--fanout` moons and a chain of `--chain` Saturns under
it; every moon writes into the sun's queen at once.

With `--hostpool H1,H2,...` the Saturns are spread over several hosts and run
through a site wrapper, `<remote-base>/run-in-container.sh <command>`
(`--remote-base` defaults to `$GLED_T2_BASE`), which must make the build tree
available at its local path and source `build_env.sh`. Saturns are started
through existing ssh master connections, `/tmp/claude-$UID/<host>.sock`.

## signals/

**`sigtest.sh`** starts `gled --run --noprompt --logflush` with a macro from
this directory, waits for a line of output, sends a signal (`SIG=INT` for
SIGINT, default SIGTERM) and times the exit:

```sh
signals/sigtest.sh out.txt 'canvas batch' canv.C --root-web server:8899   # start-up
signals/sigtest.sh out.txt 'pump start' late_loop.C                        # in the event loop
```

- `canv.C` with `--root-web server:8899` is the start-up case: after the macro,
  `TWebCanvas` waits about 30 s for a browser, pumping events. A SIGTERM or
  SIGINT sent then must be held until the event loop starts; gled exits when
  the wait ends.
- `late_loop.C` pumps events for 10 s from a timer after the event loop has
  started. Gled must exit right after "pump end".

**`ctrlc_pty.py`** runs gled with the ROOT prompt on a pseudo-terminal, types
Ctrl-C once a line of output appears, then `.q`:

```sh
signals/ctrlc_pty.py out.txt 'loop start' --logflush startup_loop.C
```

Expected: `*** Break *** keyboard interrupt`, no "loop end", the prompt, and
exit status 0 after `.q`.

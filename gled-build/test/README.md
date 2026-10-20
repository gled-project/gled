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
7. `auth/run_auth.sh` — when authentication or `GKeyRSA` changed.

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

**`rnr_deps.C`** links one Text from two ZNodeLinks, then, through delayed
MIRs, gives the Text a font, adds a child to it, removes the child and unlinks
one ZNodeLink. With `GLED_RNR_CHECK=1` the render driver reassembles every
program each frame and compares it with the cached one, printing `stale
program` for each invalidation that was missed. `GLED_RNR_TRACE=<prefix>`
writes each distinct frame, the lenses and calls in order, to
`<prefix>.<driver>.trace`:

```sh
cd $GLEDSYS/demos/GledCore
GLED_RNR_CHECK=1 GLED_RNR_TRACE=/tmp/deps timeout 22 gled --noprompt $GLEDSYS/test/regress/rnr_deps.C
```

There must be no `stale program` line, and the trace must hold a frame with
the font's pre and post calls around the Text's draw under each link.

Each pre op of a program is paired with the next post op of the same renderer.
Check mode prints `unpaired pre` or `unpaired post` when one has no partner;
such a program is never culled. A renderer reached again inside its own
subtree is not entered again, and `RnrDriver::Render cycle at` is printed once
for it. **`rnr_cycle.C`** links a sphere's child back to the sphere. It must
print the cycle line once and run until the timeout:

```sh
cd $GLEDSYS/demos/GledCore
GLED_RNR_CHECK=1 timeout 22 gled --noprompt $GLEDSYS/test/regress/rnr_cycle.C
```

Blended draws, those under a `ZGlBlending` with blending on, are deferred
and drawn after the opaque ones, in tree order or back to front by the
blending's `RnrAlgo`; `RA_InPlace` and `GLED_RNR_BLENDED=0` draw them in
place. **`blended.C`** draws two overlapping spheres of alpha 0.5, the near
one first in the tree, dumps images under each `RnrAlgo` and checks the
colour of the overlap. One run cannot turn the pass off, so the macro reads
`GLED_RNR_BLENDED` and expects the colours of the pass in place when it is 0.
Both runs must print `blended: all passed`; on an off-screen X server
(`Xvnc :77`, as at the top of this file):

```sh
cd $GLEDSYS/demos/GledCore
DISPLAY=:77 timeout 30 gled --noprompt $GLEDSYS/test/regress/blended.C
DISPLAY=:77 GLED_RNR_BLENDED=0 timeout 30 gled --noprompt $GLEDSYS/test/regress/blended.C
```

Lenses whose renderer gives a box (so far Extendio, in `tringula.C`) are culled
against the view frustum; `GLED_RNR_CULL=0` turns that off. Check mode also
reports a GL modelview that differs from the driver's position stack at a cull
test, and a culled box whose corners are not all outside one clip plane.

The terrain of `tringula.C(mode, faces_per_patch)` is split into patches of
about that many triangles (default 1000, 0 for none). Patches are culled by
their box and, while GL culls back faces, by their normal cone; check mode
confirms each cone cull triangle by triangle. Smooth and flat shading draw from
vertex buffers; `GLED_RNR_VBO=0` turns them off. Check mode confirms the flat
buffers when they are built and reports GL errors after each render.
`GLED_RNR_STATS=<n>` prints the
mean and maximum render time and the culled lenses and parts every n renders.
It calls `glFinish`, so measure with vsync off (`__GL_SYNC_TO_VBLANK=0` on
NVIDIA). The terrain draws flat by default; `regress/tringula_shading.C`, run
after it, switches to smooth shading, and with `strips=1` to triangle strips:

```sh
cd $GLEDSYS/demos/Var1
GLED_RNR_CHECK=1 timeout 22 gled --noprompt 'tringula.C(2,300)' \
  "$GLEDSYS/test/regress/tringula_shading.C(1,1)"
```

`run_demos.sh` counts the check-mode reports as failures, so
`GLED_RNR_CHECK=1 demos/run_demos.sh <outdir>` checks the render programs and
culling of every demo it runs.

**`xr_rig.C`** checks `XrRig`, the placement of a scene in a headset's space,
without a headset: recentering puts the ridden frame at the head, facing the
gaze; the head keeps its place in a ridden frame that turns and moves; one
hand carries the scene rigidly and keeps it level; two hands scale it by their
distance; flying keeps the height; realigning keeps the place and the scale
and turns the ridden frame to the gaze; a ridden frame that pitches and rolls
keeps its up as the headset's up, and in level mode keeps the world's up. It
must print `xr_rig: all passed`:

```sh
cd $GLEDSYS/demos/GledCore
timeout 30 saturn --noprompt $GLEDSYS/test/regress/xr_rig.C < /dev/null
```

**`ws_motion.C`** checks the motion along a `WSSeed` and the cart's up, without
a display: `MotionAtTime`'s velocity and acceleration are the derivatives of
its position; with `UpFromPoints` the up is the point's z axis at each point
and a unit vector normal to the tangent between them; `WSWalker`'s flight mode
banks a cart on a level circle by the angle of a coordinated turn,
tan b = v² / (r g). It must print `ws_motion: all passed`:

```sh
cd $GLEDSYS/demos/Geom1
timeout 60 saturn --noprompt $GLEDSYS/test/regress/ws_motion.C < /dev/null
```

**`patches.C`** builds terrain patches on three GTS spheres with
`TriMesh::BuildPatches()` and checks the patch table of `TringTvor`: the reorder
keeps the triangles, the patches tile them, the boxes and normal cones hold
their triangles, each tree node is its two children, and strips stay inside
their patches. It must print `patches: all passed`:

```sh
cd $GLEDSYS/demos/Var1
timeout 60 saturn --noprompt $GLEDSYS/test/regress/patches.C < /dev/null
```

## demos/

```sh
demos/run_demos.sh <outdir> [<LibSet>/<macro>.C ...]
```

Runs each demo for 22 s and checks the exit status (124), the log (no crash,
heap-corruption, cling or ROOT error lines) and that the GUI message loop
exited. A macro that does not exist fails. The demos are taken from
`$DEMOS_DIR/<LibSet>`, `$GLEDSYS/demos/<LibSet>` by default.
Without a list it runs the 14 base demos of GledCore and Geom1.
`<LibSet>/<a>.C+<b>.C` runs the macros one after the other in one `gled`,
for add-ons such as `XrdMon/xrd_suck_udp.C+xrd_udp_tree_writer.C`. The
exit status is the number of failures; each log is kept in `<outdir>`.

## cluster/

Every demo runs in one process. `run_cluster.py` starts headless `saturn`
processes on localhost — a sun with moons, and moons of moons — drives them
through phases with `cluster_node.C` (build, mirror, modify, dump), and checks
that every queen ruled on several Saturns has the same structure, content and
IDs on all of them, and that every modification arrived. Exit status 0 means
PASS. Each Saturn gets its role and run directory from `CLUSTER_ROLE` and
`CLUSTER_RUNDIR`; the script starts a phase by writing `go.<phase>` there,
and each Saturn answers with `done.<role>.<phase>` or `fail.<role>.<phase>`.

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

## auth/

```sh
auth/run_auth.sh <outdir> [<port>]
```

Makes key pairs with `gled-auth-init` in `<outdir>`, starts a sun with
`--auth` on `<port>` (default 9161) and connects a `saturn` to it, once
for each case:

- matching keys, and a 1024-bit sun key with 2048-bit client keys: the
  client connects;
- a client private key that does not match its public key, an identity the
  sun does not know, and a guest: the client is denied, with the reason in
  its log;
- the sun runs `macros/std_auth.C` and the client asks for the group
  identity `@test` (`attach_group.C`): refused when the group file lists
  `xsaturn`, granted when it lists `saturn`;
- with `DISPLAY` set, a `gled` runs `macros/moon.C` and its Eye logs in as
  `mercury`, through the sun's direct socket: with the right key it
  connects, with a wrong key it is refused and `gled` still exits on
  SIGTERM.

Takes about two minutes; the exit status is the number of failures.

# Notes on individual glasses

What some glasses do and how their parameters work, beyond what their GUI
shows.

## GledCore

### ZGlBlending::RnrAlgo

Says where the draws under a blending go:

- `RA_InPlace`, the default: where they are in the tree, as without the
  blended pass.
- `RA_Sorted`: held back, and drawn after everything else in the scene, far
  to near by each lens's origin, or by its cull-box centre when it has one.
  Use it for translucent things that overlap, such as clouds or tubes. They
  are sorted as single points, so objects that lie inside each other can
  come out in the wrong order.
- `RA_Ordered`: held back too, but drawn in tree order, before the sorted
  ones. Use it for what must come first, or must not swap as the view moves:
  a sky dome, a distant sun, flat panels in the scene.

The innermost `ZGlBlending` whose `BlendOp` is not Nop decides. Held-back
draws are made with depth writes off, unless one of their mods turns them
on: a `ZGlLightModel` with its shade-model op on does, so set
`DepthMaskOp` Off on the blending if needed. They start from the GL state at
the start of the scene, with the scene's lamps on, so give them their own
mods rather than relying on state left by other parts of the scene.

Draws are always made in place in picking, in display lists, in the
pupil's overlay, under a `ZGlPerspective` or a `ZRlNameStack`, and for
lamps. A renderer whose PreDraw moves the GL modelview without going
through the position stack, as `Extendio`'s does, would be held back to a
wrong place; `GLED_RNR_CHECK=1` reports it. `GLED_RNR_BLENDED=0` turns the
pass off. `Geom1/demos/coaster.C` uses both kinds for its sky and clouds.

## Geom1

### GForger

Makes a random fractal terrain. `Forge()` puts the heights into the linked
`RectTerrain` and, if an image is linked, a grayscale copy into the linked
`ZImage`.

The heights are fractional Brownian motion made by spectral synthesis. A
`Mesh` x `Mesh` grid of frequencies gets Gaussian random amplitudes scaled by
f^-(4 - `Dimension`), and an inverse FFT takes it back to a height field. The
power spectrum of the heights then falls as f^-(8 - 2 `Dimension`).
`Dimension` is the fractal dimension of the surface, from 2 (smooth) to 3
(rough). The height field is periodic, so copies of it tile without seams.

The heights are scaled to [0, 1] and raised to `Power`. A `Power` above 1
flattens the lowlands and sharpens the peaks.

With `Craters` on, craters are added: `CraterDensity` craters per 1/100 of
the map area, with radii from 0.4% to 10% of the map size (at least two
cells), and the number of craters above radius r falling as r^-2. Each
crater is a bowl with a raised rim. `CraterHeight` is the depth of the
largest craters as a fraction of the height range; the depth of a crater is
proportional to its radius. Craters wrap around the edges like the terrain.

The result is scaled to [0, 1] again and multiplied by `ZFactor` in the
terrain.

`Seed` 0 takes a random seed. `Forge()` stores the seed it used in `Seed`, so
forging again gives the same terrain. `Mesh` is rounded up to a power of two,
and the value used is stored in `Mesh`.

### RectTerrain::Edenify()

Demonstrates `SetFromHisto()` and `Smooth()`: fills a `TH2F` with a garden on
a mount in a plain, with four rivers flowing out of it, draws the histogram
into the canvas "Eden" and takes the terrain from it. Set `Ribbon` to
`terrain.pov` for the colors. The demo `Geom1/eden.C` sets it up.

## Tmp1

### TabletReader

Reads a drawing tablet through the kernel's evdev interface and turns pen
strokes into `TabletStroke`s of the linked `TabletStrokeList`. The first pad
button starts a new stroke list, the second ends it.

`PenDevice` and `PadDevice` are `/dev/input/eventN` paths. Empty means: take
the first input device that reports a pen with pressure, and the device of
the same tablet (same vendor and product) that has buttons. With `Grab`, the
devices are grabbed while reading, so the desktop does not get the events.

The devices are readable by root and the `input` group. The udev rule

    SUBSYSTEM=="input", ENV{ID_INPUT_TABLET}=="1", TAG+="uaccess"

gives the user logged in at the console access to tablets only.

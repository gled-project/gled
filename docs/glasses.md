# Notes on individual glasses

What some glasses do and how their parameters work, beyond what their GUI
shows.

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

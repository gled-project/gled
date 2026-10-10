# Gled

Gled is a C++ framework, built on [ROOT](https://root.cern), for sharing
collections of objects among processes arranged in a hierarchy of servers
and clients. It generates a GUI for every class from comments in its header
and renders the objects with OpenGL.

A Gled process runs a server, the Saturn, and usually a GUI, the Eye, which
shows the objects of the Saturn and of the Saturns above it. Changes travel
up the hierarchy as requests and come back down as notifications, so every
process sees the same objects.

Gled was developed from 1999 to about 2012 and has been kept building with
current ROOT and compilers since. Version 2.0.0 is the first release from
GitHub.

## Layout

| path | what |
|---|---|
| `gled-build/` | the build area: `configure`, make fragments, code generators, tests; after a build also `bin/`, `lib/`, `macros/` and `demos/` with links into the libsets |
| `libsets/<LibSet>/` | the sources, one directory per libset, each with its `demos/` |
| `docs/gledimp.md` | *Gled for the impatient*: the concepts and the class directives that the code generators read |
| `docs/glasses.md` | Notes on individual glasses: what their parameters do, device setup |

The libsets are GledCore (the framework, the GUI and the GL renderer),
Geom1 (geometry, images, terrains), Numerica, Audio1 (OpenAL sound), Net1,
GledGTS (GTS surfaces), Var1, RootGeo (ROOT geometries) and Tmp1.

## Building

Gled needs ROOT 6 built with C++ modules (`runtime_cxxmodules`, the default
on Linux; distribution packages may turn it off), FLTK 1.3 with thread and
OpenGL support, GLEW, GLU,
DevIL, freetype, GSL and OpenSSL 3. GledGTS needs GTS, and Audio1 needs
OpenAL, freealut and libvorbis. FTGL is part of GledCore.

On Fedora 42 the system packages cover all of it:

```sh
dnf install fltk-devel glew-devel mesa-libGLU-devel DevIL-devel freetype-devel \
  gsl-devel openssl-devel gts-devel openal-soft-devel freealut-devel libvorbis-devel
```

The gled-builder repository can instead build the externals into one
prefix; pass that prefix to `configure` with `--external`.

```sh
cd gled-build
export ROOTSYS=<ROOT installation> GLEDSYS=$PWD
./configure --libsets '<auto>'
source build_env.sh
make -j8
```

`configure` takes the libsets from `../libsets`. Set `GLEDSYS` as above if
your environment already sets it for another Gled tree. See
`gled-build/INSTALL` for the options.

## Running

```sh
cd gled-build
source build_env.sh
cd demos/GledCore
gled hello_gled.C
```

`gled -h` lists the options; `saturn` is the server without a GUI. In a GL
window, the mouse moves the camera, and the Ctrl key changes what it does:

| | vertical drag | horizontal drag |
|---|---|---|
| left | forward, backward | slide left, right |
| Ctrl + left | slide up, down | slide left, right |
| middle | turn up, down | turn left, right |
| Ctrl + middle | turn down, up | roll left, right |

Home returns the camera to the origin.

`gled-build/test/README.md` describes the regression and demo tests.

## License

Gled is free software under the GNU Lesser General Public License, version
3 or later. See `LICENSE`, which also lists the third-party code in the
repository and its terms, and `AUTHORS`.

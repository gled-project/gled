# NAME

gledimp - `Gled` for the impatient

# DESCRIPTION

This document covers four main topics:

- [Installing Gled](#installing-gled): how to get and install `Gled`
- [Running Gled](#running-gled): how to run `Gled` and what you can do with it
- [Building Gled](#building-gled): how to build `Gled` from sources; CVS instructions
- [Extending Gled](#extending-gled): how to create new libsets and write your own classes

# Installing Gled

Currently, `Gled` only compiles and runs on `GNU/Linux`, `gcc` is
the only supported compiler. In principle, it should be possible to
make it run on any platform that has POSIX threads.

## Binary install

Distributions of `Gled` are available from
[ftp://ftp.gled.org/](ftp://ftp.gled.org/)&lt;major>.&lt;minor>.&lt;patch>. The link `current`
points to the latest release.

The binary distribution of `Gled` provides a tarball blob install: it
resides in its own directory and user's environment needs to be
configured for its use. Some top-level scripts for running demos will
set it up for you, otherwise one just needs to source the environment
setting script that resides in the top-level directory. A `README`
file giving the details is also there.

In short:

    v=1.3.0
    b=slc4-x86
    wget ftp://ftp.gled.org/$v/gled-$v-$b.tar.gz
    tar xzf gled-$v-$b.tar.gz
    cd gled-$v
    ./greed-demo-menu.sh

The `Gled` system can be extended via **`libsets`**. Each libset
contains three libraries (base, GUI, GL rendering) and possibly also
executables and additional libraries. The core of the `Gled` system
itself is contained in the `GledCore` libset. The default
`gled` distribution contains seven libsets: `GledCore`,
`Geom1`, `Numerica`, `GTS`, `Audio1`, `RootGeo` and `Var1`.

To run other demos one needs to setup the environment:

    # Setup the environment
    . gled-env.sh
    # Go to demos directory
    cd gled/demos
    # Choose libset / module
    cd Geom1
    # Run the chosen demo
    gled images.C

Reading section [Running Gled](#running-gled) is recommended. Instructions for
running demos are also there.

## Required software

Each libset can depend on external software, some of which might not
be widely available. To minimize the fuss, such software packages are
shipped within the binary distribution.

Basic distribution of `Gled` depends on the following software:

- **`ROOT`:** Homepage: [http://root.cern.ch/](http://root.cern.ch/)
Download: [http://root.cern.ch/root/Availability.html](http://root.cern.ch/root/Availability.html)

    `ROOT` is released quite often and quite often, also, there is no
    binary compatibility between patch releases. For development of
    `Gled` the latest release is used, unless there are some
    unresolved issues.

    If you have your own `ROOT` and intend to use `Gled` binaries, make
    sure that the versions of your `ROOT` and `ROOT` that `Gled` was
    compiled with are the same.

- **`FLTK` 
Homepage: [http://www.fltk.org/](http://www.fltk.org/):** `Gled` uses `FLTK v1.1` branch, usually the latest patch version.

    If you want to build your own `FLTK`, use `--enable-threads` and
    `--enable-shared` options to `configure`. Optionally, use
    `--enable-xft` to enable true type font support.

- **`fltk-mt`
FTP: [ftp://ftp.gled.org/current/required\_software/source/](ftp://ftp.gled.org/current/required_software/source/):** `fltk-mt` is a small library of widgets, partially collected from
fltk bazaar and partially written for `Gled` itself. It is
distributed together with other required software.
- **`DevIL` (Developers Image Library)
Homepage: [http://openil.sourceforge.net/](http://openil.sourceforge.net/):** This is the neatest image library I could find (and I've been
searching for a long time). It handles image loading/saving as well
as transforming in a simple, `GL`-like manner.
- **`Little cms`
Homepage: [http://www.littlecms.com/](http://www.littlecms.com/):** Optionally used by `DevIL` as a color management engine. Since
`gled-1.3.0` `lcms` is not provided in the distribution of `Gled`,
but can be activated in the build-system.

# Running Gled

`Gled` has two main executables. `gled` is the standard one and
spawns `Gled` native GUI (FLTK) while `saturn` has command-line
interface only and is useful for pure servers and proxies.

Both will list their command-line options if started with `-h` or `--help`
switch:

    # prompt> gled --help
    Arguments: [options] [dir] [file(s)]
                         dir   ~ cd to dir prior to exec of files
                         files ~ ROOT macro scripts to process

      Short options use a single '-' and can not be aggregated.

    Gled options:
    -------------
      --datadir   <dir>   directory containing Gled data
      --libdir    <dir>   directory containing Gled libraries
      --preexec <m1:m2..> pre-exec specified macros
      --r[un]             spawn Saturn/Sun immediately (before processing files)
                          Saturn if -master is specified, Sun otherwise
      --opensrvsock       open server socket early on (needed for eyes over IP)
      --allowmoons        accept moon connections
      --root-web  <type>  ROOT web display for TCanvas & co., as root --web=<type>:
                          on, chrome, firefox, server:<port>, ... (def: off;
                          a web display starts an http server thread)
      --noprompt          no ROOT prompt (runs TApplication insted of TRint)
      --pidfile   <file>  write PID into given file
      --s[ssize]  <num>   specify size of sun-space (can be eg. 2e20)
      --p[ort]    <num>   specify server port (def: 9061)
      --portscan  <num>   if server port can not be opened, try <num> higher ports
      --m[aster] <host>[:<port>] master Saturn address (def port: 9061)
      --n[ame]    <str>   name of Saturn
      --t[itle]   <str>   title of Saturn

    Logging options:
      --log    <file>     specify log file name (saturn:'<stdout>', gled:'<null>')
                          shorthands: '-' => <null>, '+' => <stdout>
      --out    <file>     specify file name for stdout redirection
      --err    <file>     specify file name for stderr redirection
      --outerr <file>     specify file name for stdout and stderr redirection
                          shorthands: '-' => <null>, '=' => same as log
      --logflush          flush log and stdout after every line, stderr at once
                          (follow the files live; nothing is lost on a hang)
      --debug  <lvl>      set debug level (some messages require debug build)
      -l                  no splash info

    Authentication options:
      --auth              use authentication
      --authdir   <str>   directory containing auth data (def: ~/.gled/auth)
      --saturnid  <str>   identity of the Saturn (def: 'sun.absolute' or 'saturn')
      --eyeid     <str>   default identity of Eyes (def: 'guest')

    Renderer loading options:
      --rnr <r1>:<r2>:... specify which rendering libraries to load (for gled: GL)

    GledGUI options:
    ----------------
      --swm   fs:dh:dw    specify font-size, vert-space and char width
                          default: 12:6:0 (dw~0 means measure font)
      --font  font-id     use fltk's font-id as default font
      --iconize           iconize main window on start-up
      --nomsgwin | --nomw start gled without the message window (consider '-log +')
      --stereo            use stereo GL visuals by default

`saturn --help` lists the same options without the GledGUI ones, and adds
`--daemon`: run as daemon; implies `--noprompt`, std streams get closed.

But usually, at least for first-time usage, the following will do:

    gled some_demo.C

This starts `gled` without authentication and executes `c++` macro
script `some_scene.C`. Most macros in the demo section 
start the server and GUI automatically.

There is no rule as to what macros should contain, but before you
start adding objects into the system, the server must be spawned:

    Gled::AssertMacro("sun.C");

`Gled`'s search path for macros is initially set to
`.:$HOME/.gled:$GLEDSYS/macros`. Some standard macros are supplied
with the distribution and reside in the `$GLEDSYS/macros` directory
(eg. `sun.C`). Typically the demo macros execute `sun.C` somwhere
close to the beginning and `eye.C` (standard GUI) at the end.

To quit `Gled`, click on the `Eat flaming death` button in the main
window or type `.q` on the `ROOT` prompt.

## Setting-up the environment

If you downloaded the full binary packet, then sourcing the script
`gled-env.sh` (or `gled-env.csh`) should do the job:

    . gled-env.sh          # for sh
    source gled-env.csh    # for csh

The scripts sets the environment variables `GLEDSYS`
and `ROOTSYS` and extends the `PATH` and
`LD_LIBRARY_PATH` appropriately.

To dump the environment script, one can use:

    ./gled-env-dump.sh     # dumps env for sh
    ./gled-env-dump.csh    # dumps env for csh

and store its output to a file or login script.

## Configuration of Gled authentication

To run Gled with authentication, use the `-auth` command-line option.

Additional configuration is needed for authentication support.
All other configuration can be done via command-line options and
start-up scripts. See the `$GLEDSYS/macros` directory and the
provided demos for examples.

Current implementation of the authentication procedure is based on the
RSA public key cryptography and uses challenge-response
authentication. All further communication \`trusts' the accepted socket
and is not encrypted. The authentication and group membership data are
stored in a file-system directory structure. The root of the structure
defaults to `ENV{HOME}/.gled/auth`, but can be changed with the
`-authdir` command-line option (e.g. `gled -authdir
/etc/gled/auth`). The keys are stored in the `public_keys` and
`private_keys` directories with the individual filename being equal
to the name of the identity it is representing. This means that if you
want to allow user `foo.bar@baz.org` to login to the cluster, you
must obtain his public key and copy it to the `public_keys`
directory.  Group membership information is stored in the `groups`
directory.  Each group is represented by a file (again, with the same
name as the group) which is simply a new-line separated list of
identities that are allowed to claim this group identity. By
convention the group names begin with the `@` character
(e.g. `@admin` or `@admin@baz.org`).

To initialize the authentication system do:

    gled-auth-init -dir <auth-directory>
    # default -dir=~/.gled/auth

This will create the standard directory structure in the given
directory and populate it with standard identities for servers,
proxies and clients. To add additional identities use:

    gled-auth-genkeypair -name <some-user-name>

Use standard shell commands and editors to modify the authentication
structures.

## Running demos

All demos are available as ROOT macros. Note that `Gled`'s search
path for macros is initially set to `.:$HOME/.gled:$GLEDSYS/macros`
and that other scripts can be called from within the scripts using the
`gROOT->Macro("sun.C");` syntax. From `ROOT`'s prompt it can be
executed simply by `.x sun.C`.

All of the demo scripts contain instructions that start the
server, create some objects and open a viewer (a GUI shell and a GL
window).

Some scripts might not start the GUI. They should be run as:

    gled <demo-name>.C

Note that you will need the `-auth` option to enable authentication
and `-eyeid sun.absolute` to spawn GUI with the adminstrator
privileges. Most demo scripts do not setup any authorization scheme
and GUI requests to top-level objects would be refused.

## The `Gled` GUI

`Gled`'s GUI attempts to offer a generic representation of object
graphs (GUI shell/browser) and moderate introspection into the
functioning of the system itself (log viewer, direct access to cluster
graph). At the same time `ROOT`'s interactive prompt has been
retained: it allows invoking of `C/C++` calls directly. It plays the
role of a start-up console, system monitor, debugger and execution
environment for `C/C++` scripts.

The following is a brief overview of command-line interface and GUI,
that should get you going.

### The terminal: `ROOT` prompt

In the terminal from which you launch gled, you will be presented with
a command-line prompt of cling, the `C++` interpreter of ROOT. This prompt
in principle belongs to `ROOT` (whose event-loop is running in a
separate thread), but as `Gled`'s objects can be exported into (or
created from) the context of the interpreter, it belongs also
to `Gled`. 

It really helps on this point if you are a bit familiar
with `ROOT`. To run it's demos, do:

    cd $ROOTSYS/tutorials
    root demos.C demoshelp.C 

More detailed tutorials can be found at `ROOT`'s homepage:
[http://root.cern.ch/](http://root.cern.ch/).

**Executing macros.** As mentioned above, macros can be executed from
the `ROOT` prompt using the `.x <macro-name.C>` syntax. Most
macros from the `macros/` and `demos/` directories are plain scripts
enclosed in curly braces. When such a script is executed, all
variables declared in it (even in inner scopes ... for example, try
with `demos/Geom1/spheres.C`) can still be referenced from the
interpreter. You can clear interpreter's list of global variables by
issuing `gInterpreter->ResetGlobals()`.

If the file invoked by `.x` contains functions (i.e. has a proper
`C/C++` syntax), than the functions will be imported into the
interpreter and the function with the name equal to that of the file
will be executed.  In this case you can also just import (load) the
functions by doing `.L <file-name>`.

**Importing objects from Gled.** By right clicking on the name box in
the object browser you can select the `Export to CINT` entry. You
will be prompted for variable name under which the object will be
exported to the interpreter.

Now you can in principle call any method on the `Gled` object. BUT if
you are running a server with connected clients, those method calls
will not be sent to clients. In this case the use of interpreter for
direct manipulation of exported `Gled` objects is limited to server
start-up. There is a way to properly post method invocation requests
from the interpreter, but it is beyond the scope of this introduction.

### Gled window

The main `gled` window contains basic controls for spawning
eyes, loading libsets, setting of debug level and a prominent exit
button. The reset of the window is a message browser, where output
from different parts of `Gled` will accumulate.

### Shell / Nest window

The shell window displays object graphs. Each object can have two types
of references to other objects:

- **Links**
are by default shown on the right hand-side of the shell and
can be expanded/collapsed by using the left collapsor box. A link is
in fact a named pointer. The link functionality is an extension of
standard tree structure that allows `Gled` to handle arbitrarily
complex object graphs.
- **Lists**
are collections of pointers to other objects. The right
collapsor box will expand/collapse the list contents for a given
object. This is the functionality you are used to from standard
directory browsers.

In default shell (the one spawned by macro `eye.C` or from GUI
button) you get to the loaded scenes by list-expanding `SunKing` and
then `Scenes`.

By double clicking on the name box of an object you get its _full
view_: all auto generated widgets are stacked in separate
window. Changing the values will send proper change requests to the
server (then it gets back update notifications). Value fields can be
click-dragged in manner of sliders (middle mouse moves by factor 10,
right mouse by 100). Right-mouse opens a context menu for a given
object, offering many possibilities).

Selecting `View->Link/Custom` will toggle between display of
links for a given object and a custom view of control widgets (such as
you have in the full view). Some predefined views are available. You
can edit the format string by selecting `View->Edit Custom`.

Drag-and-drop somewhat works. Dragging an object on the link-name-box
sets the link. Dragging on the name-box in the graph (tree) browser
appends the object as the last element in the list if the target of
the drop operation is a `ZList` (`Gled`'s list base-class).

### Pupil (GL window)

To navigate in a GL window (pupil) use left/middle mouse
buttons. Control key modifies behaviour and is really useful.

    left-drag          forward/backwards slide left/right
    ctrl-left-drag     slide up/down     slide left/right
    mid-drag           turn up/down      turn left/right
    ctrl-mid-drag      turn down/up      roll left/right (think pilot stick)
    <Home>             return to the origin
    <F1>               spawn Pupil control window
    <F2>               spawn Camera control window

The pupil renders the conents of its `PupilInfo`. You can find active
`PupilInfo` lenses (those that have an associated GL window) of a
given shell by expanding `ShellInfo`'s link `Pupils`. By opening
full view of the `PupilInfo` object (named Example Pupil in the
default shell) you can set up some rendering parameters (same as `F1`).

Picking works (bound to right mouse button) and opens up a context
menu, together with some pupil-specific options (render
stack (parents), z-distance and `Jump To`).

The thing that is really missing is the ability to move/rotate objects
directly from the pupil itself. This has been implemented in
`Gled-1.0` and was quite useful.

## Connecting a Gled cluster

To start gled in a server mode, use:

    gled -allowmoons <other options>

or call

    Gled::theOne->AllowMoonConnections()

from the command line or server initialization script. Consider using
the authentication as well.

To connect to an already running gled, use:

    gled -m <host-to-connect-to> moon.C

`moon.C` script will open the connection, download the server-objects
and spawn GUI shell and a GL viewer on the first available scene. For
specific uses the macro should be modified to open views of concrete,
predetermined objects).

Once connected, you cannot reconnect to another saturn: quit gled and
run a new one.

If the master dies (or closes socket), gled will die with an assertion
failure. In principle local instance could adopt server's objects and
continue running in a detached mode.

## Gled Bootstrap

The following is a bootstrap procedure done in `gled` and
`saturn` executables.

1. Create Gled/GledGUI object (`saturn`/`gled`), parse command-line
options. The Gled object is a singleton, accessible via
`Gled::theOne` (or `GledGUI::theOne`) both from your compiled code
and `ROOT` prompt/scripts.
2. Execute `-preexec` macros.
3. Create ROOT application interface (`TRint`); let it parse
command-line arguments.
4. Start the GUI thread that manages all FLTK windows and eye's
event-loop.
5. If `-r` option is given, start-up the Saturn (server). Note that
moons (clients) are not allowed to connect yet, while eyes are.

    In case of a moon (when `-master <host>` option is given), this
    step (call to function that starts server threads) will finish after
    ALL mandatory queens have been received from the master server. This
    means that the scripts (being executed on the next step) can rely on
    having client object-space set-up properly.

6. `TRint` processes macros given on the command line. Note that
the server is not allowing moon connections yet and the scripts can
freely create objects and attach them into the object graphs without
worrying about the clients.
7. If `-allowmoons` option is given, allow moon connections. This can
also be achieved later by calling
`Gled::theOne->AllowMoonConnections()`.
8. Unless `-norint` option is given, run `TRint` in a dedicated
thread. After that the command-line prompt is receiving input. This
thread also runs all `ROOT`'s X-windows (canvases containing
histograms, ROOT browsers). `FLTK` and `ROOT` have different X
displays.

## When things go wrong

Sometimes, `Gled` can die quite horribly. It's a very generic
multi-threaded application, allowing libsets to perform just about
anything - and not all of them have undergone rigorous
testing.

`ROOT`'s interactive application (interface to cling) that handles the
terminal input thread is by default set to handle most of the signals
(like SIGSEGV) - it attaches gdb, dumps the stack, and pretends
everything is fine. This is rarely true and there is a high
probability that your terminal will lock up and you will not be able
to kill `gled` in any other way than to `kill` it (as in `killall
\-9 gled`).  This behaviour can be switched off by adding `gSystem->IgnoreSignal(kSigSegmentationViolation, kTRUE)` somewhere in
the initialization script or to your `.rootrc` or `rootlogon.C`
file. Then `gled` will crash properly, dumping the core if
environment allows it. See also script `isegv.C`. 

# Building Gled

## Building `Gled` and external software

There is a special module in `Gled` SVN repository that does all the
work.

    svn co https://svn.gled.org/gled/trunk/gled-builder
    cd gled-builder
    autoconf
    ./configure          # default prefix is `pwd`/install
    make

Note, that this will get all the sources from the web (rsync and svn
servers at `www.gled.org`) so network connection is
required. Obviously, you will need `subversion` client and `GNU
autotools`. `CMake` is also needed for building of `OpenAL`. Further,
as the external software gets configured and built, it will require
presence of several low level libraries. The easiest way to ensure you
have all this stuff is to run:

    PKGS="subversion autoconf automake libtool cmake \
          glew glew-devel \
          fltk DevIL gts openal freealut \
          fltk-devel DevIL-devel gts-devel openal-devel freealut-devel"

    [apt-get | yum | <whatever>] install $PKGS

`gled-builder` will build its own versions of `fltk`, `DevIL`,
`gts`, `openal` and `freealut` but their system-level install will
ensure you have all the dependencies. Eventually (and optionally) we
will check if appropriate versions of those are available and skip
them during the build. However, `Gled` requires the latest versions
of some libraries that are not shipped even with the latest
distributions (`fltk-1.1.9`, `gts-HEAD`).

`Gled` is also put into the install directory ... but one can also
run it from the build directory itself.

### Running from the install directory

This is meant for normal usage. The install directory is fully
relocatable - you can tarball it and use it on any compatible machine.

To setup the environment:

    cd install-dir
    . gled-env.sh

Run demos:

    cd gled/demos
    cd Var1
    gled tringula.C

### Running from the build directory

This is to be used if you intend to do development. `Gled` sources
are downloaded via `subversion`, so you have a working development
environment.

Setup the environment:

    cd gled/gled-build
    . build_env.sh # source  build_env.csh

The demos are in the `demos/` directory.

For details see the `README` file provided in `gled-builder`.

## Bulding `Gled`

### Getting the sources

All `Gled` sources reside in a [SVN
repository](http://svn.gled.org/gled/) (with
[viewcvs](http://www.gled.org/cgi-bin/viewcvs.cgi/)). The build
directories, libsets, demos and the documentaion are all stored
there. To allow for builds with specific sub-set of libsets, the
repository structure is not entirely standard ... usually one does not
checkout the whole trunk and some top-level directories need to be
checked-out separately. All this is encoded in the helper checkout
script available from the SVN:

    svn export http://svn.gled.org/gled/trunk/gled-build-checkout

Inscpect it for details, or simply run it as:

    ./gled-build-checkout trunk

to get the default libsets or as:

    ./gled-build-checkout trunk GledCore Geom1

if you want to explicitly specify which libsets to checkout.

By default, this will create directory `gled-build`; use
`--dir=<my-gled-dir`> to override it.

### Setting up the build environment

The header files of all required external software must be accessible.
Further, `$ROOTSYS` and `$GLEDSYS` variables must be set
appropriately and corresponding entries added into `$PATH` and
`$LD_LIBRARY_PATH`.

There are several environment scripts in the `gled-build` directory,
in particular see `binenv.sh` and `hereenv.sh`. Use / modify one of
those or roll your own ... then source it.

Anyway, the build environment is exactly the same as the run-time
environment. So the environment set-up can be done as described in the
section [Setting-up the environment](#setting-up-the-environment). The `configure` script
produces the environment set-up scripts `build_env.sh` and
`build_env.csh` which can be used to easily access an existing
`Gled` build.

### Configuring

**NOTE:** This is not standard configure! It determines particularities
of the `Gled` build itself:

- order of libset building;
- the libset ids and the list of glasses of all libsets, read from
the `glass.list` file of each libset.

The command-line options for `configure` script and their default
values are stored in the `configure.rc` file.  You can edit the file
or use command-line options. Options can be given as `--<opt1>=<val1>` or `-<opt1> <val1>`, option names can be
abbreviated.

The most relevant options are (see `configure.rc` for full listing
and default values): 

`-ucpp <str>`: user cpp-flags (e.g. -D\_SOMETHING\_),

`-ucxx <str>`: user cxx-flags,

`-uld <str>`: user ld-flags,

`-debug`: implies -DDEBUG and -g (otherwise -O),

`-libsets <auto> | <auto-prompt> | <file:[filename]> |
[libset1:libset2...]`: libsets to build. The default is `<auto-prompt>` which scans the directory, displays the list of found
libsets and asks for confirmation. If `file:some-file` syntax is
used, them `some-file` should contain a new-line separated list of
libset names.

`-external <search-path>`: `search-path` is a ':' separated list
of directories to include/link from; each `dir` implies
`-I<dir>/include` and `-L<dir>/lib`. It is
empty by default.

`-base <search-path>`: `search-path` is a ':' separated list of
existing `Gled` build directories in which build has been
completed. `configure` will resolve dependencies of local libsets by
trying to locate the missing libsets in `base` directories.

Now run `./configure <whatever options you need>`. `configure`
creates a file `build_config` which contains a perl-data-dump of two hashes
holding information needed for compilation of libsets.  Dependencies
are resolved, so the order of compilation is determined properly even
if the libsets are listed in a wrong order.

The `configure` script also creates all auto-generated files and runs
`make depend` in all lib-set directories.

### Building

In principle ... just run `make`.

The default target builds all libraries and executables and installs
them into `$GLEDSYS/lib` and `bin` directories.

The `make` process uses the default compiler. Export the `CXX`
variable if you want to use some other compiler or, perhaps,
`distcc`.

### Testing

As the environment is already setup, just choose the demo and run it:

    cd demos/Geom1
    gled images.C

# Extending Gled

In principle, `Gled` should be extended by creating new
libsets. However, the `Gled` project is still young and some
functionality is still missing in the core libsets. See
[status](http://www.gled.org/status/),
[libsets](http://www.gled.org/libsets/) and
[contributing](http://www.gled.org/contributing/) for details on
development status/plans and other information for
developers/contributors.

## Creating a libset

To create a libset directory, use the `gled-create` script. You need
to decide on the libset's name and select an unused library id
(`unsigned short int`; it can be trivially changed as long as you
don't save any objects into files you want to use later).
`gled-create` refuses an id that is already used in `build_config`.

Then you need an existing `Gled` build containing all the libsets
that the new libset will depend upon (note that all libsets depend on
`GledCore`). Also, you need to set-up the environment.

To create a libset `FooSet` with id 129 that uses `Geom1`, use

    gled-create libset FooSet 129 --requires 'GledCore Geom1'

This will create directory `libsets/FooSet` containing all standard
directories and files, with the required libsets in the
`REQUIRES_LIB_SETS` line of the Makefile. It also creates the symlink
`$GLEDSYS/FooSet` pointing to it: `configure` finds libsets by these
symlinks. If your libset will not contain any executables and does not
depend on any external libraries, this is all you will ever need to
do. Otherwise see `GledCore::Makefile` for an example of building
executables and `Geom1::Makefile` for an example of linking against
additional libraries.

Another important file is `glass.list` in the libset directory: this
is a simple catalog of all `Gled` classes (or `glasses`) in this
libset. Initially it is empty (containing some commented examples from
`GledCore`). `gled-create glass` and `gled-create rnr` maintain it.

Now you have to reconfigure the `Gled` build so that the new libset
is built. Pass the full list of libsets, or `<auto`> or
`<auto-prompt`>, to the reconfigure command:

    ./configure --reconfigure GledCore:Geom1:FooSet

The `--reconfigure` option must be the only option to
`configure`. It takes an optional argument specifying which libsets
to build (the syntax is the same as for option `--libsets`). It
defaults to the old value.

And now we're ready to go:

    make

This will produce libset `FooSet` with glue code only - you will be
able to load it, but it will not contain any classes.

## Adding classes to a libset

Classes making up a libset can be divided into several categories,
depending on the role they play in the `Gled` system. For clarity, as
well as to simplify the build process, the files (header and
implementation) for each category are placed in a separate directory.

- **Fully Gled enabled classes** or **glasses**: instances of such
classes, called **lenses**, enter the object graphs of `Gled` and are
capable of generating and processing method invocation requests
(MIRs). Further, they are representable in `Gled` GUI and can spawn
object views. Glasses can have their own renderers that are used to
render their representations in 3D space.

    Glasses must be subclassed from the `ZGlass` class and by default
    reside in the `Glasses` directory. `ZGlass` is not a `TObject`; it
    has a `ROOT` dictionary (`ClassDef`) that provides streaming.

- **Fully `ROOT` enabled classes** or **stones** can be streamed
and accessed from the `ROOT` interpreter (`cling`). In `Gled` context
they are used as data containers for structures that can be data
members of glasses (either directly or via a pointer).

    Stones reside in the `Stones` directory. They need a `ClassDef` (or
    `ClassDefNV`) in the class declaration and an entry in
    `Stones/LinkDef.h`; a base class is optional.

- **Other classes and namespaces**. Sometimes you will need other
supporting structures, internal representations or functions and
wrappers for external libraries. In principle, you can place them
anywhere, but you should edit the `Makefile` accordingly. The
preferred place for namespaces that will be accessed by other libsets
is in `<libset-name>` directory. (The `GledCore` libset departs
from this convention and holds public namespaces in `Gled` and
`GledView` directories.)

All classes are declared in `namespace gled`. Source files have
`using namespace gled;` after their includes, before the `.c7` file
of a glass is included.

### Adding glasses

To get a skeleton code of a glass `cd` to libset directory and type:

    gled-create glass <new-glass-name> --base <base-glass-name>

There are quite some conventions that a glass header file should
conform to. All the basic ones are fulfilled automatically by the above
script. It also adds a line describing the new glass to `glass.list`,
with the next free class id (`--id` chooses one).

Then you will almost certainly add some member data and
methods. You should follow the `ROOT` convention for basic types: it
assures interoperability of different architectures (`int ~ Int_t`,
`unsigned short ~ UShort_t` etc.).  Examples will be most fruitful
... so take a look at some glasses from the base distribution
(`Geom1/SMorph` can serve you well).

Header files of glasses are parsed by `project7` parser / code generator.
It creates `Gled` compliant `Get/Set` methods, wrappers for MIR
creation and processing and a separate class named `<glass-name>View`
which contains GUI code needed to manage widgets representing the glass.

Instructions for `project7` are contained in comments directly
following the declarations. The output is stored in two files: `<glass-name>.h7` (included back into the class declaration itself)
and `<glass-name>.c7` which is included from the
corresponding source (`.cxx`) file.

Comments have two parts, separated by the `7` character:

    Int_t         mFoo;           // Xport{GS} 7 ValueOut()

The left part (`Xport{GS}`) specifies what parts of code need to be
auto-generated (in this case `GetFoo` and `SetFoo` methods will be generated,
in accordance with `Gled` coding requirements) while the right one is a
constructor call for a `perl` class that will generate the widget
code (see `GledCore/perllib/Gled_Widgets.pm`). The widget part is
optional: if not specified, the widget-code will not be generated.

The next section gives some examples ([Examples of data and function
meber declarations](#examples-of-data-and-function-meber-declarations)), more can be found by browsing the code. The full
list of all recognized directives is in a dedicated section
([Specification of `project7` directives](#specification-of-project7-directives)).

#### Examples of data and function meber declarations

**Data member example** (`Geom1/SMorph`):

    Real_t        mSx;            // X{GS}  7 Value(-range=>[0,100, 1,100], -join=>1)

`X{GS} ~ Xport{GS}` generates `Get/SetSx` methods. The widget will be
created by the `Value` `perl` class. 1/100 is the default step for
drag operations. In full view the next widget will be possibly
appended to the same line.

**Link example** (`GledCore/ZNodeLink`):

    ZGlass*       mGlass;         // X{GS} L{} RnrBits{0,0,0,4}

`X` as above, but this is a link: `L{}`. `RnrBits` are explained below.

**Method example** (`GledCore/Eventor`):

    virtual void  Start();        // X{E} 7 MButt(-join=>1)

`X{E}` ~ generate MIR bindings and create a button that will send
request for invocation of this method. 

And from `GledCore/ZList`:

    virtual void Add(ZGlass* g);  // X{E} C{1}

Export as above, but this method has to be called with one `context`
argument. From GUI shell up to two such arguments can be defined by setting
`Beta` and `Gamma` links of the `ShellInfo`.

At the moment it is only possible to call methods that take glass and
basic type arguments from the shell (list of methods is available from
the context menu).

Stone members can have their specialized widgets. As an example see
`Geom1/SMorph::mColor` declaration.

#### Comment parsing by `rootcling`

Header files are also parsed by `rootcling`, `ROOT`'s dictionary and
I/O method (`Streamer`) generator. The build runs it once per
libset and builds the dictionary as a C++ module: all glasses get a
dictionary entry automatically, and the other classes of a directory
are listed in its `LinkDef.h` file (for example `Stones/LinkDef.h`).

By adding an `!` on the first character of the comment directly
following the data member declaration, this member will not be stored/retrieved to/from the stream. Example
from the `ZGlass` class:

    Bool_t     bMIRActive;      //  X{GS} 7 BoolOut(-join=>1)
    Short_t    mRefCount;       //! X{G}  7 ValOut(-width=>4)

In this case, the `mRefCount` variable is not streamed. This is to be
used for variables that store temporary data, have a local (per
Saturn) meaning only or can be regenerated from other data.

#### Building

Run `make` as usual: it picks up the new glass from `glass.list`
and regenerates the libset glue and the dictionary rules. Run
`./configure --reconfigure` in the top build directory before a
method of any glass takes the new glass as an argument: `project7`
looks up glasses in `build_config` to stream such arguments.

### Adding stones

To create a stone skeleton code do:

    gled-create stone <class-name> [--base <base-class>] [--p7]

in the libset directory. It writes the class and adds it to
`Stones/LinkDef.h`. No configuring is necessary; run `make`.

Header files of stones (as well as all other headers) can also be
processed by `project7`, but only `Get/Set/Ptr/Ref` methods will be
generated for non-glasses. `make` scans the header files for lines
like `#include "some-name.h7"` and runs `project7` on the headers
that have them. The `--p7` option of `gled-create stone` adds such a
line.

### Rendering classes

So far only OpenGL rendering is supported. Glass renderers are stored in
`Rnr/GL` directory and should be named `<glass-name>_GL_Rnr`.
To create a skeleton code type (do not postfix the `GL_Rnr` part):

    gled-create rnr <glass-name> [--base <rnr-base-glass>]
    e.g.: gled-create rnr SMorph --base ZNode

The renderer derives from the renderer of the base glass, which
defaults to the base class of the glass. The script also marks the
glass in `glass.list` as having its own renderer.

See examples of some rendering classes (note that some renderers are
not optimised for speed yet).

Rendering of each glass is separated in three functions:

`PreDraw`: set-up of render data, pushing of some attributes,
sometimes change of transformation matrix,

`Draw`: the actual drawing code,

`PostDraw`: clean-up after `PreDraw`.

Further, `ZList` descendants contain any number of children and any
`ZGlass` descendant possesses any number of _links_ to other
glasses. `RnrDriver` is quite general and each glass can be
configured to invoke `Draw` methods of self, of list members, of
links and of list members of glasses pointed to by links in a definite
order. This information is encoded in `GledViewNS::RnrCtrl` and
`GledViewNS::RnrBits` structures.

`RnrDriver` performs rendering of a lens on seven (7) levels and on
each level it executes the drawing functions specified by the
render-bits (in fact a `rnr-bit` is just a specification of level at
which the method should be called). At each level the execution is
done in the following order:

1. links that are marked for rendering, in order of declaration,
starting from base-classes;
2. the lens itself;
3. the list-members.

This is the default behaviour ... it can be changed by re-implementing
virtual method `CreateRnrScheme()` in the render-class. See `ZNode`
and `ZNode_GL_Rnr` for an example. The logic for invoking link
rendering before the lens rendering is that most often the links are
used for two purposes in rendering.

1. To temporarily setup some GL-state - in this case the the
`PreDraw()` method of the link can be called on the same level as the
`Draw()` method of the lens. The `PostDraw()` of link can be called
on the next level.
2.

Each glass with its own renderer can have a `RnrCtrl` declaration in
comments at the top of the glass declaration (or the default values
are used, see the example below). It contains the overall `RnrBits`
for the glass (self rendering and list rendering) and two rnr-steering
flags: `fRnrSelf` and `fRnrLinks`. Each link has a separate
collection of `RnrBits` which specify at which levels the links
draw-methods will be called. This data is parsed by the `project7`
parser/code generator and stored into the generated `.c7` file.

Let's decode this data for `Geom1/Board` glass which
renders a textured rectangle:

    class Board : public ZNode
    {
      //                      render   render
      //                      self     list members
      // 7777 RnrCtrl(RnrBits(2,4,6,0, 0,0,0,5))
      MAC_RNR_FRIENDS(Board);
    protected:
      ZImage*       mTexture;       // X{GS} L{} RnrBits{4,0,5,0, 0,0,0,0}
                                    //                   self     list members
      ...
    };

`RnrBits` constructor takes eight render levels, divided in two
groups of four. The first group pertains to self-rendering (or
rendering of object pointed to in case of a link) and the second group
to list-member rendering.

The first three numbers tell on which levels to invoke `PreDraw`,
`Draw` and `PostDraw`. The last one is for full descent: here
render driver calls for full render of the object in question
according to its own render bits. It usually doesn't make much sense
to use both explicit and full rendering for a given class/link.

Rendering of `Board` will execute some code on four out of seven
render-levels:

- **lvl 2:**
    1. Own `PreDraw` is called (in fact, by virtuality,
    `ZNode_GL_Rnr::PreDraw`, which sets up transformation matrix).
- **lvl 4:**
    1. `PreDraw` of `mTexture` (bind texture/enable texturing).
    2. Own `Draw` (just a rectangle with texture coordinates).
- **lvl 5:**
    1. `PostDraw` of `mTexture` (unbind texture / disable texturing).
    2. Full render descent is made for all list members. Notice, that local
    transformation matrix is still in order as required by the hierachical
    model.
- **lvl 6:**
    1. Own `PostDraw` is called (in fact `ZNode_GL_Rnr::PostDraw`; pop
    transformation matrix).

As subclasses still render links of their descendants, it is important
to inspect `RnrBits` of parent glasses before deciding on `RnrCtrl`
for a new glass. This is the primary reason for fixing number of
rendering levels to 7: it's almost certainly general enough to allow
for almost anything.

The declaration above:

    // 7777 RnrCtrl(RnrBits(2,4,6,0, 0,0,0,5))

is in fact a standard and can omitted.

For details see `GledCore/GledView/GledViewNS.h` where `RnrCtrl` and
`RnrBits` structures are declared.

#### `RnrBits` details

`RnrBits` consist of two groups of four numbers. The first group
tells `RnrDriver` on which levels to invoke object's own rendering
functions: the first number is for `PreDraw`, the second for `Draw`
and the third one for `PostDraw`. The fourth number is only relevant
for links and specifies on which render level to perform a _full
renderer descent_, that is to recursively descent into the rendering
of the object pointed to by the link.

The second group specifies the same information for ALL list members
of this object (if it is a `ZList` descendant, otherwise these bits
are ignored). At a given level all corresponding methods (`PreDraw/Draw/PostDraw/<full-descent>`) will be invoked in list
order. For links this information is relevant only if the link is
referencing a `ZList` descendant. Then the link is dereferenced
as a list and the corresponding methods are called for this list.

This last option was intended for complex renderer state set-ups with
pushing and popping of a collection of attributes stored in a
list. For a GL example, imagine having a link to a list having
elements for: a) setting render mode to wire-frame, b) changing
lightning conditions, c) etc. Besides, this list could then easily be
shared among several scene elements.

#### Future of `RnrCtrl` and `RnrBits`

`RnrCtrl` and `RnrBits` are now hard-coded in the class
definition. The original idea was to allow setting of custom render
bits for each object on GUI shell level. This is not yet implemented,
although the base viewing classes (`OptoStructs::*View`) and whole
rendering infrastructure is designed to allow for an easy
implementation.

This was to be combined with the option of adding _local links_ to
object entries in the GUI shell (these links are only visible by shell user
and renderers). By setting those links to render state modification
objects/lists a full local control over renderer state could be achieved.

Another option that seems reasonable is to store `RnrCtrl/RnrBits` on
lens level (via auto-generated link partners (e.g. `ZNode* mParent;`
implies also `RnrBits  mParentRnrBits;`) and a specialized GUI window
for inspecting/setting rendering related information).

Or even, one could traverse the render tree and render the tree itself
with nodes displaying object properties and rendering control
information. Clicking on that would then open the appropriate
rnr-control window.

# Specification of `project7` directives

`project7` directives are parsed differently for data-members and for
methods. Further, they are grouped into several families, each
specified on the declaration line as `familiy{options}` (see
examples in [Examples of data and function meber declarations](#examples-of-data-and-function-meber-declarations)).

## Directives for data-members

- **`Xport` (shorthand `X`)** The `Xport` family steers the generation of member-access-methods
(get/set etc) and MIR creation functions. Thus it exports certain
functionality of a given member.

    All flags but `dD` can be given for data members. Only `eE` and
    `dD` can be given for member functions.

    Let's pretend, we have the following declaration:

        Foo_t         mBar;

    - **gG:** Auto-generate
    `Foo_t GetBar()` (g) |
    `const Foo_t GetBar() const` (G).

        In fact, the return-type is only made `const` if the variable is
        a pointer or a reference. Also, for _link_-members, the
        constness is ignored.

        In principle a MIR-producing `Get` method could also be implemented
        ... the MIR would then return the streamed value in a `ReturnResult`
        buffer. The process would be synchronous, therefore limiting the usage
        of this concept to dedicated threads. This feature would be
        interesting for querying state of lenses in lower object spaces (whose
        ID has been obtained in some way) or local data of certain
        lenses. This would allow an easy implementation of pull-mode queries.

    - **rR:** Auto-generate
    `Foo_t& RefBar()` (r) |
    `const Foo_t& RefBar() const` (R).
    - **pP:** Auto-generate
    `Foo_t* PtrBar()` (p) |
    `const Foo_t* PtrBar() const` (P).
    - **sS:** Auto generate `void SetBar(Foo_t bar)`. With **S** also
    generate the Stamp method, that will send update request to the
    viewer.

        Occurrence of `s|S` automatically triggers creation of a MIR producing 
        `MIR* S_SetBar(Foo_t bar)`, like if the `e|E` flag is specified.

        If **q|Q** is specified, the set method signature is `SetFoo(Foo_t& bar)`
        | `SetFoo(const Foo_t& bar)`.

    - **tT:** Mark that transformation matrix (t) or triangulation data
    (T) has been changed (and needs to be re-calculated when
    used). Accompanies set-methods.

        These are special cases of themore general `Stamp` pragma:
        `t =~ ReqTrans`, `T =~ ReqTring`.

    - **x:** Mark that a structural change has occured (link and list operations do
    that automatically). This results in rebuilding of render-scheme for
    the lens, but can also be used for other purposes by custom handlers
    (A\_View::AbsorbRay()).
    - **eE:** Auto-generate MIR producing `MIR* S_SetBar(Foo_t bar)` but NOT the
    `void SetBar(Foo_t bar)` method -- it needs to be declared and
    implemented by the user. `E` and `e` are equivalent.

        If the first character on the comment line is '`!`' the method will
        be tagged as _local_: MIR will be sent to the local saturn only. This
        is useful for updating or printing of local data members.

    - **dD:** Auto-generate the `void DeltaBar(Foo_t bar)` method that calls
    `operator+` on current value and the argument. With **D** also
    generate the Stamp method, that will send update request to the
    viewer.

        **Note:** the **dD** flag for function-members requests member execution
        in a detached thread.

    - **fF:** Auto-generate MIR producing `MIR* S_DeltaBar(Foo_t bar)` but NOT the
    `void DeltaBar(Foo_t bar)` method -- it needs to be declared and
    implemented by the user. `F` and `f` are equivalent.

        This is not applicable for link data-members.

        Same comments as for **eE** apply.

- **`Stamp`:** Comma separated list of time-stamps to set. User must declare
`TimeStamp_t mStamp<stamp-name>` data members.
- **`Ray`:** Comma separated list of custom rays to emit. User must still specify
`Emit<ray-name>Ray()` method. See `NestInfo` for an example.

### Additional directives for link data-members

- **`Link` (shorthand `L`)** Declares a data member as _link_. If or `aA` is specified the
following methods that can be used to assert that a link is set are
auto-generated:
    - **aA:** Generate assert methods that check if link is set.

        **a** -- `protected: <link-type*> assert_<lower-case-link-name>(const Exc_t& exc_header);`

        **A** -- `public: <link-type>* Assert<link-name>(const Exc_t& exc_header);`

    - **fF:** Assumes that data-member `mLinkName_fid` exists. The link set
    method does additional check if the argument matches to this `FID`. By
    convention, if **f** is given the `fid`-variable is not streamed and should be
    set in `_init()` method (or constructor). If `F` is given, the
    `fid`-variable is streamed and it should be set in `AdEnlightenment()`. This
    convention is not enforced by auto-generated code.
- **`RnrBits`:** See section [Rendering classes](#rendering-classes) for explanation and syntax.

## Directives for function-members

- **`Xport` (shorthand `X`)**
    - **eE:** Assuming function:

            void FooBar(arguments)

        auto-generate MIR producing `MIR* S_FooBar(arguments)`. `E` and `e`
        are equivalent.

        If the first character on the comment line is '`!`' the method will
        be tagged as _local_: MIR will be sent to the local saturn only. This
        is useful for updating or printing of local data members.

    - **dD:** Specifyies that the method needs to be executed in a
    dedicated thread. Only relevant for member functions, ignored for
    data members.

        Further, `d` means only execute the method on the _Sun_ of a given
        lens while `D` instructs the MIR to be broadcasted for execution to all
        connected _Saturns_ (corresponds to `Multix` flag in `Eventor`,
        user-thread base class).

        To get the same behaviour for `Set` methods one has to write them
        manually and declare them like this:

            some_type   some_var;          // X{G}
            void Setsome_var(some_type x); // X{ED}

        **Note:** the **dD** flag for data-members requests creation of _Delta_ method.
- **`Ctx` (shorthand `C`)** Specifies the number of lens-type arguments, like `Ctx{1}`. Name
context is chosen as these arguments can specify an execution context
within a lens graph. Context arguments **must** always be specified in
the **front** of the argument list.

    Only relevant for function members, as `Set` methods have a well
    defined syntax, while for _links_ this is is set-up automatically.

## Directives valid for data and function-members

- **`Tags` (shorthand `T`)** A comma separated list of tags to assign to the method (to the `Set`
method in case of a data member). The tags are used by some
authorization modules to give special access control for methods
holding a certain tag.

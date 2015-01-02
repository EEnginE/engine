```
     _____ _____            _       _____
    |  ___|  ___|          (_)     |  ___|
    | |__ | |__ _ __   __ _ _ _ __ | |__
    |  __||  __| '_ \ / _` | | '_ \|  __|
    | |___| |__| | | | (_| | | | | | |___
    \____/\____/_| |_|\__, |_|_| |_\____/
                       __/ |
                      |___/
```

# EEnginE [![Build Status](<https://travis-ci.org/EEnginE/engine.svg?branch=master>)](<https://travis-ci.org/EEnginE/engine>)

EEnginE is (or will be) a simple OpenGL 3D engine.

# Denpendencies

-   CMake v2.8.9 or newer [<http://www.cmake.org/>]
-   The OpenGL headers (often in a mesa package)
-   Linux/UNIX (at least one of those):
    -   X11 and it's headers (including RandR)
    -   Wayland and it's headers (not yet supported)
    -   Mir and it's headers (not yet supported)
-   Boost v1.48.0 or newer [<http://www.boost.org/>]
-   GLEW v1.6.0 or newer [<http://glew.sourceforge.net/>]
-   gawk - needed for some scripts, mawk wont work!

# Installation

```
$ git clone https://github.com/EEnginE/engine
$ ./generate.sh
$ mkdir build
$ cd build
$ cmake <Additional CMake options> ..
$ make
$ make install
```

Alternatively, you can use ninja instead of make by adding -G Ninja to your cmake options.

# CMake options

## ENGINE

-DCMAKE\_INSTALL\_PREFIX=/Install/prefix/of/engine

-DENGINE\_VERBOSE=<0/1>

Special cmake output and verbose compiler messages &#x2026;

   - 0: disabled - default
   - 1: enabled

-DDISPLAY\_SERVER=<which>

Possible options:

   - UNIX\_X11     - default
   - WINDOWS      - alpha
   - UNIX\_WAYLAND - not yet supported
   - UNIX\_MIR     - not yet supported

-DENGINE\_LINK\_SHARED=<0/1>

Link the engine &#x2026;

   - 0: static
   - 1: shared - default

-DENGINE\_BUILD\_SHARED=<0/1>

Build the engine &#x2026;

   - 0: static
   - 1: shared - default

-DEXTRA\_CXX\_FLAGS=<Extra CXX compiler options>

-DSANITIZERS=<sanitizers>
-DSANITIZER\_OPTIONS=<options>

Select sanitizers to use. Only works with clang

## GLEW<a id="sec-4-2" name="sec-4-2"></a>

-DGLEW\_USE\_DEFAULT=<0/1>

if you want to have your system GLEW

   - 0: disabled - default
   - 1: enabled

-DGLEW\_ROOT=/Path/to/GLEW/install/prefix

## BOOST<a id="sec-4-3" name="sec-4-3"></a>

-DBoost\_NO\_SYSTEM\_PATHS=<0/1>

To tell CMake to only search boost in BOOST\_ROOT

   - 0: disabled - default
   - 1: enabled

-DBOOST\_ROOT=/Path/to/Boost/install/prefix

# Troubleshooting:<a id="sec-5" name="sec-5"></a>

## CMake can not find Boost<a id="sec-5-1" name="sec-5-1"></a>

Try to set:

-DBOOST\_ROOT=/Path/to/Boost/install/prefix

## CMake complains about an outdated Boost version even though the newest Version has been compiled by myself<a id="sec-5-2" name="sec-5-2"></a>

You can either remove your native boost installation or set

-DBoost\_NO\_SYSTEM\_PATHS=1

and

-DBOOST\_ROOT=/Path/to/Boost/install/prefix

## CMake can not find GLEW<a id="sec-5-3" name="sec-5-3"></a>

Try to set:

-DGLEW\_ROOT=/Path/to/GLEW/install/prefix

## I crosscompiled the project on Linux and want to test the result with WINE but WINE crashes immediately.<a id="sec-5-4" name="sec-5-4"></a>

Wine doesnt like boost::wregex because a function was unimplemented: [BUG](http://bugs.winehq.org/show_bug.cgi?id=36617)

Try to update your WINE version.
Or set:

-DEXTRA\_CXX\_FLAGS=-DUGLY\_WINE\_WORKAROUND

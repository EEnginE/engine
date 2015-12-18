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

# EEnginE [![Build Status](<https://travis-ci.org/EEnginE/engine.svg?branch=master>)](<https://travis-ci.org/EEnginE/engine>) [![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/EEnginE/engine?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

EEnginE is (or will be) a simple - but fullfeatured - OpenGL 3D engine.
Providing modules for networking, physic and 2D/3D graphic.

# Denpendencies

-   [CMake](http://www.cmake.org/) v3.4 or newer
-   The OpenGL headers (often in a mesa package)
-   Linux/UNIX (at least one of those):
    -   X11 and it's headers (including RandR)
    -   Wayland and it's headers (not yet supported)
    -   Mir and it's headers (not yet supported)

-   GLEW - downloaded and built automatically [<http://glew.sourceforge.net/>]

# Installation

## Linux

```sh
$ git clone --recursive https://github.com/EEnginE/engine
$ mkdir build
$ cd build
$ cmake <Additional CMake options> ..
$ make
$ make install
```

Alternatively, you can use ninja instead of make by adding `-G Ninja` to your cmake options.

## Windows

Install Git Bash from [here](http://git-scm.com/) and download the project with:

```sh
git clone https://github.com/EEnginE/engine
```

**OR**

Download the sourcecode from https://github.com/EEnginE/engine directly

Now install CMake and GIT.

You need at least Visual Studio [2015](https://www.visualstudio.com/en-us/downloads/visual-studio-2015-downloads-vs.aspx) to compile the project.
Using Mingw or Clang may also work.

# CMake options

## ENGINE

-   `-DCMAKE_INSTALL_PREFIX=/install/prefix`

-   `-DDISPLAY_SERVER=<which>`

    Possible options:
      - UNIX_X11: stable - default
      - WIN32_WINDOWS: stable
      - UNIX_WAYLAND: not yet supported
      - UNIX_MIR: not yet supported


-   `-DENGINE_LINK_SHARED=<0/1>`

    Link the engine:
      - 0: static
      - 1: shared - default


-   `-DEXTRA_CXX_FLAGS=<CXX compiler options>`

-   `-DSANITIZERS=<sanitizers>`

    Select sanitizers to use. Only works with clang.

-   `-DSANITIZER_OPTIONS=<options>`


-   `-DDEBUG_LOG_ALL=<0/1>`

    Enables / Disables all ALL additional debug messages. You can
    enable and disable those seperately with `-DLOG_<type>=<0/1>`.
    This will overwrite `-DDEBUG_LOG_ALL=<0/1>`

## GLEW

-   `-DUSE_SYSTEM_GLEW=<0/1>`

    Use the system GLEW version or build GLEW with the engine

# Troubleshooting


### CMake can not find GLEW

Try to set:
-    `-DUSE_SYSTEM_GLEW=OFF -DBUILD_GLEW=ON`

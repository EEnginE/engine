# - Find the OpenGL Extension Wrangler Library (GLEW)
#
#  GLEW_ROOT - where GLEW is installed
#
# This module defines the following variables:
#  GLEW_INCLUDE_DIRS - include directories for GLEW
#  GLEW_LIBRARIES - libraries to link against GLEW
#  GLEW_FOUND - true if GLEW has been found and can be used

#=============================================================================
# Copyright 2012 Benjamin Eikel
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# Daniel Mensinger: Added GLEW_ROOT support

find_path(GLEW_INCLUDE_DIR GL/glew.h PATHS ${GLEW_ROOT}/include )
find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s PATHS ${GLEW_ROOT}/lib PATH_SUFFIXES lib64)

set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
set(GLEW_LIBRARIES ${GLEW_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLEW REQUIRED_VARS GLEW_INCLUDE_DIR GLEW_LIBRARY)

mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY)

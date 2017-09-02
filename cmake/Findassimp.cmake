# Try to find Assimp : Open Asset Import Library
# This module defines the following variables:
#  ASSIMP_FOUND
#  ASSIMP_INCLUDE_DIRS
#  ASSIMP_LIBRARIES

if (WIN32)

	find_path(
		ASSIMP_INCLUDE_DIR
		NAMES assimp/version.h
		PATHS
			"$ENV{PROGRAMFILES}/Assimp/include"
			"${ASSIMP_ROOT_DIR}/include")

	find_library(
		ASSIMP_LIBRARY
		NAMES assimp
		PATHS
			"$ENV{PROGRAMFILES}/Assimp/lib/x64"
			"${ASSIMP_ROOT_DIR}/lib/x64")
else()

	find_path(
		ASSIMP_INCLUDE_DIR
		NAMES assimp/version.h
		PATHS
			/usr/include
			/usr/local/include
			/opt/local/include
            ${ASSIMP_ROOT_DIR}/include)

	find_library(
		ASSIMP_LIBRARY
		NAMES assimp
		PATHS
			/usr/lib64
			/usr/lib
			/usr/local/lib64
			/usr/local/lib
			/opt/local/lib
			${ASSIMP_ROOT_DIR}/lib)
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Assimp DEFAULT_MSG ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY)

if(ASSIMP_FOUND)
    set(ASSIMP_LIBRARIES    ${ASSIMP_LIBRARY})
    set(ASSIMP_INCLUDE_DIRS ${ASSIMP_INCLUDE_DIR})
endif()

mark_as_advanced(ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY)
# Try to find the GMP librairies
#
# GMP_ROOT        - The root install path of the GMP lib
#
# GMP_FOUND       - system has GMP lib
# GMP_INCLUDE_DIR - the GMP include directory
# GMP_LIBRARIES   - Libraries needed to use GMP

if (GMP_INCLUDE_DIR AND GMP_LIBRARIES)
                # Already in cache, be silent
                set(GMP_FIND_QUIETLY TRUE)
endif (GMP_INCLUDE_DIR AND GMP_LIBRARIES)

find_path(GMP_INCLUDE_DIR NAMES gmp.h PATHS ${GMP_ROOT}/include /usr/include /usr/local/include )
find_library(GMP_LIBRARIES NAMES gmp libgmp PATHS ${GMP_ROOT}/lib /usr/lib /usr/local/lib )
find_library(GMPXX_LIBRARIES NAMES gmpxx libgmpxx PATHS ${GMP_ROOT}/lib /usr/lib /usr/local/lib )

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GMP DEFAULT_MSG GMP_INCLUDE_DIR GMP_LIBRARIES)

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARIES)

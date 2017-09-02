# Try to find the Vulkan
# VULKAN_PREFIX       - The install location of the engine
# VULKAN_FOUND        - system has VULKAN lib
# VULKAN_INCLUDE_DIRS - the VULKAN include directory
# VULKAN_LIBRARIES    - Libraries needed to use VULKAN

if (VULKAN_INCLUDE_DIR AND VULKAN_LIBRARIES)
                # Already in cache, be silent
                set(VULKAN_FIND_QUIETLY TRUE)
endif (VULKAN_INCLUDE_DIR AND VULKAN_LIBRARIES)

if(WIN32)

 find_path(
  VULKAN_INCLUDE_DIR
  NAMES vulkan/vulkan.h
  PATHS
   "$ENV{VULKAN_SDK}/Include"
   "$ENV{VK_SDK_PATH}/Include"
 )

 find_library(
  VULKAN_LIBRARY
  NAMES vulkan-1
  PATHS
   "$ENV{VULKAN_SDK}/Lib"
   "$ENV{VK_SDK_PATH}/Lib"
 )

else()

find_path(
 VULKAN_INCLUDE_DIR
 NAMES vulkan.h
 PATH_SUFFIXES vulkan
 PATHS
  ${VULKAN_PREFIX}/include
  ${VULKAN_PREFIX}/include/vulkan
  ${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_INSTALL_PREFIX}/include/vulkan
  /usr/include
  /usr/include/vulkan
  /usr/local/include
  /usr/local/include/vulkan
)

find_library(
 VULKAN_LIBRARY
 NAMES vulkan
 PATHS
  ${VULKAN_PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib
  /usr/lib
  /usr/local/lib
)

endif()


set( VULKAN_LIBRARIES    ${VULKAN_LIBRARY} )
set( VULKAN_INCLUDE_DIRS ${VULKAN_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Vulkan DEFAULT_MSG VULKAN_INCLUDE_DIR VULKAN_LIBRARY)

mark_as_advanced(VULKAN_INCLUDE_DIR VULKAN_LIBRARY)

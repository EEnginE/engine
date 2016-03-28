cmake_minimum_required(VERSION 3.4)

project(engine)

message( STATUS "Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}\n" )


######################
# Set some variables #
######################

# Set the binary output directories
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake )

# Helper variables
set( ENGINE_LIB_ROOT    ${PROJECT_SOURCE_DIR} )
set( ENGINE_TEST_ROOT   ${PROJECT_SOURCE_DIR}/tests )
set( TEMPLATES_DIR      ${PROJECT_SOURCE_DIR}/cmake/templates )
set( COMPILER_TESTS_DIR ${PROJECT_SOURCE_DIR}/cmake/tests )

# Build options
option( ENGINE_LINK_SHARED  "Link the project shared (1/ON) or static (0/OFF)" ON )
option( DEBUG_LOG_ALL       "Enable all debug defines"                         OFF )

option( STD_FILESYSTEM_IS_EXPERIMENTAL "Use the experimental filesystem instead of std IF ENGINE_USE_BOOST is false" ON )

if( NOT DEFINED GLSL_TO_SPIRV_COMPILER )
   set( GLSL_TO_SPIRV_COMPILER "glslangValidator" )
endif( NOT DEFINED GLSL_TO_SPIRV_COMPILER )

if( NOT EXISTS "${PROJECT_SOURCE_DIR}/include" )
   file( MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/include" )
endif( NOT EXISTS "${PROJECT_SOURCE_DIR}/include" )

############################
# Add the uninstall target #
############################

configure_file("${TEMPLATES_DIR}/cmake_uninstall.cmake.in"
               "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

###################################################
# Load all modules in ${PROJECT_SOURCE_DIR}/cmake #
###################################################

file( GLOB MODULE_LIST ${PROJECT_SOURCE_DIR}/cmake/*.cmake )

foreach( I IN LISTS MODULE_LIST )
   include( ${I} )
endforeach( I IN LISTS MODULE_LIST )

add_platform( WIN32 windows )
add_platform( UNIX  x11 mir wayland )
check_platform()

run_git()
gen_log_macros( "${TEMPLATES_DIR}/uMacros.in.hpp" "${PROJECT_SOURCE_DIR}/utils/log/uMacros.hpp" "i" "w" "e" "d" )

add_compiler( GNU MIN_VERSION "5.3"
               ALL      "-Wall -Wextra -Wno-comment -std=c++14"
            #   DEBUG    "-Werror"
               SANITIZE "-fsanitize=${SANITIZERS}"
               RELEASE  "-O3" )

add_compiler( Clang
               ALL      "-fvisibility=hidden -std=c++14 -Weverything -ftemplate-backtrace-limit=0"
                        "-Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-float-equal"
                        "-Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-exit-time-destructors"
                        "-Wno-global-constructors -Wno-switch-enum -Wno-reserved-id-macro"
            #   DEBUG    "-Werror"
               SANITIZE "-fsanitize=${SANITIZERS}"
               RELEASE  "-O2" )

add_compiler( MSVC
               DEBUG   "/Od"
               RELEASE "/O2" )

################
# Dependencies #
################

if( ENGINE_LINK_SHARED )
   message( STATUS "Linking shared. => Do not change CMAKE_FIND_LIBRARY_SUFFIXES (${CMAKE_FIND_LIBRARY_SUFFIXES})" )
else( ENGINE_LINK_SHARED )
   set( OLD_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} )
   set( CMAKE_FIND_LIBRARY_SUFFIXES .a .lib .so .dll ${CMAKE_FIND_LIBRARY_SUFFIXES} )
   message( STATUS "Linking static. => Changed CMAKE_FIND_LIBRARY_SUFFIXES to ${CMAKE_FIND_LIBRARY_SUFFIXES}" )
endif( ENGINE_LINK_SHARED )

if( STD_FILESYSTEM_IS_EXPERIMENTAL )
   set( CM_FILESYSTEM_NAMESPACE "std::experimental::filesystem" )
   set( CM_FILESYSTEM_INCLUDE   "<experimental/filesystem>" )

   if( CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL asd )
      set( FS_LIBRARIES stdc++fs )
   endif( CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL asd )
else( STD_FILESYSTEM_IS_EXPERIMENTAL )
   set( CM_FILESYSTEM_NAMESPACE "std::filesystem" )
   set( CM_FILESYSTEM_INCLUDE   "<filesystem>" )
endif( STD_FILESYSTEM_IS_EXPERIMENTAL )

find_package(Threads REQUIRED)
find_package(Vulkan  REQUIRED)

if( CM_UNIX_X11 )
   find_package(X11 REQUIRED)

   if( X11_Xrandr_FOUND )
      message( STATUS "Found Xrandr" )
   else( X11_Xrandr_FOUND )
      message( SEND_ERROR "Xrandr NOT FOUND!" )
   endif( X11_Xrandr_FOUND )
endif( CM_UNIX_X11 )

set( ENGINE_LINK ${VULKAN_LIBRARIES}   ${X11_LIBRARIES}    ${X11_Xrandr_LIB} ${CMAKE_THREAD_LIBS_INIT} ${FS_LIBRARIES} )
set( ENGINE_INCL ${VULKAN_INCLUDE_DIR} ${X11_INCLUDE_DIRS} ${X11_Xrandr_INCLUDE_PATH} )

message( "" ) # Newline

##################
# Config Project #
##################

# Generate dummy files
file( WRITE "${PROJECT_SOURCE_DIR}/utils/uEnum2Str.hpp" "" )
file( WRITE "${PROJECT_SOURCE_DIR}/utils/uEnum2Str.cpp" "" )

add_engine_lib( utils )
add_engine_lib( init   utils )
add_engine_lib( loader utils )
add_engine_lib( render utils init loader )

add_engine_test( oglTest )
add_engine_test( test1 )
add_engine_test( benchmarks )

generate_engine_hpp( ${TEMPLATES_DIR}/engine.in.hpp ${PROJECT_SOURCE_DIR}/include/engine.hpp )
generate_format_command( format 3.7.1 ${TEMPLATES_DIR}/cmake_format.cmake.in ${PROJECT_BINARY_DIR}/cmake_format.cmake )
generate_debug_defines()

include_directories( ${ENGINE_INCLUDE_DIRECTORIES} ${PROJECT_SOURCE_DIR}/include ${ENGINE_INCL} )

configure_file( "${TEMPLATES_DIR}/defines.in.hpp"      "${PROJECT_SOURCE_DIR}/include/defines.hpp" )
configure_file( "${TEMPLATES_DIR}/Doxyfile.in"         "${PROJECT_SOURCE_DIR}/Doxyfile" )
configure_file( "${TEMPLATES_DIR}/FindEngine.cmake.in" "${PROJECT_SOURCE_DIR}/include/FindEngine.cmake" @ONLY )

enum2str_generate(
   PATH "${PROJECT_SOURCE_DIR}/utils"
   CLASS_NAME "uEnum2Str"
   NAMESPACE "e_engine"
   FUNC_NAME "toStr"
   INCLUDES "defines.hpp" "vulkan/vulkan.h" "lGLTF_map.hpp" "rObjectBase.hpp"
   ENUMS
      # EEnginE
      "LOG_COLOR_TYPE" "LOG_PRINT_TYPE" "WINDOW_ATTRIBUTE" "WINDOW_TYPE" "ACTION" "MESH_TYPES" "E_BUTTON"
      "glTF::lGLTF_map::ELEMENTS" "rObjectBase::MATRIX_TYPES" "rObjectBase::VECTOR_TYPES" "rObjectBase::LIGHT_MODEL_T"
      "rObjectBase::OBJECT_HINTS" "rObjectBase::ERROR_FLAGS"
      # Vulkan
      "VkPipelineCacheHeaderVersion"
      "VkResult" "VkStructureType" "VkSystemAllocationScope" "VkInternalAllocationType"
      "VkFormat" "VkImageType" "VkImageTiling" "VkPhysicalDeviceType" "VkQueryType"
      "VkSharingMode" "VkImageLayout" "VkImageViewType" "VkComponentSwizzle" "VkVertexInputRate"
      "VkPrimitiveTopology" "VkPolygonMode" "VkFrontFace" "VkCompareOp" "VkStencilOp"
      "VkLogicOp" "VkBlendFactor" "VkBlendOp" "VkDynamicState" "VkFilter" "VkSamplerMipmapMode"
      "VkSamplerAddressMode" "VkBorderColor" "VkDescriptorType" "VkAttachmentLoadOp"
      "VkAttachmentStoreOp" "VkPipelineBindPoint" "VkCommandBufferLevel" "VkIndexType"
      "VkSubpassContents" "VkFormatFeatureFlagBits" "VkImageUsageFlagBits" "VkImageCreateFlagBits"
      "VkSampleCountFlagBits" "VkQueueFlagBits" "VkMemoryPropertyFlagBits" "VkMemoryHeapFlagBits"
      "VkPipelineStageFlagBits" "VkImageAspectFlagBits" "VkSparseImageFormatFlagBits"
      "VkSparseMemoryBindFlagBits" "VkFenceCreateFlagBits" "VkQueryPipelineStatisticFlagBits"
      "VkQueryResultFlagBits" "VkBufferCreateFlagBits" "VkBufferUsageFlagBits" "VkPipelineCreateFlagBits"
      "VkShaderStageFlagBits" "VkCullModeFlagBits" "VkColorComponentFlagBits" "VkDescriptorPoolCreateFlagBits"
      "VkAttachmentDescriptionFlagBits" "VkAccessFlagBits" "VkDependencyFlagBits"
      "VkCommandPoolCreateFlagBits" "VkCommandPoolResetFlagBits" "VkCommandBufferUsageFlagBits"
      "VkQueryControlFlagBits" "VkCommandBufferResetFlagBits" "VkStencilFaceFlagBits"
      # Vulkan Surface
      "VkColorSpaceKHR" "VkPresentModeKHR" "VkSurfaceTransformFlagBitsKHR" "VkCompositeAlphaFlagBitsKHR"
      "VkDisplayPlaneAlphaFlagBitsKHR"
      # Vulkan etc
      "VkDebugReportObjectTypeEXT" "VkDebugReportErrorEXT" "VkDebugReportFlagBitsEXT"

   BLACKLIST "VK_SAMPLER_ADDRESS_MODE_RANGE_SIZE" "VK_PIPELINE_CACHE_HEADER_VERSION_RANGE_SIZE"
   USE_CONSTEXPR OFF
)

set(DEPENDENCES "${PROJECT_SOURCE_DIR}/include/FindEngine.cmake" )
set(OTHER_INC   "${PROJECT_SOURCE_DIR}/include/engine.hpp" "${PROJECT_SOURCE_DIR}/include/defines.hpp" )

install( FILES ${DEPENDENCES} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/engine)
install( FILES ${OTHER_INC}   DESTINATION ${CMAKE_INSTALL_PREFIX}/include/engine)


# Add all libs and tests directories
foreach( I IN LISTS ENGINE_SUBDIRECTORIES )
   add_subdirectory( ${I} )
endforeach( I IN LISTS ENGINE_SUBDIRECTORIES )

# Not clearing CM_OGLTEST_BINDINGS_LIST results in cache error
set( CM_OGLTEST_BINDINGS_LIST "" CACHE STRING "List of all ogl test bindings" FORCE )


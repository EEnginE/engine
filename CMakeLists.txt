cmake_minimum_required(VERSION 3.8)

project(EEnginE)

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
option( DEBUG_LOG_ALL       "Enable all debug defines"                         OFF )
option( OVERRIDE_VK_NULL    "Overrides the VK_NULL_HADNLE with nullptr"        ON )

set( CMAKE_CXX_STANDARD               17 )
set( CMAKE_CXX_STANDARD_REQUIRED      ON )
set( CMAKE_C_STANDARD                 11 )
set( CMAKE_C_STANDARD_REQUIRED        ON )
set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )

if( NOT DEFINED GLSL_TO_SPIRV_COMPILER )
   set( GLSL_TO_SPIRV_COMPILER "glslangValidator" )
endif( NOT DEFINED GLSL_TO_SPIRV_COMPILER )

if(OVERRIDE_VK_NULL)
  set(CM_OVERRIDE_VK_NULL_HANDLE 1)
else()
  set(CM_OVERRIDE_VK_NULL_HANDLE 0)
endif()

if( NOT EXISTS "${PROJECT_SOURCE_DIR}/include" )
   file( MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/include" )
endif( NOT EXISTS "${PROJECT_SOURCE_DIR}/include" )

############################
# Add the uninstall target #
############################

include( cmake/cmakeBuildTools/cmakeBuildTools.cmake )

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

add_platform( OS WIN32 TARGET windows )
add_platform( OS UNIX  TARGET x11 mir wayland )
check_platform()

run_git()
gen_log_macros( "${TEMPLATES_DIR}/uMacros.in.hpp" "${PROJECT_SOURCE_DIR}/utils/log/uMacros.hpp" "i" "w" "e" "d" )

add_compiler( GNU MIN_VERSION "5.3"
              ALL      "-Wall -Wextra -Wno-comment -Wfloat-equal -Wshadow -Wno-unknown-pragmas -fdiagnostics-color=always"
              DEBUG    "-O0 -Werror"
              SANITIZE "-fsanitize=${SANITIZERS}"
              RELEASE  "-O3" )

add_compiler( Clang
              ALL      "-Weverything -ftemplate-backtrace-limit=0 -fcolor-diagnostics"
                       "-Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-float-equal"
                       "-Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-exit-time-destructors"
                       "-Wno-global-constructors -Wno-switch-enum -Wno-reserved-id-macro"
                       "-Wno-documentation-unknown-command -Wno-unused-macros -Wno-weak-vtables"
                       "-Wno-double-promotion -Wno-undefined-func-template"
              DEBUG    "-Werror"
              SANITIZE "-fsanitize=${SANITIZERS}"
              RELEASE  "-O2" )

add_compiler( MSVC
              DEBUG   "/Od"
              RELEASE "/O2" )

set( DEBUG_LOGGING 0 )

if( ENGINE_VERBOSE OR "${CMAKE_BUILD_TYPE}" MATCHES "DEBUG" )
   set( DEBUG_LOGGING 1 )
endif( ENGINE_VERBOSE OR "${CMAKE_BUILD_TYPE}" MATCHES "DEBUG" )

################
# Dependencies #
################

find_package(Threads REQUIRED)
find_package(Vulkan  REQUIRED)
find_package(assimp  REQUIRED)
find_package(glm     REQUIRED)

if( CM_UNIX_X11 )
  find_package(XCB COMPONENTS xcb xcb-randr REQUIRED)
endif( CM_UNIX_X11 )


set( ENGINE_LINK ${VULKAN_LIBRARIES} ${XCB_LIBRARIES} ${ASSIMP_LIBRARIES} Threads::Threads stdc++fs)

message( "" ) # Newline

##################
# Config Project #
##################

# Generate dummy files
file( WRITE "${PROJECT_SOURCE_DIR}/utils/uEnum2Str.hpp" "" )
file( WRITE "${PROJECT_SOURCE_DIR}/utils/uEnum2Str.cpp" "" )

new_project_library(
  PATH         "${PROJECT_SOURCE_DIR}/utils"
  NAME         "utils"
  TEMPLATE     "${TEMPLATES_DIR}/CMakeLists.lib.in"
  DEPENDENCIES
)

new_project_library(
  PATH         "${PROJECT_SOURCE_DIR}/vulkanUtils"
  NAME         "vulkanUtils"
  TEMPLATE     "${TEMPLATES_DIR}/CMakeLists.lib.in"
  DEPENDENCIES utils_shared
)

new_project_library(
  PATH         "${PROJECT_SOURCE_DIR}/init"
  NAME         "init"
  TEMPLATE     "${TEMPLATES_DIR}/CMakeLists.lib.in"
  DEPENDENCIES utils_shared vulkanUtils_shared
)

new_project_library(
  PATH         "${PROJECT_SOURCE_DIR}/render"
  NAME         "render"
  TEMPLATE     "${TEMPLATES_DIR}/CMakeLists.lib.in"
  DEPENDENCIES utils_shared init_shared vulkanUtils_shared glm
)

generate_engine_hpp( "${TEMPLATES_DIR}/engine.in.hpp" "${PROJECT_SOURCE_DIR}/include/engine.hpp" )

new_project_executable(
  PATH         "${PROJECT_SOURCE_DIR}/tests/test1"
  NAME         "test1"
  TEMPLATE     "${TEMPLATES_DIR}/CMakeLists.test.in"
  DEPENDENCIES utils_shared init_shared render_shared glm
)

new_project_executable(
  PATH         "${PROJECT_SOURCE_DIR}/tests/benchmarks"
  NAME         "benchmarks"
  TEMPLATE     "${TEMPLATES_DIR}/CMakeLists.test.in"
  DEPENDENCIES utils_shared init_shared render_shared glm
)

generate_format_command( format 4.0.0 )
generate_debug_defines()

include_directories(
  ${${PROJECT_NAME}_LIB_INCLUDE_DIRECTORIES}
  ${${PROJECT_NAME}_EXE_INCLUDE_DIRECTORIES}
  ${PROJECT_SOURCE_DIR}/include
)

include_directories(
  SYSTEM
    ${VULKAN_INCLUDE_DIR}
    ${XCB_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/external/plf_colony
)

configure_file( "${TEMPLATES_DIR}/defines.in.hpp"      "${PROJECT_SOURCE_DIR}/include/defines.hpp" )
configure_file( "${TEMPLATES_DIR}/Doxyfile.in"         "${PROJECT_SOURCE_DIR}/Doxyfile" )
configure_file( "${TEMPLATES_DIR}/FindEngine.cmake.in" "${PROJECT_SOURCE_DIR}/include/FindEngine.cmake" @ONLY )

enum2str_generate(
  PATH "${PROJECT_SOURCE_DIR}/utils"
  CLASS_NAME      "uEnum2Str"
  NAMESPACE       "e_engine"
  FUNC_NAME       "toStr"
  INDENT_STR      "  "
  BITFIELD_TYPE   uint32_t
  CONCATINATE_STR " | "
  ENABLE_BITFIELDS
  INCLUDES
    "defines.hpp" "vulkan.h"
    "rObjectBase.hpp" "rShaderBase.hpp"
    "iRandRBasic.hpp" "iWindowBasic.hpp" "iInit.hpp"
  ENUMS
    # EEnginE
    "LOG_COLOR_TYPE" "LOG_PRINT_TYPE" "WINDOW_ATTRIBUTE" "WINDOW_TYPE" "ACTION" "MESH_TYPES" "E_BUTTON"
    "rObjectBase::MATRIX_TYPES" "rObjectBase::VECTOR_TYPES" "rObjectBase::LIGHT_MODEL_T"
    "rObjectBase::ERROR_FLAGS" "rObjectBase::VERTEX_DATA_LAYOUT" "rShaderBase::UNIFORM_ROLE"
    "internal::SHADER_INPUT_NAME_INDEX"

    # EEnginE error codes
    "iRandRBasic::ErrorCode" "iWindowBasic::ErrorCode" "iInit::ErrorCode"

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
    "VkQueryResultFlagBits" "VkBufferCreateFlagBits" "VkBufferUsageFlagBits"
    "VkPipelineCreateFlagBits" "VkShaderStageFlagBits" "VkCullModeFlagBits" "VkObjectType"
    "VkColorComponentFlagBits" "VkDescriptorPoolCreateFlagBits" "VkAttachmentDescriptionFlagBits"
    "VkSubpassDescriptionFlagBits" "VkAccessFlagBits"  "VkDependencyFlagBits"
    "VkCommandPoolCreateFlagBits" "VkCommandPoolResetFlagBits" "VkCommandBufferUsageFlagBits"
    "VkQueryControlFlagBits" "VkCommandBufferResetFlagBits" "VkStencilFaceFlagBits"
    # Vulkan Surface
    "VkColorSpaceKHR" "VkPresentModeKHR" "VkSurfaceTransformFlagBitsKHR"
    "VkCompositeAlphaFlagBitsKHR" "VkDisplayPlaneAlphaFlagBitsKHR"
    "VkSwapchainCreateFlagBitsKHR"
    # Vulkan etc
    "VkDescriptorUpdateTemplateTypeKHR" "VkSamplerReductionModeEXT"
    "VkDebugReportObjectTypeEXT" "VkDebugReportFlagBitsEXT" "VkBlendOverlapEXT"

    # AMD
    "VkRasterizationOrderAMD"
    # Nvidia
    "VkExternalMemoryHandleTypeFlagBitsNV" "VkExternalMemoryFeatureFlagBitsNV"
    "VkIndirectCommandsTokenTypeNVX" "VkObjectEntryTypeNVX"
    "VkIndirectCommandsLayoutUsageFlagBitsNVX" "VkObjectEntryUsageFlagBitsNVX"
    "VkViewportCoordinateSwizzleNV" "VkCoverageModulationModeNV"

    # KHX
    "VkPeerMemoryFeatureFlagBitsKHX" "VkMemoryAllocateFlagBitsKHX"
    "VkDeviceGroupPresentModeFlagBitsKHX" "VkValidationCheckEXT"
    "VkSurfaceCounterFlagBitsEXT" "VkDisplayPowerStateEXT" "VkDeviceEventTypeEXT"
    "VkDisplayEventTypeEXT" "VkDiscardRectangleModeEXT"

    "VkExternalFenceFeatureFlagBitsKHR" "VkExternalFenceHandleTypeFlagBitsKHR" "VkExternalMemoryFeatureFlagBitsKHR"
    "VkExternalMemoryHandleTypeFlagBitsKHR" "VkExternalSemaphoreFeatureFlagBitsKHR"
    "VkExternalSemaphoreHandleTypeFlagBitsKHR" "VkFenceImportFlagBitsKHR" "VkSemaphoreImportFlagBitsKHR"

  BLACKLIST "VK_SAMPLER_ADDRESS_MODE_RANGE_SIZE" "VK_PIPELINE_CACHE_HEADER_VERSION_RANGE_SIZE"
)

set(DEPENDENCES "${PROJECT_SOURCE_DIR}/include/FindEngine.cmake" )
set(OTHER_INC   "${PROJECT_SOURCE_DIR}/include/engine.hpp" "${PROJECT_SOURCE_DIR}/include/defines.hpp" )

install( FILES ${DEPENDENCES} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/engine)
install( FILES ${OTHER_INC}   DESTINATION ${CMAKE_INSTALL_PREFIX}/include/engine)


# Add all libs and tests directories
foreach( I IN LISTS EEnginE_SUBDIR_LIST )
  add_subdirectory( "${I}" )
endforeach( I IN LISTS EEnginE_SUBDIR_LIST )

# Not clearing CM_OGLTEST_BINDINGS_LIST results in cache error
set( CM_OGLTEST_BINDINGS_LIST "" CACHE STRING "List of all ogl test bindings" FORCE )


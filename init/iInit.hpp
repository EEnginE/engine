/*!
 * \file iInit.hpp
 * \brief \b Classes: \a iInit
 * \sa e_iInit.cpp e_event.cpp
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

#if UNIX_X11 || UNIX_WAYLAND
#include "iInitSignals.hpp"
#include "iMouse.hpp"
#include "iWindowBasic.hpp"
#endif

#if UNIX_X11
#include "x11/iWindow.hpp"

#elif UNIX_WAYLAND
#include "wayland/iWindow.hpp"

#elif WINDOWS
#include "windows/iContext.hpp"

#else
#error "PLATFORM not supported"
#endif

#include <thread>
#include <unordered_map>


namespace e_engine {

typedef void (*RENDER_FUNC)(iEventInfo info);


/*!
 * \class e_engine::iInit
 * \brief Init the <b>E Engine</b>
 *
 * This class handels the context with the iContext class. It
 * sets the Crt-C handle function and starts the main and eventLoop
 * loop. This should be after uConfig the 1st class you generate
 * and you will do all window and curser config with it.
 *
 * \note Until now there is no MS Windows or Mac support
 *
 * \note Cursor functions are not implemented yet
 *
 * \sa iContext uConfig e_iInit.cpp e_event.cpp
 */
#if UNIX_X11 || UNIX_WAYLAND
class iInit : public iInitSignals, public iMouse {
#elif WINDOWS
class iInit : public windows_win32::iContext {
#else
#error "PLATFORM not supported"
#endif
 public:
  SLOT vGrabControl_SLOT; //!< Slot for grab control \sa iInit::s_advancedGrabControl

  typedef struct PhysicalDevice_vk {
    VkPhysicalDevice                     device;
    VkPhysicalDeviceProperties           properties;
    VkPhysicalDeviceFeatures             features;
    VkPhysicalDeviceMemoryProperties     memoryProperties;
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    VkFormatProperties                   formats[VK_FORMAT_RANGE_SIZE];
  } PhysicalDevice_vk;

  typedef struct Device_vk {
    PhysicalDevice_vk *pDevice = nullptr;
    VkDevice           device  = NULL;
  } Device_vk;

  typedef struct Queue_vk {
    VkQueue      queue;
    float        priority;
    VkQueueFlags flags;
    u_int32_t    familyIndex;
    u_int32_t    index;
    bool         surfaceSupport;

    Queue_vk(float _priority, VkQueueFlags _flags, u_int32_t _familyIndex, u_int32_t _index, bool _surfaceSupport)
        : priority(_priority),
          flags(_flags),
          familyIndex(_familyIndex),
          index(_index),
          surfaceSupport(_surfaceSupport) {}
  } Queue_vk;

  typedef struct SurfaceInfo_vk {
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModels;
    VkSurfaceCapabilitiesKHR        surfaceInfo;
  } SurfaceInfo_vk;

 private:
#if UNIX_X11
  unix_x11::iWindow vWindow;
#elif UNIX_WAYLAND
  unix_wayland::iWindow vWindow;
#endif

  std::vector<std::string> vExtensionList;
  std::vector<std::string> vDeviceExtensionList;
  std::vector<std::string> vExtensionsToUse;
  std::vector<std::string> vDeviceExtensionsToUse;
  std::vector<std::string> vLayersToUse;
  std::vector<std::string> vDeviceLayersToUse;

  std::vector<VkLayerProperties> vLayerProperties_vk;
  std::vector<VkLayerProperties> vDeviceLayerProperties_vk;
  std::vector<PhysicalDevice_vk> vPhysicalDevices_vk;
  std::vector<Queue_vk>          vQueues_vk;

  std::unordered_map<VkQueue, std::mutex> vQueueMutexMap;
  std::mutex vQueueAccessMutex;

  VkDebugReportCallbackCreateInfoEXT vDebugCreateInfo_vk;

  VkDebugReportCallbackEXT vCallback    = nullptr;
  VkInstance               vInstance_vk = nullptr;
  VkSurfaceKHR             vSurface_vk  = nullptr;

  SurfaceInfo_vk vSurfaceInfo_vk;

  Device_vk vDevice_vk;

  bool vMainLoopRunning_B      = false; //!< Should the main loop be running?
  bool vEventLoopHasFinished_B = true;  //!< Has the event loop finished?

  std::thread vEventLoop_BT; //!< The thread for the event loop

  bool vWasMouseGrabbed_B               = false;
  bool vAreRenderLoopSignalsConnected_B = false;
  bool vIsVulkanSetup_B                 = false;
  bool vEnableVulkanDebug               = false;
  bool vEnableVSync                     = false;
  int  vCreateWindowReturn_I            = -1000;

#if WINDOWS
  std::mutex              vCreateWindowMutex_BT;
  std::condition_variable vCreateWindowCondition_BT;

  std::mutex              vStartEventMutex_BT;
  std::condition_variable vStartEventCondition_BT;

  std::mutex              vStopEventLoopMutex;
  std::condition_variable vStopEventLoopCondition;

  bool vContinueWithEventLoop_B;
#endif

  PhysicalDevice_vk *chooseDevice();

  std::vector<VkExtensionProperties> getExtProprs(std::string _layerName);
  std::vector<VkExtensionProperties> getDeviceExtProprs(std::string _layerName, VkPhysicalDevice _dev);
  int loadExtensionList();
  int loadDeviceExtensionList(VkPhysicalDevice _dev);
  int loadDevices();
  int loadDeviceSurfaceInfo();
  int createDevice(std::vector<std::string> _layers);
  int initVulkan(std::vector<std::string> _layers);
  int initDebug();

  void destroyVulkan();

  // Thread Functions --------------------------------------------------------- ###
  int eventLoop(); //!< The event loop function ( In PLATFORM/e_event.cpp )

  // Signal handling ---------------------------------------------------------- ###
  static void handleSignal(int _signal); //!< The signal handle function
  void _setThisForHandluSignal();        //!< Unfortunately you cannot pass this with signal()

  // Standard callbacks NEW --------------------------------------------------- ###

  void s_advancedGrabControl(iEventInfo const &_info);

  SurfaceInfo_vk getSurfaceInfo();

  virtual void makeEInitEventBasicAbstract() {}

 public:
  iInit();
  virtual ~iInit();

  int init(std::vector<std::string> _layers = {});
  int handleResize();
  int shutdown();
  int startMainLoop(bool _wait = true);

  void quitMainLoop();

  bool enableDefaultGrabControl();
  bool disableDefaultGrabControl();

  void enableVSync();
  void disableVSync();

  iWindowBasic *getWindow();
  void          closeWindow();

  uint32_t getQueueFamily(VkQueueFlags _flags);

  VkQueue getQueue(VkQueueFlags _flags, float _priority, uint32_t *_queueFamily = nullptr);
  std::mutex &getQueueMutex(VkQueue _queue);

  VkDevice getDevice();

  bool isExtensionSupported(std::string _extension);
  bool isDeviceExtensionSupported(std::string _extension);

  uint32_t getMemoryTypeIndexFromBitfield(uint32_t _bits, VkMemoryHeapFlags _flags = 0);

  bool isFormatSupported(VkFormat _format);
  bool formatSupportsFeature(VkFormat _format, VkFormatFeatureFlagBits _flags, VkImageTiling _type);

  void enableVulkanDebug() { vEnableVulkanDebug = true; }
  void vulkanDebugHandler(VkDebugReportFlagsEXT      _flags,
                          VkDebugReportObjectTypeEXT _objType,
                          uint64_t                   _obj,
                          size_t                     _location,
                          int32_t                    _msgCode,
                          std::string                _layerPrefix,
                          std::string                _msg);

  VkSurfaceKHR getVulkanSurface();

  friend class rWorld;
};

namespace internal {

/*!
 * \class __iInit_Pointer
 * \brief Need this for the static signal Callback
 *
 * \todo Remove this workaround / hack
 *
 */
class __iInit_Pointer {
 private:
  iInit *pointer;
  bool   is_set;

 public:
  __iInit_Pointer() {
    pointer = 0;
    is_set  = false;
  }
  ~__iInit_Pointer() { pointer = 0; }
  bool set(iInit *_THIS) {
    if (is_set == true) { return false; }
    pointer = _THIS;
    is_set  = true;
    return true;
  }
  iInit *get() { return pointer; }
};
extern __iInit_Pointer __iInit_Pointer_OBJ;
}
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;

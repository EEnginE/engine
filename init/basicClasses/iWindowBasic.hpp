/*!
 * \file iWindowBasic.hpp
 *
 * Basic class for different wm.
 *
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

#include <string>
#include <vulkan.h>


namespace e_engine {

/*!
 * \class e_engine::iWindowBasic
 * \brief The wm basic window class
 *
 * This class provides an interface to create a
 * connection to a wm and create a window.
 */
class INIT_API iWindowBasic {

 public:
   iWindowBasic()          = default;
   virtual ~iWindowBasic() = default;

   virtual int  createWindow()  = 0;
   virtual void destroyWindow() = 0;

   virtual void changeWindowConfig( unsigned int _width,
                                    unsigned int _height,
                                    int          _posX,
                                    int          _posY ) = 0;

   virtual void setWindowType( WINDOW_TYPE _type ) = 0;
   virtual void setWindowNames( std::string _windowName, std::string _iconName = "<NONE>" ) = 0;
   virtual void setAttribute( ACTION           _action,
                              WINDOW_ATTRIBUTE _type1,
                              WINDOW_ATTRIBUTE _type2 = NONE ) = 0;

   virtual void fullScreen( ACTION _action, bool _allMonitors = false ) = 0;
   virtual void maximize( ACTION _action )      = 0;
   virtual void setDecoration( ACTION _action ) = 0;

   virtual bool grabMouse()               = 0;
   virtual void freeMouse()               = 0;
   virtual bool getIsMouseGrabbed() const = 0;

   virtual void moveMouse( unsigned int _posX, unsigned int _posY ) = 0;

   virtual void hideMouseCursor()          = 0;
   virtual void showMouseCursor()          = 0;
   virtual bool getIsCursorHidden() const  = 0;
   virtual bool getIsWindowCreated() const = 0;

   virtual VkSurfaceKHR getVulkanSurface( VkInstance _instance ) = 0;
};

} // e_engine


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;

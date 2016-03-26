/*!
 * \file rScene.cpp
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

#include "rScene.hpp"
#include "uLog.hpp"

namespace e_engine {


rSceneBase::~rSceneBase() {}

/*!
 * \brief Tests if it is safe to render the scene
 * \returns true when it is safe
 */
bool rSceneBase::canRenderScene() {
   bool lCanRender = true;
   for ( auto const &d : vObjects ) {
      int64_t lIsObjectReady, lFlags;

      if ( !d ) {
         wLOG( "Invalid Object Pointer" );
         lCanRender = false;
         continue;
      }

      d->getHints( rObjectBase::IS_DATA_READY, lIsObjectReady, rObjectBase::FLAGS, lFlags );

      if ( lIsObjectReady != true ) {
         wLOG( "Object data for '",
               d->getName(),
               "' is not completely loaded --> Do not render scene '",
               vName_str,
               "'" );
         lCanRender = false;
         continue;
      }

      //! \todo Add vulkan stuff

      if ( lCanRender )
         iLOG( "Scene '", vName_str, "' with ", vObjects.size(), " objects ready for rendering" );

      return lCanRender;
   }
   wLOG( "No objects in scene" );
   return false;
}

/*!
 * \brief Adds an object to render
 *
 * \todo Implement functions to remove / disable / enable objects
*
 * \param[in] _obj Pointer to an object
 *
 * \returns The Index of the object
 */
unsigned rSceneBase::addObject( e_engine::rObjectBase *_obj ) {
   std::lock_guard<std::mutex> lLockObjects( vObjects_MUT );

   vObjects.emplace_back( _obj );

   int64_t lFlags;

   _obj->getHints( rObjectBase::FLAGS, lFlags );
   if ( lFlags & LIGHT_SOURCE )
      vLightSourcesIndex.emplace_back( vObjects.size() - 1 );

   return static_cast<unsigned>( vObjects.size() - 1 );
}

std::vector<rObjectBase *> rSceneBase::getObjects() { return vObjects; }
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;

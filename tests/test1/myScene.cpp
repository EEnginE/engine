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

#include "myScene.hpp"

using namespace e_engine;

int myScene::init() {
   // updateCamera();

   vPipeline.setDynamicViewports( 1 )->setDynamicScissors( 1 )->enableDepthTest()->enableCulling();
   vPipeline.setShader( &vShader );

   beginInitObject();

   auto lNames = loadFile( vFilePath );
   for ( auto const &i : lNames ) {
      if ( i.type != MESH_3D )
         continue;

      vObjects.emplace_back( std::make_shared<rSimpleMesh>( this, i.name ) );

      initObject( vObjects.back(), i.index );
      vObjects.back()->setPosition( rVec3f( 0, 0, -5 ) );
   }

   endInitObject();

   //    vLight1.setPosition( rVec3f( 1, 1, -4 ) );
   //    vLight2.setPosition( rVec3f( -1, -1, -4 ) );
   //
   //    vLight1.setColor( rVec3f( 1.0f, 0.2f, 0.2f ), rVec3f( 0.1f, 0.0f, 0.0f ) );
   //    vLight2.setColor( rVec3f( 0.2f, 0.2f, 1.0f ), rVec3f( 0.0, 0.0f, 0.1f ) );
   //    vLight3.setColor( rVec3f( 0.9f, 0.9f, 0.9f ), rVec3f( 0.05f, 0.05f, 0.05f ) );
   //
   //    vLight1.setAttenuation( 0.1f, 0.01f, 0.1f );
   //    vLight2.setAttenuation( 0.1f, 0.02f, 0.2f );


   for ( auto &i : vObjects ) {
      i->setPipeline( &vPipeline );
      addObject( i );
   }

   //    addObject( &vLight1 );
   //    addObject( &vLight2 );
   //    addObject( &vLight3 );

   if ( !canRenderScene() ) {
      eLOG( "Cannot render scene!" );
      return 2;
   }

   return 0;
}

void myScene::keySlot( const iEventInfo &_inf ) {
   if ( _inf.eKey.state != E_PRESSED )
      return;

   for ( auto &i : vObjects ) {
      switch ( _inf.eKey.key ) {
         case L'z':
            vRotationAngle += 0.25;
            i->setRotation( rVec3f( 0, 1, 0 ), vRotationAngle );
            break;
         case L't':
            vRotationAngle -= 0.25;
            i->setRotation( rVec3f( 0, 1, 0 ), vRotationAngle );
            break;
      }
   }
}


void myScene::afterCameraUpdate() {
   for ( auto &i : vObjects )
      i->updateFinalMatrix();

   //    vLight1.updateFinalMatrix();
   //    vLight2.updateFinalMatrix();
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;

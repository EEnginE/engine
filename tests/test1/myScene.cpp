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
#include "rRenderVertexNormal_3_3.hpp"

using namespace e_engine;

int myScene::init() {
   updateCamera();

   vObject1.loadData();
   vObject1.setOGLData();
   vObject1.setPosition( rVec3f( 0, 0, -5 ) );

   vLight1.setPosition( rVec3f( 1, 1, -4 ) );
   vLight2.setPosition( rVec3f( -1, -1, -4 ) );

   vLight1.setColor( rVec3f( 1.0f, 0.2f, 0.2f ), rVec3f( 0.1f, 0.0f, 0.0f ) );
   vLight2.setColor( rVec3f( 0.2f, 0.2f, 1.0f ), rVec3f( 0.0, 0.0f, 0.1f ) );
   vLight3.setColor( rVec3f( 0.9f, 0.9f, 0.9f ), rVec3f( 0.05f, 0.05f, 0.05f ) );

   vLight1.setAttenuation( 0.1f, 0.01f, 0.1f );
   vLight2.setAttenuation( 0.1f, 0.02f, 0.2f );

   uint64_t lLight;
   vObject1.getHints( rObjectBase::LIGHT_MODEL, lLight );
   if ( lLight != rObjectBase::SIMPLE_ADS_LIGHT ) {
      wLOG( "Light not supported! Normals missing!" );
      vRenderNormals = false;
   }

   GLint lShaderID = addShader( vShader_str ), lNormalShader = -1;

   if ( vRenderNormals )
      lNormalShader = addShader( vNormalShader_str );

   if ( !compileShaders() ) {
      eLOG( "Failed to compile the shaders" );
      return 5;
   }

   parseShaders();

   addObject( &vLight1, -1 );
   addObject( &vLight2, -1 );
   addObject( &vLight3, -1 );

   auto lObjID = addObject( &vObject1, lShaderID );
   auto lRet = setObjectRenderer<rRenderMultipleLights_3_3>( lObjID );

   switch ( lRet ) {
      case 0:
         iLOG( "setObjectRenderer(): DONE" );
         break;
      case 1:
         eLOG( "setObjectRenderer(): Index out of range Error" );
         return 10;
      case 2:
         eLOG( "setObjectRenderer(): Invalid object Error" );
         return 10;
      case 3:
         eLOG( "setObjectRenderer(): Invalid shader Error" );
         return 10;
      case 4:
         eLOG( "setObjectRenderer(): No Renderer found Error" );
         return 10;
      default:
         eLOG( "setObjectRenderer(): UNKNOWN Error" );
         return 10;
   }

   if ( vRenderNormals ) {
      setObjectRenderer<rRenderVertexNormal_3_3>( addObject( &vObject1, lNormalShader ) );
   }

   if ( !canRenderScene() ) {
      eLOG( "Cannot render scene!" );
      return 2;
   }

   return 0;
}

void myScene::keySlot( const iEventInfo &_inf ) {
   if ( _inf.eKey.state != E_PRESSED )
      return;

   switch ( _inf.eKey.key ) {
      case L'z':
         vRotationAngle += 0.25;
         vObject1.setRotation( rVec3f( 0, 1, 0 ), vRotationAngle );
         break;
      case L't':
         vRotationAngle -= 0.25;
         vObject1.setRotation( rVec3f( 0, 1, 0 ), vRotationAngle );
         break;
   }
}


void myScene::afterCameraUpdate() {
   vObject1.updateFinalMatrix();
   vLight1.updateFinalMatrix();
   vLight2.updateFinalMatrix();
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;

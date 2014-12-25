#include "myScene.hpp"
#include "rRenderVertexNormal_3_3.hpp"

int myScene::init() {
   updateCamera();

   vObject1.loadData();
   vObject1.setOGLData();
   vObject1.setPosition( rVec3f( 0, 0, -5 ) );

   vLight1.setPosition( rVec3f( 10, 10, 10 ) );

   int lLight;
   vObject1.getHints( rObjectBase::LIGHT_MODEL, lLight );
   if( lLight != rObjectBase::SIMPLE_ADS_LIGHT ) {
      wLOG( "Light not supported! Normals missing!" );
      vRenderNormals = false;
   }

   GLuint lShaderID     = addShader( vShader_str );
   GLuint lNormalShader = addShader( vNormalShader_str );

   if( !compileShaders() ) {
      eLOG( "Failed to compile the shaders" );
      return 5;
   }

   parseShaders();

   addObject( &vAmbient, -1 );
   addObject( &vLight1, -1 );

   int lObjID = addObject( &vObject1, lShaderID );
   int lRet = setObjectRenderer <
         rRenderNormal_3_3,
         rRenderBasicLight_3_3
         > ( lObjID );

   switch( lRet ) {
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

   if( vRenderNormals ) {
      setObjectRenderer<rRenderVertexNormal_3_3>( addObject( &vObject1, lNormalShader ) );
   }

   if( ! canRenderScene() ) {
      eLOG( "Cannot render scene!" );
      return 2;
   }

   return 0;
}

void myScene::keySlot( const iEventInfo &_inf ) {
   if( _inf.eKey.state != E_PRESSED )
      return;

   switch( _inf.eKey.key ) {
      case L'z': vRotationAngle += 0.25; vObject1.setRotation( rVec3f( 0, 1, 0 ), vRotationAngle ); break;
      case L't': vRotationAngle -= 0.25; vObject1.setRotation( rVec3f( 0, 1, 0 ), vRotationAngle ); break;
   }
}


void myScene::afterCameraUpdate() {
   vObject1.updateFinalMatrix();
   vLight1.updateFinalMatrix();
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;


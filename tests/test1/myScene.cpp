#include "myScene.hpp"

int myScene::init() {
   updateCamera();

   vObject1.loadData();
   vObject1.setOGLData();
   vObject1.setPosition( rVec3f( 0, 0, -5 ) );

   GLuint lShaderID = addShader( vShader_str );

   if ( ! compileShaders() ) {
      eLOG( "Failed to compile the shaders" );
      return 5;
   }

   switch ( setObjectRenderer<rRenderNormal_3_3>( addObject( &vObject1, lShaderID ) ) ) {
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

   if ( ! canRenderScene() ) {
      eLOG( "Can not render scene!" );
      return 2;
   }

   return 0;
}

void myScene::afterCameraUpdate() {
   vObject1.updateFinalMatrix();
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;


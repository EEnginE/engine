/*!
 * \file myScene.hpp
 */

#ifndef MY_SCENE_HPP
#define MY_SCENE_HPP

#include <engine.hpp>
#include "cmdANDinit.hpp"

using namespace e_engine;

class myScene final : public rScene<float>, public rCameraHandler<float> {
      typedef uSlot<void, myScene, iEventInfo> _SLOT_;

   private:
      rSimpleMesh vObject1;

      rAmbientLight<float>      vAmbient;
      rSimpleLightSource<float> vLight1;

      std::string vShader_str;
      std::string vNormalShader_str;

      _SLOT_ vKeySlot;
      float  vRotationAngle;
      bool   vRenderNormals;

   public:
      myScene() = delete;

      myScene( iInit *_init, cmdANDinit &_cmd ) :
         rScene( "MAIN SCENE" ),
         rCameraHandler( this, _init ),
         vObject1( this, "OBJ 1", _cmd.getMesh() ),
         vAmbient( "Ambient Light", rVec3f( 0.075, 0.05, 0.075 ) ),
         vLight1( this, "L1", rVec3f( 1, 0.75, 0.75 ) ),
         vShader_str( _cmd.getShader() ),
         vNormalShader_str( _cmd.getNormalShader() ),
         vKeySlot( &myScene::keySlot, this ),
         vRotationAngle( 0 ),
         vRenderNormals( _cmd.getRenderNormals() )
         { _init->addKeySlot( &vKeySlot ); }

      int init();

      void keySlot( iEventInfo _inf );

      virtual void afterCameraUpdate();
};

#endif

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;


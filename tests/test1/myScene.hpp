/*!
 * \file myScene.hpp
 */

#ifndef MY_SCENE_HPP
#define MY_SCENE_HPP

#include <engine.hpp>
#include "cmdANDinit.hpp"

using namespace e_engine;

class myScene final : public rScene<float>, public rCameraHandler<float> {
   private:
      rSimpleMesh vObject1;
      std::string vShader_str;

   public:
      myScene() = delete;

      myScene( iInit *_init, cmdANDinit &_cmd ) :
         rScene( "MAIN SCENE" ),
         rCameraHandler( this, _init ),
         vObject1( this, "OBJ 1", _cmd.getMesh() ),
         vShader_str( _cmd.getShader() ) {}

      int init();

      virtual void afterCameraUpdate();
};

#endif

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;


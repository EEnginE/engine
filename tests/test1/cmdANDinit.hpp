#ifndef CMDANDINIT_HPP
#define CMDANDINIT_HPP

#include <engine.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace e_engine;

class cmdANDinit {
   private:
      vector<string> args;
      string         argv0;
      string         dataRoot;
      string         meshToRender;
      string         vShader       = "shaderBasic";
      string         vNormalShader = "normals";

      uJSON_data     vData_JSON;

      bool           vCanUseColor;
      bool           vRenderNormals = false;

      GLfloat        vNearZ         = 0.1;
      GLfloat        vFarZ          = 100;

      cmdANDinit() {}

      void postInit();
      void preInit();
      void usage();
   public:
      cmdANDinit( int argc, char *argv[] );

      string  getMesh()          const {return dataRoot + meshToRender;}
      string  getShader()        const {return dataRoot + string( "shaders/" ) + vShader;}
      string  getNormalShader()  const {return dataRoot + string( "shaders/" ) + vNormalShader;}

      GLfloat getNearZ()         const {return vNearZ;}
      GLfloat getFarZ()          const {return vFarZ;}

      bool    getRenderNormals() const {return vRenderNormals;}

      bool parseArgsAndInit();

};

#endif // CMDANDINIT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;

/*!
 * \file ShaderInfo.hpp
 * \brief \b Classes: \a ShaderInfo
 * 
 * Class for testing:
 * Tests if it is possible to query shader information
 *
 */

#ifndef SHADERINFO_HPP
#define SHADERINFO_HPP

#include <engine.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace e_engine;

class ShaderInfo {
   private:
      // Private stuff goes here
      
   public:
      ShaderInfo() {}
      
      const static string desc;
      
      void runTest( uJSON_data &_data, string _dataRoot );
};

#endif // SHADERINFO_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;


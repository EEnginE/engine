#include "cmdANDinit.hpp"
#include "config.hpp"
#include "oglTestBind.hpp"
#include <regex>

cmdANDinit::cmdANDinit( int argc, char *argv[] ) {
   argv0        = argv[0];
   vCanUseColor = COLOR == 1 ? true : false;
#if WINDOWS
   dataRoot     = INSTALL_PREFIX + string( "\\share\\engineTests\\test1\\data\\" );
#else
   dataRoot     = INSTALL_PREFIX + string( "/share/engineTests/test1/data/" );
#endif

   for( auto i = 1; i < argc; ++i ) {
      args.push_back( argv[i] );
   }

   GlobConf.win.width           = 1280;
   GlobConf.win.height          = 1024;
   GlobConf.win.fullscreen      = false;
   GlobConf.win.windowName      = "Engine Test";
   GlobConf.win.iconName        = "ICON is missing";
   GlobConf.win.xlibWindowName  = "My icon";
   //GlobConf.win.winType         = e_engine::TOOLBAR;
   GlobConf.useAutoOpenGLVersion();
   GlobConf.config.appName      = "E Engine";


   GlobConf.win.restoreOldScreenRes = true;

   GlobConf.versions.glMajorVersion = 4;
   GlobConf.versions.glMinorVersion = 0;


   if( vCanUseColor ) {
      GlobConf.log.logOUT.colors   = FULL;
      GlobConf.log.logERR.colors   = FULL;
   } else {
      GlobConf.log.logOUT.colors   = DISABLED;
      GlobConf.log.logERR.colors   = DISABLED;

      GlobConf.log.width           = 175;
   }
   GlobConf.log.logOUT.Time     = LEFT_REDUCED;
   GlobConf.log.logOUT.File     = RIGHT_FULL;
   GlobConf.log.logERR.Time     = LEFT_REDUCED;
   GlobConf.log.logERR.File     = RIGHT_FULL;
   GlobConf.log.logFILE.File    = RIGHT_FULL;

   GlobConf.log.logFILE.logFileName =  SYSTEM.getLogFilePath();
#if UNIX
   GlobConf.log.logFILE.logFileName += "/Log";
#elif WINDOWS
   GlobConf.log.logFILE.logFileName += "\\Log";
#endif

   GlobConf.log.waitUntilLogEntryPrinted = false;
}



void cmdANDinit::usage() {
   iLOG( "Usage: ", argv0, " [OPTIONS] MODE [BENCHMARK OPTIONS]" );
   iLOG( "" );
   iLOG( "OPTIONS:" );
   dLOG( "    -h | --help        : show this help message" );
   dLOG( "    --log=<path>       : set a custom log file path to <path>" );
   dLOG( "    -w | --wait        : wait until log entry is printed" );
   dLOG( "    --data=<path>      : set a custom root path for the data dir\n        - Default:      ", dataRoot );
   dLOG( "    --mesh=<name>      : set the mesh to render IN the data dir (default: ", meshToRender, ")" );
   dLOG( "    -N | --normals     : Visulize mesh normals" );
   dLOG( "    --shader=<shader>  : set the shader to use (default: ", vShader, ")" );
   dLOG( "    --Nshader=<shader> : set the shader to use for rendering normals (default: ", vNormalShader, ")" );
   dLOG( "    --conf=<path>      : add a config file to parse" );
   dLOG( "    --glMajor=<v>      : the OpenGL Major version (default: ", GlobConf.versions.glMajorVersion, ")" );
   dLOG( "    --glMinor=<v>      : the OpenGL Major version (default: ", GlobConf.versions.glMinorVersion, ")" );
   dLOG( "    -n | --nocolor     : disable colored output" );
   dLOG( "    -c | --color       : enabel colored output" );
   dLOG( "    --near=<z>         : set near z clipping to <z> (default: ", vNearZ, ")" );
   dLOG( "    --far=<z>          : set far  z clipping to <z> (default: ", vFarZ, ")" );
}


bool cmdANDinit::parseArgsAndInit() {

   // Try to parse oglTest.json
   uParserJSON lTrialAndErrorParser( "oglTest.json" );
   if( lTrialAndErrorParser.parse() == 1 ) {
      iLOG( "Found oglTest.json" );
      auto lTempData = lTrialAndErrorParser.getData();
      vData_JSON.merge( lTempData );
   }

   for( auto const & arg : args ) {
      if( arg == "-h" || arg == "--help" ) {
         postInit();
         usage();
         return false;
      }

      if( arg == "-w" || arg == "--wait" ) {
         iLOG( "Wait is enabled" );
         GlobConf.log.waitUntilLogEntryPrinted = true;
         continue;
      }

      if( arg == "-n" || arg == "--nocolor" ) {
         iLOG( "Color is disabled" );
         GlobConf.log.logOUT.colors   = DISABLED;
         GlobConf.log.logERR.colors   = DISABLED;
         continue;
      }

      if( arg == "-c" || arg == "--color" ) {
         iLOG( "Color is enabled" );
         GlobConf.log.logOUT.colors   = FULL;
         GlobConf.log.logERR.colors   = FULL;
         continue;
      }

      if( arg == "-N" || arg == "--normals" ) {
         iLOG( "Normals will be renderd if possible" );
         vRenderNormals = true;
         continue;
      }

      std::regex lLogRegex( "^\\-\\-log=[\\/a-zA-Z0-9 \\._\\-\\+\\*]+$" );
      if( std::regex_match( arg, lLogRegex ) ) {
         std::regex lLogRegexRep( "^\\-\\-log=" );
         const char *lRep = "";
         string logPath = std::regex_replace( arg, lLogRegexRep, lRep );
         GlobConf.log.logFILE.logFileName = logPath;
         continue;
      }

      std::regex lDataRegex( "^\\-\\-data=[\\/a-zA-Z0-9 \\._\\-\\+\\*]+$" );
      if( std::regex_match( arg, lDataRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-data=" );
         const char *lRep = "";
         dataRoot = std::regex_replace( arg, lDataRegexRep, lRep );
         iLOG( "Using custom data path: '", dataRoot, "'" );
         continue;
      }

      std::regex lMeshRegex( "^\\-\\-mesh=[\\/a-zA-Z0-9 \\._\\-\\+\\*]+$" );
      if( std::regex_match( arg, lMeshRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-mesh=" );
         const char *lRep = "";
         meshToRender = std::regex_replace( arg, lDataRegexRep, lRep );
         continue;
      }


      std::regex lShaderRegex( "^\\-\\-shader=[\\/a-zA-Z0-9 \\._\\-\\+\\*]+$" );
      if( std::regex_match( arg, lShaderRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-shader=" );
         const char *lRep = "";
         vShader = std::regex_replace( arg, lDataRegexRep, lRep );
         continue;
      }

      std::regex lNShaderRegex( "^\\-\\-Nshader=[\\/a-zA-Z0-9 \\._\\-\\+\\*]+$" );
      if( std::regex_match( arg, lNShaderRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-Nshader=" );
         const char *lRep = "";
         vNormalShader = std::regex_replace( arg, lDataRegexRep, lRep );
         continue;
      }


      std::regex lConfRegex( "^\\-\\-conf=[0-9]+$" );
      if( std::regex_match( arg, lConfRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-conf=" );
         const char *lRep = "";
         string conf = std::regex_replace( arg, lDataRegexRep, lRep );
         uParserJSON parser( conf );

         if( parser.parse() == 1 ) {
            auto lTempData = parser.getData();
            vData_JSON.merge( lTempData );
            iLOG( "Successfully parsed additional JSON config '", conf, "'" );
         } else {
            wLOG( "Failed to parse additional JSON config '", conf, "'" );
         }

         continue;
      }

      std::regex lMajorRegex( "^\\-\\-glMajor=[0-9]+$" );
      if( std::regex_match( arg, lMajorRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-glMajor=" );
         const char *lRep = "";
         string version = std::regex_replace( arg, lDataRegexRep, lRep );
         GlobConf.versions.glMajorVersion = atoi( version.c_str() );
         continue;
      }

      std::regex lMinorRegex( "^\\-\\-glMinor=[0-9]+$" );
      if( std::regex_match( arg, lMinorRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-glMinor=" );
         const char *lRep = "";
         string version = std::regex_replace( arg, lDataRegexRep, lRep );
         GlobConf.versions.glMinorVersion = atoi( version.c_str() );
         continue;
      }


      std::regex lNearRegex( "^\\-\\-near=[0-9]+(\\.[0-9]+)?$" );
      if( std::regex_match( arg, lNearRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-near=" );
         const char *lRep = "";
         string version = std::regex_replace( arg, lDataRegexRep, lRep );
         vNearZ = atof( version.c_str() );
         continue;
      }

      std::regex lFarRegex( "^\\-\\-far=[0-9]+(\\.[0-9]+)?$" );
      if( std::regex_match( arg, lFarRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-far=" );
         const char *lRep = "";
         string version = std::regex_replace( arg, lDataRegexRep, lRep );
         vFarZ = atof( version.c_str() );
         continue;
      }

      eLOG( "Unkonwn option '", arg, "'" );
   }

   // Automatically parse the output from oglTest into GlobConf
   oglTestBind::process( vData_JSON );

   return true;
}


void cmdANDinit::postInit() {
   LOG.devInit();
   LOG.startLogLoop();
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;

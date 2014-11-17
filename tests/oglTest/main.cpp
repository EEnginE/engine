#include "cmdANDinit.hpp"
#include "testStarter.hpp"

using namespace e_engine;

int main( int argc, char *argv[] ) {
   testStarter tests;
   bool errors;

   cmdANDinit parseCMD( argc, argv, tests, errors );

   if( errors ) {
      LOG.devInit();
      LOG.startLogLoop();
      LOG.stopLogLoop();
      return 0;
   }

   iInit init;

   uJSON_data data;

   if( init.init() == 1 ) {
      data( "oglTest", "init", "canCreateWindow", S_BOOL( true ) );
   } else {
      eLOG( "Failed to create a window" );
      data( "oglTest", "init", "canCreateWindow", S_BOOL( false ) );
   }

   data(
         "oglTest", "init", "version", "major", S_NUM( GlobConf.versions.glMajorVersion ),
         "oglTest", "init", "version", "minor", S_NUM( GlobConf.versions.glMinorVersion )
   );

   tests.run( data, parseCMD.getDataRoot() );

   parseCMD.generate( data );
   
   iLOG( "" );
   iLOG( "Tipp: You can use the output of 'oglTestBindings.sh' to 'parse' all data this program produced into GlobConf" );
   iLOG( "" );

   init.shutdown();

   return 1;
}


/*
 * Begin recommended Bindings
 * 
 * Syntax: '//#!BIND ' <location in json file> , G_<TYPE>( <GlobConf value>, <default> )
 */
//#!BIND "oglTest", "init", "version", "major", G_NUM( GlobConf.versions.glMajorVersion, 4 )
//#!BIND "oglTest", "init", "version", "minor", G_NUM( GlobConf.versions.glMinorVersion, 6 )


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;

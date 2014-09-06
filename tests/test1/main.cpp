
#include "config.hpp"
#include "handler.hpp"
#include "cmdANDinit.hpp"
#include <time.h>
#include <engine.hpp>

#if WINDOWS
#include <windows.h>
#endif

using namespace std;
using namespace e_engine;

#define DO_SHA      0
#define OLD_RENDER  0
#define TEST_SHADER 0

void hexPrint( std::vector<unsigned char> const &_v ) {
   for( unsigned char const & c : _v )
      printf( "%02X ", c );
   printf( "\n\n" );
   fflush( stdout );
}

// #undef  UNIX
// #define UNIX 0


int main( int argc, char *argv[] ) {
   cmdANDinit cmd( argc, argv );

   if( ! cmd.parseArgsAndInit() ) {
      B_SLEEP( seconds, 1 );
      return 1;
   }

#if OLD_RENDER
   uRandomISAAC myRand;

   const int ValChange = 50;

   r = 0;
   g = 0;
   b = 0;

   R = ( ( float ) myRand( 10, ValChange ) / 5000 );
   G = ( ( float ) myRand( 10, ValChange ) / 5000 );
   B = ( ( float ) myRand( 10, ValChange ) / 5000 );

   rr = myRand( 0, 1 ) ? true : false;
   gg = myRand( 0, 1 ) ? true : false;
   bb = myRand( 0, 1 ) ? true : false;
#endif


   iLOG "User Name:     " ADD SYSTEM.getUserName()          END
   iLOG "User Login:    " ADD SYSTEM.getUserLogin()         END
   iLOG "Home:          " ADD SYSTEM.getUserHomeDirectory() END
   iLOG "Main config:   " ADD SYSTEM.getMainConfigDirPath() END
   iLOG "Log File Path: " ADD SYSTEM.getLogFilePath()       END


   iInit start;

   if( start.init() == 1 ) {
      MyHandler handler( cmd.getDataRoot(), &start );
      start.addWindowCloseSlot( handler.getSWindowClose() );
      start.addResizeSlot( handler.getSResize() );
      start.addKeySlot( handler.getSKey() );
      start.addMousuSlot( handler.getSMouse() );
      start.addFocusSlot( start.getAdvancedGrabControlSlot() );

#if 0
      vector<iDisplays> displays = start.getDisplayResolutions();

      iLOG "Displays: " ADD displays.size() END

      for( GLuint i = 0; i < displays.size(); ++i ) {
         iLOG "Display " ADD i ADD ": " ADD displays[i].getName() END
      }

      if( displays.size() == 2 ) {
//          displays[0].disable();
//          displays[1].disable();

//          iLOG start.setDisplaySizes( displays[0] ) END
//          iLOG start.setDisplaySizes( displays[1] ) END

//          start.applyNewRandRSettings();

//          B_SLEEP( seconds, 1 );

//          displays.clear();
//          displays = start.getDisplayResolutions();

         displays[0].enable();
         displays[1].enable();
         displays[0].autoSelectBest();
         displays[1].autoSelectBest();
//          iLOG start.setDisplaySizes( displays[0] ) END
//          iLOG start.setDisplaySizes( displays[1] ) END
//          start.applyNewRandRSettings();

//          displays.clear();
//          displays = start.getDisplayResolutions();

         displays[0].setNoClones();
         displays[1].setNoClones();
         displays[1].setPositionAbsolute( 0, 0 );
         displays[0].setPositionRelative( iDisplays::RIGHT_OFF, displays[1] );
         iLOG start.setDisplaySizes( displays[0] ) END
         iLOG start.setDisplaySizes( displays[1] ) END
         start.setPrimary( displays[1] );
         start.applyNewRandRSettings();
         start.setPrimary( displays[1] );
      }
#endif

#if TEST_SHADER
      string temp;
      temp += cmd.getDataRoot() + "shaders/colors_p";

      rShader prog( temp );
      GLuint dummy;
      prog.compile( dummy );
#endif

      if( handler.initGL() == 1 )
         start.startMainLoop();

      start.closeWindow();
   }

   uParserJSON parseJSON( cmd.getDataRoot() + "test.json" );

   if( parseJSON.parse() == 1 ) {
      iLOG "Parsing OK" END

      uJSON_data data = parseJSON.getData();
      JSON_DATA_TYPE type;
      std::string *lRet   = nullptr;
      std::string lStr;
      int         lSize;
      double      lDouble = 0;

//       data( "num",  &lDouble, (double)5 );
//       iLOG "NUM:  " ADD lDouble END
//
//
//       data( "obj1", "ddd", "hihi", &lStr, std::string( "--- N/A --- :P" ) );
//       iLOG "FAIL: " ADD lStr END
//
//       //data( "obj1", "objects", lSize );
//       iLOG "SIZE: " ADD lSize END
//
//       data( "obj1", "objects", 2, "name", &lStr, std::string( "FAILED" ) );
//       iLOG "STR:  " ADD lStr END


      // -----------

      int         lNum1,  lNum2;
      std::string lStr1,  lStr2;
      bool        lBool1, lBool2;


      // Read form data
      data(
            "obj1", "fTest1", G_STR( lStr, "Default" ),   // Test if fails are ok
            "obj2", "num1",   G_NUM( lNum1,  0 ),
            "obj2", "num2",   G_NUM( lNum2,  0 ),
            "obj2", "str1",   G_STR( lStr1, "Default" ),
            "obj1", "fTest2", G_STR( lStr,  "UNDEFINED" ), // Test if fails are ok
            "obj2", "str2",   G_STR( lStr2, "Something" ),
            "obj2", "bool1",  G_BOOL( lBool1, false ),
            "obj2", "bool2",  G_BOOL( lBool2, false )
      );

      iLOG "==> " ADD lNum1 ADD "; " ADD lNum2 ADD "; " ADD lStr1 ADD "; " ADD lStr2 ADD "; " ADD lBool1 ADD "; " ADD lBool2 ADD ";" END

      eLOG data.unique( true ) END
      eLOG data.unique( true ) END

      // Write to data
      uJSON_data lDataNew;
      lDataNew(
            "filename",              S_STR( "test.json" ),
            "obj1", "containsArray", S_BOOL( true ),
            "obj1", "arrayName",     S_STR( "test_array" ),
            "obj1", "arraySize",     S_NUM( 3 ),
            "obj1", "array", -1,     S_NUM( 4 ), // -1 is array push back
            "obj1", "array", -1,     S_NUM( 2 ),
            "obj1", "array", -1,     S_NUM( 42 ),
            "obj1", "numbers", -1,   S_NUM( 42 ),
            "obj1", "numbers", -1,   S_NUM( 43 ),
            "obj1", "numbers", -1,   S_NUM( 44 ),
            "obj1", "numbers", -1,   S_NUM( 45 ),
            "obj1", "numbers", -1,   S_NUM( 46 )
      );

      lDataNew.merge( data );

      uParserJSON lWriteParser( "test.json" );
      lWriteParser.write( lDataNew, true );

      iLOG true  && true  END
      iLOG false && true  END
      iLOG false && false END

   } else {
      eLOG "Failed Parsing" END
   }


#if DO_SHA == 1
   uSHA_2 mySHA( SHA2_384 );
   mySHA.selftest();
#endif

//    iLOG "Credits: "
//    POINT "Daniel ( Mense ) Mensinger"
//    POINT "Dennis Schunder"
//    POINT "Silas Bartel"
//    END

//    B_SLEEP( seconds, 1 );

   B_SLEEP( seconds, 1 );

   start.shutdown();

   return EXIT_SUCCESS;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 




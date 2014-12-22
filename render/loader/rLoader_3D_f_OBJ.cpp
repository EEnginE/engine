/*!
 * \file rLoader_3D_OBJ.cpp
 * \brief \b Classes: \a rLoader_3D_OBJ
 */

#include "rLoader_3D_f_OBJ.hpp"

#include "uLog.hpp"
#include "uFileIO.hpp"

#define DO_NOT_FAIL_PARSING 1

namespace e_engine {

rLoader_3D_f_OBJ::rLoader_3D_f_OBJ() {
   vIsDataLoaded_B = false;
}

rLoader_3D_f_OBJ::rLoader_3D_f_OBJ( std::string _file ) {
   vIsDataLoaded_B = false;
   vFilePath_str   = _file;
}

bool rLoader_3D_f_OBJ::getNum( float &_num ) {
   std::string lNum;

   bool lHasDot = false;

   if( *vIter == '-' ) {
      lNum += '-';
      ++vIter;
   }

   while( vIter != vEnd ) {
      switch( *vIter ) {
         case '.':
            if( lHasDot ) {
               eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
               return false;
            }
            lHasDot = true;
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            lNum += *vIter;
            ++vIter;
            break;

         default:
            if( lNum.empty() ) {
               eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine, ": not a number" );
               return false;
            }
            _num = std::stof( lNum );
            return true;
      }
   }

   eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
   return false;
}

bool rLoader_3D_f_OBJ::getInt( unsigned int &_num ) {
   std::string lNum;

   while( vIter != vEnd ) {
      switch( *vIter ) {
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            lNum += *vIter;
            ++vIter;
            break;

         default:
            if( lNum.empty() ) {
               eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine, ": not a number" );
               return false;
            }
            _num = std::stoi( lNum );
            return true;
      }
   }

   eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
   return false;
}


/*!
 * \brief loads the 3D content frome the OBJ file
 * \returns 1 on success
 * \returns 2 if there was a parsing error
 * \returns 3 if the OBJ file doesn't exists
 * \returns 4 if the OBJ file is not a regular file
 * \returns 5 if the OBJ file is not readable
 * \returns 6 if already loaded
 */
int rLoader_3D_f_OBJ::load() {
   if( vIsDataLoaded_B )
      return 6;

   uFileIO lFile( vFilePath_str );
   int lRet = lFile();
   if( lRet != 1 ) return lRet;

   vIter = lFile.begin();
   vEnd  = lFile.end();

   float        lWorker;
   unsigned int lIWorker;

   std::vector<GLfloat> *lPointer;
   unsigned short int lMax = 3;

   while( vIter != vEnd ) {
      switch( *vIter ) {

            // Comments
         case 'o':
         case 's':
         case '#':
            while( *vIter != '\n' && vIter != vEnd )
               ++vIter;

            ++vCurrentLine;
            ++vIter;
            break;

            // Vertex and normals
         case 'v':
            ++vIter;

            // Normals
            if( *vIter == 'n' ) {
               lPointer = &vDataRaw.vNormalesData;
               ++vIter;
               lMax = 3;
            } else if( *vIter == 't' ) {
               lPointer = &vDataRaw.vUVData;
               ++vIter;
               lMax = 2;
            } else if( *vIter == ' ' ) {
               lPointer = &vDataRaw.vVertexData;
               lMax = 3;
            } else {
               eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine, ": expected ' ' or 'n'" );
               return 2;
            }

            while( *vIter == ' ' && vIter != vEnd )
               ++vIter;

            for( short unsigned int i = 0; i < lMax; ++i ) {
               if( !getNum( lWorker ) )
                  return 2;

               lPointer->emplace_back( lWorker );

               while( *vIter == ' ' && vIter != vEnd )
                  ++vIter;
            }

            if( *vIter != '\n' ) {
               eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine, ": expected a newline" );
               return false;
            }

            ++vIter;
            ++vCurrentLine;
            break;

            // Face
         case 'f':
            ++vIter;

            // Normals
            if( *vIter != ' ' ) {
               eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine, ": expected ' ' or 'n'" );
               return 2;
            }

            while( *vIter == ' ' && vIter != vEnd )
               ++vIter;

            for( short unsigned int i = 0; i < 3; ++i ) {
               if( !getInt( lIWorker ) )
                  return 2;

               vDataRaw.vIndexVertexData.emplace_back( lIWorker );

               if( *vIter == '/' ) {
                  ++vIter;

                  // Normal Index
                  if( *vIter == '/' ) {
                     ++vIter;

                     if( !getInt( lIWorker ) )
                        return 2;

                     vDataRaw.vIndexNormalData.emplace_back( lIWorker );
                  } else {
                     // UV index
                     ++vIter;

                     if( !getInt( lIWorker ) )
                        return 2;

                     vDataRaw.vIndexUVData.emplace_back( lIWorker );

                     // Normal index
                     if( *vIter == '/' ) {
                        ++vIter;

                        if( !getInt( lIWorker ) )
                           return 2;

                        vDataRaw.vIndexNormalData.emplace_back( lIWorker );
                     }
                  }
               }

               while( *vIter == ' ' && vIter != vEnd )
                  ++vIter;
            }

            if( *vIter != '\n' ) {
               eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine, ": expected a newline" );
               return false;
            }

            ++vCurrentLine;
            ++vIter;
            break;

         default:
            eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
            return 2;
      }
   }

   vIsDataLoaded_B = true;
   return 1;
}








}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;


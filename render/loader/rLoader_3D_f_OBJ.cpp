/*!
 * \file rLoader_3D_OBJ.cpp
 * \brief \b Classes: \a rLoader_3D_OBJ
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rLoader_3D_f_OBJ.hpp"

#include "uLog.hpp"
#include "uFileIO.hpp"

namespace e_engine {

rLoader_3D_f_OBJ::rLoader_3D_f_OBJ() { vIsDataLoaded_B = false; }

rLoader_3D_f_OBJ::rLoader_3D_f_OBJ( std::string _file ) : rLoaderBase( _file ) {
   vIsDataLoaded_B = false;
   vFilePath_str = _file;
}

bool rLoader_3D_f_OBJ::getNum( float &_num ) {
   std::string lNum;

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case '.':
         case '-':
         case 'e':
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
            if ( lNum.empty() ) {
               eLOG( "Failed parsing file '",
                     vFilePath_str,
                     "' at char '",
                     *vIter,
                     "' Line ",
                     vCurrentLine,
                     ": not a number" );
               return false;
            }
            try {
               _num = std::stof( lNum );
            } catch ( ... ) {
               eLOG( "Failed parsing file '",
                     vFilePath_str,
                     "' at char '",
                     *vIter,
                     "' Line ",
                     vCurrentLine,
                     ": not a number" );
               return false;
            }

            return true;
      }
   }

   eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
   return false;
}

bool rLoader_3D_f_OBJ::getInt( unsigned int &_num ) {
   std::string lNum;

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
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
            if ( lNum.empty() ) {
               eLOG( "Failed parsing file '",
                     vFilePath_str,
                     "' at char '",
                     *vIter,
                     "' Line ",
                     vCurrentLine,
                     ": not a number" );
               return false;
            }
            _num = static_cast<unsigned>( std::stoi( lNum ) );
            return true;
      }
   }

   eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
   return false;
}



int rLoader_3D_f_OBJ::load_IMPL() {
   vIter = vFile.begin();
   vEnd = vFile.end();

   vName = "<OBJ name not set>";

   float lWorker;
   unsigned int lIWorker;

   std::vector<GLfloat> *lPointer;
   unsigned short int lMax = 3;

   internal::_3D_Data_RAWF lDataRaw;

   while ( vIter != vEnd ) {
      switch ( *vIter ) {

         // Comments
         case 'o':
            vName.clear();
            while ( *vIter != '\n' && vIter != vEnd ) {
               ++vIter;
               vName += *vIter;
            }
            eLOG( vName );

            ++vCurrentLine;
            ++vIter;
            break;
         case 's':
         case '#':
            while ( *vIter != '\n' && vIter != vEnd )
               ++vIter;

            ++vCurrentLine;
            ++vIter;
            break;

         // Vertex and normals
         case 'v':
            ++vIter;

            // Normals
            if ( *vIter == 'n' ) {
               lPointer = &lDataRaw.vNormalesData;
               ++vIter;
               lMax = 3;
            } else if ( *vIter == 't' ) {
               lPointer = &lDataRaw.vUVData;
               ++vIter;
               lMax = 2;
            } else if ( *vIter == ' ' ) {
               lPointer = &lDataRaw.vVertexData;
               lMax = 3;
            } else {
               eLOG( "Failed parsing file '",
                     vFilePath_str,
                     "' at char '",
                     *vIter,
                     "' Line ",
                     vCurrentLine,
                     ": expected ' ' or 'n'" );
               return 2;
            }

            while ( *vIter == ' ' && vIter != vEnd )
               ++vIter;

            for ( short unsigned int i = 0; i < lMax; ++i ) {
               if ( !getNum( lWorker ) )
                  return 2;

               lPointer->emplace_back( lWorker );

               while ( *vIter == ' ' && vIter != vEnd )
                  ++vIter;
            }

            if ( *vIter != '\n' ) {
               eLOG( "Failed parsing file '",
                     vFilePath_str,
                     "' at char '",
                     *vIter,
                     "' Line ",
                     vCurrentLine,
                     ": expected a newline" );
               return false;
            }

            ++vIter;
            ++vCurrentLine;
            break;

         // Face
         case 'f':
            ++vIter;

            // Normals
            if ( *vIter != ' ' ) {
               eLOG( "Failed parsing file '",
                     vFilePath_str,
                     "' at char '",
                     *vIter,
                     "' Line ",
                     vCurrentLine,
                     ": expected ' ' or 'n'" );
               return 2;
            }

            while ( *vIter == ' ' && vIter != vEnd )
               ++vIter;

            for ( short unsigned int i = 0; i < 3; ++i ) {
               if ( !getInt( lIWorker ) )
                  return 2;

               lDataRaw.vIndexVertexData.emplace_back( lIWorker );

               if ( *vIter == '/' ) {
                  ++vIter;

                  // Normal Index
                  if ( *vIter == '/' ) {
                     ++vIter;

                     if ( !getInt( lIWorker ) )
                        return 2;

                     lDataRaw.vIndexNormalData.emplace_back( lIWorker );
                  } else {
                     // UV index
                     ++vIter;

                     if ( !getInt( lIWorker ) )
                        return 2;

                     lDataRaw.vIndexUVData.emplace_back( lIWorker );

                     // Normal index
                     if ( *vIter == '/' ) {
                        ++vIter;

                        if ( !getInt( lIWorker ) )
                           return 2;

                        lDataRaw.vIndexNormalData.emplace_back( lIWorker );
                     }
                  }
               }

               while ( *vIter == ' ' && vIter != vEnd )
                  ++vIter;
            }

            if ( *vIter != '\n' ) {
               eLOG( "Failed parsing file '",
                     vFilePath_str,
                     "' at char '",
                     *vIter,
                     "' Line ",
                     vCurrentLine,
                     ": expected a newline" );
               return false;
            }

            ++vCurrentLine;
            ++vIter;
            break;

         default:
            eLOG( "Failed parsing file '",
                  vFilePath_str,
                  "' at char '",
                  *vIter,
                  "' Line ",
                  vCurrentLine );
            return 2;
      }
   }

   reindex( lDataRaw );

   if ( !vData.empty() )
      vData.front().vName = vName;

   vIsDataLoaded_B = true;
   return 1;
}
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;

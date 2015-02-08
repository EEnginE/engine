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

bool rLoader_3D_f_OBJ::load_IMPL() {
   vName = "<OBJ name not set>";

   float lWorker;
   unsigned short lIWorker;

   std::vector<GLfloat> *lPointer;
   unsigned short int lMax = 3;

   internal::_3D_Data_RAWF lDataRaw;

   while ( continueWhitespace( true ) ) {
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
            } else { return unexpectedCharError(); }

            if ( !continueWhitespace() )
               return false;

            for ( short unsigned int i = 0; i < lMax; ++i ) {
               if ( !getNum( lWorker ) )
                  return false;

               lPointer->emplace_back( lWorker );
            }

            break;

         // Face
         case 'f':
            ++vIter;

            // Normals
            if ( !continueWhitespace() )
               return false;

            for ( short unsigned int i = 0; i < 3; ++i ) {
               if ( !getNum( lIWorker ) )
                  return false;

               lDataRaw.vIndexVertexData.emplace_back( lIWorker );

               if ( *vIter == '/' ) {
                  ++vIter;

                  // Normal Index
                  if ( *vIter == '/' ) {
                     ++vIter;

                     if ( !getNum( lIWorker ) )
                        return false;

                     lDataRaw.vIndexNormalData.emplace_back( lIWorker );
                  } else {
                     // UV index
                     ++vIter;

                     if ( !getNum( lIWorker ) )
                        return false;

                     lDataRaw.vIndexUVData.emplace_back( lIWorker );

                     // Normal index
                     if ( *vIter == '/' ) {
                        ++vIter;

                        if ( !getNum( lIWorker ) )
                           return false;

                        lDataRaw.vIndexNormalData.emplace_back( lIWorker );
                     }
                  }
               }
            }

            break;

         default:
            return unexpectedCharError();
      }
   }

   internal::_3D_DataF lTemp;
   lTemp.vName = vName;

   internal::converter::reindex( lDataRaw, lTemp, vFilePath_str );
   return internal::convert<GLfloat, GLushort, 3>::toEngine3D( lTemp, vData );
}
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;

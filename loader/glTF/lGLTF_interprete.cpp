/*!
 * \file lGLTF_interprete.hpp
 * \brief \b Classes: \a lGLTF
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

#include "lGLTF.hpp"
#include "uFileIO.hpp"
#include <string.h> // memcpy
#include "uLog.hpp"
#include FILESYSTEM_INCLUDE

namespace e_engine {

bool lGLTF::interprete() {
   // Self Tests:
   for ( auto const &i : vAccessors )
      if ( !i.test() )
         return selfTestFailed( "accessor", i.id );

   for ( auto const &i : vBuffers )
      if ( !i.test() )
         return selfTestFailed( "buffer", i.id );

   for ( auto const &i : vBufferViews )
      if ( !i.test() )
         return selfTestFailed( "bufferView", i.id );

   for ( auto const &i : vMeshs )
      if ( !i.test() )
         return selfTestFailed( "mesh", i.id );

   //    ______        __  __
   //    | ___ \      / _|/ _|
   //    | |_/ /_   _| |_| |_ ___ _ __
   //    | ___ \ | | |  _|  _/ _ \ '__|
   //    | |_/ / |_| | | | ||  __/ |
   //    \____/ \__,_|_| |_| \___|_|
   //

   // Load binary data
   std::vector<uFileIO> lBuffers;
   FILESYSTEM_NAMESPACE::path lTemp( vFilePath_str );

   for ( auto const &i : vBuffers ) {
      FILESYSTEM_NAMESPACE::path lBufferFile( lTemp.parent_path().string() + "/" + i.uri );

      if ( !FILESYSTEM_NAMESPACE::exists( lBufferFile ) ) {
         eLOG( "Unable to find buffer file '", lBufferFile.string(), "'" );
         return false;
      }

      if ( !FILESYSTEM_NAMESPACE::is_regular_file( lBufferFile ) ) {
         eLOG( "File '", lBufferFile.string(), "' is not a regular file!" );
         return false;
      }

      lBuffers.emplace_back( lBufferFile.string() );
      if ( !lBuffers.back().read() )
         return false;
   }

   if ( vBuffers.size() != lBuffers.size() ) {
      eLOG( "Internal error! (Buffer count) [", vFilePath_str, "]" );
      return false;
   }

   dLOG_glTF( "" );
   dLOG_glTF( "START BUFFERS" );

   // The binary data is in little endian. We can just memcopy the data into the float vector
   for ( auto const &i : vBufferViews ) {
      switch ( i.target ) {
         case TG_ARRAY_BUFFER: {
            vRaw.resize( static_cast<size_t>( i.byteLength ) / sizeof( float ) );
            auto *lData = lBuffers[i.buffer].getData();

            if ( static_cast<size_t>( i.byteLength + i.byteOffset ) > lData->size() ) {
               eLOG( "Invalid buffer view size and / or offset!" );
               return false;
            }

            memcpy( vRaw.data(),                                       // Destination
                    &lData->at( static_cast<size_t>( i.byteOffset ) ), // Source
                    static_cast<size_t>( i.byteLength ) );             // Length

            dLOG_glTF( " - Buffer (ARRAY_BUFFER):         Offset = ",
                       i.byteOffset,
                       "; Size = ",
                       i.byteLength );

            break;
         }

         case TG_ELEMENT_ARRAY_BUFFER: {
            vRawIndex.resize( static_cast<size_t>( i.byteLength ) / sizeof( unsigned short ) );
            auto *lData = lBuffers[i.buffer].getData();

            if ( static_cast<size_t>( i.byteLength + i.byteOffset ) > lData->size() ) {
               eLOG( "Invalid buffer view size and / or offset!" );
               return false;
            }

            memcpy( vRawIndex.data(),                                  // Destination
                    &lData->at( static_cast<size_t>( i.byteOffset ) ), // Source
                    static_cast<size_t>( i.byteLength ) );             // Length

            dLOG_glTF( " - Buffer (ELEMENT_ARRAY_BUFFER): Offset = ",
                       i.byteOffset,
                       "; Size = ",
                       i.byteLength );

            break;
         }
         default: eLOG( "Invalid buffer view target" ); return false;
      }
   }

   //    ___  ___          _
   //    |  \/  |         | |
   //    | .  . | ___  ___| |__
   //    | |\/| |/ _ \/ __| '_ \
   //    | |  | |  __/\__ \ | | |
   //    \_|  |_/\___||___/_| |_|
   //

   dLOG_glTF( "START MESHES: " );

   for ( auto const &i : vMeshs ) {
      dLOG_glTF( " - Begin new Mesh: '", i.userDefName, "'" );

      vData.emplace_back();
      vData.back().name = i.userDefName;

      for ( auto const &j : i.primitives ) {

         auto const &lAcc = vAccessors[j.indices];
         auto const &lBV  = vBufferViews[lAcc.bufferView];

         if ( lBV.target != TG_ELEMENT_ARRAY_BUFFER ) {
            eLOG( "Bad target for index accessor (need 34963)! [", i.userDefName, "]" );
            return false;
         }

         if ( ( lAcc.count % 3 ) != 0 ) {
            eLOG( "Invalid index buffer! (count % 3 != 0) [", i.userDefName, "]" );
            return false;
         }

         dLOG_glTF( "   - Begin new Primitive" );

         switch ( j.mode ) {
            case P_POINTS:
               vData.back().type = POINTS_3D;
               dLOG_glTF( "     - Type: POINTS_3D" );
               break;
            case P_LINES:
            case P_LINE_LOOP:
            case P_LINE_STRIP:
               vData.back().type = LINES_3D;
               dLOG_glTF( "     - Type: LINES_3D" );
               break;
            case P_TRIANGLES:
            case P_TRIANGLE_STRIP:
            case P_TRIANGLE_FAN:
               vData.back().type = MESH_3D;
               dLOG_glTF( "     - Type: MESH_3D" );
               break;
            default:
               eLOG( "Internal error: unsupported primitive type [", i.userDefName, "]" );
               return false;
         }

         vData.back().index.resize( lAcc.count );

         for ( int32_t k = lAcc.byteOffset / sizeof( uint16_t ), counter = 0; counter < lAcc.count;
               k += 3 + lAcc.byteStride / sizeof( uint16_t ), counter += 3 ) {
            vData.back().index[counter + 0] = static_cast<uint32_t>( vRawIndex[k + 0] );
            vData.back().index[counter + 1] = static_cast<uint32_t>( vRawIndex[k + 1] );
            vData.back().index[counter + 2] = static_cast<uint32_t>( vRawIndex[k + 2] );
         }

         dLOG_glTF( "     - Num Index: ", lAcc.count );
         dLOG_glTF( "     - (Index) Offset: ", lAcc.byteOffset );
         dLOG_glTF( "     - (Index) Stride: ", lAcc.byteStride );

         for ( auto const &k : j.attributes ) {
            auto const &lAcc2 = vAccessors[k.accessor];
            auto const &lBV2  = vBufferViews[lAcc2.bufferView];

            if ( lBV2.target != TG_ARRAY_BUFFER ) {
               eLOG( "Bad target for index accessor (need 34962)! [", i.userDefName, "]" );
               return false;
            }

            std::vector<float> *lDataNew = nullptr;
            int32_t lNumEle              = 0;

            switch ( k.type ) {
               case SM_POSITION:
                  lDataNew = &vData.back().vertex;
                  lNumEle = 3;
                  dLOG_glTF( "     - New accessor: POSITION" );
                  break;
               case SM_NORMAL:
                  lDataNew = &vData.back().normal;
                  lNumEle = 3;
                  dLOG_glTF( "     - New accessor: NORMAL" );
                  break;
               case SM_TEXCOORD:
                  lDataNew = &vData.back().uv;
                  lNumEle = 2;
                  dLOG_glTF( "     - New accessor: TEXTCORD" );
                  break;
               default: eLOG( "Unsupported type! [", i.userDefName, "]" ); return false;
            }

            lDataNew->resize( lAcc2.count * lNumEle );

            for ( int32_t l = lAcc2.byteOffset / sizeof( float ), counter = 0;
                  counter < lAcc2.count * lNumEle;
                  l += lNumEle, counter += lNumEle ) {
               for ( int32_t m = 0; m < lNumEle; m++ )
                  lDataNew->operator[]( counter + m ) = vRaw[l + m];
            }

            dLOG_glTF( "       - Offset: ", lAcc2.byteOffset );
            dLOG_glTF( "       - Stride: ", lAcc2.byteStride );
         }
      }
   }

   return true;
}
}

/*!
 * \file rLoader_glTF_interprete.hpp
 * \brief \b Classes: \a rLoader_glTF
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

#include "rLoader_glTF.hpp"
#include "uFileIO.hpp"
#include <string.h> // memcpy
#include FILESYSTEM_INCLUDE

namespace e_engine {

bool rLoader_glTF::interprete() {
   // Self Tests:
   for ( auto const &i : vAccessors )
      if ( !i.test() )
         return selfTestFailed( "accessor", i.name );

   for ( auto const &i : vBuffers )
      if ( !i.test() )
         return selfTestFailed( "buffer", i.name );

   for ( auto const &i : vBufferViews )
      if ( !i.test() )
         return selfTestFailed( "bufferView", i.name );

   for ( auto const &i : vMeshs )
      if ( !i.test() )
         return selfTestFailed( "mesh", i.name );

//    ___  ___          _
//    |  \/  |         | |
//    | .  . | ___  ___| |__
//    | |\/| |/ _ \/ __| '_ \
//    | |  | |  __/\__ \ | | |
//    \_|  |_/\___||___/_| |_|
//

#if D_LOG_GLTF
   dLOG( "START MESHES: " );
#endif

   for ( auto const &i : vMeshs ) {
#if D_LOG_GLTF
      dLOG( " - Begin new Mesh: '", i.userDefName, "'" );
#endif

      vData->vName = i.userDefName;

      for ( auto const &j : i.primitives ) {
         vData->vObjects.emplace_back();
         auto &lObject = vData->vObjects.back();

         auto const &lAcc = vAccessors[j.indices];
         auto const &lBV  = vBufferViews[lAcc.bufferView];

         auto lOffset = lBV.byteOffset + lAcc.byteOffset;

         if ( lBV.target != TG_ELEMENT_ARRAY_BUFFER ) {
            eLOG( "Bad target for index accessor (need 34963)! [", i.userDefName, "]" );
            return false;
         }

         lObject.vOffset    = static_cast<unsigned>( lAcc.byteOffset );
         lObject.vStride    = static_cast<unsigned>( lAcc.byteStride );
         lObject.vNumVertex = static_cast<unsigned>( lAcc.count );

#if D_LOG_GLTF
         dLOG( "   - Begin new Primitive" );
#endif

         switch ( j.primitive ) {
            case P_POINTS: lObject.vType = POINTS_3D;
#if D_LOG_GLTF
               dLOG( "     - Type: POINTS_3D" );
#endif
               break;
            case P_LINES:
            case P_LINE_LOOP:
            case P_LINE_STRIP: lObject.vType = LINES_3D;
#if D_LOG_GLTF
               dLOG( "     - Type: LINES_3D" );
#endif
               break;
            case P_TRIANGLES:
            case P_TRIANGLE_STRIP:
            case P_TRIANGLE_FAN: lObject.vType = MESH_3D;
#if D_LOG_GLTF
               dLOG( "     - Type: MESH_3D" );
#endif
               break;
            default:
               eLOG( "Internal error: unsupported primitive type [", i.userDefName, "]" );
               return false;
         }

#if D_LOG_GLTF
         dLOG( "     - Num Index: ", lAcc.count );
         dLOG( "     - (Index) Offset: ", lBV.byteOffset, " + ", lAcc.byteOffset, " = ", lOffset );
         dLOG( "     - (Index) Stride: ", lAcc.byteStride );
#endif

         for ( auto const &k : j.attributes ) {
            lObject.vAccessor.emplace_back();
            auto &lObject2 = lObject.vAccessor.back();

            auto const &lAcc2 = vAccessors[k.accessor];
            auto const &lBV2  = vBufferViews[lAcc2.bufferView];

            if ( lBV2.target != TG_ARRAY_BUFFER ) {
               eLOG( "Bad target for index accessor (need 34962)! [", i.userDefName, "]" );
               return false;
            }

            lOffset = lBV2.byteOffset + lAcc2.byteOffset;

            lObject2.vOffset = static_cast<unsigned>( lAcc2.byteOffset );
            lObject2.vStride = static_cast<unsigned>( lAcc2.byteStride );

            typedef internal::_3D_Engine<float, unsigned short> lDATA;

            switch ( k.type ) {
               case TP_POSITION: lObject2.vType = lDATA::POSITION;
#if D_LOG_GLTF
                  dLOG( "     - New accessor: POSITION" );
#endif
                  break;
               case TP_NORMAL: lObject2.vType = lDATA::NORMAL;
#if D_LOG_GLTF
                  dLOG( "     - New accessor: NORMAL" );
#endif
                  break;
               default: eLOG( "Unsupported type! [", i.userDefName, "]" ); return false;
            }

#if D_LOG_GLTF
            dLOG( "       - Offset: ", lBV2.byteOffset, " + ", lAcc2.byteOffset, " = ", lOffset );
            dLOG( "       - Stride: ", lAcc2.byteStride );
#endif
         }
      }
   }

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

#if D_LOG_GLTF
   dLOG( "" );
   dLOG( "START BUFFERS" );
#endif

   // The binary data is in little endian. We can just memcopy the data into the float vector
   for ( auto const &i : vBufferViews ) {
      switch ( i.target ) {
         case TG_ARRAY_BUFFER: {
            vData->vData.resize( static_cast<size_t>( i.byteLength ) / sizeof( float ) );
            auto *lData = lBuffers[i.buffer].getData();

            if ( static_cast<size_t>( i.byteLength + i.byteOffset ) > lData->size() ) {
               eLOG( "Invalid buffer view size and / or offset!" );
               return false;
            }

            memcpy( vData->vData.data(),                               // Destination
                    &lData->at( static_cast<size_t>( i.byteOffset ) ), // Source
                    static_cast<size_t>( i.byteLength ) );             // Length

#if D_LOG_GLTF
            dLOG( " - Buffer (ARRAY_BUFFER):         Offset = ",
                  i.byteOffset,
                  "; Size = ",
                  i.byteLength );
#endif

            break;
         }

         case TG_ELEMENT_ARRAY_BUFFER: {
            vData->vIndex.resize( static_cast<size_t>( i.byteLength ) / sizeof( unsigned short ) );
            auto *lData = lBuffers[i.buffer].getData();

            if ( static_cast<size_t>( i.byteLength + i.byteOffset ) > lData->size() ) {
               eLOG( "Invalid buffer view size and / or offset!" );
               return false;
            }

            memcpy( vData->vIndex.data(),                              // Destination
                    &lData->at( static_cast<size_t>( i.byteOffset ) ), // Source
                    static_cast<size_t>( i.byteLength ) );             // Length

#if D_LOG_GLTF
            dLOG( " - Buffer (ELEMENT_ARRAY_BUFFER): Offset = ",
                  i.byteOffset,
                  "; Size = ",
                  i.byteLength );
#endif

            break;
         }
         default: eLOG( "Invalid buffer view target" ); return false;
      }
   }


   return true;
}
}

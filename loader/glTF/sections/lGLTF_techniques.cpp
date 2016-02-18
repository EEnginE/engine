/*!
 * \file lGLTF_scenes.cpp
 * \brief \b Classes: \a lGLTF
 */
/*
 * Copyright (C) 2016 EEnginE project
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

#define ENABLE_GLTF_MACRO_HELPERS

#include "lGLTF.hpp"
#include "lGLTF_macroHelpers.hpp"

namespace e_engine {

bool lGLTF::sectionTechniquesParameters( size_t _id ) {
   BEGIN_GLTF_SECTION( lSection, lName, lID );

   while ( vIter != vEnd ) {
      BEGIN_GLTF_SECTION_MAIN_LOOP(
            lName, vTechniques[_id].parameters, vTechniques[_id].parametersMap, lID );

      while ( vIter != vEnd ) {
         lName.clear();

         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            // clang-format off
            case NAME:     READ_STRING(     vTechniques[_id].parameters[lID].name );
            case COUNT:    READ_NUM(        vTechniques[_id].parameters[lID].count );
            case TYPE:     READ_MAP_EL_ETC( vTechniques[_id].parameters[lID].type );
            case SEMANTIC: READ_MAP_EL(     vTechniques[_id].parameters[lID].semantic, false );
            case VALUE:    READ_VALUE(      vTechniques[_id].parameters[lID].lValue );
            case NODE:     READ_ITEM( vNodes, vNodesMap, lName, vTechniques[_id].parameters[lID].node );
            // clang-format on
            case EXTENSIONS:
            case EXTRAS:
               if ( !skipSection() )
                  return false;

               break;
            default: return wrongKeyWordError();
         }

         END_GLTF_OBJECT
      }

      END_GLTF_OBJECT
   }
   return true;
}


bool lGLTF::sectionTechniquesAttributes( size_t _id, std::vector<technique::attribute> &_attrs ) {
   if ( !expect( '{' ) )
      return false;

   if ( expect( '}', true, true ) )
      return true;

   std::string lName;

   while ( vIter != vEnd ) {
      lName.clear();
      if ( !getString( lName ) )
         return false;

      _attrs.emplace_back();
      _attrs.back().id = lName;

      if ( !expect( ':' ) )
         return false;

      if ( !getString( lName ) )
         return false;

      _attrs.back().parameter =
            getItem( vTechniques[_id].parameters, vTechniques[_id].parametersMap, lName );

      END_GLTF_OBJECT
   }

   return true;
}

bool lGLTF::sectionTechniquesStates( size_t _id ) {
   if ( !expect( '{' ) )
      return false;

   if ( expect( '}', true, true ) )
      return true;

   ELEMENTS lSection;
   std::string lName;

   while ( vIter != vEnd ) {
      lName.clear();
      if ( !getMapElement( lSection ) )
         return false;

      switch ( lSection ) {
         case ENABLE: READ_ARRAY( vTechniques[_id].states.enable );
         case FUNCTIONS:
            if ( !expect( '{' ) )
               return false;

            if ( expect( '}', true, true ) )
               break;

            while ( vIter != vEnd ) {
               if ( !getMapElement( lSection ) )
                  return false;

               if ( !expect( ':' ) )
                  return false;

               switch ( lSection ) {
                  // clang-format off
                  case BLENDCOLOR:            READ_ARRAY( vTechniques[_id].states.functions.blendColor );
                  case BLENDEQUATIONSEPARATE: READ_ARRAY( vTechniques[_id].states.functions.blendEquationSeparate );
                  case BLENDFUNCSEPARATE:     READ_ARRAY( vTechniques[_id].states.functions.blendFuncSeparate );
                  case COLORMASK:             READ_ARRAY( vTechniques[_id].states.functions.colorMask );
                  case CULLFACE:              READ_ARRAY( vTechniques[_id].states.functions.cullFace );
                  case DEPTHFUNC:             READ_ARRAY( vTechniques[_id].states.functions.depthFunc );
                  case DEPTHMASK:             READ_ARRAY( vTechniques[_id].states.functions.depthMask );
                  case DEPTHRANGE:            READ_ARRAY( vTechniques[_id].states.functions.depthRange );
                  case FRONTFACE:             READ_ARRAY( vTechniques[_id].states.functions.frontFace );
                  case LINEWIDTH:             READ_ARRAY( vTechniques[_id].states.functions.lineWidth );
                  case POLYGONOFFSET:         READ_ARRAY( vTechniques[_id].states.functions.polygonOffset );
                  case SCISSOR:               READ_ARRAY( vTechniques[_id].states.functions.scissor );
                  // clang-format on
                  default: return wrongKeyWordError();
               }

               END_GLTF_OBJECT
            }
            break;

         case EXTENSIONS:
         case EXTRAS:
            if ( !skipSection() )
               return false;

            break;
         default: return wrongKeyWordError();
      }

      END_GLTF_OBJECT
   }

   return true;
}


bool lGLTF::sectionTechniques() {
   BEGIN_GLTF_SECTION( lSection, lName, lID );

   while ( vIter != vEnd ) {
      BEGIN_GLTF_SECTION_MAIN_LOOP( lName, vTechniques, vTechniqueMap, lID );

      while ( vIter != vEnd ) {
         lName.clear();

         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            // clang-format off
            case NAME:       READ_STRING( vTechniques[lID].name );
            case PROGRAM:    READ_ITEM( vPrograms, vProgramsMap, lName, vTechniques[lID].program );
            case PARAMETERS: SUB_SECTION( sectionTechniquesParameters, lID );
            case ATTRIBUTES: SUB_SECTION( sectionTechniquesAttributes, lID, vTechniques[lID].attributes );
            case UNIFORMS:   SUB_SECTION( sectionTechniquesAttributes, lID, vTechniques[lID].uniforms );
            case STATES:     SUB_SECTION( sectionTechniquesStates,     lID );
            // clang-format on
            case EXTENSIONS:
            case EXTRAS:
               if ( !skipSection() )
                  return false;

               break;
            default: return wrongKeyWordError();
         }

         END_GLTF_OBJECT
      }

#if D_LOG_GLTF
      vTechniques[lID].print( this );
#endif

      END_GLTF_OBJECT
   }

   return true;
}
}

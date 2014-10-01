/*!
 * \file uParserJSON_data.hpp
 * \brief \b Classes: \a uJSON_data
 */

#include "uParserJSON_data.hpp"
#include "uLog.hpp"
#include <boost/lexical_cast.hpp>


namespace e_engine {


/*!
 * \brief Checks (recursivly) for multiple ID's
 *
 * \param[in] _renoveDuplicates When true, removes duplicate entries (the first entry will be kept)
 * \param[in] _quiet            Doesn't print anything when true
 * \param[in] _patent_IDs       -- Only for internam usage --
 *
 * \returns true if no dupliates where found
 */
bool uJSON_data::unique( bool _renoveDuplicates, bool _quiet, std::string _patent_IDs ) {
   bool lReturn = true;
   if( type == JSON_ARRAY ) {
      for( unsigned int i = 0; i < value_array.size(); ++i ) {
         // An array has no ID's
         lReturn = value_array[i].unique( _renoveDuplicates, _quiet, _patent_IDs + "[" + boost::lexical_cast<std::string>( i ) + "]." ) && lReturn;
      }
   } else if( type == JSON_OBJECT ) {
      std::vector<std::string>  lIDs;
      std::vector<unsigned int> lErase;
      unsigned int lDulicatesFound_uI = 0;

      for( auto iter = value_obj.begin(); iter != value_obj.end() ; ++iter ) {
         for( auto const & lID : lIDs ) {
            if( iter->id == lID ) {
               unsigned int lOffset_uI = ( value_obj.end() - value_obj.begin() ) - ( value_obj.end() - iter );
               // Found duplicate ID
               lReturn = false;
               if( !_quiet ) {
                  std::string lTypeStr, lValueStr;
                  switch( iter->type ) {
                     case JSON_STRING: lTypeStr = "string";  lValueStr = "'" + iter->value_str + "'";                                                        break;
                     case JSON_NUMBER: lTypeStr = "number";  lValueStr = boost::lexical_cast<std::string>( iter->value_num );                                break;
                     case JSON_BOOL:   lTypeStr = "boolean"; lValueStr = iter->value_bool ? "true" : "false";                                                break;
                     case JSON_NULL:   lTypeStr = "NULL";    lValueStr = "nil";                                                                              break;
                     case JSON_ARRAY:  lTypeStr = "array";   lValueStr = "[...]; Elements: " + boost::lexical_cast<std::string>( iter->value_array.size() ); break;
                     case JSON_OBJECT: lTypeStr = "object";  lValueStr = "{...}; Elements: " + boost::lexical_cast<std::string>( iter->value_obj.size() );   break;
                     default:          lTypeStr = "UNKNOWN"; lValueStr = "UNKNOWN";
                  }
                  wLOG(
                        "JSON: found duplicate ID '", _patent_IDs + ( id.empty() ? "" : ( id + "." ) ) + iter->id,
                        "' ==> ", _renoveDuplicates ? "REMOVE" : "KEEP",
                        "\n  - Type:  ", lTypeStr,
                        "\n  - Value: ", lValueStr,
                        "\n  - Pos:   ", lOffset_uI
                  );
               }
               if( _renoveDuplicates ) lErase.push_back( lOffset_uI - lDulicatesFound_uI );
               ++lDulicatesFound_uI;
               break;
            }
         }

         lIDs.push_back( iter->id );

         lReturn = iter->unique( _renoveDuplicates, _quiet, _patent_IDs + ( id.empty() ? "" : ( id + "." ) ) ) && lReturn;
      }

      for( auto it : lErase ) value_obj.erase( value_obj.begin() + it );
   }
   return lReturn;
}


/*!
 * \brief Merges a opbject into the current object
 *
 * \param[in] _toMerge   the object to be merged into this object
 * \param[in] _overWrite Overwrites values eith the same id when true
 *
 * \returns nothing
 */
void uJSON_data::merge( uJSON_data &_toMerge, bool _overWrite ) {
   if( _toMerge.type == JSON_ARRAY && type == JSON_ARRAY ) {
      value_array.insert( value_array.end(), _toMerge.value_array.begin(), _toMerge.value_array.end() );
      return;
   }

   if( _toMerge.type == JSON_OBJECT && type == JSON_OBJECT ) {
      for( auto & toM : _toMerge.value_obj ) {
         bool lFoundConflict_B = false;
         for( auto & current : value_obj ) {
            if( current.id == toM.id ) {
               current.merge( toM, _overWrite );
               lFoundConflict_B = true;
               break;
            }
         }

         if( !lFoundConflict_B ) {
            value_obj.push_back( toM );
         }
      }
      return;
   }

   if( _overWrite ) {
      type        = _toMerge.type;
      value_str   = _toMerge.value_str;
      value_num   = _toMerge.value_num;
      value_bool  = _toMerge.value_bool;
      value_array = _toMerge.value_array;
      value_obj   = _toMerge.value_obj;
   }

   return;
}




}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
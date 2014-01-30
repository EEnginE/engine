/*!
 * \file log_resources.cpp
 * \brief \b Classes: \a eLogType, \a __eLogStore, \a eLogEntry, \a __eLogStoreHelper 
 */

#include "log_resources.hpp"
#include "log.hpp"
#include <iomanip>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#define E_COLOR_NO_TERMTEST
#include "color.hpp"

namespace e_engine {
namespace e_engine_internal {


unsigned int __eLogStoreHelper::getLogEntry( std::vector< e_engine::e_engine_internal::eLogType > &_vLogTypes_V_eLT, e_engine::eLogEntry &_entry ) {
   _entry.data.vLogEntries_V_eLS = vElements_V_eLS;
   _entry.data.vFilename_STR     = vRawFilename_STR;
   _entry.data.vLine_I           = vLogLine_I;
   _entry.data.vTime_lI          = vTime_lI;
   _entry.data.vType_STR         = "UNKNOWN";
   _entry.data.vNewColor_STR     = testNewColor();
   
   if( _vLogTypes_V_eLT.empty() ) {
      eLOG "No Log type found!! Please add at least one manualy or run 'eLog.devInit();', which will be run now to prevent other Errors" END
      LOG.devInit();
   }

   for ( GLuint i = 0; i < _vLogTypes_V_eLT.size(); ++i ) {
      if ( _vLogTypes_V_eLT[i].getType() == vType_C ) {
         _entry.data.vType_STR     = _vLogTypes_V_eLT[i].getString();
         _entry.data.vBasicColor_C = _vLogTypes_V_eLT[i].getColor();
         _entry.data.vBold_B       = _vLogTypes_V_eLT[i].getBold();
         return i;
      }
   }

   std::string ltemp_STR = "WARNING!! Log type '";
   ltemp_STR += vType_C;
   ltemp_STR += "' not Found";
   
   vElements_V_eLS.push_back( e_engine_internal::__eLogStore( ltemp_STR, 'B', 'R', '-' ) );

   return 0;
}

std::string __eLogStoreHelper::testNewColor()  {
if ( vFG_C != '-' ) {
      if ( vAttrib_C != '-' ) {
         if ( vBG_C != '-' ) {
            return eCMDColor::color( vAttrib_C, vFG_C, vBG_C );
         }
         return eCMDColor::color( vAttrib_C, vFG_C );
      }
      return eCMDColor::color( vFG_C );
   }
   return "";
}


bool __eLogStore::hasColor()  {
   if ( vFG_C != '-' ) {
      if ( vAttrib_C != '-' ) {
         if ( vBG_C != '-' ) {
            vColor_STR = eCMDColor::color( vAttrib_C, vFG_C, vBG_C );
            return true;
         }
         vColor_STR = eCMDColor::color( vAttrib_C, vFG_C );
         return true;
      }
      vColor_STR = eCMDColor::color( vFG_C );
      return true;
   }
   return false;
}


}

GLvoid eLogEntry::configure( e_engine::LOG_COLOR_TYPE _color, e_engine::LOG_PRINT_TYPE _time, e_engine::LOG_PRINT_TYPE _file, e_engine::LOG_PRINT_TYPE _errorType, GLint _columns ) {
   config.vColor_LCT     = _color;
   config.vTime_LPT      = _time;
   config.vFile_LPT      = _file;
   config.vErrorType_LPT = _errorType;
   config.vColumns_uI    = _columns;
}


}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

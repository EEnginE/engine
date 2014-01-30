/// \file error.cpp
/// \brief \b Classes: \a eError
/// \sa e_error.hpp 
#include "error.hpp"
#include "platform.hpp"
#include "log.hpp"
#include "color.hpp"
namespace e_engine {
   

eError::eError( E_CLASS_ID _class_id, E_ERROR_TYPE _error_type, E_ERROR_ID _error_id, std::string _error_str ) {
   class_id        = _class_id;
   error_type      = _error_type;
   error_id        = _error_id;
   error_str       = _error_str;
   switch ( class_id ) {
      case CLASS_eContext:       class_id_str   = "eContext";             break;
      case CLASS_eError:         class_id_str   = "eError";               break;
      case CLASS_eInit:          class_id_str   = "eInit";                break;
      case CLASS_eLinker:        class_id_str   = "eLinker";              break;
      case CLASS_eLog:           class_id_str   = "eLog";                 break;
      case CLASS_eShader:        class_id_str   = "eShader";              break;
      case CLASS_eThread:        class_id_str   = "eThread";              break;
      default:                   class_id_str   = "UNKNOWN";              break;
      
   }
   switch ( error_type ) {
      case ERROR_DummyError:     error_type_str = "Dummy Error";          break;
      case ERROR_Special:        error_type_str = "Special Error";        break;
      case ERROR_InputOutput:    error_type_str = "Input-Output Error";   break;
      case ERROR_Shader:         error_type_str = "Shader Error";         break;
      case ERROR_Multithreading: error_type_str = "Error with threads";   break;
      default:                   error_type_str = "Unknown Error source"; break;
      
   }
   switch ( error_id ) {
      case E_ONLY_ONE:           error_id_str   = "E_ONLY_ONE";           break;
      case E_OUT_OF_RANGE:       error_id_str   = "E_OUT_OF_RANGE";       break;
      case E_LINKING_FAILURE:    error_id_str   = "E_LINKING_FAILURE";    break;
      case E_COMPILING_FAILURE:  error_id_str   = "E_COMPILING_FAILURE";  break;
      case E_CAN_NOT_READ_FILE:  error_id_str   = "E_CAN_NOT_READ_FILE";  break;
      case E_CAN_NOT_WRITE_FILE: error_id_str   = "E_CAN_NOT_WRITE_FILE"; break;
      case E_NOT_SUCH_A_FILE:    error_id_str   = "E_NOT_SUCH_A_FILE";    break;
      case E_VARIABLE_NOT_SET:   error_id_str   = "E_VARIABLE_NOT_SET";   break;
      case E_END_BUT_NOT_START:  error_id_str   = "E_END_BUT_NOT_START";  break;
      case E_START_BUT_NOT_END:  error_id_str   = "E_START_BUT_NOT_END";  break;
      case E_NOT_STARTED:        error_id_str   = "E_NOT_STARTED";        break;
      case E_UNKNOWN:
      default:                   error_id_str   = "E_UNKNOWN";            break;
   
   }
}


void eError::what() {
   if ( error_str.empty() ) {
      error_str = "NO DESCRIPTION";
   }
   eLOG "Class " ADD 'B', 'R', class_id_str ADD 'O', 'R', " has thrown this Error:"
   POINT "type:        " ADD class_id_str
   POINT "id:          " ADD error_id_str ADD " (" ADD error_id ADD ')'
   POINT "description: " ADD error_str END
   for ( GLuint i = 0; i < errors_attached.size(); i++ ) {
      wLOG "   - Attached Error " ADD i + 1 ADD "of" ADD errors_attached.size() ADD ':' END
      errors_attached[i].what();
   }
}

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 




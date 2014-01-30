#include "signal_slot.hpp"
#include "log.hpp"

namespace e_engine {
namespace e_engine_internal {

void __eSigSlotLogFunctionClass::sigSlotLogFunktion( std::string _errStr, const char* _file, const int _line ) {
   LOG( 'E', _file , _line , _errStr )->end();
}

unsigned int __eSigSlotConnection::vSignalsCounter_uI = 0;

}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

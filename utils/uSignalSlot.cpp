/*!
 * \file uSignalSlot.cpp
 * \brief \b Classes: \a uSignal, \a uSlot
 */

#include "uSignalSlot.hpp"
#include "uLog.hpp"

namespace e_engine {
namespace internal {

void __uSigSlotLogFunctionClass::sigSlotLogFunktion( std::string _errStr, const char* _file, const int _line, const char* _function ) {
   LOG( _E, _errStr );
}

unsigned int __uSigSlotConnection::vSignalsCounter_uI = 0;

}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;

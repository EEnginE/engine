/*!
 * \file x11/eCMDColor.cpp
 * \brief \b Classes: \a eCMDColor
 * \sa e_color.cpp
 */

#include "eCMDColor.hpp"

namespace e_engine {
#ifdef E_COLOR_DISABLED
   const std::wstring eCMDColor::RESET = L"";
#else
   const std::wstring eCMDColor::RESET = L"\x1b[0m";
#endif
}
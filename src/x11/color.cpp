#include "color.hpp"

namespace e_engine {
#ifdef E_COLOR_DISABLED
   const std::string eCMDColor::RESET = "";
#else
   const std::string eCMDColor::RESET = "\x1b[0m";
#endif
}
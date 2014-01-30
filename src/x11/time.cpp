/// \filetime.cpp 
/// \brief \b Classes: \a eTime
/// \sa e_time.hpp
/*
 *  E Engine
 *  Copyright (C) 2013 Daniel Mensinger
 * 
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "time.hpp"

namespace e_engine {

eStartTime E_CLOCK;

eStartTime::eStartTime() {
   gettimeofday( &tv, NULL );
   initTime_sec  = tv.tv_sec;
   initTime_usec = TIMEVAL2USEC( tv );
}

}

/// \file time.hpp
/// \brief \b Classes: \a eTime
/// \sa e_time.cpp
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

#ifndef E_TIME_HPP
#define E_TIME_HPP

#include <sys/time.h>
#include <cstdlib>

#define TIMEVAL2USEC(x) ((x.tv_sec * 1000) + (x.tv_usec / 1000))

namespace e_engine {

/*!
 * \class e_engine::eTime
 * \brief The main time class
 * 
 * The class for the intern time handling
 * 
 * \note Most time only the standard object from eStartTime is used
 * 
 * \sa eStartTime
 */
class eTime {
   private:
      long int timer;

      timeval tv;

   public:
      eTime() {}
      virtual ~eTime() {}

      /*!
       * \brief Add 2 timevals
       * \param[in]  a      timeval 1
       * \param[in]  b      timeval 2
       * \param[out] result the result
       * \returns Nothing
       */
      static inline void addTimeval( timeval &a, timeval &b, timeval &result ) {
         result.tv_sec  = a.tv_sec  + b.tv_sec;
         result.tv_usec = a.tv_usec + b.tv_usec;
         while ( result.tv_usec >= 1000000 ) {
            result.tv_sec++;
            result.tv_usec -= 1000000;
         }
      }
      
      /*!
       * \brief Subtract 2 timevals
       * \param[in]  a      timeval 1
       * \param[in]  b      timeval 2
       * \param[out] result the result
       * \returns Nothing
       */
      static inline void subTimeval( timeval &a, timeval &b, timeval &result ) {
         result.tv_sec  = a.tv_sec  - b.tv_sec;
         result.tv_usec = a.tv_usec - b.tv_usec;
         while ( result.tv_usec < 0 ) {
            result.tv_sec--;
            result.tv_usec += 1000000;
         }
      }

      /*!
       * \brief Sleep some time
       * \param sec Sleep this time
       * \returns Nothing
       */
      inline void sleepSec( long int sec )   {sleepUsec( ( sec * 1000000 ) );};
      
      /*!
       * \brief Sleep some time
       * \param usec Sleep this time
       * \returns Nothing
       */
      inline void sleepUsec( long int usec ) {
         tv.tv_sec  = 0;
         tv.tv_usec = usec;

         select( 1, NULL, NULL, NULL, &tv );
      }

      /*!
       * \brief Start the timer
       * \returns Nothing
       */
      inline void     startTimer()        {gettimeofday( &tv, NULL ); timer = TIMEVAL2USEC( tv );}
      /*!
       * \brief Stop the timer
       * \returns The time elapsed
       */
      inline long int stopTimer()         {gettimeofday( &tv, NULL ); return ( TIMEVAL2USEC( tv ) - timer ) * 1000;}

      /*!
       * \brief Get the global time
       * \returns The global time in usec
       */
      inline long int getGlobalTime()     {gettimeofday( &tv, NULL ); return TIMEVAL2USEC( tv );}
      
      /*!
       * \brief Get the global time
       * \returns The global time in sec
       */
      inline long int getGlobalTimeSec()  {gettimeofday( &tv, NULL ); return tv.tv_sec;}
};

/*!
 * \class eStartTime
 * \brief Class that gives information about the start time
 * 
 * This class stores the time when it is created. The instance
 * CLOCK is every time at the start of the programm created.
 * 
 * \sa eTime
 */
class eStartTime : public eTime {
   private:
      timeval tv;

      long int initTime_usec;
      long int initTime_sec;

   public:
      eStartTime();
      virtual ~eStartTime() {}

      /*!
       * \brief Get the elapsed time since programm start
       * \returns The elapsed time in usec
       */
      inline long int getTimeElapsed()     {gettimeofday( &tv, NULL ); return ( TIMEVAL2USEC( tv ) - initTime_usec );}
      
      /*!
       * \brief Get the time when programm started
       * \returns The time when programm started time in usec
       */
      inline long int getInitTime()        {return initTime_usec;}

      /*!
       * \brief Get the elapsed time since programm start
       * \returns The elapsed time in sec
       */
      inline long int getTimeElapsed_sec() {gettimeofday( &tv, NULL ); return tv.tv_sec - initTime_sec;}
      
      /*!
       * \brief Get the time when programm started
       * \returns The time when programm started time in sec
       */
      inline long int getInitTime_sec()    {return initTime_sec;}
};

extern eStartTime CLOCK;

}

#endif // TIME_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

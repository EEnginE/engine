/*!
 * \file rand_isaac.hpp
 * \brief \b Classes: \a RandISAAC
 */

#ifndef RADND_ISAAC_HPP
#define RADND_ISAAC_HPP

#include <stdint.h>

namespace e_engine {

class RandISAAC {
   private:
      uint32_t mm[256];
      uint32_t aa;
      uint32_t bb;
      uint32_t cc;

      uint8_t  step;

      void mixUp( uint32_t _seed[256] );
   public:
      RandISAAC()            {init( 0 );}
      RandISAAC( uint32_t _seed ) {init( _seed );}

      void init( uint32_t _seed = 0 );

      uint32_t        get();
      inline uint32_t get( uint32_t _min, uint32_t _max ) { return ( _max <= _min ) ? _min : ( get() % ( ( _max + 1 ) - _min ) + _min ); }

      inline uint32_t operator()()                               {return get();}
      inline uint32_t operator()( uint32_t _min, uint32_t _max ) {return get( _min, _max );}
};

}

#endif // RADND_ISAAC_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

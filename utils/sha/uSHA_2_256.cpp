/*!
 * \file sha2_256.cpp
 * \brief \b Classes: \a uSHA_2
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "uSHA_2.hpp"

namespace e_engine {

namespace {

inline uint32_t ROTR( uint32_t x, uint32_t n ) { return ( x >> n ) | ( x << ( 32 - n ) ); }

inline uint32_t Ch( uint32_t x, uint32_t y, uint32_t z ) {
   return ( x & y ) ^ ( ~x & z ); // FIPS-180-4  --- 4.2
}

inline uint32_t Maj( uint32_t x, uint32_t y, uint32_t z ) {
   return ( x & y ) ^ ( x & z ) ^ ( y & z ); // FIPS-180-4  --- 4.3
}

inline uint32_t Sum0( uint32_t x ) {
   return ROTR( x, 2 ) ^ ROTR( x, 13 ) ^ ROTR( x, 22 ); // FIPS-180-4  --- 4.4
}

inline uint32_t Sum1( uint32_t x ) {
   return ROTR( x, 6 ) ^ ROTR( x, 11 ) ^ ROTR( x, 25 ); // FIPS-180-4  --- 4.5
}

inline uint32_t S0( uint32_t x ) {
   return ROTR( x, 7 ) ^ ROTR( x, 18 ) ^ ( x >> 3 ); // FIPS-180-4  --- 4.6
}

inline uint32_t S1( uint32_t x ) {
   return ROTR( x, 17 ) ^ ROTR( x, 19 ) ^ ( x >> 10 ); // FIPS-180-4  --- 4.7
}
}


/*!
 * \brief Calculates the hash for one block
 *
 * This is the main hash function of the SHA 2 algorithm
 * ( 256 AND 224 bit )
 *
 * \param _data A pointer to the data (MUST have 512 bit)
 */
void uSHA_2::block( std::array<unsigned char, 64> const &_data ) {
   const static uint32_t K[] = {
         0x428a2f98,
         0x71374491,
         0xb5c0fbcf,
         0xe9b5dba5,
         0x3956c25b,
         0x59f111f1,
         0x923f82a4,
         0xab1c5ed5,
         0xd807aa98,
         0x12835b01,
         0x243185be,
         0x550c7dc3,
         0x72be5d74,
         0x80deb1fe,
         0x9bdc06a7,
         0xc19bf174,
         0xe49b69c1,
         0xefbe4786,
         0x0fc19dc6,
         0x240ca1cc,
         0x2de92c6f,
         0x4a7484aa,
         0x5cb0a9dc,
         0x76f988da,
         0x983e5152,
         0xa831c66d,
         0xb00327c8,
         0xbf597fc7,
         0xc6e00bf3,
         0xd5a79147,
         0x06ca6351,
         0x14292967,
         0x27b70a85,
         0x2e1b2138,
         0x4d2c6dfc,
         0x53380d13,
         0x650a7354,
         0x766a0abb,
         0x81c2c92e,
         0x92722c85,
         0xa2bfe8a1,
         0xa81a664b,
         0xc24b8b70,
         0xc76c51a3,
         0xd192e819,
         0xd6990624,
         0xf40e3585,
         0x106aa070,
         0x19a4c116,
         0x1e376c08,
         0x2748774c,
         0x34b0bcb5,
         0x391c0cb3,
         0x4ed8aa4a,
         0x5b9cca4f,
         0x682e6ff3,
         0x748f82ee,
         0x78a5636f,
         0x84c87814,
         0x8cc70208,
         0x90befffa,
         0xa4506ceb,
         0xbef9a3f7,
         0xc67178f2,
   };


   uint32_t a, b, c, d, e, f, g, h, t1, t2;
   std::array<uint32_t, 64> word;
   uint16_t t;

   for ( t = 0; t < 16; ++t ) {
      word[t] = ( static_cast<uint32_t>( _data[t * 4 + 0] ) << 24 ) +
                ( static_cast<uint32_t>( _data[t * 4 + 1] ) << 16 ) +
                ( static_cast<uint32_t>( _data[t * 4 + 2] ) << 8 ) +
                ( static_cast<uint32_t>( _data[t * 4 + 3] ) );
   }


   for ( ; t < 64; ++t ) {
      word[t] = S1( word[t - 2] ) + word[t - 7] + S0( word[t - 15] ) + word[t - 16];
   }

   a = h_512[0];
   b = h_512[1];
   c = h_512[2];
   d = h_512[3];
   e = h_512[4];
   f = h_512[5];
   g = h_512[6];
   h = h_512[7];

   for ( t = 0; t < 64; t += 8 ) { // the faster unrolled version
      t1 = h + Sum1( e ) + Ch( e, f, g ) + K[t] + word[t];
      t2 = Sum0( a ) + Maj( a, b, c );
      d += t1;
      h = t1 + t2;

      t1 = g + Sum1( d ) + Ch( d, e, f ) + K[t + 1] + word[t + 1];
      t2 = Sum0( h ) + Maj( h, a, b );
      c += t1;
      g = t1 + t2;

      t1 = f + Sum1( c ) + Ch( c, d, e ) + K[t + 2] + word[t + 2];
      t2 = Sum0( g ) + Maj( g, h, a );
      b += t1;
      f = t1 + t2;

      t1 = e + Sum1( b ) + Ch( b, c, d ) + K[t + 3] + word[t + 3];
      t2 = Sum0( f ) + Maj( f, g, h );
      a += t1;
      e = t1 + t2;

      t1 = d + Sum1( a ) + Ch( a, b, c ) + K[t + 4] + word[t + 4];
      t2 = Sum0( e ) + Maj( e, f, g );
      h += t1;
      d = t1 + t2;

      t1 = c + Sum1( h ) + Ch( h, a, b ) + K[t + 5] + word[t + 5];
      t2 = Sum0( d ) + Maj( d, e, f );
      g += t1;
      c = t1 + t2;

      t1 = b + Sum1( g ) + Ch( g, h, a ) + K[t + 6] + word[t + 6];
      t2 = Sum0( c ) + Maj( c, d, e );
      f += t1;
      b = t1 + t2;

      t1 = a + Sum1( f ) + Ch( f, g, h ) + K[t + 7] + word[t + 7];
      t2 = Sum0( b ) + Maj( b, c, d );
      e += t1;
      a = t1 + t2;
   }

   h_512[0] += a;
   h_512[1] += b;
   h_512[2] += c;
   h_512[3] += d;
   h_512[4] += e;
   h_512[5] += f;
   h_512[6] += g;
   h_512[7] += h;

   ++vBlockCounter_ulI;
}

void uSHA_2::padd512() {
   uint32_t lElementsInBuffer_uI =
         static_cast<uint32_t>( vCurrentPos512_A_IT - vBuffer512_A_uC.begin() );

   // Are is there a full block?
   if ( vCurrentPos512_A_IT == vBuffer512_A_uC.end() ) {
      block( vBuffer512_A_uC );
      vCurrentPos512_A_IT = vBuffer512_A_uC.begin();
      lElementsInBuffer_uI = 0;
   }

   *vCurrentPos512_A_IT = 0x80; // Add the bit 1 in the end
   ++vCurrentPos512_A_IT;

   // -----------------------------------------------
   // - Calculate the number of bits in the message -
   // -----------------------------------------------

   uint32_t v1, v2, temp;

   // vBlockCounter_ulI * 64 = number of bytes in the FULL blocks
   temp = static_cast<uint32_t>( vBlockCounter_ulI );
   v1 = temp << 6;
   v2 = temp >> 26;

   // Add the remaining bytes
   temp = v1;
   if ( ( v1 += lElementsInBuffer_uI ) < temp )
      ++v2;

   // All the bytes * 8 = number of bits in the message
   temp = v1;
   v1 <<= 3;
   v2 <<= 3;
   v2 |= temp >> 29;



   // ------------------
   // - Add the zeros -
   // ------------------

   if ( lElementsInBuffer_uI < 56 ) { // There is enough room for the padding
      // Fill with zeroes
      while ( ( vCurrentPos512_A_IT - vBuffer512_A_uC.begin() ) < 56 ) {
         *vCurrentPos512_A_IT = 0;
         ++vCurrentPos512_A_IT;
      }
   } else { // We need a extra block
      while ( vCurrentPos512_A_IT != vBuffer512_A_uC.end() ) {
         *vCurrentPos512_A_IT = 0;
         ++vCurrentPos512_A_IT;
      }

      block( vBuffer512_A_uC );
      vCurrentPos512_A_IT = vBuffer512_A_uC.begin();

      // Fill with zeros
      while ( ( vCurrentPos512_A_IT - vBuffer512_A_uC.begin() ) < 56 ) {
         *vCurrentPos512_A_IT = 0;
         ++vCurrentPos512_A_IT;
      }
   }

#if COMPILER_CLANG
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wconversion"
#endif

   // -----------------------------
   // - Append the number of bits -
   // -----------------------------

   vBuffer512_A_uC[56] = v2 >> 24;
   vBuffer512_A_uC[57] = v2 >> 16;
   vBuffer512_A_uC[58] = v2 >> 8;
   vBuffer512_A_uC[59] = v2;
   vBuffer512_A_uC[60] = v1 >> 24;
   vBuffer512_A_uC[61] = v1 >> 16;
   vBuffer512_A_uC[62] = v1 >> 8;
   vBuffer512_A_uC[63] = v1;

#if COMPILER_CLANG
#  pragma clang diagnostic pop
#endif

   block( vBuffer512_A_uC );
}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;

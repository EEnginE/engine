/*!
 * \file sha2_512.cpp
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

inline uint64_t ROTR( uint64_t x, uint64_t n ) { return ( x >> n ) | ( x << ( 64 - n ) ); }

inline uint64_t Ch( uint64_t x, uint64_t y, uint64_t z ) {
   return ( x & y ) ^ ( ~x & z ); // FIPS-180-4  --- 4.8
}

inline uint64_t Maj( uint64_t x, uint64_t y, uint64_t z ) {
   return ( x & y ) ^ ( x & z ) ^ ( y & z ); // FIPS-180-4  --- 4.9
}

inline uint64_t Sum0( uint64_t x ) {
   return ROTR( x, 28 ) ^ ROTR( x, 34 ) ^ ROTR( x, 39 ); // FIPS-180-4  --- 4.10
}

inline uint64_t Sum1( uint64_t x ) {
   return ROTR( x, 14 ) ^ ROTR( x, 18 ) ^ ROTR( x, 41 ); // FIPS-180-4  --- 4.11
}

inline uint64_t S0( uint64_t x ) {
   return ROTR( x, 1 ) ^ ROTR( x, 8 ) ^ ( x >> 7 ); // FIPS-180-4  --- 4.12
}

inline uint64_t S1( uint64_t x ) {
   return ROTR( x, 19 ) ^ ROTR( x, 61 ) ^ ( x >> 6 ); // FIPS-180-4  --- 4.13
}
}


/*!
 * \brief calculates the hash for one block
 *
 * This is the main hash function of the SHA 2 algorithm
 * ( 384 AND 512 bit )
 *
 * \param _data A pointer to the data (MUST have 512 bit)
 */
void uSHA_2::block( std::array<unsigned char, 128> const &_data ) {
   const static uint64_t K[] = {0x428a2f98d728ae22,
                                0x7137449123ef65cd,
                                0xb5c0fbcfec4d3b2f,
                                0xe9b5dba58189dbbc,
                                0x3956c25bf348b538,
                                0x59f111f1b605d019,
                                0x923f82a4af194f9b,
                                0xab1c5ed5da6d8118,
                                0xd807aa98a3030242,
                                0x12835b0145706fbe,
                                0x243185be4ee4b28c,
                                0x550c7dc3d5ffb4e2,
                                0x72be5d74f27b896f,
                                0x80deb1fe3b1696b1,
                                0x9bdc06a725c71235,
                                0xc19bf174cf692694,
                                0xe49b69c19ef14ad2,
                                0xefbe4786384f25e3,
                                0x0fc19dc68b8cd5b5,
                                0x240ca1cc77ac9c65,
                                0x2de92c6f592b0275,
                                0x4a7484aa6ea6e483,
                                0x5cb0a9dcbd41fbd4,
                                0x76f988da831153b5,
                                0x983e5152ee66dfab,
                                0xa831c66d2db43210,
                                0xb00327c898fb213f,
                                0xbf597fc7beef0ee4,
                                0xc6e00bf33da88fc2,
                                0xd5a79147930aa725,
                                0x06ca6351e003826f,
                                0x142929670a0e6e70,
                                0x27b70a8546d22ffc,
                                0x2e1b21385c26c926,
                                0x4d2c6dfc5ac42aed,
                                0x53380d139d95b3df,
                                0x650a73548baf63de,
                                0x766a0abb3c77b2a8,
                                0x81c2c92e47edaee6,
                                0x92722c851482353b,
                                0xa2bfe8a14cf10364,
                                0xa81a664bbc423001,
                                0xc24b8b70d0f89791,
                                0xc76c51a30654be30,
                                0xd192e819d6ef5218,
                                0xd69906245565a910,
                                0xf40e35855771202a,
                                0x106aa07032bbd1b8,
                                0x19a4c116b8d2d0c8,
                                0x1e376c085141ab53,
                                0x2748774cdf8eeb99,
                                0x34b0bcb5e19b48a8,
                                0x391c0cb3c5c95a63,
                                0x4ed8aa4ae3418acb,
                                0x5b9cca4f7763e373,
                                0x682e6ff3d6b2b8a3,
                                0x748f82ee5defb2fc,
                                0x78a5636f43172f60,
                                0x84c87814a1f0ab72,
                                0x8cc702081a6439ec,
                                0x90befffa23631e28,
                                0xa4506cebde82bde9,
                                0xbef9a3f7b2c67915,
                                0xc67178f2e372532b,
                                0xca273eceea26619c,
                                0xd186b8c721c0c207,
                                0xeada7dd6cde0eb1e,
                                0xf57d4f7fee6ed178,
                                0x06f067aa72176fba,
                                0x0a637dc5a2c898a6,
                                0x113f9804bef90dae,
                                0x1b710b35131c471b,
                                0x28db77f523047d84,
                                0x32caab7b40c72493,
                                0x3c9ebe0a15c9bebc,
                                0x431d67c49c100d4c,
                                0x4cc5d4becb3e42b6,
                                0x597f299cfc657e2a,
                                0x5fcb6fab3ad6faec,
                                0x6c44198c4a475817};

   uint64_t a, b, c, d, e, f, g, h, t1, t2;
   std::array<uint64_t, 80> word;
   uint16_t t;

   for ( t = 0; t < 16; ++t ) {
      word[t] = ( static_cast<uint64_t>( _data[t * 8 + 0] ) << 56 ) +
                ( static_cast<uint64_t>( _data[t * 8 + 1] ) << 48 ) +
                ( static_cast<uint64_t>( _data[t * 8 + 2] ) << 40 ) +
                ( static_cast<uint64_t>( _data[t * 8 + 3] ) << 32 ) +
                ( static_cast<uint64_t>( _data[t * 8 + 4] ) << 24 ) +
                ( static_cast<uint64_t>( _data[t * 8 + 5] ) << 16 ) +
                ( static_cast<uint64_t>( _data[t * 8 + 6] ) << 8 ) +
                ( static_cast<uint64_t>( _data[t * 8 + 7] ) );
   }

   for ( ; t < 80; ++t ) {
      word[t] = S1( word[t - 2] ) + word[t - 7] + S0( word[t - 15] ) + word[t - 16];
   }

   a = h_1024[0];
   b = h_1024[1];
   c = h_1024[2];
   d = h_1024[3];
   e = h_1024[4];
   f = h_1024[5];
   g = h_1024[6];
   h = h_1024[7];

   for ( t = 0; t < 80; t += 8 ) { // the faster unrolled version
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

   h_1024[0] += a;
   h_1024[1] += b;
   h_1024[2] += c;
   h_1024[3] += d;
   h_1024[4] += e;
   h_1024[5] += f;
   h_1024[6] += g;
   h_1024[7] += h;

   ++vBlockCounter_ulI;
}



void uSHA_2::padd1024() {
   uint64_t lElementsInBuffer_uI =
         static_cast<uint64_t>( vCurrentPos1024_A_IT - vBuffer1024_A_uC.begin() );

   // Are is there a full block?
   if ( vCurrentPos1024_A_IT == vBuffer1024_A_uC.end() ) {
      block( vBuffer1024_A_uC );
      vCurrentPos1024_A_IT = vBuffer1024_A_uC.begin();
      lElementsInBuffer_uI = 0;
   }

   *vCurrentPos1024_A_IT = 0x80; // Add the bit 1 in the end
   ++vCurrentPos1024_A_IT;

   // -----------------------------------------------
   // - Calculate the number of bits in the message -
   // -----------------------------------------------

   uint64_t v1, v2, temp;

   // vBlockCounter_ulI * 128 = number of bytes int the FULL blocks
   temp = vBlockCounter_ulI;
   v1 = temp << 7;
   v2 = temp >> 57;

   // Add the remaining bytes
   temp = v1;
   if ( ( v1 += lElementsInBuffer_uI ) < temp )
      ++v2;

   // All the bytes * 8 = number of bits in the message
   temp = v1;
   v1 <<= 3;
   v2 <<= 3;
   v2 |= temp >> 61;



   // ------------------
   // - Add the zeros -
   // ------------------

   if ( lElementsInBuffer_uI < 112 ) { // There is enough room for the padding
      // Fill with zeroes
      while ( ( vCurrentPos1024_A_IT - vBuffer1024_A_uC.begin() ) < 112 ) {
         *vCurrentPos1024_A_IT = 0;
         ++vCurrentPos1024_A_IT;
      }
   } else { // We need a extra block
      while ( vCurrentPos1024_A_IT != vBuffer1024_A_uC.end() ) {
         *vCurrentPos1024_A_IT = 0;
         ++vCurrentPos1024_A_IT;
      }

      block( vBuffer1024_A_uC );
      vCurrentPos1024_A_IT = vBuffer1024_A_uC.begin();

      // Fill with zeros
      while ( ( vCurrentPos1024_A_IT - vBuffer1024_A_uC.begin() ) < 112 ) {
         *vCurrentPos1024_A_IT = 0;
         ++vCurrentPos1024_A_IT;
      }
   }


#if COMPILER_CLANG
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wconversion"
#endif

   // -----------------------------
   // - Append the number of bits -
   // -----------------------------

   vBuffer1024_A_uC[112] = v2 >> 56;
   vBuffer1024_A_uC[113] = v2 >> 48;
   vBuffer1024_A_uC[114] = v2 >> 40;
   vBuffer1024_A_uC[115] = v2 >> 32;
   vBuffer1024_A_uC[116] = v2 >> 24;
   vBuffer1024_A_uC[117] = v2 >> 16;
   vBuffer1024_A_uC[118] = v2 >> 8;
   vBuffer1024_A_uC[119] = v2;
   vBuffer1024_A_uC[120] = v1 >> 56;
   vBuffer1024_A_uC[121] = v1 >> 48;
   vBuffer1024_A_uC[122] = v1 >> 40;
   vBuffer1024_A_uC[123] = v1 >> 32;
   vBuffer1024_A_uC[124] = v1 >> 24;
   vBuffer1024_A_uC[125] = v1 >> 16;
   vBuffer1024_A_uC[126] = v1 >> 8;
   vBuffer1024_A_uC[127] = v1;

#if COMPILER_CLANG
#  pragma clang diagnostic pop
#endif


   block( vBuffer1024_A_uC );
}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;

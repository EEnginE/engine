/*!
 * \file uSHA_2.cpp
 * \brief \b Classes: \a uSHA_2
 */

#include "uSHA_2.hpp"
#include <stdio.h>

namespace e_engine {

/*!
 * \brief Constructor from uSHA_2
 * 
 * \note If _type is not a SHA2 function, SHA-256 wil be used
 */
uSHA_2::uSHA_2( HASH_FUNCTION _type ) {
   if ( _type == SHA2_224 || _type == SHA2_256 || _type == SHA2_384 || _type == SHA2_512 ) {
      vType          = _type;
   } else {
      vType          = SHA2_256;
   }
   vBlockCounter_ulI = 0;

   vEnded_B          = false;

   if ( vType == SHA2_224 || vType == SHA2_256 ) {
      vBlockSize_uI = 512  / 8; // 64
   } else {
      vBlockSize_uI = 1024 / 8; // 128
   }

   vCurrentPos512_A_IT  = vBuffer512_A_uC.begin();
   vCurrentPos1024_A_IT = vBuffer1024_A_uC.begin();

   init();
}


/*!
 * \brief Fills the data in a buffer and calculates the hash when it's full
 *
 * \param _message What should be hashed
 * \returns false if the hash is already calculated or true if all went fine
 */
bool uSHA_2::add( std::string const &_message ) {
   if ( vEnded_B )
      return false;

   if ( vType == SHA2_224 || vType == SHA2_256 ) {
      for ( unsigned char const & c : _message ) {

         if ( vCurrentPos512_A_IT == vBuffer512_A_uC.end() ) {
            block( vBuffer512_A_uC );
            vCurrentPos512_A_IT = vBuffer512_A_uC.begin();
         }

         *vCurrentPos512_A_IT = c;
         ++vCurrentPos512_A_IT;
      }
   } else {
      for ( unsigned char const & c : _message ) {

         if ( vCurrentPos1024_A_IT == vBuffer1024_A_uC.end() ) {
            block( vBuffer1024_A_uC );
            vCurrentPos1024_A_IT = vBuffer1024_A_uC.begin();
         }

         *vCurrentPos1024_A_IT = c;
         ++vCurrentPos1024_A_IT;
      }
   }

   return true;
}

/*!
 * \brief Fills the data in a buffer and calculates the hash when it's full
 *
 * \param _binary What should be hashed
 * \returns false if the hash is already calculated or true if all went fine
 */
bool uSHA_2::add( std::vector< unsigned char > const &_binary ) {
   if ( vEnded_B )
      return false;

   if ( vType == SHA2_224 || vType == SHA2_256 ) {
      for ( unsigned char const & c : _binary ) {

         if ( vCurrentPos512_A_IT == vBuffer512_A_uC.end() ) {
            block( vBuffer512_A_uC );
            vCurrentPos512_A_IT = vBuffer512_A_uC.begin();
         }

         *vCurrentPos512_A_IT = c;
         ++vCurrentPos512_A_IT;
      }
   } else {
      for ( unsigned char const & c : _binary ) {

         if ( vCurrentPos1024_A_IT == vBuffer1024_A_uC.end() ) {
            block( vBuffer1024_A_uC );
            vCurrentPos1024_A_IT = vBuffer1024_A_uC.begin();
         }

         *vCurrentPos1024_A_IT = c;
         ++vCurrentPos1024_A_IT;
      }
   }

   return true;
}


std::vector< unsigned char > uSHA_2::quickHash( HASH_FUNCTION _type, std::string _message ) {
   reset( _type );
   add( _message );
   return end();
}


std::vector< unsigned char > uSHA_2::quickHash( HASH_FUNCTION _type, std::vector< unsigned char > _binary ) {
   reset( _type );
   add( _binary );
   return end();
}



void uSHA_2::init() {
   switch ( vType ) {
      case SHA2_224:
         h_512[0] = 0xc1059ed8;
         h_512[1] = 0x367cd507;
         h_512[2] = 0x3070dd17;
         h_512[3] = 0xf70e5939;
         h_512[4] = 0xffc00b31;
         h_512[5] = 0x68581511;
         h_512[6] = 0x64f98fa7;
         h_512[7] = 0xbefa4fa4;
         break;

      case SHA2_256:
         h_512[0] = 0x6a09e667;
         h_512[1] = 0xbb67ae85;
         h_512[2] = 0x3c6ef372;
         h_512[3] = 0xa54ff53a;
         h_512[4] = 0x510e527f;
         h_512[5] = 0x9b05688c;
         h_512[6] = 0x1f83d9ab;
         h_512[7] = 0x5be0cd19;
         break;

      case SHA2_384:
         h_1024[0] = 0xcbbb9d5dc1059ed8;
         h_1024[1] = 0x629a292a367cd507;
         h_1024[2] = 0x9159015a3070dd17;
         h_1024[3] = 0x152fecd8f70e5939;
         h_1024[4] = 0x67332667ffc00b31;
         h_1024[5] = 0x8eb44a8768581511;
         h_1024[6] = 0xdb0c2e0d64f98fa7;
         h_1024[7] = 0x47b5481dbefa4fa4;
         break;

      case SHA2_512:
         h_1024[0] = 0x6a09e667f3bcc908;
         h_1024[1] = 0xbb67ae8584caa73b;
         h_1024[2] = 0x3c6ef372fe94f82b;
         h_1024[3] = 0xa54ff53a5f1d36f1;
         h_1024[4] = 0x510e527fade682d1;
         h_1024[5] = 0x9b05688c2b3e6c1f;
         h_1024[6] = 0x1f83d9abfb41bd6b;
         h_1024[7] = 0x5be0cd19137e2179;
         break;
   }
}

/*!
 * \brief Resets all hashing in progress and sets the new Hash function
 *
 * If _type is NOT a SHA2 hash function, the last SHA function will be used
 */
void uSHA_2::reset( HASH_FUNCTION _type ) {
   if ( _type == SHA2_224 || _type == SHA2_256 || _type == SHA2_384 || _type == SHA2_512 ) {
      vType             = _type;
   }
   vBlockCounter_ulI = 0;

   vEnded_B          = false;

   vCurrentPos512_A_IT  = vBuffer512_A_uC.begin();
   vCurrentPos1024_A_IT = vBuffer1024_A_uC.begin();
   vResult_str.clear();

   if ( vType == SHA2_224 || vType == SHA2_256 ) {
      vBlockSize_uI = 512  / 8;
   } else {
      vBlockSize_uI = 1024 / 8;
   }

   init();
}

namespace {

void int32ToString( std::vector<unsigned char> &_str, uint32_t const &_num, uint32_t _level ) {
   _level *= 4;
   _str[_level]     = ( unsigned char )( _num >> 24 );
   _str[_level + 1] = ( unsigned char )( _num >> 16 );
   _str[_level + 2] = ( unsigned char )( _num >> 8 );
   _str[_level + 3] = ( unsigned char )( _num );
}

void int64ToString( std::vector<unsigned char> &_str, uint64_t const &_num, uint32_t _level ) {
   _level *= 8;
   _str[_level]     = ( unsigned char )( _num >> 56 );
   _str[_level + 1] = ( unsigned char )( _num >> 48 );
   _str[_level + 2] = ( unsigned char )( _num >> 40 );
   _str[_level + 3] = ( unsigned char )( _num >> 32 );
   _str[_level + 4] = ( unsigned char )( _num >> 24 );
   _str[_level + 5] = ( unsigned char )( _num >> 16 );
   _str[_level + 6] = ( unsigned char )( _num >> 8 );
   _str[_level + 7] = ( unsigned char )( _num );
}

}

/*!
 * \brief Ends the hash operation and returns the hash (std::string)
 *
 * \returns The Hash or an empty string when already ended.
 */
std::vector<unsigned char> uSHA_2::end() {
   if ( vEnded_B )
      return std::vector<unsigned char>();

   std::vector<unsigned char> lRsult_V_uC;

   if ( vType == SHA2_224 || vType == SHA2_256 ) {
      padd512();
   } else {
      padd1024();
   }

   switch ( vType ) {
      case SHA2_224:
         lRsult_V_uC.resize( 224 / 8 );
         int32ToString( lRsult_V_uC, h_512[0], 0 );
         int32ToString( lRsult_V_uC, h_512[1], 1 );
         int32ToString( lRsult_V_uC, h_512[2], 2 );
         int32ToString( lRsult_V_uC, h_512[3], 3 );
         int32ToString( lRsult_V_uC, h_512[4], 4 );
         int32ToString( lRsult_V_uC, h_512[5], 5 );
         int32ToString( lRsult_V_uC, h_512[6], 6 );
         break;

      case SHA2_256:
         lRsult_V_uC.resize( 256 / 8 );
         int32ToString( lRsult_V_uC, h_512[0], 0 );
         int32ToString( lRsult_V_uC, h_512[1], 1 );
         int32ToString( lRsult_V_uC, h_512[2], 2 );
         int32ToString( lRsult_V_uC, h_512[3], 3 );
         int32ToString( lRsult_V_uC, h_512[4], 4 );
         int32ToString( lRsult_V_uC, h_512[5], 5 );
         int32ToString( lRsult_V_uC, h_512[6], 6 );
         int32ToString( lRsult_V_uC, h_512[7], 7 );
         break;

      case SHA2_384:
         lRsult_V_uC.resize( 384 / 8 );
         int64ToString( lRsult_V_uC, h_1024[0], 0 );
         int64ToString( lRsult_V_uC, h_1024[1], 1 );
         int64ToString( lRsult_V_uC, h_1024[2], 2 );
         int64ToString( lRsult_V_uC, h_1024[3], 3 );
         int64ToString( lRsult_V_uC, h_1024[4], 4 );
         int64ToString( lRsult_V_uC, h_1024[5], 5 );
         break;

      case SHA2_512:
         lRsult_V_uC.resize( 512 / 8 );
         int64ToString( lRsult_V_uC, h_1024[0], 0 );
         int64ToString( lRsult_V_uC, h_1024[1], 1 );
         int64ToString( lRsult_V_uC, h_1024[2], 2 );
         int64ToString( lRsult_V_uC, h_1024[3], 3 );
         int64ToString( lRsult_V_uC, h_1024[4], 4 );
         int64ToString( lRsult_V_uC, h_1024[5], 5 );
         int64ToString( lRsult_V_uC, h_1024[6], 6 );
         int64ToString( lRsult_V_uC, h_1024[7], 7 );
         break;
   }

   vEnded_B = true;
   return lRsult_V_uC;
}

/*!
 * \brief Returns the hash as a string
 *
 * \param _space Puts a space after 8 chars when true (default false)
 * \returns The hash as a string or an empty string when not already terminated
 */
std::string uSHA_2::get( bool _space ) {
   if ( ! vEnded_B )
      return "";

   std::string lString_str;
   char lBuffer_CSTR[9];
   uint16_t lEnd_suI = 8;

   switch ( vType ) {
      case SHA2_224: lEnd_suI = 7;
      case SHA2_256:
         for ( uint16_t i = 0; i < lEnd_suI; ++i ) {
		#ifdef _MSC_VER
			_snprintf(lBuffer_CSTR, 9, "%08x", h_512[i] );
		#else
            snprintf( lBuffer_CSTR, 9, "%08x", h_512[i] );
		#endif

            lString_str += lBuffer_CSTR;

            if ( _space && i != ( lEnd_suI - 1 ) )
               lString_str += ' ';
         }
         break;

      case SHA2_384: lEnd_suI = 6;
      case SHA2_512:
         uint32_t v1, v2;
         for ( uint16_t i = 0; i < lEnd_suI; ++i ) {
            v1 = ( uint32_t )( h_1024[i] >> 32 );
            v2 = ( uint32_t )( h_1024[i] );
			#ifdef _MSC_VER
				_snprintf(lBuffer_CSTR, 9, "%08x", v1);
			#else
				snprintf( lBuffer_CSTR, 9, "%08x", v1 );
			#endif
            lString_str += lBuffer_CSTR;

            if ( _space )
               lString_str += ' ';

			#ifdef _MSC_VER
				_snprintf(lBuffer_CSTR, 9, "%08x", v2);
			#else
				snprintf(lBuffer_CSTR, 9, "%08x", v2);
			#endif
            lString_str += lBuffer_CSTR;

            if ( _space && i != ( lEnd_suI - 1 ) )
               lString_str += ' ';
         }
         break;
   }

   return lString_str;
}

unsigned int uSHA_2::getHashLength() {
   switch( vType ) {
      case SHA2_224: return 224 / 8;
      case SHA2_256: return 256 / 8;
      case SHA2_384: return 384 / 8;
      case SHA2_512: return 512 / 8;
      default: return 0;
   }
}


}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;



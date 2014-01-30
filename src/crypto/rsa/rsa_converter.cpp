/*!
 * \file rsa_converter.cpp
 * \brief \b Classes: \a RSA (key generation)
 */

#include "rsa.hpp"
#include <stdio.h>
#include <boost/lexical_cast.hpp>

namespace e_engine {


void RSA::Bytes2Int( std::vector<unsigned char> const &_bytes, mpz_t &_int ) const {
   if ( _bytes.empty() )
      return;

   static const char *lHex_CSTR = "0123456789abcdef";

   char *lGmpNumber_A_uC = new char[_bytes.size() * 2 + 1];
   char *lWorker_uC      = lGmpNumber_A_uC;

   for ( unsigned char const & c : _bytes ) {
      *lWorker_uC++ = lHex_CSTR[ c >> 4 ];
      *lWorker_uC++ = lHex_CSTR[ c & 0x0F ];
   }

   *lWorker_uC = 0;

   mpz_set_str( _int, lGmpNumber_A_uC, 16 );

   delete[] lGmpNumber_A_uC;
}

void RSA::Int2Bytes( std::vector<unsigned char> &_bytes, mpz_t const &_int, unsigned int _fixedSize ) const {

   unsigned int lSizeOfBasicGMPNum_uI = mpz_sizeinbase( _int, 16 );

   char *lTemp_CSTR = new char[lSizeOfBasicGMPNum_uI + 3];
   mpz_get_str( lTemp_CSTR, 16, _int );

   unsigned int lPos_uI = 0;
   char *lWorker_C = lTemp_CSTR;

   if ( ( lSizeOfBasicGMPNum_uI % 2 ) != 0 ) {
      ++lSizeOfBasicGMPNum_uI;
      if ( _fixedSize > 0 ) {
         _bytes.resize( _fixedSize );

         if ( ( lSizeOfBasicGMPNum_uI / 2 ) < _fixedSize ) {
            for( unsigned int i = 0; i < ( _fixedSize - ( lSizeOfBasicGMPNum_uI / 2 ) ) ; ++i ) {
               _bytes[ lPos_uI++ ] = 0x00;
            }
         }
      } else {
         _bytes.resize( lSizeOfBasicGMPNum_uI / 2 );
      }

      _bytes[lPos_uI] = 0;
      if ( *lWorker_C > '9' )
         _bytes[lPos_uI] |= *lWorker_C - 'a' + 10;
      else
         _bytes[lPos_uI] |= *lWorker_C - '0';
      ++lWorker_C;
      ++lPos_uI;
   } else {
      if ( _fixedSize > 0 ) {
         _bytes.resize( _fixedSize );

         if ( ( lSizeOfBasicGMPNum_uI / 2 ) < _fixedSize ) {
            for( unsigned int i = 0; i < ( _fixedSize - ( lSizeOfBasicGMPNum_uI / 2 ) ) ; ++i ) {
               _bytes[ lPos_uI++ ] = 0x00;
            }
         }
      } else {
         _bytes.resize( lSizeOfBasicGMPNum_uI / 2 );
      }
   }

   while ( lPos_uI < _bytes.size() ) {
      if ( *lWorker_C > '9' )
         _bytes[lPos_uI] = ( ( *lWorker_C - 'a' ) + 10 ) << 4;
      else
         _bytes[lPos_uI] = ( *lWorker_C - '0' ) << 4;
      ++lWorker_C;
      if ( *lWorker_C > '9' )
         _bytes[lPos_uI] |= *lWorker_C - 'a' + 10;
      else
         _bytes[lPos_uI] |= *lWorker_C - '0';
      ++lWorker_C;
      ++lPos_uI;
   }


   delete[] lTemp_CSTR;
}


void RSA::FourChars2Int( unsigned char *_data, uint32_t &_int ) {
   _int     = 0;
   _int    |= _data[0] << 24;
   _int    |= _data[1] << 16;
   _int    |= _data[2] << 8;
   _int    |= _data[3];
}

void RSA::Int2FourChars( unsigned char *_data, uint32_t _int ) {
   _data[0] = _int >> 24;
   _data[1] = _int >> 16;
   _data[2] = _int >> 8;
   _data[3] = _int;
}

/*!
 * \brief Get all numbers
 *
 * Range of _base: 2 - 62
 *
 * | _data Level | Number |
 * | :---------: | :----: |
 * | 0           | p      |
 * | 1           | q      |
 * | 2           | n      |
 * | 3           | e      |
 * | 4           | d      |
 * | 5           | dP     |
 * | 6           | dQ     |
 * | 7           | qInv   |
 *
 * \param[out] _data The numbers
 * \param[in]  _base The base of the number
 *
 * \returns true whenn everything went fine
 * \returns false if _base is out of range
 */
bool RSA::getNumbers( std::vector<std::vector<unsigned char>> &_data, unsigned int _base ) {
   if ( _base < 2 || _base > 62 )
      return false;

   _data.clear();
   _data.resize( 8 );

   if ( ! vHavePrivateKey_B && ! vHavePublicKey_B )
      return true;

   char *lWorker_C;

   char *ln_CSTR = new char[mpz_sizeinbase( n, _base ) + 2];
   mpz_get_str( ln_CSTR, _base, n );

   _data[2].resize( mpz_sizeinbase( n, _base ) );
   lWorker_C = ln_CSTR;
   for ( unsigned char & c : _data[2] ) {
      c = *lWorker_C;
      ++lWorker_C;
   }

   delete[] ln_CSTR;

   if ( vHavePublicKey_B ) {
      char *le_CSTR = new char[mpz_sizeinbase( e, _base ) + 2];
      mpz_get_str( le_CSTR, _base, e );

      _data[3].resize( mpz_sizeinbase( e, _base ) );
      lWorker_C = le_CSTR;
      for ( unsigned char & c : _data[3] ) {
         c = *lWorker_C;
         ++lWorker_C;
      }

      delete[] le_CSTR;
   }

   if ( vHavePrivateKey_B ) {
      char *lp_CSTR    = new char[mpz_sizeinbase( p,    _base ) + 2];
      char *lq_CSTR    = new char[mpz_sizeinbase( q,    _base ) + 2];
      char *ld_CSTR    = new char[mpz_sizeinbase( d,    _base ) + 2];
      char *ldP_CSTR   = new char[mpz_sizeinbase( dP,   _base ) + 2];
      char *ldQ_CSTR   = new char[mpz_sizeinbase( dQ,   _base ) + 2];
      char *lqInv_CSTR = new char[mpz_sizeinbase( qInv, _base ) + 2];

      mpz_get_str( lp_CSTR,    _base, p );
      mpz_get_str( lq_CSTR,    _base, q );
      mpz_get_str( ld_CSTR,    _base, d );
      mpz_get_str( ldP_CSTR,   _base, dP );
      mpz_get_str( ldQ_CSTR,   _base, dQ );
      mpz_get_str( lqInv_CSTR, _base, qInv );

      _data[0].resize( mpz_sizeinbase( p, _base ) );
      lWorker_C = lp_CSTR;
      for ( unsigned char & c : _data[0] ) {
         c = *lWorker_C;
         ++lWorker_C;
      }

      _data[1].resize( mpz_sizeinbase( q, _base ) );
      lWorker_C = lq_CSTR;
      for ( unsigned char & c : _data[1] ) {
         c = *lWorker_C;
         ++lWorker_C;
      }

      _data[4].resize( mpz_sizeinbase( d, _base ) );
      lWorker_C = ld_CSTR;
      for ( unsigned char & c : _data[4] ) {
         c = *lWorker_C;
         ++lWorker_C;
      }

      _data[5].resize( mpz_sizeinbase( dP, _base ) );
      lWorker_C = ldP_CSTR;
      for ( unsigned char & c : _data[5] ) {
         c = *lWorker_C;
         ++lWorker_C;
      }

      _data[6].resize( mpz_sizeinbase( dQ, _base ) );
      lWorker_C = ldQ_CSTR;
      for ( unsigned char & c : _data[6] ) {
         c = *lWorker_C;
         ++lWorker_C;
      }

      _data[7].resize( mpz_sizeinbase( qInv, _base ) );
      lWorker_C = lqInv_CSTR;
      for ( unsigned char & c : _data[7] ) {
         c = *lWorker_C;
         ++lWorker_C;
      }


      delete[] lp_CSTR;
      delete[] lq_CSTR;
      delete[] ld_CSTR;
      delete[] ldP_CSTR;
      delete[] ldQ_CSTR;
      delete[] lqInv_CSTR;
   }

   return true;
}

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

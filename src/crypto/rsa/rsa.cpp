/*!
 * \file rsa.cpp
 * \brief \b Classes: \a RSA
 */

#include "rsa.hpp"
#include "rand_isaac.hpp"
#include <iostream>

namespace e_engine {

RSA::RSA() {
   vBits_I                 = -1;
   vBlockSize_uI           = 0;
   vRealCryptoBlockSize_uI = 0;

   vHavePrivateKey_B       = false;
   vHavePublicKey_B        = false;

   vHashFunction_E         = SHA2_256;
   vHashLength_uI          = 32;

   mpz_init( n );
   mpz_init( e );
   mpz_init( d );

   mpz_init( p );
   mpz_init( q );
   mpz_init( dP );
   mpz_init( dQ );
   mpz_init( qInv );
}

RSA::~RSA() {
   mpz_clear( n );
   mpz_clear( e );
   mpz_clear( d );

   mpz_clear( p );
   mpz_clear( q );
   mpz_clear( dP );
   mpz_clear( dQ );
   mpz_clear( qInv );
}

void RSA::reset() {
   vBits_I                 = -1;
   vBlockSize_uI           = 0;
   vRealCryptoBlockSize_uI = 0;

   vHavePrivateKey_B       = false;
   vHavePublicKey_B        = false;

   vHashFunction_E         = SHA2_256;
   vHashLength_uI          = 32;

   mpz_clear( n );
   mpz_clear( e );
   mpz_clear( d );

   mpz_clear( p );
   mpz_clear( q );
   mpz_clear( dP );
   mpz_clear( dQ );
   mpz_clear( qInv );



   mpz_init( n );
   mpz_init( e );
   mpz_init( d );

   mpz_init( p );
   mpz_init( q );
   mpz_init( dP );
   mpz_init( dQ );
   mpz_init( qInv );
}

/*!
 * \brief Set the hash function to use for padding
 *
 * Supported functions: SHA2-224, SHA2-256, SHA2-384, SHA2-512
 *
 * \param _func The hash function to use
 *
 * \returns false when the hash function is not supported, else true
 */
bool RSA::setHashFunction( HASH_FUNCTION _func ) {
   switch ( _func ) {
      case SHA2_224: vHashLength_uI = 28; break;
      case SHA2_256: vHashLength_uI = 32; break;
      case SHA2_384: vHashLength_uI = 48; break;
      case SHA2_512: vHashLength_uI = 64; break;
      default: return false;
   }

   if ( vBlockSize_uI != 0 )
      vRealCryptoBlockSize_uI = vBlockSize_uI - ( 2 * vHashLength_uI - 2 );

   vHashFunction_E = _func;
   return true;
}

}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

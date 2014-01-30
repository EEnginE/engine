/*!
 * \file rsa_generator.cpp
 * \brief \b Classes: \a RSA (key generation)
 */

#include "rsa.hpp"
#include "rand_isaac.hpp"
#include "rsa_save_bits.hpp"
#include <boost/thread.hpp>
// #include "log.hpp"

namespace e_engine {


/*!
 * \brief generates a RSA key pair
 *
 * \param _nBits     the length of the key
 * \param _blockSize the number of chars to encrypt at once. <b>leave it ZERO if you don't know what you are doing</b>
 *
 * \note Both keys will be reset
 *
 * \returns 1  on success
 * \returns 0  if _nBits is less 512 (the min key length)
 * \returns -1 if already a key is stored
 */
int RSA::generateNewKeyPair( unsigned int _nBits ) {
   if ( vBits_I > 0 )
      return -1;

   if ( _nBits < 512 )
      return 0;

   RandISAAC lRandom;

   mpz_t x, pm1, qm1, gcd;

   mpz_init( x );
   mpz_init( pm1 );
   mpz_init( qm1 );
   mpz_init( gcd );


   char *lPrimeBuffer = new char[( _nBits / 2 ) + 1];

   // Prime one (p)
   lPrimeBuffer[0] = '1';
   for ( unsigned i = 1; i < ( _nBits / 2 ); ++i )
      lPrimeBuffer[i] =  lRandom( ( int )'0', ( int )'1' ); // create char '0' or '1'
   lPrimeBuffer[_nBits / 2] = '\0';
   mpz_set_str( p, lPrimeBuffer, 2 );


   // Prime one (q)
   lPrimeBuffer[0] = '1';
   for ( unsigned i = 1; i < ( _nBits / 2 ); ++i )
      lPrimeBuffer[i] =  lRandom( ( int )'0', ( int )'1' ); // create char '0' or '1'
   lPrimeBuffer[_nBits / 2] = '\0';
   mpz_set_str( q, lPrimeBuffer, 2 );

   delete[] lPrimeBuffer;

   boost::thread t1( mpz_nextprime, p, p ); // make it to a prime
   boost::thread t2( mpz_nextprime, q, q ); // make it to a prime

   t1.join();
   t2.join();

   // --- Make sure P != Q
   if ( mpz_cmp( p, q ) == 0 ) {
      // Well... this is not verry likely but unfortunately possible
      // Try again
      return generateNewKeyPair( _nBits );
   }

   // --- Make P > Q
   if ( mpz_cmp( p, q ) < 0 )
      mpz_swap( p, q );

   // --- Claculate n                                  [n = p * q]
   mpz_mul( n, p, q );
   mpz_set( n, n );

   // --- Calculate x                                  [x = (p - 1) * (q - 1)]
   mpz_sub_ui( pm1, p, 1 );
   mpz_sub_ui( qm1, q, 1 );
   mpz_mul( x, pm1, qm1 );

   // --- Calculate e                                  [3 < e < x AND gcd(e,x) = 1]
   mpz_set_ui( e, 65537 );

   while ( true ) {
      mpz_gcd( gcd, e, x );

      if ( mpz_cmp_ui( gcd, 1 ) == 0 ) // gcd == 1
         break;

      mpz_add_ui( e, e, 1 );
   }

   // --- Calculate d                                  [d = 1/e mod x]
   if ( mpz_invert( d, e, x ) == 0 ) {
      // Well there is no invert... (shouldn't happen)
      // ... try again
      mpz_clear( x );
      mpz_clear( pm1 );
      mpz_clear( qm1 );
      mpz_clear( gcd );
      return generateNewKeyPair( _nBits );
   }

   // --- Calculate dP                                 [dP = d mod (p - 1)]
   mpz_mod( dP, d, pm1 );

   // --- Calculate dP                                 [dQ = d mod (q - 1)]
   mpz_mod( dQ, d, qm1 );

   // --- Calculate qInv                               [qInv = 1/q mod p]
   if ( mpz_invert( qInv, q, p ) == 0 ) {
      // Well there is no invert... :(
      // ... try again
      mpz_clear( x );
      mpz_clear( pm1 );
      mpz_clear( qm1 );
      mpz_clear( gcd );
      return generateNewKeyPair( _nBits );
   }

   // --- Claculate k (vBlockSize_uI)
   vBlockSize_uI = getSaveBlockSize( _nBits );

   // DONE
   
   vRealCryptoBlockSize_uI = vBlockSize_uI - ( 2 * vHashLength_uI ) - 2;


   // Cleare the useless stuff
   mpz_clear( x );
   mpz_clear( pm1 );
   mpz_clear( qm1 );
   mpz_clear( gcd );

   vBits_I           = _nBits;

   vHavePrivateKey_B = true;
   vHavePublicKey_B  = true;

   return 1;
}


}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

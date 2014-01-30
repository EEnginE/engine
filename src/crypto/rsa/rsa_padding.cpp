/*!
 * \file rsa_padding.cpp
 * \brief \b Classes: \a RSA (padding (RSAES-OAEP))
 */

#include "rsa.hpp"
#include "rand_isaac.hpp"
#include <stdio.h>

namespace e_engine {

/*!
 * \brief The Mask generation function
 * \param[in]  _seed The input seed
 * \param[out] _out  The output
 * \param[in]  _len  The output length
 */
void RSA::MGF1( std::vector<unsigned char> const &_seed, std::vector<unsigned char> &_out, unsigned int _len ) const {
   _out.clear();
   SHA_2 lHash( vHashFunction_E );

   for ( uint32_t i = 0; _out.size() < _len; ++i ) {
      std::vector<unsigned char> lStringToHash_V_uC = _seed;
      lStringToHash_V_uC.push_back( ( unsigned char )( i >> 24 ) );
      lStringToHash_V_uC.push_back( ( unsigned char )( i >> 16 ) );
      lStringToHash_V_uC.push_back( ( unsigned char )( i >> 8 ) );
      lStringToHash_V_uC.push_back( ( unsigned char )( i ) );

      std::vector<unsigned char> lHashResult = lHash.quickHash( vHashFunction_E, lStringToHash_V_uC );

      _out.insert( _out.end(), lHashResult.begin(), lHashResult.end() );
   }

   _out.resize( _len );
}


/*!
 * \brief The RSAES-OAEP Padding function (encoding)
 *
 * \param[in]  _in    The message input
 * \param[out] _out   The padded message
 * \param[in]  _label OPTIONAL label. Empty if not set
 *
 * \note _out will ALWAYS be cleared when the encoding starts.
 *
 * \returns 1  on success
 * \returns -1 when _in is empty
 * \returns -2 when _in.size() > ( vBlockSize_uI - ( 2 * hashLen ) - 2 )
 */
int RSA::encode( std::vector<unsigned char> const &_in, std::vector<unsigned char> &_out, std::vector<unsigned char> const &_label ) const {
   // ------------------------------------------------------------------------------------------------------
   // -----        -----------------------------------------------------------------------------------------------
   // ---  Checking  ------------------------------------------------------------------------------------------------
   // -----        -----------------------------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------
   if ( _in.empty() )
      return -1;

   SHA_2 lHashFunction( vHashFunction_E );

   if ( _in.size() > ( vBlockSize_uI - ( 2 * lHashFunction.getHashLength() ) - 2 ) )
      return -2;


   // ------------------------------------------------------------------------------------------------------
   // -----                               ------------------------------------------------------------------------
   // ---  Variables (names from RFC 3447)  -----------------------------------------------------------------
   // -----                               ------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------
   unsigned int               hLen = lHashFunction.getHashLength();

   std::vector<unsigned char> lHash;
   std::vector<unsigned char> PS;
   std::vector<unsigned char> DB;
   std::vector<unsigned char> seed;
   std::vector<unsigned char> dbMask;
   std::vector<unsigned char> maskedDB;
   std::vector<unsigned char> seedMask;
   std::vector<unsigned char> maskedSeed;

   uint64_t lNumOfZeroes_uI = vBlockSize_uI - _in.size() - ( 2 * hLen ) - 2;


   // ------------------------------------------------------------------------------------------------------
   // -----                      ---------------------------------------------------------------------------------
   // ---  lHash = HASH( _label )  ----------------------------------------------------------------------------------
   // -----                      ---------------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   lHash = lHashFunction.quickHash( vHashFunction_E, _label );


   // ------------------------------------------------------------------------------------------------------
   // -----                       --------------------------------------------------------------------------------
   // ---  Generate zero string PS  ---------------------------------------------------------------------------------
   // -----                       --------------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   PS.clear();
   if ( lNumOfZeroes_uI != 0 )
      PS.insert( PS.begin(), lNumOfZeroes_uI, 0x00 );


   // ------------------------------------------------------------------------------------------------------
   // -----                                     ------------------------------------------------------------------
   // ---  Generate DB = lHash + PS + 0x01 + _in  -------------------------------------------------------------------
   // -----                                     ------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   DB = lHash;

   if ( ! PS.empty() )
      DB.insert( DB.end(), PS.begin(), PS.end() );

   DB.push_back( 0x01 );
   DB.insert( DB.end(), _in.begin(), _in.end() );


   // ------------------------------------------------------------------------------------------------------
   // -----               ----------------------------------------------------------------------------------------
   // ---  Generate dbMask  ---------------------------------------------------------------------------------
   // -----               ----------------------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   RandISAAC lRand;
   seed.resize( hLen );
   for ( unsigned char & c : seed ) {
      c = lRand( 0, 255 );
   }

   MGF1( seed, dbMask, vBlockSize_uI - hLen - 1 );


   // ------------------------------------------------------------------------------------------------------
   // -----                 ---------------------------------------------------------------------
   // ---  Generate maskedDB  -------------------------------------------------------------------------------
   // -----                 --------------------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   maskedDB.resize( vBlockSize_uI - hLen - 1 );
   for ( unsigned int i = 0; i < maskedDB.size(); ++i )
      maskedDB[i] = DB[i] ^ dbMask[i];


   // ------------------------------------------------------------------------------------------------------
   // -----                 --------------------------------------------------------------------------------------
   // ---  Generate seedMask  -------------------------------------------------------------------------------
   // -----                 --------------------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   MGF1( maskedDB, seedMask, hLen );


   // ------------------------------------------------------------------------------------------------------
   // -----                   ------------------------------------------------------------------------------------
   // ---  Generate maskedSeed  -----------------------------------------------------------------------------
   // -----                   ------------------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   maskedSeed.resize( hLen );
   for ( unsigned int i = 0; i < maskedSeed.size() ; ++i )
      maskedSeed[i] = seed[i] ^ seedMask[i];


   // ------------------------------------------------------------------------------------------------------
   // -----                                   --------------------------------------------------------------------
   // ---  _out = 0x00 + maskedSeed + maskedDB  ---------------------------------------------------------------------
   // -----                                   --------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   _out.clear();
   _out.push_back( 0x00 );
   _out.insert( _out.end(), maskedSeed.begin(), maskedSeed.end() );
   _out.insert( _out.end(), maskedDB.begin(),   maskedDB.end() );

   return 1;
}

/*!
 * \brief The RSAES-OAEP Padding function (encoding)
 *
 * \param[in]  _in    The message input
 * \param[out] _out   The padded message
 * \param[in]  _label OPTIONAL label. Empty if not set
 *
 * \note _out will ALWAYS be cleared when the decoding starts.
 *
 * \note For security reasons there is only ONE error return statement
 *
 * \returns 1 on success
 * \returns 0 when ther was a decoding Error. _out will also be empty.
 */
int RSA::decode( std::vector<unsigned char> const &_in, std::vector<unsigned char> &_out, std::vector<unsigned char> const &_label ) const {
   unsigned short int lReturnValue_usI = 1;

   SHA_2 lHashFunction( vHashFunction_E );

   // ------------------------------------------------------------------------------------------------------
   // -----                               ------------------------------------------------------------------------
   // ---  Variables (names from RFC 3447)  -----------------------------------------------------------------
   // -----                               ------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------
   unsigned int               hLen = lHashFunction.getHashLength();

   unsigned char              Y;

   std::vector<unsigned char> lHash;
   std::vector<unsigned char> EM = _in;
   std::vector<unsigned char> maskedSeed;
   std::vector<unsigned char> maskedDB;
   std::vector<unsigned char> seedMask;
   std::vector<unsigned char> seed;
   std::vector<unsigned char> dbMask;
   std::vector<unsigned char> DB;
   std::vector<unsigned char> lHashFromDB;


   // ------------------------------------------------------------------------------------------------------
   // -----                      ---------------------------------------------------------------------------------
   // ---  lHash = HASH( _label )  ----------------------------------------------------------------------------------
   // -----                      ---------------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   lHash = lHashFunction.quickHash( vHashFunction_E, _label );


   // ------------------------------------------------------------------------------------------------------
   // -----                                         --------------------------------------------------------------
   // ---  Splitting _in = Y + maskedSeed + maskedDB  ---------------------------------------------------------------
   // -----                                         --------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   if ( EM.size() != vBlockSize_uI ) {
      lReturnValue_usI = 0;
      EM.resize( vBlockSize_uI );
   }

   std::vector<unsigned char>::iterator lEM_Iter = EM.begin();

   Y = *lEM_Iter;
   ++lEM_Iter;

   maskedSeed.resize( hLen );
   for ( unsigned char & c : maskedSeed ) {
      c = *lEM_Iter;
      ++lEM_Iter;
   }

   maskedDB.resize( vBlockSize_uI - hLen - 1 );
   for ( unsigned char & c : maskedDB ) {
      c = *lEM_Iter;
      ++lEM_Iter;
   }


   // ------------------------------------------------------------------------------------------------------
   // -----             ------------------------------------------------------------------------------------------
   // ---  Generate seed  -----------------------------------------------------------------------------------
   // -----             ------------------------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   MGF1( maskedDB, seedMask, hLen );

   seed.resize( hLen );
   for ( unsigned int i = 0; i < maskedSeed.size() ; ++i )
      seed[i] = maskedSeed[i] ^ seedMask[i];


   // ------------------------------------------------------------------------------------------------------
   // -----           --------------------------------------------------------------------------------------------
   // ---  Generate DB  -------------------------------------------------------------------------------------
   // -----           --------------------------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   MGF1( seed, dbMask, vBlockSize_uI - hLen - 1 );

   DB.resize( vBlockSize_uI - hLen - 1 );
   for ( unsigned int i = 0; i < DB.size() ; ++i )
      DB[i] = maskedDB[i] ^ dbMask[i];

   // ------------------------------------------------------------------------------------------------------
   // -----                                        ---------------------------------------------------------------
   // ---  Splitting DB = lHash' + PS + 0x01 + _out  ----------------------------------------------------------------
   // -----                                        ---------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   std::vector<unsigned char>::iterator lDB_Iter = DB.begin();

   lHashFromDB.resize( hLen );
   for ( unsigned char & c : lHashFromDB ) {
      c = *lDB_Iter;
      ++lDB_Iter;
   }


   // PS
   while ( *lDB_Iter == 0x00 && lDB_Iter != DB.end() )
      ++lDB_Iter;

   if ( *lDB_Iter != 0x01 )
      lReturnValue_usI = 0;

   ++lDB_Iter;

   _out.clear();
   _out.resize( DB.end() - lDB_Iter );
   for ( unsigned char & c : _out ) {
      c = *lDB_Iter;
      ++lDB_Iter;
   }


   // ------------------------------------------------------------------------------------------------------
   // -----        -----------------------------------------------------------------------------------------------
   // ---  Checking  ------------------------------------------------------------------------------------------------
   // -----        -----------------------------------------------------------------------------------------------
   // -----------------------------------------------------------------------------------------------------

   if ( Y != 0x00 )
      lReturnValue_usI = 0;

   if ( lHash != lHashFromDB )
      lReturnValue_usI = 0;

   if ( lReturnValue_usI == 0 )
      _out.clear();

   return lReturnValue_usI;
}

}



// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

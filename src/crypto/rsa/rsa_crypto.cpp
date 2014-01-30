/*!
 * \file rsa_crypto.cpp
 * \brief \b Classes: \a RSA (main crypto functions)
 */

#include "rsa.hpp"
#include <boost/filesystem.hpp>
#include <fstream>

namespace e_engine {


/*!
 * \brief Encrypts a file using RSA
 *
 * \param[in]  _keyCrypto     The key, which schould be used to encrypt the file
 * \param[in]  _keySignature  The key, which should be used for the signature
 * \param[in]  _inFile        Path to the file to encrypt
 * \param[in]  _outFile       Path to the output file
 * \param[out] _signature     The signature
 *
 * \returns 1  if everything went fine
 * \returns -1 if _keyCrypto has no public key
 * \returns -2 if _keySignature has no private key
 * \returns 2  if the input file doesnt exists
 * \returns 3  if the input file is not a regular file
 * \returns 4  if the output file exists already
 * \returns 5  if the block size of the signature key is to smal
 * \returns 6  if it is impossible to open the files (check your rights)
 */
int RSA::encrypt(
   RSA const &_keyCrypto,
   RSA const &_keySignature,
   std::string _inFile,
   std::string _outFile,
   std::vector<unsigned char> &_signature
) {
   if ( _keyCrypto.vHavePublicKey_B == false )
      return -1;

   if ( _keySignature.vHavePrivateKey_B == false )
      return -2;

   boost::filesystem::path in( _inFile );
   boost::filesystem::path out( _outFile );

   if ( boost::filesystem::exists( in ) ) {
      if ( ! boost::filesystem::is_regular_file( in ) )
         return 3;
   } else {
      return 2;
   }

   if ( boost::filesystem::exists( out ) )
      return 4;

   HASH_FUNCTION lHashType;

   // Select the most biggest possible hash function
   if ( _keySignature.vRealCryptoBlockSize_uI > 64 ) {        // 224 / 8
      lHashType = SHA2_512;
   } else if ( _keySignature.vRealCryptoBlockSize_uI > 48 ) {
      lHashType = SHA2_384;
   } else if ( _keySignature.vRealCryptoBlockSize_uI > 32 ) {
      lHashType = SHA2_256;
   } else if ( _keySignature.vRealCryptoBlockSize_uI > 28 ) {
      lHashType = SHA2_224;
   } else {
      return 5;
   }

   SHA_2 lHash( lHashType );

   std::ifstream lInFile;
   std::ofstream lOfFile;

   lInFile.open( _inFile,  std::ios::binary | std::ios::in );
   lOfFile.open( _outFile, std::ios::binary | std::ios::out | std::ios::trunc );

   if ( lInFile.is_open() == false || lOfFile.is_open() == false )
      return 6;

   std::vector<unsigned char> lBlock_V_uC;
   std::vector<unsigned char> lTemp_V_uC;
   unsigned int               lCipherTextBlockSize_uI;

   {
      // Do a single of '0xFF' * vBlockSize_uI encryption to get the blocksize of the encrypted data
      mpz_t lFF;
      mpz_t lC;
      mpz_init( lFF );
      mpz_init( lC );

      lBlock_V_uC.resize( _keyCrypto.vBlockSize_uI );

      for ( unsigned char & c : lBlock_V_uC )
         c = 0xFF;

      _keyCrypto.Bytes2Int( lBlock_V_uC, lFF );

      // The real encryption function
      mpz_powm( lC, lFF, _keyCrypto.e, _keyCrypto.n );

      _keyCrypto.Int2Bytes( lTemp_V_uC, lC );
      mpz_clear( lFF );
      mpz_clear( lC );
      lCipherTextBlockSize_uI = lTemp_V_uC.size();
   }

   lBlock_V_uC.resize( _keyCrypto.vRealCryptoBlockSize_uI );

   mpz_t lNumToEncrypt;
   mpz_t lEncrypted;
   mpz_init( lNumToEncrypt );
   mpz_init( lEncrypted );

   while ( ! lInFile.eof() ) {
      // Read file
      for ( unsigned char & c : lBlock_V_uC ) {
         c = lInFile.get();
         if ( lInFile.eof() ) {
            lBlock_V_uC.resize( &c - &lBlock_V_uC[0] );
            break;
         }
      }

      lHash.add( lBlock_V_uC );

      _keyCrypto.encode( lBlock_V_uC, lTemp_V_uC );
      _keyCrypto.Bytes2Int( lTemp_V_uC, lNumToEncrypt );

      // The real encryption function
      mpz_powm( lEncrypted, lNumToEncrypt, _keyCrypto.e, _keyCrypto.n );

      _keyCrypto.Int2Bytes( lTemp_V_uC, lEncrypted );


      // Fill with 0x00 if number is to small
      while ( lTemp_V_uC.size() < lCipherTextBlockSize_uI )
         lTemp_V_uC.insert( lTemp_V_uC.begin(), 0x00 );


      // Print file
      for ( unsigned char const & c : lTemp_V_uC ) {
         lOfFile.put( c );
      }
   }

   lInFile.close();
   lOfFile.close();

   std::vector<unsigned char> lHashOfFile_V_uC = lHash.end();
   _keySignature.encode( lHashOfFile_V_uC, lHashOfFile_V_uC );
   _keySignature.Bytes2Int( lHashOfFile_V_uC, lNumToEncrypt );

   // The real encryption function
   mpz_powm( lEncrypted, lNumToEncrypt, _keySignature.d, _keySignature.n );

   _keySignature.Int2Bytes( _signature, lEncrypted );

   mpz_clear( lNumToEncrypt );
   mpz_clear( lEncrypted );

   return 1;
}


/*!
 * \brief Decrypts a file using RSA
 *
 * \param[in]  _keyCrypto     The key, which schould be used to decrypt the file
 * \param[in]  _keySignature  The key, which should be used for the signature
 * \param[in]  _inFile        Path to the file to decrypt
 * \param[in]  _outFile       Path to the output file
 * \param[out] _signature     The signature
 *
 * If _signature is empty, there will be no signature checking
 *
 * \returns 1  if everything went fine
 * \returns -1 if _keyCrypto has no private key
 * \returns -2 if _keySignature has no public key
 * \returns -3 if the signature failed
 * \returns -4 if there was a decryption error
 * \returns 2  if the input file doesnt exists
 * \returns 3  if the input file is not a regular file
 * \returns 4  if the output file exists already
 * \returns 5  if the block size of the signature key is to smal
 * \returns 6  if it is impossible to open the files (check your rights)
 */
int RSA::decrypt(
   RSA const &_keyCrypto,
   RSA const &_keySignature,
   std::string _inFile,
   std::string _outFile,
   std::vector<unsigned char> &_signature
) {
   if ( _keyCrypto.vHavePrivateKey_B == false )
      return -1;

   if ( _keySignature.vHavePublicKey_B == false )
      return -2;

   boost::filesystem::path in( _inFile );
   boost::filesystem::path out( _outFile );
   
   if ( boost::filesystem::exists( in ) ) {
      if ( ! boost::filesystem::is_regular_file( in ) )
         return 3;
   } else {
      return 2;
   }

   if ( boost::filesystem::exists( out ) )
      return 4;

   HASH_FUNCTION lHashType;

   // Select the most biggest possible hash function
   if ( _keySignature.vRealCryptoBlockSize_uI > 64 ) {        // 224 / 8
      lHashType = SHA2_512;
   } else if ( _keySignature.vRealCryptoBlockSize_uI > 48 ) {
      lHashType = SHA2_384;
   } else if ( _keySignature.vRealCryptoBlockSize_uI > 32 ) {
      lHashType = SHA2_256;
   } else if ( _keySignature.vRealCryptoBlockSize_uI > 28 ) {
      lHashType = SHA2_224;
   } else {
      return 5;
   }

   SHA_2 lHash( lHashType );

   std::ifstream lInFile;
   std::ofstream lOfFile;

   lInFile.open( _inFile,  std::ios::binary | std::ios::in );
   lOfFile.open( _outFile, std::ios::binary | std::ios::out | std::ios::trunc );

   if ( lInFile.is_open() == false || lOfFile.is_open() == false )
      return 6;


   std::vector<unsigned char> lBlock_V_uC;
   std::vector<unsigned char> lTemp_V_uC;

   {
      // Do a single of '0xFF' * vBlockSize_uI encryption to get the blocksize of the encrypted data
      mpz_t lFF;
      mpz_t lC;
      mpz_init( lFF );
      mpz_init( lC );

      lBlock_V_uC.resize( _keyCrypto.vBlockSize_uI );

      for ( unsigned char & c : lBlock_V_uC )
         c = 0xFF;

      _keyCrypto.Bytes2Int( lBlock_V_uC, lFF );

      // The real encryption function
      mpz_powm( lC, lFF, _keyCrypto.e, _keyCrypto.n );

      _keyCrypto.Int2Bytes( lTemp_V_uC, lC );
      mpz_clear( lFF );
      mpz_clear( lC );
   }

   lBlock_V_uC.resize( lTemp_V_uC.size() );
   lTemp_V_uC.clear();

   mpz_t lNumToDecrypt;
   mpz_t lDecrypted;
   mpz_t lM1;
   mpz_t lM2;
   mpz_t lH;
   mpz_init( lNumToDecrypt );
   mpz_init( lDecrypted );
   mpz_init( lM1 );
   mpz_init( lM2 );
   mpz_init( lH );

   while ( ! lInFile.eof() ) {
      // Read file
      for ( unsigned char & c : lBlock_V_uC ) {
         if ( lInFile.eof() )
            break;

         c = lInFile.get();
      }

      _keyCrypto.Bytes2Int( lBlock_V_uC, lNumToDecrypt );


      // The real encryption function
      mpz_powm( lM1, lNumToDecrypt, _keyCrypto.dP, _keyCrypto.p );
      mpz_powm( lM2, lNumToDecrypt, _keyCrypto.dQ, _keyCrypto.q );
      mpz_sub( lH, lM1, lM2 );
      mpz_mul( lH, lH, _keyCrypto.qInv );
      mpz_mod( lH, lH, _keyCrypto.p );
      mpz_mul( lM1, _keyCrypto.q, lH );
      mpz_add( lDecrypted, lM1, lM2 );

      _keyCrypto.Int2Bytes( lTemp_V_uC, lDecrypted, _keyCrypto.vBlockSize_uI );
      
      while ( lTemp_V_uC.size() < _keyCrypto.vBlockSize_uI ) {
         lTemp_V_uC.insert( lTemp_V_uC.begin(), 0x00 );
      }

      _keyCrypto.decode( lTemp_V_uC, lTemp_V_uC );

      if ( ! _signature.empty() )
         lHash.add( lTemp_V_uC );

      // Print file
      for ( unsigned char const & c : lTemp_V_uC ) {
         lOfFile.put( c );
      }
   }

   lInFile.close();
   lOfFile.close();

   bool lSignatureOK_B = true;
   std::vector<unsigned char> lHashOfFile_V_uC;

   if ( ! _signature.empty() ) {
      lHashOfFile_V_uC = lHash.end();
      _keySignature.Bytes2Int( _signature, lNumToDecrypt );
      mpz_powm( lDecrypted, lNumToDecrypt, _keySignature.e, _keySignature.n );

      _keySignature.Int2Bytes( _signature, lDecrypted, _keySignature.vBlockSize_uI );
      if( _keySignature.decode( _signature, _signature ) != 1 ) {
         lSignatureOK_B = false;
      }
   }

   mpz_clear( lNumToDecrypt );
   mpz_clear( lDecrypted );
   mpz_clear( lM1 );
   mpz_clear( lM2 );
   mpz_clear( lH );

   if ( _signature == lHashOfFile_V_uC || ( _signature.empty() && lSignatureOK_B == true ) )
      return 1;
   else
      return -3;
}

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

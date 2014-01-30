/*!
 * \file rsa_key_io.cpp
 * \brief \b Classes: \a RSA (main crypto functions)
 */

#include "rsa.hpp"
#include <boost/filesystem.hpp>
#include <fstream>

namespace e_engine {


void RSA::printKey( unsigned char _id, const mpz_t &_int, std::ofstream &_of ) {
   _of.put( _id );

   std::vector<unsigned char> lOutData_V_uC;
   unsigned char              lLength[4];

   Int2Bytes( lOutData_V_uC, _int );
   Int2FourChars( lLength, lOutData_V_uC.size() );

   _of.put( lLength[0] );
   _of.put( lLength[1] );
   _of.put( lLength[2] );
   _of.put( lLength[3] );

   for ( unsigned char const & c : lOutData_V_uC )
      _of.put( c );
}

/*!
 * \brief Exports the key to a specific file
 *
 * \param _file Where to save the key file
 * \param _what What to export
 *
 * | _what value | meaning                   |
 * | :---------: | :-----------------------: |
 * | 1           | public key only           |
 * | 2           | private key only          |
 * | 3           | public \b and private key |
 *
 * \par Save file Specification
 *
 * [...] <= 1 byte
 *
 * \b Headder:
 *
 * 00 | 52 (R) | 53 (S) | 41 (A) | 5F (_) | 6B (k) | 65 (e) | 79 (y) | FF |
 * [version major] [version minor] |
 * [blockSize (1)] [blockSize (2)] [blockSize (3)] [blockSize (4)] |
 *  | 00 | 00 | 00 | 00
 *
 * \b Body
 *
 * [id] | [length (1)] [length (2)] [length (3)] [length (4)] | [number in hex] * length
 *
 * length is saved in little endian
 *
 * | ID (in hex)  | Number | Explanation             |
 * | :----------: | :----: | :---------------------: |
 * | 01           | n      | public modulus          |
 * | 02           | e      | public exponent         |
 * | 03           | d      | private exponent        |
 * |              |        |                         |
 * | 04           | p      | prime one               |
 * | 05           | q      | prime two               |
 * | 06           | dP     | e * dP == 1 (mod (p-1)) |
 * | 07           | dQ     | e * dQ == 1 (mod (q-1)) |
 * | 08           | qInv   | q * qInv == 1 (mod p)   |
 *
 * \returns 1  on success
 * \returns 0  when _what out of range
 * \returns -1 if the public key is missing
 * \returns -2 if the private key is missing
 * \returns 2  if the _file already exists
 * \returns 3  if ther was a problem while opening _file
 */
int RSA::exportKey( std::string _file, int _what ) {

   if ( _what < 1 || _what > 3 )
      return 0;

   if ( ( _what == 1 || _what == 3 ) && vHavePublicKey_B == false )
      return -1;

   if ( ( _what == 2 || _what == 3 ) && vHavePrivateKey_B == false )
      return -2;

   boost::filesystem::path lFile( _file );

   if ( boost::filesystem::exists( _file ) )
      return 2;

   std::ofstream lOutStream;
   lOutStream.open( _file, std::ios::binary | std::ios::out | std::ios::trunc );

   if ( ! lOutStream.is_open() )
      return 3;

   // Header

   lOutStream.put( 0x00 );
   lOutStream.put( 0x52 ); // R
   lOutStream.put( 0x53 ); // S
   lOutStream.put( 0x41 ); // A
   lOutStream.put( 0x5F ); // _
   lOutStream.put( 0x6B ); // k
   lOutStream.put( 0x65 ); // e
   lOutStream.put( 0x79 ); // y
   lOutStream.put( 0xFF );
   lOutStream.put( keySaveFileVersion_major );
   lOutStream.put( keySaveFileVersion_minor );

   unsigned char lBS[4];
   Int2FourChars( lBS, vBlockSize_uI );

   lOutStream.put( lBS[0] );
   lOutStream.put( lBS[1] );
   lOutStream.put( lBS[2] );
   lOutStream.put( lBS[3] );

   lOutStream.put( 0x00 );
   lOutStream.put( 0x00 );
   lOutStream.put( 0x00 );
   lOutStream.put( 0x00 );

   printKey( 0x01, n, lOutStream );

   if ( _what == 1 || _what == 3 ) {
      printKey( 0x02, e,    lOutStream );
   }

   if ( _what == 2 || _what == 3 ) {
      printKey( 0x03, d,    lOutStream );
      printKey( 0x04, p,    lOutStream );
      printKey( 0x05, q,    lOutStream );
      printKey( 0x06, dP,   lOutStream );
      printKey( 0x07, dQ,   lOutStream );
      printKey( 0x08, qInv, lOutStream );
   }


   return 1;
}

/*!
 * \brief Imports a key
 *
 * \warning This will clear all information stored in this object
 *
 * \param _file The path to the key file
 *
 * \returns 1  When the key was impoted successfully
 * \returns 2  If _file doesnt exists
 * \returns 3  If it was not possible to open _file
 * \returns -1 If it is not a valid key file
 * \returns -2 If the version of the key file is to high
 */
int RSA::importKey( std::string _file ) {
   reset();
   boost::filesystem::path lFile( _file );

   if ( ! boost::filesystem::exists( _file ) )
      return 2;

   std::ifstream lInStream;
   lInStream.open( _file, std::ios::binary | std::ios::in );

   unsigned short int lZeroCounter_usI = 0;

   if ( ! lInStream.is_open() )
      return 3;

   // The constant header

   std::array<unsigned char, 9> lCheckHeader_A_uC;
   lCheckHeader_A_uC[0] = 0x00;
   lCheckHeader_A_uC[1] = 0x52; // R
   lCheckHeader_A_uC[2] = 0x53; // S
   lCheckHeader_A_uC[3] = 0x41; // A
   lCheckHeader_A_uC[4] = 0x5F; // _
   lCheckHeader_A_uC[5] = 0x6B; // k
   lCheckHeader_A_uC[6] = 0x65; // e
   lCheckHeader_A_uC[7] = 0x79; // y
   lCheckHeader_A_uC[8] = 0xFF;

   std::array<bool, 8> lNumberFound_A_B;
   for ( bool & b : lNumberFound_A_B )
      b = false;

   for ( unsigned char const & c : lCheckHeader_A_uC ) {
      if ( c != lInStream.get() )
         return -1;

      if ( lInStream.eof() )
         return -1;
   }

   // The two version bytes

   unsigned char lVersionMajor_uC = lInStream.get();
   unsigned char lVersionMinor_uC = lInStream.get();
   if ( lInStream.eof() )
      return -1;

   if (
      ( lVersionMajor_uC >  keySaveFileVersion_major ) ||
      ( lVersionMajor_uC == keySaveFileVersion_major && lVersionMinor_uC > keySaveFileVersion_minor )
   ) { return -2; }
   
   // Extract Block size
   
   unsigned char lBS[4];
   lBS[0] = lInStream.get();
   lBS[1] = lInStream.get();
   lBS[2] = lInStream.get();
   lBS[3] = lInStream.get();
   
   if ( lInStream.eof() )
      return -1;
   
   FourChars2Int( lBS, vBlockSize_uI );
   vRealCryptoBlockSize_uI = vBlockSize_uI - ( 2 * vHashLength_uI ) - 2;

   // The four zeros, finishing the header

   while ( lZeroCounter_usI < 4 ) {
      if ( lInStream.get() == 0x00 )
         ++lZeroCounter_usI;

      if ( lInStream.eof() )
         return -1;
   }

   // The main body

   std::vector<unsigned char> lNumber_V_uC;

   while ( ! lInStream.eof() ) {
      unsigned char lID_uC = lInStream.get();
      if ( lInStream.eof() )
         break;
      if ( lID_uC > 0x08 )
         return -1;

      unsigned char lLength_CSTR[4];
      u_int32_t     lLength_uI;

      for ( unsigned char & c : lLength_CSTR ) {
         c = lInStream.get();

         if ( lInStream.eof() )
            return -1;
      }

      FourChars2Int( lLength_CSTR, lLength_uI );
      lNumber_V_uC.resize( lLength_uI );

      for ( unsigned char & c : lNumber_V_uC ) {
         c = lInStream.get();

         if ( lInStream.eof() )
            return -1;
      }

      lNumberFound_A_B[lID_uC - 1] = true;

      switch ( lID_uC ) {
         case 0x01: Bytes2Int( lNumber_V_uC, n );    break;
         case 0x02: Bytes2Int( lNumber_V_uC, e );    break;
         case 0x03: Bytes2Int( lNumber_V_uC, d );    break;
         case 0x04: Bytes2Int( lNumber_V_uC, p );    break;
         case 0x05: Bytes2Int( lNumber_V_uC, q );    break;
         case 0x06: Bytes2Int( lNumber_V_uC, dP );   break;
         case 0x07: Bytes2Int( lNumber_V_uC, dQ );   break;
         case 0x08: Bytes2Int( lNumber_V_uC, qInv ); break;
      }

      if ( lInStream.eof() )
         break;
   }

   if (
      lNumberFound_A_B[0] &&
      lNumberFound_A_B[1]
   ) { vHavePublicKey_B = true; }

   if (
      lNumberFound_A_B[0] &&
      lNumberFound_A_B[2] &&
      lNumberFound_A_B[3] &&
      lNumberFound_A_B[4] &&
      lNumberFound_A_B[5] &&
      lNumberFound_A_B[6] &&
      lNumberFound_A_B[7]
   ) { vHavePrivateKey_B = true; }

   return 1;
}

}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

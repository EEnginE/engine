/*!
 * \file rsa.hpp
 * \brief \b Classes: \a RSA
 */

#include <gmp.h>
#include "sha2.hpp"

#ifndef E_RSA_HPP
#define E_RSA_HPP

namespace e_engine {

class RSA {
   private:
      int           vBits_I;
      unsigned int  vBlockSize_uI;
      unsigned int  vRealCryptoBlockSize_uI;

      unsigned int  vHashLength_uI;

      bool          vHavePrivateKey_B;
      bool          vHavePublicKey_B;

      HASH_FUNCTION vHashFunction_E;

      mpz_t         n;    //!< the RSA modulus, a positive integer                     [p*q]
      mpz_t         e;    //!< the RSA public exponent, a positive integer
      mpz_t         d;    //!< the RSA private exponent, a positive integer

      mpz_t         p;    //!< the first factor, a positive integer
      mpz_t         q;    //!< the second factor, a positive integer
      mpz_t         dP;   //!< the first factor's CRT exponent, a positive integer  [e * dP == 1 (mod (p-1))]
      mpz_t         dQ;   //!< the second factor's CRT exponent, a positive integer [e * dQ == 1 (mod (q-1))]
      mpz_t         qInv; //!< the (first) CRT coefficient, a positive integer      [q * qInv == 1 (mod p)]

      void MGF1( std::vector<unsigned char> const &_seed, std::vector<unsigned char> &_out, unsigned int _len ) const;

      void Bytes2Int( std::vector<unsigned char> const &_bytes, mpz_t &_int ) const;
      void Int2Bytes( std::vector<unsigned char> &_bytes, mpz_t const &_int, unsigned int _fixedSize = 0 ) const;
      
      void Int2FourChars( unsigned char *_data, uint32_t _int );
      void FourChars2Int( unsigned char *_data, uint32_t &_int );
      
      void printKey( unsigned char _id, mpz_t const& _int, std::ofstream &_of );
      
      void readBlock( unsigned int _blockSize );
   public:
      static const unsigned char keySaveFileVersion_major = 0;
      static const unsigned char keySaveFileVersion_minor = 1;
      
      RSA();
      ~RSA();

      void reset();

      int encode(
         std::vector<unsigned char> const &_in,
         std::vector<unsigned char>       &_out ,
         std::vector<unsigned char> const &_label = std::vector<unsigned char>()
      ) const;

      int decode(
         std::vector<unsigned char> const &_in,
         std::vector<unsigned char>       &_out ,
         std::vector<unsigned char> const &_label = std::vector<unsigned char>()
      ) const;

      static int encrypt(
         RSA const &_keyCrypto,
         RSA const &_keySignature,
         std::string _inFile,
         std::string _outFile,
         std::vector<unsigned char> &_signature
      );
      static int decrypt(
         RSA const &_keyCrypto,
         RSA const &_keySignature,
         std::string _inFile,
         std::string _outFile,
         std::vector<unsigned char> &_signature
      );
      
      int exportKey( std::string _file, int _what = 1 );
      int importKey( std::string _file );

      int generateNewKeyPair( unsigned int _nBits );

      bool setHashFunction( e_engine::HASH_FUNCTION _func );

      unsigned int getKeylength()   { return ( vBits_I < 0 ) ? 0 : vBits_I; } //!< \returns the key length in bits \b or 0 if there is no key loaded or generated
      unsigned int getBlockSize()   { return vBlockSize_uI; }                 //!< \returns the block size \b or 0 if there is no key loaded or generated
      
      bool getHasPublicKey()  const { return vHavePublicKey_B; }
      bool getHasPrivateKey() const { return vHavePrivateKey_B; }
      
      bool getNumbers( std::vector<std::vector<unsigned char>> &_data, unsigned int _base );
};

}

#endif // E_RSA_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

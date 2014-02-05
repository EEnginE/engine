/*!
 * \file rsa_save_bits.hpp
 * \brief \b Functions: \a getSaveBlockSize (get the numbers of chars which save for a specific number of bits)
 * \warning this is a automatically generated file from ./etc/rsa_max <b> DO NOT EDIT </b>
 *
 * Arguments:
 * - lower limit:   256
 * - upper limit:   65536
 * - byte number:   255
 * - output file:   src/crypto/rsa/rsa_save_bits [.cpp/.hpp]
 * - namespace:     e_engine
 * - data type:     unsigned int
 * - function name: getSaveBlockSize
 * - indent:        3
 * - nums per line: 15
 * - min num width: 7
 *
 * \note Full command: ./etc/rsa_max 256 65536 255 src/crypto/rsa/rsa_save_bits e_engine unsigned_int getSaveBlockSize 3 15 7 
 */

#ifndef RSA_SAVE_BITS_HPP
#define RSA_SAVE_BITS_HPP

namespace e_engine {

unsigned int getSaveBlockSize( unsigned int _nBits );

}


#endif // RSA_SAVE_BITS_HPP


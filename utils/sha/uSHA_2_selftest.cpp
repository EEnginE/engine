/*!
 * \file sha2_selftest.cpp
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

#include "uLog.hpp"
#include "uSHA_2.hpp"

namespace e_engine {

bool uSHA_2::test(HASH_FUNCTION _type, const std::string &_message, const std::string &_result) {
  reset(_type);
  add(_message);

  std::vector<unsigned char> lRes1_V_uC = end();
  char                       lBuffer_CSTR[3];

  std::string lName_str, lRes1_str;

  switch (_type) {
    case SHA2_224: lName_str = "SHA2-224"; break;
    case SHA2_256: lName_str = "SHA2-256"; break;
    case SHA2_384: lName_str = "SHA2-384"; break;
    case SHA2_512: lName_str = "SHA2-512"; break;
  }

  for (unsigned char const &c : lRes1_V_uC) {
#ifdef _MSC_VER
    _snprintf(lBuffer_CSTR, 3, "%02x", c);
#else
    snprintf(lBuffer_CSTR, 3, "%02x", c);
#endif
    lRes1_str += lBuffer_CSTR;
  }

  if (lRes1_str != get()) {
    eLOG(lName_str, ": '", _message, '\'', "\nRESULT 1: ", lRes1_str, "\nRESULT 2: ", get(), "\nEXPECTED: ", _result);
    return false;
  }

  if (get() != _result) {
    eLOG(lName_str, ": '", _message, '\'', "\nRESULT:   ", get(), "\nEXPECTED: ", _result);
    return false;
  }

  iLOG("[OK] ", lName_str, ": '", _message, '\'');

  return true;
}


bool uSHA_2::selftest() {
  std::string lResult_str;
  bool        lReturn_B = true;

  iLOG("========== BEGIN SHA 2 selftest ==========");

  // SHA 224: abc
  // Hash:    23097D22 3405D822 8642A477 BDA255B3 2AADBCE4 BDA0B3F7 E36C9DA7
  if (!test(SHA2_224, "abc", "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7")) {
    lReturn_B = false;
  }


  // SHA 224: abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq
  // Hash:    75388B16 512776CC 5DBA5DA1 FD890150 B0C6455C B4F58B19 52522525
  if (!test(SHA2_224,
            "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
            "75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525")) {
    lReturn_B = false;
  }


  // SHA 256: abc
  // Hash:    BA7816BF 8F01CFEA 414140DE 5DAE2223 B00361A3 96177A9C B410FF61 F20015AD
  if (!test(SHA2_256, "abc", "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")) {
    lReturn_B = false;
  }


  // SHA 256: abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq
  // Hash:    248D6A61 D20638B8 E5C02693 0C3E6039 A33CE459 64FF2167 F6ECEDD4 19DB06C1
  if (!test(SHA2_256,
            "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
            "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1")) {
    lReturn_B = false;
  }


  // SHA 384: abc
  // Hash:    CB00753F 45A35E8B B5A03D69 9AC65007 272C32AB 0EDED163 1A8B605A 43FF5BED 8086072B
  // A1E7CC23 58BAECA1 34C825A7
  if (!test(SHA2_384,
            "abc",
            "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed80"
            "86072ba1e7cc2358baeca134c825a7")) {
    lReturn_B = false;
  }


  // SHA 384:
  // abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu
  // Hash:    09330C33 F71147E8 3D192FC7 82CD1B47 53111B17 3B3B05D2 2FA08086 E3B0F712 FCC7C71A
  // 557E2DB9 66C3E9FA 91746039
  if (!test(SHA2_384,
            "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklm"
            "nopqrlmnopqrsmnopqrstnopqrstu",
            "09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712fcc7c71a557e2db966c"
            "3e9fa91746039")) {
    lReturn_B = false;
  }


  // SHA 512: abc
  // Hash:    DDAF35A1 93617ABA CC417349 AE204131 12E6FA4E 89A97EA2 0A9EEEE6 4B55D39A 2192992A
  // 274FC1A8 36BA3C23 A3FEEBBD 454D4423 643CE80E 2A9AC94F A54CA49F
  if (!test(SHA2_512,
            "abc",
            "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a21"
            "92992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49"
            "f")) {
    lReturn_B = false;
  }



  // SHA 512:
  // abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu
  // Hash:    8E959B75 DAE313DA 8CF4F728 14FC143F 8F7779C6 EB9F7FA1 7299AEAD B6889018 501D289E
  // 4900F7E4 331B99DE C4B5433A C7D329EE B6DD2654 5E96E55B 874BE909
  if (!test(SHA2_512,
            "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklm"
            "nopqrlmnopqrsmnopqrstnopqrstu",
            "8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331"
            "b99dec4b5433ac7d329eeb6dd26545e96e55b874be909")) {
    lReturn_B = false;
  }


  iLOG("========== END SHA 2 selftest ==========");

  return lReturn_B;
}
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;

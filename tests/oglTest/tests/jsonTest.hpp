/*!
 * \file jsonTest.hpp
 * \brief \b Classes: \a jsonTest
 *
 * Class for testing:
 * Tests the JSON parser
 *
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

#ifndef JSONTEST_HPP
#define JSONTEST_HPP

#include <engine.hpp>
#include <string>
#include <vector>


class jsonTest {
 private:
  // Private stuff goes here

 public:
  jsonTest() {}

  const static std::string desc;

  void runTest(e_engine::uJSON_data &_data, std::string _dataRoot);
};

#endif // JSONTEST_HPP

/*!
 * \file uSystem.hpp
 * \brief \b Classes: \a uSystem
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

#pragma once

#include "defines.hpp"
#include <string>

namespace e_engine {

/*!
 * \class e_engine::uSystem
 * \brief class to get information about the user of the programm
 *
 * This class stores information about the actual user
 * running the programm and the operating system.
 *
 */
class uSystem {
 private:
  std::string vUserName;  //!< The user name var
  std::string vUserLogin; //!< The user login name var
  std::string vUserHome;  //!< The user home dir var

  std::string vMainConfigDir;
  std::string vLogFilePath;
  std::string vConfigFilePath;

 public:
  uSystem();

  /*!
   * \brief Get the user name
   * \returns User name
   */
  std::string getUserName() { return vUserName; }

  /*!
   * \brief Get the user login name
   * \returns User login name
   */
  std::string getUserLogin() { return vUserLogin; }

  /*!
   * \brief Get the user home dir
   * \returns User home dir
   */
  std::string getUserHomeDirectory() { return vUserHome; }


  std::string getMainConfigDirPath();
  std::string getLogFilePath();
  std::string getConfigFilePath();
};

/*!
 * \brief The standard \c uSystem object
 *
 * The standard \c uSystem object
 */
extern uSystem SYSTEM;
} // namespace e_engine

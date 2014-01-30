/// \file system.hpp
/// \brief \b Classes: \a eSystem
/*
 *  E Engine
 *  Copyright (C) 2013 Daniel Mensinger
 * 
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <string>

#ifndef E_SYSTEM_HPP
#define E_SYSTEM_HPP

namespace e_engine {

/*!
 * \class e_engine::eSystem
 * \brief Class to get information about the user of the programm
 * 
 * This calss stores information about the aktuall user
 * running the programm. 
 * 
 */
class eSystem {
private:
   unsigned int userID;    //!< The user ID var
   std::string  userName;  //!< The user name var
   std::string  userLogin; //!< The user login name var
   std::string  userHome;  //!< The user home dir var
   
   std::string  mainConfigDir;
   std::string  logFilePath;
   std::string  configFilePath;
public:
   eSystem();
   
   /*!
    * \brief Get the user ID
    * \returns User ID
    */
   unsigned int getUserID() {return userID;}
   
   /*!
    * \brief Get the user name
    * \returns User name
    */
   std::string  getUserName() {return userName;}
   
   /*!
    * \brief Get the user login name
    * \returns User login name
    */
   std::string  getUserLogin() {return userLogin;}
   
   /*!
    * \brief Get the user home dir
    * \returns User home dir
    */
   std::string  getUserHomeDirectory() {return userHome;}
   
   /*!
    * \brief Get the main config dir path
    * 
    * Serch for an existing main config dir and if it doesn't
    * exists  creates it. The settings from \c WinData.config
    * will be used.
    * 
    * \returns The main config dir path
    * \sa _eWindowData
    */
   std::string  getMainConfigDirPath();
   
   /*!
    * \brief Get the log file dir
    * 
    * Serch for an existing log file dir in the main config dir and
    * if it doesn't exists creates it. The settings from \c WinData.config
    * will be used.
    * 
    * \returns The log file dir path
    * \sa _eWindowData
    */
   std::string getLogFilePath();
   
   /*!
    * \brief Get the config file dir
    * 
    * Serch for an existing log config dir in the main config dir and
    * if it doesn't exists creates it. The settings from \c WinData.config
    * will be used.
    * 
    * \returns The config file dir path
    * \sa _eWindowData
    */
   std::string getConfigFilePath();
};

/*!
 * \brief The standard \c eSystem object
 * 
 * The standard \c eSystem object
 */
extern eSystem SYSTEM;

}

#endif // E_SYSTEM_H

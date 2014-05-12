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
 * This class stores information about the actual user
 * running the programm. 
 * 
 */
class eSystem {
private:
   std::string  vUserName;  //!< The user name var
   std::string  vUserLogin; //!< The user login name var
   std::string  vUserHome;  //!< The user home dir var
   
   std::string  vMainConfigDir;
   std::string  vLogFilePath;
   std::string  vConfigFilePath;
public:
   eSystem();
   
   /*!
    * \brief Get the user name
    * \returns User name
    */
   std::string  getUserName() {return vUserName;}
   
   /*!
    * \brief Get the user login name
    * \returns User login name
    */
   std::string  getUserLogin() {return vUserLogin;}
   
   /*!
    * \brief Get the user home dir
    * \returns User home dir
    */
   std::string  getUserHomeDirectory() {return vUserHome;}
   
   
   std::string getMainConfigDirPath();
   std::string getLogFilePath();
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

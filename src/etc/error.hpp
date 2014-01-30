/// \fileerror.hpp 
/// \brief \b Classes: \a eError
/// \sa e_error.cpp
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


#ifndef E_ERROR_HPP
#define E_ERROR_HPP

#include <string>
#include <vector>
#include <GL/glew.h>

namespace e_engine {

typedef unsigned int E_CLASS_ID;
typedef unsigned int E_ERROR_TYPE;
typedef unsigned int E_ERROR_ID;

const E_CLASS_ID   CLASS_eContext       = 1;        //!< Self-explanatory constant
const E_CLASS_ID   CLASS_eError         = 2;        //!< Self-explanatory constant
const E_CLASS_ID   CLASS_eInit          = 3;        //!< Self-explanatory constant
const E_CLASS_ID   CLASS_eLinker        = 4;        //!< Self-explanatory constant
const E_CLASS_ID   CLASS_eLog           = 5;        //!< Self-explanatory constant
const E_CLASS_ID   CLASS_eShader        = 6;        //!< Self-explanatory constant
const E_CLASS_ID   CLASS_eThread        = 7;        //!< Self-explanatory constant

const E_ERROR_TYPE ERROR_DummyError     = 50;       //!< Self-explanatory constant
const E_ERROR_TYPE ERROR_Special        = 51;       //!< Self-explanatory constant
const E_ERROR_TYPE ERROR_InputOutput    = 52;       //!< Self-explanatory constant
const E_ERROR_TYPE ERROR_Shader         = 53;       //!< Self-explanatory constant
const E_ERROR_TYPE ERROR_Multithreading = 54;       //!< Self-explanatory constant

const E_ERROR_ID   E_UNKNOWN            = 100;      //!< Self-explanatory constant
const E_ERROR_ID   E_ONLY_ONE           = 101;      //!< Self-explanatory constant
const E_ERROR_ID   E_OUT_OF_RANGE       = 102;      //!< Self-explanatory constant
const E_ERROR_ID   E_LINKING_FAILURE    = 103;      //!< Self-explanatory constant
const E_ERROR_ID   E_COMPILING_FAILURE  = 104;      //!< Self-explanatory constant
const E_ERROR_ID   E_CAN_NOT_READ_FILE  = 105;      //!< Self-explanatory constant
const E_ERROR_ID   E_CAN_NOT_WRITE_FILE = 106;      //!< Self-explanatory constant
const E_ERROR_ID   E_NOT_SUCH_A_FILE    = 107;      //!< Self-explanatory constant
const E_ERROR_ID   E_VARIABLE_NOT_SET   = 108;      //!< Self-explanatory constant
const E_ERROR_ID   E_END_BUT_NOT_START  = 109;      //!< Self-explanatory constant
const E_ERROR_ID   E_START_BUT_NOT_END  = 110;      //!< Self-explanatory constant
const E_ERROR_ID   E_NOT_STARTED        = 111;      //!< Self-explanatory constant

/*!
 * \class e_engine::eError
 * \brief The main exeption class
 * 
 * This class is thrown by every class function of
 * this project, which can throw. The class can
 * store all important information and can print
 * a 'user friendly' error
 * 
 */
class eError {
   private:
      std::string  error_str;
      std::string  class_id_str;
      std::string  error_type_str;
      std::string  error_id_str;
      E_CLASS_ID   class_id;
      E_ERROR_TYPE error_type;
      E_ERROR_ID   error_id;

      std::vector<eError> errors_attached;

      eError();
   public:
      /*!
       * \brief The constructor to init an eError object
       * \param _class_id   The ID of the class which is throwing
       * \param _error_type The type of the error
       * \param _error_id   Specificed error
       * \param _error_str  A short description
       */
      eError( E_CLASS_ID _class_id, E_ERROR_TYPE _error_type, E_ERROR_ID _error_id, std::string _error_str );
      ~eError() {}
      
      /*!
       * \brief Print a 'user friendly' error message
       * \returns Nothing
       */
     GLvoid what();

      /*!
       * \brief Add an other eError object
       * \param e The error object
       * \returns Nothing
       */
     GLvoid        addError( eError e )      {errors_attached.push_back( e );}

      
      std::string  getErrorString()          {return error_str;}                 //!< \brief Get the error description         \returns The error description
      std::string  getClassname()            {return class_id_str;}              //!< \brief Get the name of class throwing    \returns The name of class throwing
      std::string  getErrorTypeString()      {return error_type_str;}            //!< \brief Get the error type string         \returns The error type string
      std::string  getErrorIDString()        {return error_id_str;}              //!< \brief Get the error type ID string      \returns The error type ID string
      E_CLASS_ID   getClassID()              {return class_id;}                  //!< \brief Get the class ID                  \returns The class ID
      E_ERROR_TYPE getErrorType()            {return error_type;}                //!< \brief Get the error type ID             \returns The error type ID
      E_ERROR_ID   getErrorID()              {return error_id;}                  //!< \brief Get the error ID                  \returns The error ID
      unsigned int getNumOfShadersAttached() {return errors_attached.size();}    //!< \brief Get the number of attached errors \returns The number of attached errors

      /*!
       * \brief Get a error object
       * \returns The error object
       */
      eError operator[]( unsigned int index ) {
         if ( index >= errors_attached.size() ) {
            eError e( CLASS_eError, ERROR_DummyError, E_OUT_OF_RANGE, "operator[] out of range" );
            return e;
         }
         return errors_attached[index];
      }

};

}

#endif // E_ERROR_HPP

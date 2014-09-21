/*!
 * \file uLog_resources.hpp
 * \brief \b Classes: \a uLogType, \a __uLogStore, \a uLogEntry, \a __uLogStoreHelper
 */

#ifndef E_LOG_STRUCTS_HPP
#define E_LOG_STRUCTS_HPP

#include <boost/variant.hpp>
#include <boost/thread.hpp>
#include <type_traits>
#include "uSignalSlot.hpp"
#include "uConfig.hpp"  // Only for internal::LOG_COLOR_TYPE and internal::LOG_PRINT_TYPE


namespace e_engine {

class uLogEntryRaw;

namespace internal {

enum LOG_OBJECT_TYPE { STRING, NEW_LINE, NEW_POINT };



/*!
 * \struct e_engine::internal::uLogType
 * \brief Holds information about a (new) output type for class \c uLog
 *
 * This structure defines all variables which are important
 * for a (new) output type for the class \c uLog
 *
 * \sa uLog
 */
class uLogType {
      typedef uSignal<void, uLogEntryRaw &> _SIGNAL_;
   private:
      char         vType_C;       //!< The character witch is associated with color and output mode
      std::wstring vType_STR;
      char         vColor_C;      //!< The ID from struct \c eCMDColor for the color which should be used
      bool         vBold_B;


      _SIGNAL_     vSignal_eSIG;  //!< \warning The connections will never copy!

      uLogType() {}
   public:
      uLogType( char _type, std::wstring _typeString, char _color, bool _bold )
         : vType_C( _type ), vType_STR( _typeString ), vColor_C( _color ), vBold_B( _bold ) , vSignal_eSIG( false ) {}

      inline char         getType()   const { return vType_C; }
      inline std::wstring getString() const { return vType_STR; }
      inline char         getColor()  const { return vColor_C; }
      inline bool         getBold()   const { return vBold_B; }

      inline _SIGNAL_   *getSignal() { return &vSignal_eSIG; }

      //void send( uLogEntryStruct _data )   { vSignal_eSIG( _data ); }
};


/*!
 * \brief Contains raw data for a log entry
 *
 * Stores the raw input data with boost::variant and uses some
 * template metaprogramming.
 */
struct uLogPartData {
   typedef boost::variant<bool, wchar_t, uint8_t, uint16_t, uint32_t, uint64_t, std::string, std::wstring, double, const char *, const wchar_t *> B_VAR;

   bool vIsSigned_B;
   B_VAR vData;

   template<class T>
   inline typename std::enable_if < std::is_signed<T>::value && !std::is_floating_point<T>::value, void >::type
   __INRERNAL__( T &_what, B_VAR &_data ) {
      _data = ( typename std::make_unsigned<T>::type )_what;
   }

   template<class T>
   inline typename std::enable_if < !std::is_signed<T>::value && !std::is_floating_point<T>::value, void >::type
   __INRERNAL__( T &_what, B_VAR &_data ) {
      _data = _what;
   }

   template<class T>
   inline typename std::enable_if <std::is_floating_point<T>::value, void >::type
   __INRERNAL__( T &_what, B_VAR &_data ) {
      _data = ( double )_what;
   }

   template<class T>
   uLogPartData( T &_what ) : vIsSigned_B( std::is_signed<T>::value ) {__INRERNAL__( _what, vData );}

   template<class T>
   uLogPartData( T && _what ) : vIsSigned_B( std::is_signed<T>::value ) {__INRERNAL__( _what, vData );}

   // Only for Windows (mingw) WHY!?!?!
   uLogPartData( uLogPartData &_what ) : vIsSigned_B( _what.vIsSigned_B ), vData( _what.vData ) {}

   template<class T>
   uLogPartData( const T *_str ) : vIsSigned_B( false ), vData( _str ) {}

};

}

class uLog;

class uLogEntryRaw {
   public:
      struct __DATA__ {
         std::wstring vResultString_STR;

         struct __DATA_RAW__ {
            std::wstring vDataString_STR;
            std::wstring vFilename_STR;
            std::wstring vFunctionName_STR;
            std::wstring vType_STR;
            char         vBasicColor_C;
            bool         vBold_B;
            int          vLine_I;
            std::time_t  vTime_lI;

            __DATA_RAW__( std::wstring &_filename, int &_line, std::wstring &_funcName ) :
               vFilename_STR( _filename ),
               vFunctionName_STR( _funcName ),
               vLine_I( _line ) {

               std::time( &vTime_lI );
            }
         } raw;

         struct __DATA_CONF__ {
            LOG_COLOR_TYPE vColor_LCT;
            LOG_PRINT_TYPE vTime_LPT;
            LOG_PRINT_TYPE vFile_LPT;
            LOG_PRINT_TYPE vErrorType_LPT;
            int            vColumns_uI;
            uint16_t       vMaxTypeStringLength_usI;
            bool           vTextOnly_B;
            __DATA_CONF__( bool &_onlyText ) : vTextOnly_B( _onlyText ) {}
         } config;

         void configure( e_engine::LOG_COLOR_TYPE _color, e_engine::LOG_PRINT_TYPE _time, e_engine::LOG_PRINT_TYPE _file, e_engine::LOG_PRINT_TYPE _errorType, int _columns );

         __DATA__( std::wstring && _filename, int &_line, std::wstring && _funcName, bool &_textOnly ) :
            raw(
                  _filename,
                  _line,
                  _funcName
            ),
            config(
                  _textOnly
            )
         {}
      } data;

   private:
      bool                                vComplete_B;
      std::vector<internal::uLogPartData> vElements;
      char                                vType_C;

      boost::condition_variable           vWaitUntilThisIsPrinted_BT;
      boost::mutex                        vWaitMutex_BT;
      bool                                vIsPrinted_B;

      boost::condition_variable           vWaitUntilEndFinisched_BT;
      boost::mutex                        vWaitEndMutex_BT;
      bool                                vEndFinished_B;

      template<class T, class... ARGS>
      void add( T && _what, ARGS && ... _args ) {
         vElements.emplace_back( _what );
         add( std::forward<ARGS>( _args )... );
      }

      template<class T, class... ARGS>
      void add( T && _what ) {
         vElements.emplace_back( _what );
         vComplete_B = true;
      }

      void end();
      void endLogWaitAndSetPrinted();

   public:
      template<class... ARGS>
      uLogEntryRaw( char _type, bool _onlyText, std::string _rawFilename, int _logLine, std::string _functionName, ARGS && ... _args ) :
         data(
               std::wstring( _rawFilename.begin(), _rawFilename.end() ),
               _logLine,
               std::wstring( _functionName.begin(), _functionName.end() ),
               _onlyText
         ),
         vComplete_B( false ),
         vType_C( _type ),
         vIsPrinted_B( false ),
         vEndFinished_B( false ) {

         add( std::forward<ARGS>( _args )... );
      }

      inline bool   getIsComplete()   const   { return vComplete_B; }
      inline bool   getIsPrinted()    const   { return vIsPrinted_B; }
      inline size_t getElementsSize() const   { return vElements.size(); }
      unsigned int  getLogEntry( std::vector< e_engine::internal::uLogType > &_vLogTypes_V_eLT );

      void          defaultEntryGenerator();

      friend class uLog;
};



}



#endif //E_LOG_STRUCTS_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 


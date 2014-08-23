/*!
 * \file uLog_resources.hpp
 * \brief \b Classes: \a uLogType, \a __uLogStore, \a uLogEntry, \a __uLogStoreHelper
 */

#ifndef E_LOG_STRUCTS_HPP
#define E_LOG_STRUCTS_HPP

#include "uSignalSlot.hpp"
#include "uConfig.hpp"  // Only for internal::LOG_COLOR_TYPE and internal::LOG_PRINT_TYPE
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

namespace e_engine {

struct uLogEntry;

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
      typedef uSignal<void, uLogEntry> _SIGNAL_;
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


class __uLogStore {
   private:
      char vAttrib_C;
      char vFG_C;
      char vBG_C;

      LOG_OBJECT_TYPE vType_e_LOT;
      std::wstring     vWhat_STR;     //!< The Message

      std::wstring     vColor_STR;

      __uLogStore() {} //!< Nothing
   public:

      /*!
       * \brief Store the Message \c _what
       *
       * Converts the Message \c _what to \c std::string and stores it in
       * \c vWhat_str
       * It also sets \c vColor_eCLO
       *
       * \note This template is based on stringstream what makes it slow. So it
       * will only be used when the faster \c boost::lexical_cast is not working
       */
      template<class T>
      __uLogStore( T                    _what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING ) {
         vWhat_STR = boost::lexical_cast<std::wstring>( _what );
      }

      __uLogStore( std::wstring         _what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = _what; }

      __uLogStore( std::string          _what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = std::wstring( _what.begin(), _what.end() ); }

      __uLogStore( const wchar_t       *_what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = _what; }

      __uLogStore( wchar_t             *_what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = _what; }

      __uLogStore( wchar_t              _what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = _what; }

      __uLogStore( const char          *_what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { std::string lTemp_str = _what; vWhat_STR = std::wstring( lTemp_str.begin(), lTemp_str.end() ); }

      __uLogStore( char                *_what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { std::string lTemp_str = _what; vWhat_STR = std::wstring( lTemp_str.begin(), lTemp_str.end() ); }

      __uLogStore( bool                 _what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = _what ? L"TRUE" : L"FALSE"; }


      __uLogStore( LOG_OBJECT_TYPE _type ) {
         vType_e_LOT = _type;
         vAttrib_C = '-';
         vFG_C     = '-';
         vBG_C     = '-';
         vWhat_STR = L"You should not see me!";
      }

      inline std::wstring getString()        const { return vWhat_STR; }
      inline std::wstring getColorString()   const { return vColor_STR; }
      inline LOG_OBJECT_TYPE getType()      const { return vType_e_LOT; }
      bool hasColor();
};


}

struct uLogEntry {
   std::wstring vResultStrin_STR;

   struct __rawData {
      std::vector<internal::__uLogStore> vLogEntries_V_eLS;
      std::wstring                                vFilename_STR;
      std::wstring                                vFunctionName_STR;
      std::wstring                                vType_STR;
      char                                        vBasicColor_C;
      bool                                        vBold_B;
      int                                         vLine_I;
      std::time_t                                 vTime_lI;
      std::wstring                                vNewColor_STR;
   } data;

   struct __configData {
      LOG_COLOR_TYPE vColor_LCT;
      LOG_PRINT_TYPE vTime_LPT;
      LOG_PRINT_TYPE vFile_LPT;
      LOG_PRINT_TYPE vErrorType_LPT;
      int            vColumns_uI;
   } config;

   struct __tempResults {
      std::wstring vTime_STR;
      std::wstring vFile_STR;
      std::wstring vErrorType_STR;
      std::wstring vMessage_STR;
   } temp;

   void configure( e_engine::LOG_COLOR_TYPE _color, e_engine::LOG_PRINT_TYPE _time, e_engine::LOG_PRINT_TYPE _file, e_engine::LOG_PRINT_TYPE _errorType, int _columns );
};

namespace internal {

class __uLogStoreHelper {
   private:
      bool                      vComplete_B;
      std::vector<__uLogStore>  vElements_V_eLS;
      char                      vType_C;
      std::time_t               vTime_lI;
      std::wstring              vRawFilename_STR;
      std::wstring              vLogFilename_STR;
      int                       vLogLine_I;
      std::wstring              vFunctionName_STR;

      boost::condition_variable vWaitUntilThisIsPrinted_BT;
      boost::mutex              vWaitMutex_BT;
      bool                      vWaitForLogPrinted_B;
      bool                      vIsPrinted_B;

      boost::condition_variable vWaitUntilEndFinisched_BT;
      boost::mutex              vWaitEndMutex_BT;
      bool                      vEndFinished_B;

      char                      vAttrib_C;
      char                      vFG_C;
      char                      vBG_C;

      std::wstring testNewColor();
   public:
      __uLogStoreHelper( wchar_t _type, std::string _rawFilename, int _logLine, std::string _functionName, bool _wait );

      template<class T>
      inline __uLogStoreHelper *add( T _what ) {
         vElements_V_eLS.push_back( __uLogStore( _what, '-', '-', '-' ) );
         return this;
      }

      template<class T>
      inline __uLogStoreHelper *add( char _fg, T _what ) {
         vElements_V_eLS.push_back( __uLogStore( _what, '-', _fg, '-' ) );
         return this;
      }

      template<class T>
      inline __uLogStoreHelper *add( char _atrib, char _fg, T _what ) {
         vElements_V_eLS.push_back( __uLogStore( _what,  _atrib, _fg, '-' ) );
         return this;
      }

      template<class T>
      inline __uLogStoreHelper *add( char _atrib, char _fg, char _bg, T _what ) {
         vElements_V_eLS.push_back( __uLogStore( _what, _atrib, _fg, _bg ) );
         return this;
      }



      inline __uLogStoreHelper *nl() {
         vElements_V_eLS.push_back( NEW_LINE );
         return this;
      }

      template<class T>
      inline __uLogStoreHelper *nl( T _what ) {
         vElements_V_eLS.push_back( NEW_LINE );
         vElements_V_eLS.push_back( __uLogStore( _what, '-', '-', '-' ) );
         return this;
      }

      template<class T>
      inline __uLogStoreHelper *nl( char _fg, T _what ) {
         vElements_V_eLS.push_back( NEW_LINE );
         vElements_V_eLS.push_back( __uLogStore( _what, '-', _fg, '-' ) );
         return this;
      }

      template<class T>
      inline __uLogStoreHelper *nl( char _atrib, char _fg, T _what ) {
         vElements_V_eLS.push_back( NEW_LINE );
         vElements_V_eLS.push_back( __uLogStore( _what, _atrib, _fg, '-' ) );
         return this;
      }

      template<class T>
      inline __uLogStoreHelper *nl( char _atrib, char _fg, char _bg, T _what ) {
         vElements_V_eLS.push_back( NEW_LINE );
         vElements_V_eLS.push_back( __uLogStore( _what, _atrib, _fg, _bg ) );
         return this;
      }



      inline __uLogStoreHelper *point() {
         vElements_V_eLS.push_back( NEW_POINT );
         return this;
      }

      template<class T>
      inline __uLogStoreHelper *point( T _what ) {
         vElements_V_eLS.push_back( NEW_POINT );
         vElements_V_eLS.push_back( __uLogStore( _what, '-', '-', '-' ) );
         return this;
      }

      template<class T>
      inline __uLogStoreHelper *point( char _fg, T _what ) {
         vElements_V_eLS.push_back( NEW_POINT );
         vElements_V_eLS.push_back( __uLogStore( _what, '-', _fg, '-' ) );
         return this;
      }

      template<class T>
      inline __uLogStoreHelper *point( char _atrib, char _fg, T _what ) {
         vElements_V_eLS.push_back( NEW_POINT );
         vElements_V_eLS.push_back( __uLogStore( _what, _atrib, _fg, '-' ) );
         return this;
      }

      template<class T>
      inline __uLogStoreHelper *point( char _atrib, char _fg, char _bg, T _what ) {
         vElements_V_eLS.push_back( NEW_POINT );
         vElements_V_eLS.push_back( __uLogStore( _what, _atrib, _fg, _bg ) );
         return this;
      }

      inline void end() {
         vComplete_B = true;
         if ( !vWaitForLogPrinted_B )
            return;
         {
            boost::unique_lock<boost::mutex> lLock_BT( vWaitMutex_BT );
            while ( ! vIsPrinted_B ) vWaitUntilThisIsPrinted_BT.wait( lLock_BT );
         }
         boost::lock_guard<boost::mutex> lLockWait_BT( vWaitEndMutex_BT );
         vEndFinished_B = true;
         vWaitUntilEndFinisched_BT.notify_one();
      }

      inline __uLogStoreHelper *setColor( char _fg ) {
         vAttrib_C = '-';
         vFG_C     = _fg;
         vBG_C     = '-';
         return this;
      }

      inline __uLogStoreHelper *setColor( char _atrib, char _fg ) {
         vAttrib_C = _atrib;
         vFG_C     = _fg;
         vBG_C     = '-';
         return this;
      }

      inline __uLogStoreHelper *setColor( char _atrib, char _fg, char _bg ) {
         vAttrib_C = _atrib;
         vFG_C     = _fg;
         vBG_C     = _bg;
         return this;
      }

      inline __uLogStore getObjAt( size_t _i ) {
         return vElements_V_eLS.at( _i );
      }

      inline bool   getIsComplete()   const   { return vComplete_B; }
      inline bool   getIsPrinted()    const   { return vIsPrinted_B; }
      inline size_t getElementsSize() const   { return vElements_V_eLS.size(); }
      unsigned int  getLogEntry( std::vector< e_engine::internal::uLogType > &_vLogTypes_V_eLT, e_engine::uLogEntry &_entry );

      inline void   endLogWaitAndSetPrinted() {
         if ( ! vWaitForLogPrinted_B ) {
            vIsPrinted_B = true;
            return;
         }
         {
            boost::lock_guard<boost::mutex> lLockWait_BT( vWaitMutex_BT );
            vIsPrinted_B = true;
            vWaitUntilThisIsPrinted_BT.notify_one();
         }
         boost::unique_lock<boost::mutex> lLockWait_BT( vWaitEndMutex_BT );
         while ( ! vEndFinished_B ) vWaitUntilEndFinisched_BT.wait( lLockWait_BT );
      }
};


}

typedef internal::__uLogStoreHelper *LOG_ENTRY;

}



#endif //E_LOG_STRUCTS_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

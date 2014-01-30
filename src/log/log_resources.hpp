/*!
 * \file log_resources.hpp
 * \brief \b Classes: \a eLogType, \a __eLogStore, \a eLogEntry, \a __eLogStoreHelper 
 */

#ifndef E_LOG_STRUCTS_HPP
#define E_LOG_STRUCTS_HPP

#include <boost/lexical_cast.hpp>
#include <sstream>
#include "signal_slot.hpp"
#include "window_data.hpp"  // Only for e_engine_internal::LOG_COLOR_TYPE and e_engine_internal::LOG_PRINT_TYPE

namespace e_engine {

struct eLogEntry;

namespace e_engine_internal {

enum LOG_OBJECT_TYPE { STRING, NEW_LINE, NEW_POINT };

/*!
 * \struct e_engine::e_engine_internal::eLogType
 * \brief Holds information about a (new) output type for class \c eLog
 *
 * This structure defines all variables which are important
 * for a (new) output type for the class \c eLog
 *
 * \sa eLog
 */
class eLogType {
      typedef eSignal<void, eLogEntry> _SIGNAL_;
   private:
      char        vType_C;       //!< The character witch is associated with color and output mode
      std::string vType_STR;
      char        vColor_C;      //!< The ID from struct \c eCMDColor for the color which should be used
      bool        vBold_B;


      _SIGNAL_    vSignal_eSIG;  //!< \warning The connections will nerver copy!

      eLogType() {}
   public:
      eLogType( char _type, std::string _typeString, char _color, bool _bold )
         : vType_C( _type ), vType_STR( _typeString ), vColor_C( _color ), vBold_B( _bold ) , vSignal_eSIG( false ) {}

      inline char        getType()   const { return vType_C; }
      inline std::string getString() const { return vType_STR; }
      inline char        getColor()  const { return vColor_C; }
      inline bool        getBold()   const { return vBold_B; }

      inline _SIGNAL_   *getSignal() { return &vSignal_eSIG; }

      //void send( eLogEntryStruct _data )   { vSignal_eSIG( _data ); }
};


class __eLogStore {
   private:
      char vAttrib_C;
      char vFG_C;
      char vBG_C;

      LOG_OBJECT_TYPE vType_e_LOT;
      std::string     vWhat_STR;     //!< The Message

      std::string     vColor_STR;

      __eLogStore() {} //!< Nothing
   public:

      /*!
       * \brief Store the Message \c _what
       *
       * Converts the Message \c _what to \c std::string and stores it in
       * \c vWhat_str
       * It also sets \c vColor_eCLO
       *
       * \note This Template is based on stringstream what makes it slow. So it
       * will only be used when when the faster \c boost::lexical_cast is not working
       */
      template<class T>
      __eLogStore( T                    _what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING ) {
         vWhat_STR = boost::lexical_cast<std::string>( _what );
      }

      __eLogStore( std::string          _what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = _what; }

      __eLogStore( const char          *_what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = _what; }
            
      __eLogStore( char                *_what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = _what; }
      
      __eLogStore( char                 _what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = _what; }
      
      __eLogStore( unsigned char        _what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = _what; }

      __eLogStore( bool                 _what, char _atrib, char _fg, char _bg ) : vAttrib_C( _atrib ), vFG_C( _fg ), vBG_C( _bg ), vType_e_LOT( STRING )
      { vWhat_STR = _what ? "TRUE" : "FALSE"; }


      __eLogStore( LOG_OBJECT_TYPE _type ) {
         vType_e_LOT = _type;
         vAttrib_C = '-';
         vFG_C     = '-';
         vBG_C     = '-';
         vWhat_STR = "You should not see me!";
      }

      inline std::string getString()        const { return vWhat_STR; }
      inline std::string getColorString()   const { return vColor_STR; }
      inline LOG_OBJECT_TYPE getType()      const { return vType_e_LOT; }
      bool hasColor();
};


}

struct eLogEntry {
   std::string vResultStrin_STR;

   struct __rawData {
      std::vector<e_engine_internal::__eLogStore> vLogEntries_V_eLS;
      std::string                                 vFilename_STR;
      std::string                                 vType_STR;
      char                                        vBasicColor_C;
      bool                                        vBold_B;
      int                                         vLine_I;
      std::time_t                                 vTime_lI;
      std::string                                 vNewColor_STR;
   } data;

   struct __configData {
      LOG_COLOR_TYPE vColor_LCT;
      LOG_PRINT_TYPE vTime_LPT;
      LOG_PRINT_TYPE vFile_LPT;
      LOG_PRINT_TYPE vErrorType_LPT;
      int            vColumns_uI;
   } config;

   struct __tempResults {
      std::string vTime_STR;
      std::string vFile_STR;
      std::string vErrorType_STR;
      std::string vMessage_STR;
   } temp;

   void configure( LOG_COLOR_TYPE _color,
                   LOG_PRINT_TYPE _time,
                   LOG_PRINT_TYPE _file,
                   LOG_PRINT_TYPE _errorType,
                   int            _columns );
};

namespace e_engine_internal {

class __eLogStoreHelper {
   private:
      bool                     vComplete_B;
      std::vector<__eLogStore> vElements_V_eLS;
      char                     vType_C;
      std::time_t              vTime_lI;
      std::string              vRawFilename_STR;
      std::string              vLogFilename_STR;
      int                      vLogLine_I;
      
      char                     vAttrib_C;
      char                     vFG_C;
      char                     vBG_C;

      std::string testNewColor();
   public:
      __eLogStoreHelper( char _type, std::string _rawFilename, int _logLine )
         : vComplete_B( false ), vType_C( _type ), vRawFilename_STR( _rawFilename ), vLogLine_I( _logLine ) {
            std::time( &vTime_lI );
            vAttrib_C = '-';
            vFG_C     = '-';
            vBG_C     = '-';
         }

      template<class T>
      inline __eLogStoreHelper *add( T _what ) {
         vElements_V_eLS.push_back( __eLogStore( _what, '-', '-', '-' ) );
         return this;
      }

      template<class T>
      inline __eLogStoreHelper *add( char _fg, T _what ) {
         vElements_V_eLS.push_back( __eLogStore( _what, '-', _fg, '-' ) );
         return this;
      }

      template<class T>
      inline __eLogStoreHelper *add( char _atrib, char _fg, T _what ) {
         vElements_V_eLS.push_back( __eLogStore( _what,  _atrib, _fg, '-' ) );
         return this;
      }

      template<class T>
      inline __eLogStoreHelper *add( char _atrib, char _fg, char _bg, T _what ) {
         vElements_V_eLS.push_back( __eLogStore( _what, _atrib, _fg, _bg ) );
         return this;
      }

      

      inline __eLogStoreHelper *nl() {
         vElements_V_eLS.push_back( NEW_LINE );
         return this;
      }
      
      template<class T>
      inline __eLogStoreHelper *nl( T _what ) {
         vElements_V_eLS.push_back( NEW_LINE );
         vElements_V_eLS.push_back( __eLogStore( _what, '-', '-', '-' ) );
         return this;
      }
      
      template<class T>
      inline __eLogStoreHelper *nl( char _fg, T _what ) {
         vElements_V_eLS.push_back( NEW_LINE );
         vElements_V_eLS.push_back( __eLogStore( _what, '-', _fg, '-' ) );
         return this;
      }
      
      template<class T>
      inline __eLogStoreHelper *nl( char _atrib, char _fg, T _what ) {
         vElements_V_eLS.push_back( NEW_LINE );
         vElements_V_eLS.push_back( __eLogStore( _what, _atrib, _fg, '-' ) );
         return this;
      }
      
      template<class T>
      inline __eLogStoreHelper *nl( char _atrib, char _fg, char _bg, T _what ) {
         vElements_V_eLS.push_back( NEW_LINE );
         vElements_V_eLS.push_back( __eLogStore( _what, _atrib, _fg, _bg ) );
         return this;
      }
      
      
      
      inline __eLogStoreHelper *point() {
         vElements_V_eLS.push_back( NEW_POINT );
         return this;
      }
      
      template<class T>
      inline __eLogStoreHelper *point( T _what ) {
         vElements_V_eLS.push_back( NEW_POINT );
         vElements_V_eLS.push_back( __eLogStore( _what, '-', '-', '-' ) );
         return this;
      }
      
      template<class T>
      inline __eLogStoreHelper *point( char _fg, T _what ) {
         vElements_V_eLS.push_back( NEW_POINT );
         vElements_V_eLS.push_back( __eLogStore( _what, '-', _fg, '-' ) );
         return this;
      }
      
      template<class T>
      inline __eLogStoreHelper *point( char _atrib, char _fg, T _what ) {
         vElements_V_eLS.push_back( NEW_POINT );
         vElements_V_eLS.push_back( __eLogStore( _what, _atrib, _fg, '-' ) );
         return this;
      }
      
      template<class T>
      inline __eLogStoreHelper *point( char _atrib, char _fg, char _bg, T _what ) {
         vElements_V_eLS.push_back( NEW_POINT );
         vElements_V_eLS.push_back( __eLogStore( _what, _atrib, _fg, _bg ) );
         return this;
      }

      inline void end() { vComplete_B = true; }
      
      inline __eLogStoreHelper *setColor( char _fg ) {
         vAttrib_C = '-';
         vFG_C     = _fg;
         vBG_C     = '-';
         return this;
      }
      
      inline __eLogStoreHelper *setColor( char _atrib, char _fg ) {
         vAttrib_C = _atrib;
         vFG_C     = _fg;
         vBG_C     = '-';
         return this;
      }
      
      inline __eLogStoreHelper *setColor( char _atrib, char _fg, char _bg ) {
         vAttrib_C = _atrib;
         vFG_C     = _fg;
         vBG_C     = _bg;
         return this;
      }

      inline __eLogStore getObjAt( size_t _i ) {
         return vElements_V_eLS.at( _i );
      }

      inline bool getIsComplete() const      { return vComplete_B; }
      inline size_t getElementsSize() const  { return vElements_V_eLS.size(); }
      unsigned int getLogEntry( std::vector< e_engine::e_engine_internal::eLogType >& _vLogTypes_V_eLT, e_engine::eLogEntry& _entry );
};


}

typedef e_engine_internal::__eLogStoreHelper* LOG_ENTRY;

}



#endif //E_LOG_STRUCTS_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

/*!
 * \file uParserJSON_data.hpp
 * \brief \b Classes: \a uJSON_data
 */

#ifndef U_PARSER_JSON_DATA_HPP
#define U_PARSER_JSON_DATA_HPP

#include <string>
#include <vector>
#include <boost/fusion/include/adapt_struct.hpp>
#include "uLog.hpp"
#include "engine_utils_Export.hpp"

#define G_STR(  _out_, _def_ ) ::e_engine::JSON_STRING,  &_out_, std::string( _def_ ), ::e_engine::uJSON_data::END_MARKER_TYPE::GET
#define G_NUM(  _out_, _def_ ) ::e_engine::JSON_NUMBER,  &_out_, (double)     _def_  , ::e_engine::uJSON_data::END_MARKER_TYPE::GET
#define G_BOOL( _out_, _def_ ) ::e_engine::JSON_BOOL,    &_out_, (bool)       _def_  , ::e_engine::uJSON_data::END_MARKER_TYPE::GET

#define S_STR(  _def_ ) ::e_engine::JSON_STRING, (std::string *)0, std::string( _def_ ) , ::e_engine::uJSON_data::END_MARKER_TYPE::SET
#define S_NUM(  _def_ ) ::e_engine::JSON_NUMBER, (double *)0,      (double)     _def_   , ::e_engine::uJSON_data::END_MARKER_TYPE::SET
#define S_BOOL( _def_ ) ::e_engine::JSON_BOOL,   (bool *)0,        (bool)       _def_   , ::e_engine::uJSON_data::END_MARKER_TYPE::SET

namespace e_engine {

enum JSON_DATA_TYPE : unsigned char {
   JSON_STRING,
   JSON_NUMBER,
   JSON_BOOL,
   JSON_NULL,
   JSON_ARRAY,
   JSON_OBJECT,
   __JSON_FAIL__,
   __JSON_NOT_SET__
};


struct uJSON_data;

}

namespace {
typedef std::vector<::e_engine::uJSON_data>  VALUES;
}

namespace e_engine {

struct utils_EXPORT uJSON_data {
   std::string id;
   std::string value_str;
   double      value_num;
   bool        value_bool;
   VALUES      value_array;
   VALUES      value_obj;

   JSON_DATA_TYPE type;

   enum END_MARKER_TYPE {
      GET,
      SET
   };

   uJSON_data() : type( e_engine::__JSON_NOT_SET__ ) {}
   uJSON_data( std::string _id, JSON_DATA_TYPE _type ) : id( _id ), type( _type ) {}


   // Without inline the linker produces errors

   inline void _()                     {} // END of the list
   inline void _( uJSON_data *_first ) {} // END of the list

   template<class... ARGS>
   inline void _( uJSON_data *_first, std::string _id, ARGS... _args );
   template<class... ARGS>
   inline void _( uJSON_data *_first, int _id, ARGS... _args );



   template<class... ARGS, class T, class D>
   inline void _( uJSON_data *_first, JSON_DATA_TYPE _type, T *_pointer, D const &_setData, END_MARKER_TYPE _what, ARGS... _args );



   template<class... ARGS>
   inline void _( uJSON_data *_first, JSON_DATA_TYPE &_type, END_MARKER_TYPE _what, ARGS... _args );

   template<class... ARGS>
   inline void _( uJSON_data *_first, int &_size, END_MARKER_TYPE _what, ARGS... _args );


   template<class... ARGS>
   inline void process( ARGS... _args )    {_( this, _args... );}

   template<class... ARGS>
   inline void operator()( ARGS... _args ) {_( this, _args... );}

   bool unique( bool _renoveDuplicates = true, bool _quiet = false, std::string _patent_IDs = "" );
   void merge( uJSON_data &_toMerge, bool _overWrite = true );
};



template<class... ARGS>
void uJSON_data::_( uJSON_data *_first, std::string _id, ARGS... _args ) {
   if( type == __JSON_NOT_SET__ )
      type = JSON_OBJECT;

   if( type == JSON_OBJECT )
      for( auto & val : value_obj )
         if( val.id == _id )
            return val._( _first, _args... );

   value_obj.emplace_back( _id, __JSON_NOT_SET__ );
   value_obj.back()._( _first, _args... );
}

template<class... ARGS>
void uJSON_data::_( uJSON_data *_first, int _id, ARGS... _args ) {
   if( type == __JSON_NOT_SET__ )
      type = JSON_ARRAY;

   if( _id < value_array.size() && _id >= 0 && type == JSON_ARRAY )
      return value_array[_id]._( _first, _args... );

   value_array.emplace_back( "", __JSON_NOT_SET__ );
   value_array.back()._( _first, _args... );
}

template<class... ARGS, class T, class D>
void uJSON_data::_( uJSON_data *_first, JSON_DATA_TYPE _type, T *_pointer, D const &_setData, END_MARKER_TYPE _what, ARGS... _args ) {
   if( type == __JSON_NOT_SET__ || ! _pointer || type != _type ) _what = SET;

   if( _what == SET ) {
      type = _type;

      const void *lData = ( const void * )&_setData;

      switch( _type ) {
         case JSON_STRING: value_str   = *( const std::string * )lData; break;
         case JSON_NUMBER: value_num   = *( const double * )lData;      break;
         case JSON_BOOL:   value_bool  = *( const bool * )lData;        break;
         case JSON_ARRAY:  value_array = *( const VALUES * )lData;      break;
         case JSON_OBJECT: value_obj   = *( const VALUES * )lData;      break;
         default: break;
      }
      return _first->_( _first, _args... );
   }

   void *lData;

   switch( _type ) {
      case JSON_STRING: lData = ( void * )&value_str;   break;
      case JSON_NUMBER: lData = ( void * )&value_num;   break;
      case JSON_BOOL:   lData = ( void * )&value_bool;  break;
      case JSON_ARRAY:  lData = ( void * )&value_array; break;
      case JSON_OBJECT: lData = ( void * )&value_obj;   break;
      default: break;
   }

   *_pointer = ( T ) * ( D * )lData; // D must be the right type, because of the macros, while T could be something different

   _first->_( _first, _args... );
}





template<class... ARGS>
void uJSON_data::_( uJSON_data *_first, JSON_DATA_TYPE &_type, END_MARKER_TYPE _what, ARGS... _args ) {
   _type = type;
   _first->_( _first, _args... );
}

template<class... ARGS>
void uJSON_data::_( uJSON_data *_first, int &_size, END_MARKER_TYPE _what, ARGS... _args ) {
   _size = -1;

   if( type == JSON_ARRAY )
      _size = value_array.size();

   _first->_( _first, _args... );
}



}


BOOST_FUSION_ADAPT_STRUCT(
      e_engine::uJSON_data,
      ( std::string,              id )
      ( std::string,              value_str )
      ( double,                   value_num )
      ( bool,                     value_bool )
      ( VALUES,                   value_array )
      ( VALUES,                   value_obj )
      ( e_engine::JSON_DATA_TYPE, type )
)


#endif

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 





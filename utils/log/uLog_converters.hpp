/*!
 * \file uLog_converters.hpp
 */

#ifndef U_LOG_CONVERTERS
#define U_LOG_CONVERTERS

#include "defines.hpp"

#include <string>

namespace e_engine {
namespace internal {

// Default
template <class T>
struct uLogConverter {
   static void convert( std::wstring &_str, T &&_t ) {
      _str.append( std::to_wstring( std::forward<T>( _t ) ) );
   }
};

// We already have a wstring
template <>
struct uLogConverter<std::wstring &&> {
   static void convert( std::wstring &_str, std::wstring &&_t ) {
      _str.append( std::forward<std::wstring>( _t ) );
   }
};

template <>
struct uLogConverter<std::wstring &> {
   static void convert( std::wstring &_str, std::wstring &_t ) { _str.append( _t ); }
};

template <>
struct uLogConverter<const std::wstring &> {
   static void convert( std::wstring &_str, const std::wstring &_t ) { _str.append( _t ); }
};

template <>
struct uLogConverter<std::wstring> {
   static void convert( std::wstring &_str, std::wstring _t ) { _str.append( _t ); }
};


// We have a normal string
template <>
struct uLogConverter<std::string &&> {
   static void convert( std::wstring &_str, std::string &&_t ) {
      _str.append( std::wstring( _t.begin(), _t.end() ) );
   }
};

template <>
struct uLogConverter<std::string &> {
   static void convert( std::wstring &_str, std::string &_t ) {
      _str.append( std::wstring( _t.begin(), _t.end() ) );
   }
};

template <>
struct uLogConverter<const std::string &> {
   static void convert( std::wstring &_str, const std::string &_t ) {
      _str.append( std::wstring( _t.begin(), _t.end() ) );
   }
};

template <>
struct uLogConverter<std::string> {
   static void convert( std::wstring &_str, std::string _t ) {
      _str.append( std::wstring( _t.begin(), _t.end() ) );
   }
};


// C strings (wchar_t)
template <unsigned int I>
struct uLogConverter<wchar_t ( & )[I]> {
   static void convert( std::wstring &_str, wchar_t *_t ) { _str.append( _t ); }
};


template <unsigned int I>
struct uLogConverter<const wchar_t ( & )[I]> {
   static void convert( std::wstring &_str, const wchar_t *_t ) { _str.append( _t ); }
};


template <>
struct uLogConverter<const wchar_t *> {
   static void convert( std::wstring &_str, const wchar_t *_t ) { _str.append( _t ); }
};

template <>
struct uLogConverter<const wchar_t *&> {
   static void convert( std::wstring &_str, const wchar_t *&_t ) { _str.append( _t ); }
};

template <>
struct uLogConverter<wchar_t *> {
   static void convert( std::wstring &_str, wchar_t *_t ) { _str.append( _t ); }
};

template <>
struct uLogConverter<wchar_t *&> {
   static void convert( std::wstring &_str, wchar_t *&_t ) { _str.append( _t ); }
};


// C strings (char)
template <unsigned int I>
struct uLogConverter<char ( & )[I]> {
   static void convert( std::wstring &_str, char *_t ) {
      for ( uint32_t i = 0; i < I - 1;
            ++i ) // -1 because I is the size of the c string including the '\0'
         _str.append( 1, _t[i] );
   }
};


template <unsigned int I>
struct uLogConverter<const char ( & )[I]> {
   static void convert( std::wstring &_str, const char *_t ) {
      for ( uint32_t i = 0; i < I - 1;
            ++i ) // -1 because I is the size of the c string including the '\0'
         _str.append( 1, _t[i] );
   }
};

template <>
struct uLogConverter<const char *> {
   static void convert( std::wstring &_str, const char *_t ) {
      for ( uint32_t i = 0; _t[i] != 0; ++i )
         _str.append( 1, _t[i] );
   }
};

template <>
struct uLogConverter<const char *&> {
   static void convert( std::wstring &_str, const char *&_t ) {
      for ( uint32_t i = 0; _t[i] != 0; ++i )
         _str.append( 1, _t[i] );
   }
};

template <>
struct uLogConverter<char *> {
   static void convert( std::wstring &_str, char *_t ) {
      for ( uint32_t i = 0; _t[i] != 0; ++i )
         _str.append( 1, _t[i] );
   }
};

template <>
struct uLogConverter<char *&> {
   static void convert( std::wstring &_str, char *&_t ) {
      for ( uint32_t i = 0; _t[i] != 0; ++i )
         _str.append( 1, _t[i] );
   }
};


// Chars
template <>
struct uLogConverter<const wchar_t &> {
   static void convert( std::wstring &_str, const wchar_t &_t ) { _str.append( 1, _t ); }
};

template <>
struct uLogConverter<wchar_t &> {
   static void convert( std::wstring &_str, wchar_t &_t ) { _str.append( 1, _t ); }
};

template <>
struct uLogConverter<wchar_t> {
   static void convert( std::wstring &_str, wchar_t _t ) { _str.append( 1, _t ); }
};

template <>
struct uLogConverter<const char &> {
   static void convert( std::wstring &_str, const char &_t ) { _str.append( 1, _t ); }
};

template <>
struct uLogConverter<char &> {
   static void convert( std::wstring &_str, char &_t ) { _str.append( 1, _t ); }
};

template <>
struct uLogConverter<char> {
   static void convert( std::wstring &_str, char _t ) { _str.append( 1, _t ); }
};


// Bool

template <>
struct uLogConverter<bool> {
   static void convert( std::wstring &_str, bool _t ) { _str.append( _t ? L"true" : L"false" ); }
};

template <>
struct uLogConverter<bool &> {
   static void convert( std::wstring &_str, bool &_t ) { _str.append( _t ? L"true" : L"false" ); }
};

template <>
struct uLogConverter<const bool &> {
   static void convert( std::wstring &_str, const bool &_t ) {
      _str.append( _t ? L"true" : L"false" );
   }
};
}
}

#endif // U_LOG_CONVERTERS


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;

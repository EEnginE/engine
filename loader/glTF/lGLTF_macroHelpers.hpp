#ifndef L_GLTF_MACRO_HELPERS_HPP
#define L_GLTF_MACRO_HELPERS_HPP

#ifdef ENABLE_GLTF_MACRO_HELPERS

#define READ( type, ... )                                                                          \
   if ( !type( __VA_ARGS__ ) )                                                                     \
      return false;                                                                                \
                                                                                                   \
   break;

#define READ_STRING( ... ) READ( getString, __VA_ARGS__ )
#define READ_NUM( ... ) READ( getNum, __VA_ARGS__ )
#define READ_BOOL( ... ) READ( getBoolean, __VA_ARGS__ )
#define READ_MAP_EL_ETC( ... ) READ( getMapElementETC, __VA_ARGS__ )
#define READ_MAP_EL( ... ) READ( getMapElement, __VA_ARGS__ )

#define READ_ITEM( list, map, str, dest )                                                          \
   if ( !getString( str ) )                                                                        \
      return false;                                                                                \
                                                                                                   \
   dest = getItem( list, map, str );                                                               \
   break;

#define SUB_SECTION( ... ) READ( __VA_ARGS__ )

#define END_GLTF_ARRAY END_GLTF_SECTION( ']' )
#define END_GLTF_OBJECT END_GLTF_SECTION( '}' )
#define END_GLTF_SECTION( end )                                                                    \
   if ( expect( ',', true, true ) )                                                                \
      continue;                                                                                    \
                                                                                                   \
   if ( expect( end, false ) )                                                                     \
      break;                                                                                       \
                                                                                                   \
   return unexpectedCharError();


#endif // ENABLE_GLTF_MACRO_HELPERS


#endif // L_GLTF_MACRO_HELPERS_HPP
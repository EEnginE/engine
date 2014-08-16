/*!
 * \file rLoader_3D_OBJ.cpp
 * \brief \b Classes: \a rLoader_3D_OBJ
 */

#include "rLoader_3D_f_OBJ.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/filesystem.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <stdio.h>
#include "uLog.hpp"

namespace e_engine {

rLoader_3D_f_OBJ::rLoader_3D_f_OBJ() {
   vIsDataLoaded_B = false;
}

rLoader_3D_f_OBJ::rLoader_3D_f_OBJ( std::string _file ) {
   vIsDataLoaded_B = false;
   vFilePath_str   = _file;
}

/*!
 * \brief Gets wether or not the file is loaded and parsed
 * \returns The state of the file being loaded and parsed
 */

bool rLoader_3D_f_OBJ::getIsLoaded() const {
   return vIsDataLoaded_B;
}

/*!
 * \brief Gets the path of the file to parse
 * \returns The path of the file to parse
 */
std::string rLoader_3D_f_OBJ::getFilePath() const {
   return vFilePath_str;
}

/*!
 * \brief Gets the raw vertex data pointer
 * \returns The raw vertex data pointer
 */
std::vector< GLfloat > *rLoader_3D_f_OBJ::getRawVertexData() {
   return &vVertexData;
}

/*!
 * \brief Gets the raw vertex index pointer
 * \returns The raw vertex index pointer
 */
std::vector< GLuint > *rLoader_3D_f_OBJ::getRawIndexData() {
   return &vIndexData;
}



/*!
 * \brief Sets the file to load
 * \param[in] _file The file to load
 *
 * \note This will NOT load the file! You have to manually load it with load()
 *
 * \returns Nothing
 */
void rLoader_3D_f_OBJ::setFile( std::string _file ) {
   vFilePath_str = _file;
}

/*!
 * \brief Clears the memory
 * \returns Nothing
 */
void rLoader_3D_f_OBJ::unLoad() {
   vIsDataLoaded_B = false;
   vVertexData.clear();
   vVertexData.resize( 0 );
   vIndexData.clear();
   vIndexData.resize( 0 );
}

namespace spirit  = boost::spirit;
namespace qi      = boost::spirit::qi;
namespace ascii   = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template<class Iterator>
struct objGrammar_float : qi::grammar<Iterator> {
   static void unsupported_f( std::string _what ) {
      wLOG "OBJ Parser: '" ADD _what ADD "' is not supported" END
   }

   static void objName_f( std::string _name ) {
      dLOG "Found object " ADD _name ADD " in " /*ADD lPointer->getFilePath()*/ END
   }

   static void mtllib_f( std::string _lib ) {
      wLOG "The 'mtllib' command in obj files is currently not supported (do not load '" ADD _lib ADD "')" END
   }

   static void usemtl_f( std::string _mtl ) {
      wLOG "The 'usemtl' command in obj files is currently not supported (do not set the material to '" ADD _mtl ADD "')" END
   }

   static void smooth_f( std::string _smooth ) {
      wLOG "The 's' command in obj files is currently not supported (do not set the smoothing group to '" ADD _smooth ADD "')" END
   }

   objGrammar_float( std::vector<GLfloat> *_v, std::vector<GLuint> *_i )
      : objGrammar_float::base_type( start ),
        vVertixes( _v ),
        vIndexes( _i ) {
      using ascii::char_;
      using qi::float_;
      using qi::uint_;
      using phoenix::push_back;

      string    = +( char_ - '\n' );
      comment   = *ascii::space >> qi::lit( "#" ) >> string;
      space     = ( char_( ' ' ) | char_( '\t' ) );

      objName   = *ascii::space >> qi::lit( "o" ) >> +space
            >> string[&objGrammar_float::objName_f] >> '\n';

      groupName = *ascii::space >> qi::lit( "g" ) >> +space
            >> string[&objGrammar_float::objName_f]  >> '\n';

      vertex    = *ascii::space >> qi::lit( "v" ) >> +space
            >> float_[push_back( phoenix::ref( *vVertixes ), spirit::_1 )] >> +space
            >> float_[push_back( phoenix::ref( *vVertixes ), spirit::_1 )] >> +space
            >> float_[push_back( phoenix::ref( *vVertixes ), spirit::_1 )] >> *space >> '\n';

      index     = *ascii::space >> qi::lit( "f" ) >> +space
            >> uint_[push_back( phoenix::ref( *vIndexes ), spirit::_1 )] >> +space
            >> uint_[push_back( phoenix::ref( *vIndexes ), spirit::_1 )] >> +space
            >> uint_[push_back( phoenix::ref( *vIndexes ), spirit::_1 )] >> *space >> '\n';

      mtllib    = *ascii::space >> qi::lit( "mtllib" ) >> +space >> string[&objGrammar_float::mtllib_f];
      usemtl    = *ascii::space >> qi::lit( "usemtl" ) >> +space >> string[&objGrammar_float::usemtl_f];
      smooth    = *ascii::space >> qi::lit( "s" )      >> +space >> string[&objGrammar_float::smooth_f];

#if DO_NOT_FAIL_PARSING
      other     = *ascii::space >> string[&objGrammar_float::unsupported_f] >> '\n';
#endif

      start     =
            +(
                  vertex    |
                  index     |
                  objName   |
                  groupName |
                  mtllib    |
                  comment   |
                  usemtl    |
                  smooth
#if DO_NOT_FAIL_PARSING
                  | other
#endif
            );

   }

   std::vector<GLfloat>             *vVertixes;
   std::vector<GLuint>              *vIndexes;

   qi::rule<Iterator, std::string()> string;
   qi::rule<Iterator, std::string()> comment;
   qi::rule<Iterator, std::string()> space;

   qi::rule<Iterator, std::string()> objName;
   qi::rule<Iterator, std::string()> groupName;

   qi::rule<Iterator, std::string()> mtllib;
   qi::rule<Iterator, std::string()> usemtl;
   qi::rule<Iterator, std::string()> smooth;

#if DO_NOT_FAIL_PARSING
   qi::rule<Iterator, std::string()> other;
#endif

   qi::rule<Iterator, std::string()> vertex;
   qi::rule<Iterator, std::string()> index;

   qi::rule<Iterator> start;
};


/*!
 * \brief loads the 3D content frome the OBJ file
 * \returns 1 on success
 * \returns 2 if the file was not found or is not a file
 * \returns 3 if the file could not be opened
 * \returns 4 if there was a parsing error
 * \returns 5 if already loaded
 */
int rLoader_3D_f_OBJ::load() {
   if( vIsDataLoaded_B )
      return 5;

   boost::filesystem::path lFilePath_BFS( vFilePath_str.c_str() );

   if( ! boost::filesystem::exists( lFilePath_BFS ) ) {
      eLOG "File " ADD vFilePath_str ADD " does not exists" END
      return 2;
   }

   if( ! boost::filesystem::is_regular_file( lFilePath_BFS ) ) {
      eLOG vFilePath_str ADD " is not a file!" END
      return 2;
   }

   FILE *lFile = fopen( vFilePath_str.c_str(), "r" );
   if( lFile == NULL ) {
      eLOG "Unable to open " ADD vFilePath_str END
      return 3;
   }

   int c;
   std::string lFileToParse_str;

   while( ( c = fgetc( lFile ) ) != EOF )
      lFileToParse_str.append( 1, ( char )c );

   fclose( lFile );

   objGrammar_float<std::string::const_iterator> lGrammar( &vVertexData, &vIndexData );

   std::string::const_iterator lStartIter = lFileToParse_str.begin();
   std::string::const_iterator lEndIter   = lFileToParse_str.end();
   bool lReturn = qi::parse( lStartIter, lEndIter, lGrammar );

   if( ( ! lReturn ) || ( lStartIter != lEndIter ) ) {
      eLOG "Failed to parse '" ADD vFilePath_str ADD "'" END
      return 4;
   }

//    B_SLEEP( seconds, 1 );

   vIsDataLoaded_B = true;

   return 1;
}








}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

/*!
 * \file rLoader_3D_OBJ.cpp
 * \brief \b Classes: \a rLoader_3D_OBJ
 */

#include "rLoader_3D_f_OBJ.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/filesystem.hpp>
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
 * \brief get if the file is loaded and parsed
 * \returns if the file is loaded and parsed
 */

bool rLoader_3D_f_OBJ::getIsLoaded() const {
   return vIsDataLoaded_B;
}

/*!
 * \brief get the path of the file to parse
 * \returns if the path of the file to parse
 */
std::string rLoader_3D_f_OBJ::getFilePath() const {
   return vFilePath_str;
}

/*!
 * \brief get the raw vertex data pointer
 * \returns if the raw vertex data pointer
 */
std::vector< GLfloat > *rLoader_3D_f_OBJ::getRawVertexData() {
   return &vVertexData;
}

/*!
 * \brief get the raw vertex index pointer
 * \returns if the raw vertex index pointer
 */
std::vector< GLuint > *rLoader_3D_f_OBJ::getRawIndexData() {
   return &vIndexData;
}



/*!
 * \brief sets the file to load
 * \param[in] _file the file to load
 *
 * \note This will NOT load the file! You have to load it with load()
 *
 * \returns Nothing
 */
void rLoader_3D_f_OBJ::setFile( std::string _file ) {
   vFilePath_str = _file;
}

/*!
 * \brief clears the memory
 * \returns Nothing
 */
void rLoader_3D_f_OBJ::unLoad() {
   vIsDataLoaded_B = false;
   vVertexData.clear();
   vVertexData.resize( 0 );
   vIndexData.clear();
   vIndexData.resize( 0 );
}


namespace qi    = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;


// Unfortunately boost::bind causes sometimes a seg fault so we need to do this :(
namespace functions {

rLoader_3D_f_OBJ *lPointer;

struct functionCalls {
   static void addVertex( float _ver ) {
      lPointer->addVertex( _ver );
   }

   static void addIndex( float _ind ) {
      lPointer->addIndex( _ind );
   }
};

void unsupported( std::string _what ) {
   wLOG "OBJ Parser: '" ADD _what ADD "' is not supported" END
}

void objName( std::string _name ) {
   dLOG "Found object " ADD _name ADD " in " ADD lPointer->getFilePath() END
}

void mtllib( std::string _lib ) {
   wLOG "The 'mtllib' command in obj files is currently not supported (do not load '" ADD _lib ADD "')" END
}

void usemtl( std::string _mtl ) {
   wLOG "The 'usemtl' command in obj files is currently not supported (do not set the material to '" ADD _mtl ADD "')" END
}

void smooth( std::string _smooth ) {
   wLOG "The 's' command in obj files is currently not supported (do not set the smoothing group to '" ADD _smooth ADD "')" END
}

void addVertex( float _ver ) {
   functionCalls::addVertex( _ver );
}

void addIndex( float _ind ) {
   functionCalls::addIndex( _ind );
}

}

template<class Iterator>
struct objGrammar_float : qi::grammar<Iterator> {
   objGrammar_float() : objGrammar_float::base_type( start ) {
      using ascii::char_;
      using qi::float_;
      using qi::uint_;

      string    = +( char_ - '\n' );
      comment   = *ascii::space >> qi::lit( "#" ) >> string;
      space     = ( char_( ' ' ) | char_( '\t' ) );

      objName   = *ascii::space >> qi::lit( "o" ) >> +space
                  >> string[&functions::objName] >> '\n';

      groupName = *ascii::space >> qi::lit( "g" ) >> +space
                  >> string[&functions::objName]  >> '\n';

      vertex    = *ascii::space >> qi::lit( "v" ) >> +space
                  >> float_[&functions::addVertex] >> +space
                  >> float_[&functions::addVertex] >> +space
                  >> float_[&functions::addVertex] >> *space >> '\n';

      index     = *ascii::space >> qi::lit( "f" ) >> +space
                  >> uint_[&functions::addIndex] >> +space
                  >> uint_[&functions::addIndex] >> +space
                  >> uint_[&functions::addIndex] >> *space >> '\n';

      mtllib    = *ascii::space >> qi::lit( "mtllib" ) >> +space >> string[&functions::mtllib];
      usemtl    = *ascii::space >> qi::lit( "usemtl" ) >> +space >> string[&functions::usemtl];
      smooth    = *ascii::space >> qi::lit( "s" )      >> +space >> string[&functions::smooth];

#if DO_NOT_FAIL_PARSING
      other     = *ascii::space >> string[&functions::unsupported] >> '\n';
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
   if ( vIsDataLoaded_B )
      return 5;

   boost::filesystem::path lFilePath_BFS( vFilePath_str.c_str() );

   if ( ! boost::filesystem::exists( lFilePath_BFS ) ) {
      eLOG "File " ADD vFilePath_str ADD " does not exists" END
      return 2;
   }

   if ( ! boost::filesystem::is_regular_file( lFilePath_BFS ) ) {
      eLOG vFilePath_str ADD " is not a file!" END
      return 2;
   }

   FILE *lFile = fopen( vFilePath_str.c_str(), "r" );
   if ( lFile == NULL ) {
      eLOG "Unable to open " ADD vFilePath_str END
      return 3;
   }

   int c;
   std::string lFileToParse_str;

   while ( ( c = fgetc( lFile ) ) != EOF )
      lFileToParse_str.append( 1, ( char )c );

   fclose( lFile );

   objGrammar_float<std::string::const_iterator> lGrammar;
   functions::lPointer = this;

   std::string::const_iterator lStartIter = lFileToParse_str.begin();
   std::string::const_iterator lEndIter   = lFileToParse_str.end();
   bool lReturn = qi::parse( lStartIter, lEndIter, lGrammar );

   if ( ( ! lReturn ) || ( lStartIter != lEndIter ) ) {
      eLOG "Failed to parse '" ADD vFilePath_str ADD "'" END
      return 4;
   }

   vIsDataLoaded_B = true;

   return 1;
}








}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

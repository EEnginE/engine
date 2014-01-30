#include <iostream>
#include <cstdlib>
#include <fstream>
#include <list>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <iomanip>
#include "gmp.h"

using namespace std;

int main( int argc, char *argv[] ) {
   if ( argc != 11 ) {
      cerr << "Usage: " << argv[0] << " <lower limit> <upper limit> <number> <file> <namespace (0 for none)> <data type (_ for space)> <function name> <indent> <numbers per line> <min num width>" << endl;
      return 1;
   }

   list<unsigned int> oneUp;
   unsigned int lastMax = 1;
   unsigned int first = 0;

   unsigned int min = ( unsigned )atoi( argv[1] );
   unsigned int max = ( unsigned )atoi( argv[2] );
   string number = argv[3];

   if ( min > max ) {
      cout << "Swaping min and max" << endl;
      unsigned int swap = min;
      max = min;
      min = swap;
   }

   if ( min < 2 ) {
      cerr << "The minimum for the lower limit is 2" << endl;
      return 2;
   }

   if ( min == max ) {
      cerr << "ERROR: <lower limit> == <upper limit> !!" << endl;
      return 3;
   }

   if ( number.empty() ) {
      cerr << "number is empty" << endl;
      return 5;
   }

   for ( unsigned int i = min; i <= max; ++i ) {
      char *numSTR = new char[( i / 2 ) + 1];
      mpz_t num, n, comp;
      mpz_init( num );
      mpz_init( n );
      mpz_init( comp );

      numSTR[0] = '1';

      for ( unsigned int j = 1; j < ( i / 2 ); ++j ) {
         numSTR[j] = '0';
      }

      numSTR[i / 2] = '\0';

      mpz_set_str( num, numSTR, 2 );

      mpz_mul( n, num, num );  //< n

      for ( unsigned int j = lastMax ;; ++j ) {
         char *numCompare = new char[number.size() * j + 1];
         for ( unsigned int k = 0; k < j; ++k ) {
            for( unsigned int l = 0; l < number.size(); ++l ) {
               numCompare[number.size() * k + l ] = number.at(l);
            }
         }
         numCompare[number.size() * j] = '\0';
         mpz_set_str( comp, numCompare, 10 );
         delete[] numCompare;

         if ( mpz_cmp( n, comp ) <= 0 ) {
            if ( lastMax < ( j - 1 ) ) {
               lastMax = j - 1;
               oneUp.push_back( i );
            }

            break;
         }

      }

      if ( i == min )
         first = lastMax;

      mpz_clear( num );
      mpz_clear( n );
      mpz_clear( comp );
      delete[] numSTR;
   }

   ofstream f1;
   f1.open( argv[4] );

   if ( f1.is_open() != true ) {
      cerr << "Unable to open file " << argv[4] << endl;
      return 6;
   }

   boost::regex ex( "^((/[a-zA-Z0-9]+)*|([a-zA-Z0-9]*/)*)" );
   const char *lReplace_C = "";
   string filename  = argv[4];
   filename = boost::regex_replace( filename, ex, lReplace_C );
   string filename2;

   string Namespace = argv[5];
   string data_type; // = argv[6];
   string function  = argv[7];
   string indent;

   indent.append( atoi( argv[8] ), ' ' );

   unsigned int maxNumLine = atoi( argv[9] );
   unsigned int minWidth   = atoi( argv[10] );

   for ( unsigned int i = 0; i < filename.length(); ++i ) {
      if ( filename.at( i ) == '.' ) {
         filename2 += '_';
         continue;
      }

      filename2 += filename.at( i );
   }

   for ( unsigned int i = 0; i < std::strlen( argv[6] ); ++i ) {
      if ( argv[6][i] == '_' ) {
         data_type += ' ';
         continue;
      }

      data_type += argv[6][i];
   }

   boost::to_upper( filename2 );

   f1 << "/*!" << endl
      << " * \\file " << filename << endl
      << " * \\brief \\b Functions: \\a " << function << " (get the numbers of chars which save for a specific number of bits)" << endl
      << " * \\warning this is a automatically generated file from " << argv[0] << " <b> DO NOT EDIT </b>" << endl
      << " *" << endl
      << " * Arguments:" << endl
      << " * - lower limit:   " << min << endl
      << " * - upper limit:   " << max << endl
      << " * - byte number:   " << number << endl
      << " * - output file:   " << filename << endl
      << " * - namespace:     " << Namespace << endl
      << " * - data type:     " << data_type << endl
      << " * - function name: " << function << endl
      << " * - indent:        " << indent << endl
      << " * - nums per line: " << maxNumLine << endl
      << " * - min num width: " << minWidth << endl
      << " *" << endl
      << " * \\note Full command: " << argv[0] << " " << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " 
      << argv[6] << " " << argv[7] << " " << argv[8] << " " << argv[9] << " " << argv[10] << " " << endl
      << " */" << endl
      << endl
      << "#ifndef " << filename2 << endl
      << "#define " << filename2 << endl
      << endl
      << ( ( Namespace == "0" ) ? "" : "namespace " + Namespace + " {\n\n" )
      << data_type << ' ' << function << "( " << data_type << " _nBits ) {" << endl
      << indent << data_type << " startValue = " << first << ';' << endl
      << indent << "const static " << data_type << " jump[] = {" << endl;

   unsigned int count = 0;
   f1 << indent << indent;
   for ( unsigned int i : oneUp ) {
      f1 << setfill( ' ' ) << setw( minWidth ) << i << ( ( i == oneUp.back() ) ? "" : "," );
      ++count;

      if ( count == maxNumLine ) {
         count = 0;
         f1 << endl << indent << indent;
      }
   }

   f1 << endl << indent << "};" << endl
      << endl
      << indent << "for ( " << data_type << " i = 0; i < " << oneUp.size() << "; ++i ) {" << endl
      << indent << indent << "if ( _nBits <= jump[i] ) {" << endl
      << indent << indent << indent << "return startValue;" << endl
      << indent << indent << "}" << endl
      << endl
      << indent << indent << "if ( i != " << oneUp.size() - 1 << " ) {" << endl
      << indent << indent << indent << "++startValue;" << endl
      << indent << indent << "}" << endl
      << indent << "}" << endl
      << endl
      << indent << "return startValue;" <<  endl
      << '}' << endl
      << ( ( Namespace == "0" ) ? "" : "\n}" ) << endl
      << endl
      << "#endif // " << filename2 << endl
      << endl;

   f1.close();

   return 0;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

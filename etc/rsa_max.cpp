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
            for ( unsigned int l = 0; l < number.size(); ++l ) {
               numCompare[number.size() * k + l ] = number.at( l );
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

   string fCPP = argv[4];
   fCPP += ".cpp";
   string fHPP = argv[4];
   fHPP += ".hpp";

   ofstream f1, f2;
   f1.open( fHPP );
   f2.open( fCPP );

   if ( f1.is_open() != true ) {
      cerr << "Unable to open file " << argv[4] << endl;
      return 6;
   }
   
   if ( f2.is_open() != true ) {
      cerr << "Unable to open file " << argv[4] << endl;
      return 6;
   }

   boost::regex ex( "^((/[a-zA-Z0-9]+)*|([a-zA-Z0-9]*/)*)" );
   const char *lReplace_C = "";
   fCPP = boost::regex_replace( fCPP, ex, lReplace_C );
   fHPP = boost::regex_replace( fHPP, ex, lReplace_C );
   string fCPP2;
   string fHPP2;

   string Namespace = argv[5];
   string data_type; // = argv[6];
   string function  = argv[7];
   string indent;

   indent.append( atoi( argv[8] ), ' ' );

   unsigned int maxNumLine = atoi( argv[9] );
   unsigned int minWidth   = atoi( argv[10] );

   for ( unsigned int i = 0; i < fCPP.length(); ++i ) {
      if ( fCPP.at( i ) == '.' ) {
         fCPP2 += '_';
         continue;
      }

      fCPP2 += fCPP.at( i );
   }

   for ( unsigned int i = 0; i < fHPP.length(); ++i ) {
      if ( fHPP.at( i ) == '.' ) {
         fHPP2 += '_';
         continue;
      }

      fHPP2 += fHPP.at( i );
   }

   boost::to_upper( fCPP2 );
   boost::to_upper( fHPP2 );

   for ( unsigned int i = 0; i < std::strlen( argv[6] ); ++i ) {
      if ( argv[6][i] == '_' ) {
         data_type += ' ';
         continue;
      }

      data_type += argv[6][i];
   }

   f2 << "/*!" << endl
      << " * \\file " << fCPP << endl
      << " * \\brief \\b Functions: \\a " << function << " (get the numbers of chars which save for a specific number of bits)" << endl
      << " * \\warning this is a automatically generated file from " << argv[0] << " <b> DO NOT EDIT </b>" << endl
      << " *" << endl
      << " * Arguments:" << endl
      << " * - lower limit:   " << min << endl
      << " * - upper limit:   " << max << endl
      << " * - byte number:   " << number << endl
      << " * - output file:   " << argv[4] << " [.cpp/.hpp]" << endl
      << " * - namespace:     " << Namespace << endl
      << " * - data type:     " << data_type << endl
      << " * - function name: " << function << endl
      << " * - indent:        " << indent.size() << endl
      << " * - nums per line: " << maxNumLine << endl
      << " * - min num width: " << minWidth << endl
      << " *" << endl
      << " * \\note Full command: " << argv[0] << " " << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " "
      << argv[6] << " " << argv[7] << " " << argv[8] << " " << argv[9] << " " << argv[10] << " " << endl
      << " */" << endl
      << endl
      << "#include \"" << fHPP << "\"" << endl
      << endl
      << ( ( Namespace == "0" ) ? "" : "namespace " + Namespace + " {\n\n" )
      << data_type << ' ' << function << "( " << data_type << " _nBits ) {" << endl
      << indent << data_type << " startValue = " << first << ';' << endl
      << indent << "const static " << data_type << " jump[] = {" << endl;

   unsigned int count = 0;
   f2 << indent << indent;
   for ( unsigned int i : oneUp ) {
      f2 << setfill( ' ' ) << setw( minWidth ) << i << ( ( i == oneUp.back() ) ? "" : "," );
      ++count;

      if ( count == maxNumLine ) {
         count = 0;
         f2 << endl << indent << indent;
      }
   }

   f2 << endl << indent << "};" << endl
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
      << endl;




   f1 << "/*!" << endl
      << " * \\file " << fHPP << endl
      << " * \\brief \\b Functions: \\a " << function << " (get the numbers of chars which save for a specific number of bits)" << endl
      << " * \\warning this is a automatically generated file from " << argv[0] << " <b> DO NOT EDIT </b>" << endl
      << " *" << endl
      << " * Arguments:" << endl
      << " * - lower limit:   " << min << endl
      << " * - upper limit:   " << max << endl
      << " * - byte number:   " << number << endl
      << " * - output file:   " << argv[4] << " [.cpp/.hpp]" << endl
      << " * - namespace:     " << Namespace << endl
      << " * - data type:     " << data_type << endl
      << " * - function name: " << function << endl
      << " * - indent:        " << indent.size() << endl
      << " * - nums per line: " << maxNumLine << endl
      << " * - min num width: " << minWidth << endl
      << " *" << endl
      << " * \\note Full command: " << argv[0] << " " << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " "
      << argv[6] << " " << argv[7] << " " << argv[8] << " " << argv[9] << " " << argv[10] << " " << endl
      << " */" << endl
      << endl
      << "#ifndef " << fHPP2 << endl
      << "#define " << fHPP2 << endl
      << endl
      << ( ( Namespace == "0" ) ? "" : "namespace " + Namespace + " {\n\n" )
      << data_type << ' ' << function << "( " << data_type << " _nBits );" << endl
      << endl
      << ( ( Namespace == "0" ) ? "" : "}\n" ) << endl
      << endl
      << "#endif // " << fHPP2 << endl
      << endl;

   f1.close();
   f2.close();

   return 0;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

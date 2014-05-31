#include <iostream>
#include <string>
#include <boost/regex.hpp>

using namespace std;

int main( int argc, char *argv[] ) {

   // Normal strings:
   cout << "Creating Normal string str1..." << endl;
   string       str1 = "aaa111aaa";

   cout << "Creating the boost regex ex1..." << endl;
   boost::regex ex1( "[0-9]*" );

   cout << "Creating the replce C-string rep1..." << endl;
   const char  *rep1 = "";


   cout << "Do some regex_replace with the stuff above..." << endl;
   string       changed1 = boost::regex_replace( str1, ex1, rep1 );

   cout << "Normal Strings:"        << endl
        << "str1:     " << str1     << endl
        << "changed1: " << changed1 << endl << endl;
        
   
   // Wide strings:
   cout << "Creating Wide string str2..." << endl;
   wstring        str2 = L"aaa111aaa";
   cout << "Creating the boost regex ex2..." << endl;
   boost::wregex  ex2( L"[0-9]*" ); // Crashes here
   cout << "Creating the replce C-string rep2..." << endl;
   const wchar_t *rep2 = L"";

   
   cout << "Do some regex_replace with the stuff above..." << endl;
   wstring       changed2 = boost::regex_replace( str2, ex2, rep2 );

   wcout << L"Wide Strings:"          << endl
         << L"str2:     " << str2     << endl
         << L"changed2: " << changed2 << endl << endl;

   return EXIT_SUCCESS;
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

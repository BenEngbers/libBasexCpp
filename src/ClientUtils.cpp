#include "ClientUtils.h"

using namespace std;

ByteVect getBytes(std::string const &s){
	ByteVect bytes;
	bytes.reserve( s.size() );

	std::transform(std::begin(s), std::end(s), std::back_inserter(bytes), [](char c){
		return std::byte(c);
	});
	return bytes;
}

std::string            getChar(ByteVect const &input) {
	std::string s((const char*)&(input[0]), input.size());
	return s;
};

std::string            pathExpander(const std::string & s, int flags) {
  wordexp_t exp_result;
  wordexp(s.c_str(), &exp_result, flags);
  std::string expanded = exp_result.we_wordv[0];
  wordfree(&exp_result);
  return expanded;
};

std::ostream&          debug_dump( const ByteVect& str, std::ostream& stm ) {
  // stm << str.size() << " bytes: " << " < " << std::hex << std::setfill('0') ;
  stm << std::hex << std::setfill('0') ;
  for( std::byte b : str ) stm << std::setw(2) << std::hex << (int)b << ' ' ;
  cout << endl;
  for( std::byte b : str ) { b == (byte) 0  ? putchar('|') : putchar((char) b);} ;
  return stm << "" << endl;
}
std::ostream&          debug_dump( const std::string& str, std::ostream& stm ) {
  stm << (int)str.size() << " bytes: " << " [ " << std::hex << std::setfill('0') ;
  for( unsigned int byte : str ) stm << std::setw(2) << byte << ' ' ;
  return stm << ']' << endl;;
}

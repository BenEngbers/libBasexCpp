#pragma once

#ifndef CLIENTUTILS_H_
#define CLIENTUTILS_H_

#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <wordexp.h>
#include <iostream>
#include <list>

#include <sys/types.h>
#include <stdlib.h>

using namespace std;

namespace BasexCpp {

typedef std::vector < std::vector < std::byte >> VectOfByteVect;
typedef std::vector < std::byte > ByteVect;

/**
 * ClientUtils implements several auxiliary functions.
getChar and getBytes are used for conversion.
An important auxiliary function is the pathExpander function that expands a partial path to a full or ‘canonical’ path.
debug_dump transforms a vector of bytes (std::vector < std::byte >) or a string into a readable form.
 */
ByteVect getBytes (string const &s);
string getChar (ByteVect const &b);
string pathExpander (const string & s, int flags = 0);
std::ostream & debug_dump (const ByteVect & str, std::ostream & stm =
			   std::cout);
std::ostream & debug_dump (const string & str, std::ostream & stm =
			   std::cout);

}


#endif /* CLIENTUTILS_H_ */
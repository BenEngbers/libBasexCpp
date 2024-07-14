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

typedef std::vector<std::vector<std::byte>> VectOfByteVect;
typedef std::vector<std::byte>              ByteVect;

std::vector<std::byte> getBytes(std::string const &s);
std::string            getChar (ByteVect const &b);
std::string            pathExpander(const std::string & s, int flags = 0);
std::ostream&          debug_dump(const ByteVect& str, std::ostream& stm = std::cout );
std::ostream&          debug_dump(const std::string& str, std::ostream& stm = std::cout );

#endif /* CLIENTUTILS_H_ */

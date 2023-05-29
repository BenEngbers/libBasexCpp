/*
 * Base.h
 *
 *  Created on: 18 apr. 2023
 *      Author: Ben Engbers
 */

#ifndef BASE_H_
#define BASE_H_

#include <string>
#include <vector>

typedef std::vector<std::vector<std::byte>> VectOfByteVect;
typedef std::vector<std::byte>              ByteVect;

#ifdef __WIN32__
  #include <winsock2.h>
  #include <Windows.h>
#else
  #include <wordexp.h>
  #include <sys/socket.h>
  #include <unistd.h>
#endif

#include "ClientUtils.h"

using namespace std;

#include "BasexSocket.h"

class Base {
public:
  Base(const std::string&, const std::string&, const std::string&, const std::string&);
  Base(BasexSocket * socket);
  virtual ~Base();

  Base& addVoid(std::string Add, std::vector<std::byte> & To);
  Base& addVoid(std::vector<std::byte> Add, std::vector<std::byte> &To);
  Base& pushByte(unsigned char Oct, std::vector<std::byte> &To);
  Base& handShake(std::vector<std::byte> Input, std::vector<std::byte> & Result);

  BasexSocket * Socket;
  BasexSocket * getSocket();
};

#endif /* BASE_H_ */

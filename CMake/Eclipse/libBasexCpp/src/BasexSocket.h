/*
 * BasexSocket.h
 *
 *  Created on: 7 december 2022
 *      Author: Ben Engbers
 */

#ifndef BASEXSOCKET_H_
#define BASEXSOCKET_H_

#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <utility>
#include <netdb.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <err.h>
#include <sstream>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <iomanip>
#include <stdlib.h>

#ifdef __WIN32__
#include <winsock2.h>
#include <Windows.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#endif

using namespace std;

namespace BasexCpp {

typedef std::vector < std::vector < std::byte >> VectOfByteVect;
typedef std::vector < std::byte > ByteVect;

class
  BasexSocket {
public:
  explicit
  BasexSocket (const string, const string, const string, const string);
  ~ BasexSocket ();

  BasexSocket (const BasexSocket &) = delete;
  BasexSocket & operator= (const BasexSocket &) = delete;

  int get_Socket () const;
  bool wait ();

  int readSocket (string & sockResponseString);
  ssize_t writeData (const string & input);
  ssize_t writeData (const ByteVect & input);
  ssize_t writeData (const char *input);
  int writeByte (unsigned char rawNum);

private:
  static ByteVect
  decode (const ByteVect & encoded);
  int Master_sfd;

  BasexSocket & CreateSocket (string, string);
  BasexSocket & Authenticate (string, string);
};

string md5_basex (const string & content);
int set_nonblock_flag (int desc, int value);

string md5_pwd (string rts, string user, string passwd);
void StringSplit (vector < string > &strings, string input, const char &sep);

}

#endif /* BASEXSOCKET_H_ */

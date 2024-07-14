/*
 * BasexSocket.h
 *
 *  Created on: 7 december 2022
 *      Author: Ben Engbers
 */

using namespace std;

#ifndef BASEXSOCKET_H_
#define BASEXSOCKET_H_

// #include <initializer_list>
#include <string>
#include <iostream>
#include <vector>
#include <cstring>

#ifdef __WIN32__
  #include <winsock2.h>
  #include <Windows.h>
#else
  #include <sys/socket.h>
  #include <unistd.h>
#endif

#include <netdb.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <err.h>

#include <sstream>
#include <openssl/evp.h>
#include <openssl/md5.h>

#include <iomanip>

class BasexSocket {
public:
	BasexSocket (const std::string, const std::string, const std::string, const std::string);
	~BasexSocket();
  BasexSocket(const BasexSocket&) = delete;
  BasexSocket& operator=(const BasexSocket&) = delete;

	int basex_status();                                      // moet nog geimplementeerd worden

	int get_Socket();
  bool wait();

  int readSocket( std::string &sockResponseString);
	int writeData(const std::string & input);
	int writeData(const std::vector<std::byte> & input);
	int writeData(const char* input);
	int writeByte( unsigned char rawNum);
  std::vector<std::byte> decode(const std::vector<std::byte>& encoded);

private:
	int Master_sfd;

	BasexSocket& CreateSocket (string, string);
	BasexSocket& Authenticate (string, string);
} ;

std::string    md5_basex(const string& content);
int            set_nonblock_flag (int desc, int value);

std::string    md5_pwd(std::string rts, std::string user, std::string passwd);
int            StringSplit(vector <string> & strings, std::string input, const char & sep);

#endif /* BASEXSOCKET_H_ */

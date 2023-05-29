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
// #include <string.h>
#include <err.h>

#include <sstream>
// #include <algorithm>
#include <openssl/evp.h>
#include <openssl/md5.h>

#include <iomanip>

class BasexSocket {
public:
	BasexSocket (const std::string, const std::string, const std::string, const std::string);
	~BasexSocket() { close( Master_sfd);}
  BasexSocket(const BasexSocket&) = delete;
  BasexSocket& operator=(const BasexSocket&) = delete;

	int basex_status();                                      // moet nog geimplementeerd worden
	int get_Socket() {return Master_sfd;};
  bool wait();

	int readSocket( std::string &sockResponseString) {
		// use lambdas as local functions
		auto can_read = [](int s) -> bool {
			fd_set read_set;
			FD_ZERO(&read_set);
			FD_SET(s, &read_set);
			struct timeval timeout {};
			int rc = select(s + 1, &read_set, NULL, NULL, &timeout);
			return (rc == 1) && FD_ISSET(s, &read_set);
		};
		auto do_read = [&sockResponseString](int s) -> bool {
			// don't need non-blocking checks, code works with both blocking
			// and non-blocking sockets as select() says we're good to read
			char buf[BUFSIZ];
			int nbytes = recv(s, buf, sizeof(buf), 0);
			if (nbytes <= 0)
				return false;
			sockResponseString += std::string(buf, static_cast<size_t>(nbytes));
			return true;
		};

		sockResponseString.clear();
		bool done{};
		while (!done) {                                        // keep looping until first read
			if (can_read(Master_sfd) && do_read(Master_sfd)) {
				while (!done) {                                    // then return once all the buffered input is read
					if (!can_read(Master_sfd))
						done = true;
					do_read(Master_sfd);
				}
			}
		}
		return static_cast<int>(sockResponseString.size());
	};
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

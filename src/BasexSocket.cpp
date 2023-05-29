/**
 * BasexSocket.cpp
 *
 *  Created on: 7 december 2022
 *      Author: Ben Engbers
 */

#include "BasexSocket.h"

using namespace std;

BasexSocket::BasexSocket (const std::string DBHOST, const std::string DBPORT,
		const std::string DBUSERNAME, const std::string DBPASSWORD) {
	CreateSocket (DBHOST, DBPORT).Authenticate (DBUSERNAME, DBPASSWORD);
};

BasexSocket& BasexSocket::CreateSocket (std::string host, std::string port) {
	if (host.empty() || port.empty()) {
		Master_sfd = -1; return *this;
	}
	struct addrinfo hints;
	struct addrinfo *result = NULL, *rp;
	memset(&hints, 0, sizeof(struct addrinfo));              // Initialize hints
	hints.ai_family   = AF_INET;                             // Accept both AF_INET and AF_INET6
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_NUMERICSERV;                      // Port must be specified as number

	int rc;
	rc = getaddrinfo( host.c_str(), port.c_str(), &hints, &result);
	if (rc != 0) perror(gai_strerror(rc));

	for (rp = result; rp != NULL; rp = rp->ai_next) {        // result is a linked list of address structures.
		Master_sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (Master_sfd == -1) continue;
		if (connect(Master_sfd, rp->ai_addr, rp->ai_addrlen) != -1) break; // Try to connect. Return the first successfull connect or abort
		close(Master_sfd);
	}
	set_nonblock_flag( Master_sfd, 1);
	int opt = true;
	if( setsockopt(Master_sfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
	{
	  perror("setsockopt");
	  exit(EXIT_FAILURE);
	}
	if (rp == NULL) {
		warnx("Can not connect to Basex server"); }

	freeaddrinfo(result);
	return *this;
}
/**
 * Authenticate socket
 * @param username
 * @param password
 *
 * @return *this. If not successfull, set Master_sfd to -1
 *
 */
BasexSocket& BasexSocket::Authenticate (std::string user, std::string passwd) {
	int bytes_read, bytes_sent;
	std::string sock_read_string, rts, auth;
	vector <string> strings;

	bytes_read = readSocket( rts);                           // Server sends the timestamp
	if (bytes_read == 0) { warnx("Reading timestamp failed"); Master_sfd = -1;
		return *this;	}

	user.push_back(0);                                       // Suffix zero
	bytes_sent = writeData(user);                            // Client sends the user name (must be \0-terminated)
  user.pop_back();                                         // restore user
  auth = md5_pwd(rts, user, passwd);                       // Compose the md5_auth hash,
  auth.push_back(0);                                       // Suffix zero

	bytes_sent = writeData(auth);
	wait();                                                  // Avoid race-conditions
	sock_read_string.clear();
	bytes_read = readSocket(sock_read_string);               // Handle server-response
  if (bytes_read > 0) {
		if (sock_read_string.front() != '\0') {
			Master_sfd = -1;
			warnx("Authentication failed");	cout << endl;
		}
	} else {
		Master_sfd = -1;
		warnx("Reading authentication status byte failed");
	}
	return *this;
};
bool         BasexSocket::wait() {
	fd_set read_set;
	struct timeval timeout {};
	memset(&timeout, 0, sizeof(timeout));
	bool done{};
	while (!done ) {
  	FD_ZERO(&read_set);
  	FD_SET(Master_sfd, &read_set);
		int rc = select(Master_sfd + 1, &read_set, NULL, NULL, &timeout);
		done = (rc == 1) && FD_ISSET(Master_sfd, &read_set);
	};
	return done;
};

int          BasexSocket::writeData(const std::string & input) {
	int send_len = input.size();
	int bytes_sent = send(Master_sfd, input.c_str(), send_len, 0);
	if (bytes_sent != send_len) {
		perror("Error writing to socket");
		warnx("Writing data failed");
		return -1;
	}
	return bytes_sent;
};
int          BasexSocket::writeData(const std::vector<std::byte> & input) {
	int send_len = input.size();
	int bytes_sent = send(Master_sfd, input.data(), send_len, 0);
	int err = errno;
	if (bytes_sent != send_len) {
		fprintf(stderr, "ERROR: (errno = %d), %s ", errno, strerror(errno));
		warnx("Writing data failed");
		return -1;
	}
	return bytes_sent;
};

std::vector<std::byte> BasexSocket::decode(const std::vector<std::byte>& encoded) {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	if (encoded.empty())
		throw std::runtime_error("an encoded string is never empty");
	std::vector<std::byte> decoded;

	byte keep = encoded.back();
	for (std::size_t i = 0; i < encoded.size() - 1; ++i) {
	  byte ch = encoded[i];
		byte next = encoded[i + 1];
		switch (ch) {
			case (byte)0xff:
				if (next == (byte)0x00 || next == (byte)0xff) {
					decoded.push_back(next);
					++i;
				} else
					throw std::runtime_error("expecting \00 or \ff after \ff");
				break;
			case (byte)0x00:
				decoded.push_back(ch);
				return decoded; // we're done
			default:
				decoded.push_back(ch);
		}
	}
	decoded.push_back(keep);

	// test here because we may terminate early
	if (encoded.back() != (byte)0x00)
		throw std::runtime_error("expecting \00 at end of string");

	return decoded;
}
std::string            md5_basex(const std::string& content) {
  EVP_MD_CTX*   context = EVP_MD_CTX_new();
  const EVP_MD* md = EVP_md5();
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int  md_len;
  string        output;

  EVP_DigestInit_ex2(context, md, NULL);
  EVP_DigestUpdate(context, content.c_str(), content.length());
  EVP_DigestFinal_ex(context, md_value, &md_len);
  EVP_MD_CTX_free(context);

  output.resize(md_len * 2);
  for (unsigned int i = 0 ; i < md_len ; ++i)
    std::sprintf(&output[i * 2], "%02x", md_value[i]);
  return output;
};
std::string            md5_basex(const char* content) {
  EVP_MD_CTX*   context = EVP_MD_CTX_new();
  const EVP_MD* md = EVP_md5();
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int  md_len;
  string        output;

  EVP_DigestInit_ex2(context, md, NULL);
  EVP_DigestUpdate(context, content, strlen(content));
  EVP_DigestFinal_ex(context, md_value, &md_len);
  EVP_MD_CTX_free(context);

  output.resize(md_len * 2);
  for (unsigned int i = 0 ; i < md_len ; ++i)
    std::sprintf(&output[i * 2], "%02x", md_value[i]);
  return output;
};

/**
 * Set the O_NONBLOCK flag of desc if value is nonzero,
 *  or clear the flag if value is 0.
 *  Return 0 on success, or -1 on error with errno set.
*/
int                    set_nonblock_flag (int desc, int value) {
	int oldflags = fcntl (desc, F_GETFL, 0);       // If reading the flags failed, return error indication now.
	if (oldflags == -1) return -1;
	if (value != 0) oldflags |= O_NONBLOCK;        // Set just the flag we want to set.
	else  oldflags &= ~O_NONBLOCK;

	return fcntl (desc, F_SETFL, oldflags);        // Store modified flag word in the descriptor.
};
std::string            md5_pwd(std::string rts, std::string user, std::string passwd) {
	std::string code_wd, realm, nonce;
	vector <string> strings;

  rts.pop_back();                                // Remove terminating \0
	int string_cnt = StringSplit( strings , rts, ':');  // Split string on occurence of ':'
	if (strings.size() == 1) {                     // CRAM-MD5
		nonce = rts; code_wd = passwd;
	} else {                                       // Digest
		code_wd = user; realm = strings[0]; nonce = strings[1];
		code_wd.append(":").append(realm).append(":").append(passwd);
	}
  std::string md5_hash = md5_basex(md5_basex(code_wd.c_str()).append(nonce));
	return md5_hash;
};
int                    StringSplit(vector <string> & strings, std::string input, const char & sep) {
  istringstream f(input);
  string s;
  while (getline(f, s, sep)) {
      strings.push_back(s);
  }
  return strings.size();
}

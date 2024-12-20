/**
 * BasexSocket.cpp
 *
 *  Created on: 7 december 2022
 *      Author: Ben Engbers
 */

#include "BasexSocket.h"

using namespace std;

namespace BasexCpp {

BasexSocket::BasexSocket (const string DBHOST, const string DBPORT,
			  const string DBUSERNAME, const string DBPASSWORD) {
  CreateSocket (DBHOST, DBPORT).Authenticate (DBUSERNAME, DBPASSWORD);
};

BasexSocket::~BasexSocket () {
  close (Master_sfd);
};

BasexSocket & BasexSocket::CreateSocket (string host, string port) {
	if (host.empty () || port.empty ()) {
		cout << "ERROR: Invalid hostname/port\n" << endl;
		exit(0);
	}
/*
  if (host.empty () || port.empty ()) {
    Master_sfd = -1;
    return *this;
  }
  */
  struct addrinfo hints;
  struct addrinfo * result = NULL, *rp;
  memset (&hints, 0, sizeof (struct addrinfo));	// Initialize hints
  hints.ai_family = AF_INET;	// Accept both AF_INET and AF_INET6
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_NUMERICSERV;	// Port must be specified as number

  int rc;
  rc = getaddrinfo (host.c_str (), port.c_str (), &hints, &result);
  if (rc != 0)
    perror (gai_strerror (rc));

  for (rp = result; rp != NULL; rp = rp->ai_next) {	// result is a linked list of address structures.
    Master_sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (Master_sfd == -1)
      continue;
    if (connect (Master_sfd, rp->ai_addr, rp->ai_addrlen) != -1)
      break;			// Try to connect. Return the first successfull connect or abort
    close (Master_sfd);
  }
  set_nonblock_flag (Master_sfd, 1);
  int opt = true;
  if (setsockopt
      (Master_sfd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof (opt)) < 0)
  {
    perror ("setsockopt");
    exit (EXIT_FAILURE);
  }
  if (rp == NULL) {
    warnx ("Can not connect to Basex server");
  }

  freeaddrinfo (result);
  return *this;
};

/**
 * Authenticate socket
 * @param username
 * @param password
 *
 * @return *this. If not successfull, set Master_sfd to -1
 *
 */
BasexSocket & BasexSocket::Authenticate (string user, string passwd) {
  int bytes_read;
  string sock_read_string, rts, auth;
  vector < string > strings;

  bytes_read = readSocket (rts);	// Server sends the timestamp
  if (bytes_read == 0) {
    warnx ("Reading timestamp failed");
    Master_sfd = -1;
    return *this;
  }

  user.push_back (0);		// Suffix zero
  writeData (user);		// Client sends the user name (must be \0-terminated)
  user.pop_back ();		// restore user
  auth = md5_pwd (rts, user, passwd);	// Compose the md5_auth hash,
  auth.push_back (0);		// Suffix zero

  writeData (auth);
  wait ();			// Avoid race-conditions
  sock_read_string.clear ();
  bytes_read = readSocket (sock_read_string);	// Handle server-response
  if (bytes_read > 0) {
    if (sock_read_string.front () != '\0') {
      Master_sfd = -1;
      warnx ("Authentication failed");
      cout << endl;
    }
  } else {
    Master_sfd = -1;
    warnx ("Reading authentication status byte failed");
  }
  return *this;
};

bool BasexSocket::wait () {
  fd_set read_set;
  struct timeval timeout {
  };
  memset (&timeout, 0, sizeof (timeout));
  bool done = false;
  while (!done) {
    FD_ZERO (&read_set);
    FD_SET (Master_sfd, &read_set);
    int rc = select (Master_sfd + 1, &read_set, NULL, NULL, &timeout);
    done = (rc == 1) && FD_ISSET (Master_sfd, &read_set);
  };
  return done;
};

int BasexSocket::readSocket (string & sockResponseString) {
  // use lambdas as local functions
  auto can_read =[](int s)->bool {
    fd_set read_set;
    FD_ZERO (&read_set);
    FD_SET (s, &read_set);
    struct timeval timeout {
    };
    int rc = select (s + 1, &read_set, NULL, NULL, &timeout);
    return (rc == 1) && FD_ISSET (s, &read_set);
  };
  auto do_read =[&sockResponseString] (int s)->bool {
    // don't need non-blocking checks, code works with both blocking
    // and non-blocking sockets as select() says we're good to read
    char buf[BUFSIZ];
    ssize_t nbytes = recv (s, buf, sizeof (buf), 0);
    if (nbytes <= 0)
      return
	false;
    sockResponseString +=
    string (buf, static_cast < size_t >(nbytes));
    return
      true;
  };
  sockResponseString.clear ();
  bool done = false;
  while (!done) {		// keep looping until first read
    if (can_read (Master_sfd) && do_read (Master_sfd)) {
      while (!done) {		// then return once all the buffered input is read
	if (!can_read (Master_sfd))
	  done = true;
	do_read (Master_sfd);
      }
    }
  }
  return static_cast < int >(sockResponseString.size ());
};

int BasexSocket::get_Socket () const {
  return Master_sfd;
};

ssize_t BasexSocket::writeData (const string & input) {
  string::size_type send_len = input.size ();
  ssize_t bytes_sent = send (Master_sfd, input.c_str (), send_len, 0);
  if (!std::cmp_equal (bytes_sent, send_len)) {
    perror ("Error writing to socket");
    warnx ("Writing data failed");
    return -1;
  }
  return bytes_sent;
};

ssize_t BasexSocket::writeData (const std::vector < std::byte > &input) {
  string::size_type send_len = input.size ();
  ssize_t bytes_sent = send (Master_sfd, input.data (), send_len, 0);
  if (!std::cmp_equal (bytes_sent, send_len)) {
    fprintf (stderr, "ERROR: (errno = %d), %s ", errno, strerror (errno));
    warnx ("Writing data failed");
    return -1;
  }
  return bytes_sent;
};

ByteVect BasexSocket::decode (const ByteVect & encoded) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  if (encoded.empty ())
    throw
    std::runtime_error ("an encoded string is never empty");
  std::vector < std::byte > decoded;

  byte keep = encoded.back ();
  for (std::size_t i = 0; i < encoded.size () - 1; ++i) {
    byte ch = encoded[i];
    byte next = encoded[i + 1];
    switch (ch) {
    case (byte) 0xff:
      if (next == (byte) 0x00 || next == (byte) 0xff) {
	decoded.push_back (next);
	++i;
      } else
	throw std::runtime_error ("expecting \00 or \ff after \ff");
      break;
    case (byte) 0x00:
      decoded.push_back (ch);
      return decoded;		// we're done
    default:
      decoded.push_back (ch);
    }
  }
  decoded.push_back (keep);

  // test here because we may terminate early
  if (encoded.back () != (byte) 0x00)
    throw
    std::runtime_error ("expecting \00 at end of string");

  return decoded;
};

string
md5_basex (const string & content) {
  EVP_MD_CTX * context = EVP_MD_CTX_new ();
  const EVP_MD * md = EVP_md5 ();
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len;
  string output;

  EVP_DigestInit_ex2 (context, md, NULL);
  EVP_DigestUpdate (context, content.c_str (), content.length ());
  EVP_DigestFinal_ex (context, md_value, &md_len);
  EVP_MD_CTX_free (context);

  output.resize (md_len * 2);
  for (unsigned int i = 0; i < md_len; ++i)
    std::sprintf (&output[i * 2], "%02x", md_value[i]);
  return output;
};

string
md5_basex (const char *content) {
  EVP_MD_CTX * context = EVP_MD_CTX_new ();
  const EVP_MD * md = EVP_md5 ();
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len;
  string output;

  EVP_DigestInit_ex2 (context, md, NULL);
  EVP_DigestUpdate (context, content, strlen (content));
  EVP_DigestFinal_ex (context, md_value, &md_len);
  EVP_MD_CTX_free (context);

  output.resize (md_len * 2);
  for (unsigned int i = 0; i < md_len; ++i)
    std::sprintf (&output[i * 2], "%02x", md_value[i]);
  return output;
};

/**
 * Set the O_NONBLOCK flag of desc if value is nonzero,
 *  or clear the flag if value is 0.
 *  Return 0 on success, or -1 on error with errno set.
*/
int set_nonblock_flag (int desc, int value) {
  int oldflags = fcntl (desc, F_GETFL, 0);	// If reading the flags failed, return error indication now.
  if (oldflags == -1)
    return -1;
  if (value != 0)
    oldflags |= O_NONBLOCK;	// Set just the flag we want to set.
  else
    oldflags &= ~O_NONBLOCK;

  return fcntl (desc, F_SETFL, oldflags);	// Store modified flag word in the descriptor.
};

string md5_pwd (string rts, string user, string passwd) {
  string code_wd, realm, nonce;
  vector < string > strings;

  rts.pop_back ();		// Remove terminating \0
  StringSplit (strings, rts, ':');	// Split string on occurence of ':'
  if (strings.size () == 1) {	// CRAM-MD5
    nonce = rts;
    code_wd = passwd;
  } else {			// Digest
    code_wd = user;
    realm = strings[0];
    nonce = strings[1];
    code_wd.append (":").append (realm).append (":").append (passwd);
  }
  string md5_hash = md5_basex (md5_basex (code_wd.c_str ()).append (nonce));
  return md5_hash;
};

void
StringSplit (vector < string > &strings, string input, const char &sep) {
  istringstream
  f (input);
  string s;
  while (getline (f, s, sep)) {
    strings.push_back (s);
  }
};

} // End namespace BasexCpp



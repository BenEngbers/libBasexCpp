/*
 * Base.cpp
 *
 *  Created on: 18 apr. 2023
 *      Author: Ben Engbers
 */

#include "Base.h"

namespace BasexCpp {

Base::Base (const string & DBHOST, const string & DBPORT,
	    const string & DBUSERNAME, const string & DBPASSWORD):
  Socket { std::make_shared < BasexSocket > (DBHOST, DBPORT, DBUSERNAME, DBPASSWORD)}
{
};

Base::Base (BasexSock_Sptr socket) {
  Socket = socket;
};				// Copy constructor

Base::~Base () {
};

Base Base::addVoid (string Add, ByteVect & To) {
  ByteVect bytes = getBytes (Add);	// Convert string 'Add' to std::vector<std::byte>
  addVoid (bytes, To);		// Call addVoid(bytes)
  return *this;
};

Base Base::addVoid (ByteVect Add, ByteVect & To) {
  ByteVect newAdd = Add;

  if (newAdd.size () == 0)
    To.push_back ((byte) 0x00);
  else {
    for (std::size_t i = 0; i < newAdd.size (); i++)
      To.push_back (newAdd[i]);
    To.push_back ((byte) 0x00);	// Add suffix \00
  };
  return *this;
};

Base Base::pushByte (unsigned char Oct, ByteVect & To) {
  To.push_back ((byte) Oct);
  return *this;
};

Base Base::handShake (ByteVect Input, ByteVect & Result) {
  int error = 0;
  socklen_t len = sizeof (error);
  int retval =
    getsockopt (Socket->get_Socket (), SOL_SOCKET, SO_ERROR, &error, &len);
  if (retval != 0) {
    fprintf (stderr, "error getting socket error code: %s\n",
	     strerror (errno));
  }
  string tmp;
  Socket->writeData (Input);
  Socket->wait ();
  Socket->readSocket (tmp);
  Result = getBytes (tmp);
  return *this;
};

} // End namespace BasexCpp

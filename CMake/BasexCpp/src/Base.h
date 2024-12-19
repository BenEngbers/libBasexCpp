/*
 * Base.h
 *
 *  Created on: 18 apr. 2023
 *      Author: Ben Engbers
 */
#pragma once

#ifndef BASE_H_
#define BASE_H_

#include <string>
#include <vector>
#include <memory>
#ifdef __WIN32__
#include <winsock2.h>
include <Windows.h>
#else
#include <wordexp.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "ClientUtils.h"
#include "BasexSocket.h"

class           QueryObject;
class           BasexSocket;

#include <BasexCpp/export.h>

using namespace std;

namespace BasexCpp {

typedef std::shared_ptr < BasexSocket > BasexSock_Sptr;

/**
 * 	To communicate with the server, BaseX uses a websocket.
 * 	In the server protocol for BaseX, 2 sub-protocols are distinguished:
 * 	- The command protocol applies to commands sent to the server.
  	The Query command is one of the commands. This command is used to create a Query object.
 * 	- The query command protocol applies to commands sent to the Query object.
 */

class Base {
public:
	Base();
BASEXCPP_EXPORT	explicit Base (const string &, const int &, const string &, const string &);
BASEXCPP_EXPORT	explicit Base (const string &, const string &, const string &, const string &);
BASEXCPP_EXPORT	virtual ~ Base ();

	Base (BasexSock_Sptr socket);
	Base addVoid (string Add, ByteVect & To);
	Base addVoid (ByteVect Add, ByteVect & To);
	Base pushByte (unsigned char Oct, ByteVect & To);
	Base handShake (ByteVect Input, ByteVect & Result);

protected:

	BasexSock_Sptr Socket;
};

}

#endif /* BASE_H_ */

/*
 * Base.cpp
 *
 *  Created on: 18 apr. 2023
 *      Author: Ben Engbers
 */

#include <Base.h>

Base::Base(const std::string& DBHOST, const std::string & DBPORT,
           const std::string & DBUSERNAME, const std::string & DBPASSWORD)
           : Socket(new BasexSocket(DBHOST, DBPORT, DBUSERNAME, DBPASSWORD)){};
Base::Base(BasexSocket * socket) { Socket = socket;};

Base::~Base() {};

Base&         Base::addVoid(std::string Add, std::vector<std::byte> & To) {
	std::vector<std::byte> bytes = getBytes(Add);            // Convert std::string 'Add' to std::vector<std::byte>
	addVoid(bytes, To);                                      // Call addVoid(bytes)
	return *this;
}
Base&         Base::addVoid(std::vector<std::byte> Add, std::vector<std::byte> &To) {
	std::vector<std::byte> newAdd = Add;

	if (newAdd.size() == 0) To.push_back((byte)0x00);
	else {
		for (std::size_t i=0; i < newAdd.size(); i++) To.push_back(newAdd[i]);
		To.push_back((byte)0x00);                              // Add suffix \00
	};
	return *this;
}
Base&         Base::pushByte(unsigned char Oct, std::vector<std::byte> & To) {
	To.push_back((byte) Oct);
	return *this;
}
Base&         Base::handShake(std::vector<std::byte> Input, std::vector<std::byte> &Result) {
	int error = 0;
	socklen_t len = sizeof (error);
	int retval = getsockopt (Socket -> get_Socket(), SOL_SOCKET, SO_ERROR, &error, &len);
	if (retval != 0) {
		fprintf(stderr, "error getting socket error code: %s\n", strerror(errno));
	}
	std::string tmp;
	Socket -> writeData(Input);
	Socket -> wait();
	Socket -> readSocket(tmp);
	Result =  getBytes(tmp);
	return *this;
}

BasexSocket * Base::getSocket()      { return Socket;};



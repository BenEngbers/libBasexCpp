#pragma once

#ifndef BASEXCPP_H_
#define BASEXCPP_H_

#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <wordexp.h>
#include <iostream>
#include <list>
#include <sys/types.h>
#include <stdlib.h>

typedef std::vector < std::byte > ByteVect;
typedef std::vector < ByteVect> VectOfByteVect;

#include <memory>
class BasexSocket;
typedef std::shared_ptr < BasexSocket > BasexSock_Sptr;
#include <utility>
#include <cassert>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <array>
#include <memory>
#include <regex>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <cstdio>

#ifdef __WIN32__
#include <winsock2.h>
#include <Windows.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#endif

using namespace std;

namespace BasexCpp {

class QueryObject;
typedef std::unique_ptr < QueryObject > QueryObj_Uptr;

// 1 ### #include "ClientUtils.h"
ByteVect getBytes (string const &s);
string getChar (ByteVect const &b);
string pathExpander (const string & s, int flags = 0);
std::ostream & debug_dump (const ByteVect & str, std::ostream & stm = std::cout);
std::ostream & debug_dump (const string & str, std::ostream & stm = std::cout);

// 2 ### #include "Base.h"
class Base {
public:
	explicit Base (const string &, const int &, const string &, const string &);
	explicit Base (const string &, const string &, const string &, const string &);
	explicit Base (BasexSock_Sptr socket);
protected:
	BasexSock_Sptr Socket;
};

// 3 ### #include "ResponseObj.h"
class ResponseObj {
public:
	ResponseObj ();
	explicit ResponseObj (const string &, ByteVect &);
};

// 4 ### #include "QueryObject.h"
class QueryObject: public Base {
public:
// 	explicit QueryObject (BasexSock_Sptr, const string &);
// 	~ QueryObject ();

	void Close ();
	void Bind (const string & name, const string & value, const string & type = "");
	void Bind (const string & name, const std::list < string > &valueList, const string & type = "");
	void Bind (const string & name, const std::list < string > &valueList, const std::list < string > &typeList);
	ByteVect Next ();
	bool More ();
	void Exec ();
	void Info ();
	void Options ();
	void Context (const string & value, const string & type = "");
	void Updating ();
	void Full ();

	string getQueryString ();
	string getStatus ();
	ByteVect getResultBytes ();
	ByteVect getResult ();
	string getResultString ();
	string asString (const ByteVect & vect);

	ResponseObj Response;
};

// 5 ### #include "BasexClient.h"
class BasexClient: public Base {
//	class BasexClient: virtual public Base {
public:
	explicit BasexClient (const string &, const int &, const string &, const string &);
	explicit BasexClient (const string &, const string &, const string &, const string &);
//	virtual ~ BasexClient ();
	~ BasexClient ();

	void Execute (const string & command);
	void Create (const string & dbName, const string & content = "");
	void Add (const string & path, const string & input);
	void Put (const string & path, const string & input);
	void PutBinary (const string & path, const ByteVect & input);
	QueryObj_Uptr Query (const string & query);
	string getCaller ();
	string getInfo ();
	string getStatus ();
	string getResultString ();
	ByteVect getResultBytes ();
	ByteVect getResult ();
};

}

#endif // BASEXCPP_H_

/*
 * CBasexClient.h
 *
 *  Created on: 28 apr. 2020
 *      Author: Ben Engbers
 */

#ifndef BASEXCLIENT_H_
#define BASEXCLIENT_H_

class BasexSocket;
class ResponseObj;
class QueryObject;

// #include "BasexSocket.h"
#include "ResponseObj.h"
#include "QueryObject.h"
#include "Base.h"
// #include <initializer_list>
// #include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

// #ifndef __WIN32__
//   #include <wordexp.h>
// #endif

#include <regex>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

using namespace std;

class BasexClient : public Base {
  public:
    BasexClient (const std::string&, const std::string&, const std::string&, const std::string&);
    virtual ~BasexClient();
    void Command(const std::string & command);
    void Create(const std::string & dbName, const std::string & content = "");
    void Add(const std::string & path, const std::string & input);
    void Put(const std::string & path, const std::string & input);
    void PutBinary(const std::string & path, const std::vector<std::byte>& input);
    QueryObject * Query(const std::string & query);
    QueryObject * Query(const std::string & query, BasexSocket * socket);

    std::string            getCaller();
    std::string            getInfo();
    std::string            getStatus();
    std::vector<std::byte> getResultBytes();
    std::vector<std::byte> getResult();

  private:
    ResponseObj Response;
    BasexClient& addRawInput(const std::string & App, std::vector<std::byte>& To);
    BasexClient& addRawInput(const std::vector<std::byte> & App, std::vector<std::byte>& To);
    bool         isValidURL(const string & url);
    std::vector <unsigned char> to_vector(std::string const& str);
    std::vector<std::byte>      encode(const std::vector<std::byte>& vect);
    std::string                 encode(const std::string& str);
};

#endif /* BASEXCLIENT_H_ */

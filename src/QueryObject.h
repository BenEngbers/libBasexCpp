/*
 * QueryObject.h
 *
 *  Created on: 12 apr. 2023
 *      Author: Ben Engbers
 */

#ifndef QUERYOBJECT_H_
#define QUERYOBJECT_H_

class BasexSocket;
class ResponseObj;

#include "ResponseObj.h"
#include "Base.h"

// #include <initializer_list>
#include <cassert>
#include <stdexcept>

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstddef>
#include <list>
#include <array>

using namespace std;

class QueryObject : public Base {
public:
	QueryObject(const std::string &, BasexSocket *);
	virtual ~QueryObject();

	void            Close();
  void            Bind(const std::string & name, const std::string & value, const std::string & type = "");
  void            Bind(const std::string & name, const std::list<std::string> & valueList, const std::string & type = "");
  void            Bind(const std::string & name, const std::list<std::string> & valueList, const std::list<std::string> & typeList);
  ByteVect        Next();
  bool            More();
  void            Execute();
  void            Info();
  void            Options();
  void            Context(const std::string & value, const std::string & type = "");
  void            Updating();
  void            Full();

  std::string     queryString {};
	std::string     getQueryString();
  std::string     getStatus();
  ByteVect        getResultBytes();
  ByteVect        getResult();
  std::string     getResultString();
  std::string     asString(const ByteVect & vect);

  ResponseObj     Response;
  VectOfByteVect* Cache { nullptr };

private:
  ByteVect        QueryID {};
  VectOfByteVect  Results();
  int             Pos;
};

#endif /* QUERYOBJECT_H_ */

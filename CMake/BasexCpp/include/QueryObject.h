/*
 * QueryObject.h
 *
 *  Created on: 12 apr. 2023
 *      Author: Ben Engbers
 */
#pragma once

#ifndef QUERYOBJECT_H_
#define QUERYOBJECT_H_

class BasexSocket;
class ResponseObj;

#include <BasexCpp/ResponseObj.h>
#include <BasexCpp/Base.h>

#include <cassert>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstddef>
#include <list>
#include <array>

#include <BasexCpp/export.h>


using namespace std;

namespace BasexCpp {

typedef std::shared_ptr < BasexSocket > BasexSock_Sptr;

/**
 * QueryObject implements the commands from the Query command subprotocol.
 */
class BASEXCPP_EXPORT	QueryObject: public Base
{
public:
  explicit QueryObject (BasexSock_Sptr, const string &);
  ~ QueryObject ();

BASEXCPP_EXPORT 	  void Close ();
BASEXCPP_EXPORT 	  void Bind (const string & name, const string & value, const string & type = "");
BASEXCPP_EXPORT 	  void Bind (const string & name, const std::list < string > &valueList, const string & type = "");
BASEXCPP_EXPORT 	  void Bind (const string & name, const std::list < string > &valueList, const std::list < string > &typeList);
BASEXCPP_EXPORT 	  ByteVect Next ();
BASEXCPP_EXPORT 	  bool More ();
BASEXCPP_EXPORT 	  void Exec ();
BASEXCPP_EXPORT 	  void Info ();
BASEXCPP_EXPORT 	  void Options ();
BASEXCPP_EXPORT 	  void Context (const string & value, const string & type = "");
BASEXCPP_EXPORT 	  void Updating ();
BASEXCPP_EXPORT 	  void Full ();

BASEXCPP_EXPORT 	  string getQueryString ();
BASEXCPP_EXPORT 	  string getStatus ();
BASEXCPP_EXPORT 	  ByteVect getResultBytes ();
BASEXCPP_EXPORT 	  ByteVect getResult ();
BASEXCPP_EXPORT 	  string getResultString ();
BASEXCPP_EXPORT 	  string asString (const ByteVect & vect);

  ResponseObj Response;

private:
  ByteVect QueryID { };
  VectOfByteVect Results ();
  size_t Pos;
  string queryString {
  };
  VectOfByteVect * Cache { nullptr};
};

}


#endif /* QUERYOBJECT_H_ */

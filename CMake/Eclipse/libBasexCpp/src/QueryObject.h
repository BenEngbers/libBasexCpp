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

#include <cassert>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstddef>
#include <list>
#include <array>

using namespace std;

namespace BasexCpp {

typedef std::shared_ptr < BasexSocket > BasexSock_Sptr;

/**
 * QueryObject implements the commands from the Query command subprotocol.
 */
class QueryObject: public Base
{
public:
  explicit QueryObject (BasexSock_Sptr, const string &);
  ~ QueryObject ();

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

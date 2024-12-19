/*
 * ResponseObj.h
 *
 *  Created on: 9 mrt. 2023
 *      Author: Ben Engbers
 */

#ifndef LIBBASEXCPP_SRC_RESPONSEOBJ_H_
#define LIBBASEXCPP_SRC_RESPONSEOBJ_H_

#include <utility>
#include <iterator>
#include <experimental/propagate_const>
#include <memory>
#include <BasexCpp/Base.h>
#include <BasexCpp/ClientUtils.h>

#include <BasexCpp/export.h>

using namespace std;

namespace BasexCpp {

/**
 * ResponseObj.
 * Any interaction with BasexClient or with a QueryObject results in a Response object.
 */

class           ResponseObj {
	class Impl;
	std::experimental::propagate_const<std::unique_ptr<Impl>> pImpl;
public:
BASEXCPP_EXPORT   explicit ResponseObj ();
  virtual ~ ResponseObj ();

  void            setValues (const string &, ByteVect &);
  string          getStatus () const;
  string          getCaller ();
  string          getInfo () const;
  string          getResultString () const;
  ByteVect        getResultBytes ();
  ByteVect        getResult () const;
  VectOfByteVect  Splitted;
};

}

#endif /* LIBBASEXCPP_SRC_RESPONSEOBJ_H_ */

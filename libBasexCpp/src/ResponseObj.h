/*
 * ResponseObj.h
 *
 *  Created on: 9 mrt. 2023
 *      Author: Ben Engbers
 */

#ifndef LIBBASEXCPP_SRC_RESPONSEOBJ_H_
#define LIBBASEXCPP_SRC_RESPONSEOBJ_H_

#include "Base.h"
#include "ClientUtils.h"

using namespace std;

class ResponseObj {
public:
	ResponseObj() ;
	ResponseObj(const std::string &, ByteVect &);
	virtual ~ResponseObj();

	void setValues(const std::string &, ByteVect &);

	string                getStatus();
	string                getCaller() {return CallFunction.c_str();}; // Name from 'calling' function
	ByteVect              getResultBytes() {return ResultBytes;};     // Result without inserted \FF
	string                getInfo();                                  // info or error message
	ByteVect              getResult();                                // Result from Command
	string                getResultString();                          // Result converted to string

	byte                  StatusByte;
	std::string           CallFunction;
	ByteVect              ResultBytes;
	VectOfByteVect        Splitted;

	ByteVect              decode(const ByteVect& encoded);
	static VectOfByteVect splitter(ByteVect & BytesVector, std::byte By = (byte) 0x00);
};

#endif /* LIBBASEXCPP_SRC_RESPONSEOBJ_H_ */

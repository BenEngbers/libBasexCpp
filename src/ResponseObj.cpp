/*
 * ResponseObj.cpp
 *
 *  Created on: 9 mrt. 2023
 *      Author: Ben Engbers
 */

#include "ResponseObj.h"
#include <iterator>

using namespace std;
ResponseObj::ResponseObj() {};
ResponseObj::ResponseObj(const std::string & Caller, ByteVect & Result)
  : CallFunction{ Caller},
	StatusByte{ Result.back() }
{
	Result.pop_back();
	ResultBytes  = decode(Result);    // 'Result' after removing the \FF that may have been inserted
	Splitted = splitter(ResultBytes);
}
ResponseObj::~ResponseObj() {}

void           ResponseObj::setValues(const std::string & Caller, ByteVect & Response) {
	CallFunction = Caller;
	StatusByte   = Response.back();
	ResultBytes  = decode(Response);
	Splitted     = splitter(ResultBytes);
};

string         ResponseObj::getStatus() {	return (StatusByte == (byte) 0) ?	"Status: Success" : "Status: Failed"; };
ByteVect       ResponseObj::getResult() { return Splitted.at(0); };
std::string    ResponseObj::getResultString() {
	ByteVect     from = Splitted.at(0);
	std::string  result(reinterpret_cast<const char *>(&from[0]), from.size());
	return result;
};
string         ResponseObj::getInfo() {
	std::string  splitInfo { "Success: "};
	ByteVect     Split;
	int          info {0};
	bool         StatusComp, CallComp;

	StatusComp = StatusByte == (byte) 0 ? true : false;
	CallComp   = CallFunction.compare("Command") == 0 ? true : false;
	if ((StatusComp && CallComp) || !StatusComp) {
		info = 1;
		if (!StatusComp) splitInfo = "Error:   ";
	};
	splitInfo.append(getChar(Splitted.at(info)));
	return splitInfo.c_str();
};

ByteVect       ResponseObj::decode(const ByteVect& encoded) {
	if (encoded.empty()) throw std::runtime_error("an encoded string is never empty");
	ByteVect    decoded;

	for (std::size_t i = 0; i < encoded.size() - 1; ++i) {
		byte      ch = encoded[i];
		byte      next = encoded[i + 1];
		if (ch == (byte) 0xff) {
			decoded.push_back(next);
			++i;
		} else decoded.push_back(ch);
	}
	return decoded;
}

VectOfByteVect ResponseObj::splitter(ByteVect & BytesVector, std::byte By) {
	VectOfByteVect Splitted;
	ByteVect    Split {};
	byte Zero_Byte { (byte) 0x00 };
	byte FF_Byte   { (byte) 0xff };
	bool PushSplit { false };

	for (int i = 0 ; i < (int) BytesVector.size(); i++) {
		std::byte  TestByte = BytesVector[i];
		Split.push_back(TestByte);
		if ((i == 0 && TestByte == By) || (i == (int) BytesVector.size() -1)) {
			PushSplit = true;
		} else if (TestByte == By) {
			PushSplit = true;
			if (By == Zero_Byte && BytesVector[i-1] == FF_Byte) PushSplit = false;
		};
		if (PushSplit) {
			Splitted.push_back(Split);
		    Split = {};
		    PushSplit = false;
		};
	};
	return Splitted;
};

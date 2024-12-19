/*
 * ResponseObj.cpp
 *
 *  Created on: 9 mrt. 2023
 *      Author: Ben Engbers
 */

#include "ResponseObj.h"

using namespace std;

namespace BasexCpp {

class ResponseObj::Impl {
	string 		  	CallFunction;
	byte            StatusByte;
	ByteVect        ResultBytes;
public:
	void setValues (const string & Caller, ByteVect & Response, ResponseObj& R) {
		CallFunction = Caller;
		StatusByte = Response.back ();
		ResultBytes = decode (Response);
		R.Splitted = splitter (ResultBytes);
	};

	ByteVect decode (const ByteVect & encoded) {
		if (encoded.empty ())
			throw std::runtime_error ("an encoded string is never empty");
		ByteVect decoded;

		for (std::size_t i = 0; i < encoded.size () - 1; ++i) {
			byte ch = encoded[i];
			byte next = encoded[i + 1];
			if (ch == (byte) 0xff) {
				decoded.push_back (next);
				++i;
			} else decoded.push_back (ch);
		}
		return decoded;
	};

	static VectOfByteVect splitter (ByteVect & BytesVector, std::byte By =
			(byte) 0x00) {
		VectOfByteVect Splitted;
		ByteVect Split {  };
		byte Zero_Byte { (byte) 0x00};
		byte FF_Byte { (byte) 0xff};
		bool PushSplit { false};

		for (size_t i = 0; i < BytesVector.size (); i++) {
			std::byte TestByte = BytesVector[i];
			Split.push_back (TestByte);
			if ((i == 0 && TestByte == By) || (i == BytesVector.size () - 1)) {
				PushSplit = true;
			} else if (TestByte == By) {
				PushSplit = true;
				if (By == Zero_Byte && BytesVector[i - 1] == FF_Byte)
					PushSplit = false;
			};
			if (PushSplit) {
				Splitted.push_back (Split);
				Split = { };
				PushSplit = false;
			};
		};
		return Splitted;
	};

	string getStatus () const {
		return (StatusByte == (byte) 0) ? "Status: Success" : "Status: Failed"; };

	string getCaller () { return CallFunction.c_str (); };

	string getInfo (const ResponseObj& R) const {
		string splitInfo { "Success   "};
		size_t info { 0};
		bool StatusComp, CallComp;
		string Info;

		StatusComp = StatusByte == (byte) 0 ? true : false;
		CallComp = CallFunction.compare ("Command") == 0 ? true : false;

		if ((StatusComp && CallComp) || !StatusComp) {
			info = 1;
			if (!StatusComp)
				splitInfo = "Error:   "; };
		splitInfo.append (getChar (R.Splitted.at (info)));

		return splitInfo.c_str ();
	};

	string getResultString(const ResponseObj& R) const{
		ByteVect from = R.Splitted.at (0);
		string result (reinterpret_cast < const char *>(&from[0]), from.size ());
		return result;
	};

	ByteVect getResultBytes () { return ResultBytes; };

};

// ######################################

ResponseObj::ResponseObj () : pImpl{ std::make_unique<Impl> ()} {};
ResponseObj::~ResponseObj () { }

void ResponseObj::setValues (const string & Caller, ByteVect & Response) {
	return pImpl -> setValues(Caller, Response, *this);
}

string ResponseObj::getStatus () const {
	return pImpl -> getStatus();
};

string ResponseObj::getCaller () {
	return pImpl -> getCaller();
};

ByteVect ResponseObj::getResultBytes () {
	return pImpl -> getResultBytes();
};

string ResponseObj::getResultString () const {
	return pImpl -> getResultString(*this);
};

ByteVect ResponseObj::getResult () const { return Splitted.at (0); };

string ResponseObj::getInfo () const {
	return pImpl -> getInfo(*this);
};

} // End namespace BasexCpp

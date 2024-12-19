/*
 * QueryObject.cpp
 *
 *  Created on: 12 apr. 2023
 *      Author: Ben Engbers
 */

#include "QueryObject.h"

#include <iostream>

using namespace std;

namespace BasexCpp {

QueryObject::QueryObject (BasexSock_Sptr socket, const string & Query):
		  Base { BasexSock_Sptr { socket}}
, queryString { Query}

{
	// assert (!Query.empty ());
	ByteVect exec, result;
	pushByte (0x00, exec).addVoid (Query, exec);
	handShake (exec, result);
	Response.setValues ((string) __FUNCTION__, result);
	QueryID = Response.getResultBytes ();
	QueryID.pop_back ();
}

QueryObject::~QueryObject () {
	Close ();
}

void QueryObject::Close () {
	ByteVect exec, result;
	pushByte (0x02, exec).addVoid (QueryID, exec);
	handShake (exec, result);
	Response.setValues ((string) __FUNCTION__, result);
};

void QueryObject::Bind (const string & name, const string & value,
		const string & type) {
	ByteVect        exec, result;
	pushByte (0x03, exec).addVoid (QueryID, exec).addVoid (name, exec);
	addVoid (value, exec).addVoid (type, exec);
	handShake (exec, result);
	Response.setValues ((string) __FUNCTION__, result);
};

void QueryObject::Bind (const string & name,
		const std::list < string > &valueList,
		const string & type) {
	ByteVect exec, result;
	pushByte (0x03, exec).addVoid (QueryID, exec).addVoid (name, exec);
	for (auto val = valueList.begin (); (val != valueList.end ()); ++val) {
		addVoid (*val, exec);
		exec.pop_back ();		// Delete the trailing 00
		pushByte (0x01, exec);	// Separates the values
	};
	exec.pop_back ();		// Delete the trailing 01
	pushByte (0x00, exec);	// Delimit the {value} field
	addVoid (type, exec);

	handShake (exec, result);
	Response.setValues ((string) __FUNCTION__, result);
};

void QueryObject::Bind (const string & name,
		const std::list < string > &valueList,
		const std::list < string > &typeList) {
	ByteVect exec, result;
	pushByte (0x03, exec).addVoid (QueryID, exec).addVoid (name, exec);
	for (auto val = valueList.begin (), type = typeList.begin ();
			(val != valueList.end ()) && (type != typeList.end ()); ++val, ++type)
	{
		addVoid (*val, exec);	// Add value
		exec.pop_back ();		// Delete the trailing 00 from the value(-string)
		pushByte (0x02, exec);	// Separate value and type
		addVoid (*type, exec);	// Add type
		exec.pop_back ();		// Delete the trailing 00
		pushByte (0x01, exec);	// Delete the trailing 00 from the type(-string)
	};
	exec.pop_back ();
	pushByte (0x00, exec);
	pushByte (0x00, exec);

	handShake (exec, result);
	Response.setValues ((string) __FUNCTION__, result);
};

ByteVect QueryObject::Next () {
	if (More ()) {
		ByteVect Next = Cache->at (Pos++);
		return Next;
	} else
		return ByteVect {
	};
};

bool QueryObject::More () {
	if (!Cache) {
		Cache = new VectOfByteVect;
		(*Cache) = QueryObject::Results ();
		Pos = { 0 };
	};
	if (Pos < Cache->size ()) {
		return true;
	} else {
		delete
		Cache;
		Cache = nullptr;
		Pos = { 0 };
		return false;
	};
};

VectOfByteVect QueryObject::Results () {
	ByteVect exec, result;
	pushByte (0x04, exec).addVoid (QueryID, exec);
	handShake (exec, result);
	result.pop_back ();
	Response.setValues ((string) __FUNCTION__, result);
	return Response.Splitted;
};

void QueryObject::Exec () {
	ByteVect exec, result;
	pushByte (0x05, exec).addVoid (QueryID, exec);
	handShake (exec, result);
	Response.setValues ((string) __FUNCTION__, result);
};

void QueryObject::Info () {
	ByteVect exec, result;
	pushByte (0x06, exec).addVoid (QueryID, exec);
	handShake (exec, result);
	Response.setValues ((string) __FUNCTION__, result);
};

void QueryObject::Options () {
	ByteVect exec, result;
	pushByte (0x07, exec).addVoid (QueryID, exec);
	handShake (exec, result);
	Response.setValues ((string) __FUNCTION__, result);
};

void QueryObject::Context (const string & value, const string & type) {
	ByteVect        exec, result;
	pushByte (0x0e, exec).addVoid (QueryID, exec);
	addVoid (value, exec).addVoid (type, exec);
	handShake (exec, result);
	Response.setValues ((string) __FUNCTION__, result);
};

void QueryObject::Updating () {
	ByteVect
	exec,
	result;
	pushByte (0x1e, exec).addVoid (QueryID, exec);
	handShake (exec, result);
	Response.setValues ((string) __FUNCTION__, result);
};

void QueryObject::Full () {
	ByteVect exec, result;
	pushByte (0x1f, exec).addVoid (QueryID, exec);
	handShake (exec, result);
	Response.setValues ((string) __FUNCTION__, result);
};

string QueryObject::getStatus () {
	return Response.getStatus ();
};

ByteVect QueryObject::getResultBytes () {
	return Response.getResultBytes ();
};

ByteVect QueryObject::getResult () {
	return Response.getResult ();
};

string QueryObject::getResultString () {
	return Response.getResultString ();
};

string QueryObject::asString (const ByteVect & vect) {
	string result (reinterpret_cast < const char *>(&vect[0]), vect.size ());
	return result;
};

string QueryObject::getQueryString () {
	return queryString;
};

} // End namespace BasexCpp



/*
 * BasexClient.cpp
 *
 *  Created on: 7 dec. 2022
 *      Author: bengbers
 */

#include "BasexClient.h"

using namespace std;
using namespace curlpp;
using namespace curlpp::options;

namespace BasexCpp {

// ################################################

ByteVect encode (const ByteVect & vect) {
	ByteVect encoded;
	for (auto && ch:vect) {
		if (ch == (byte) 0xff) {
			encoded.push_back ((byte) 0xff);	// (byte)0xff en '\xff' zijn synoniem!?
			encoded.push_back ((byte) 0xff);
		} else if (ch == (byte) 0x00) {
			encoded.push_back ((byte) 0xff);
			encoded.push_back ((byte) 0x00);
		} else {
			encoded.push_back (ch);
		}
	}
	return encoded;

};

bool isValidURL (const string & url) {
	  string pattern =
	    "((http|https)://)(www.)?[a-zA-Z0-9@:%._\\+~#?&//=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)";
	  // Construct regex object
	  std::regex url_regex (pattern);
	  if (std::regex_match (url, url_regex) == true) {
	    return true;
	  } else {
	    return false;
	  }
};

// ################################################

BasexClient::BasexClient (const string & DBHOST,
			  const int & DBPORT,
			  const string & DBUSERNAME,
			  const string & DBPASSWORD):
  Base {
	DBHOST, std::to_string(DBPORT), DBUSERNAME, DBPASSWORD} { };
BasexClient::BasexClient (const string & DBHOST,
			  const string & DBPORT,
			  const string & DBUSERNAME,
			  const string & DBPASSWORD):
  Base {
	DBHOST, DBPORT, DBUSERNAME, DBPASSWORD} { };
BasexClient::~BasexClient () {
};

void BasexClient::Execute (const string & command) {
  ByteVect exec, result;
  addVoid (command, exec).handShake (exec, result);
  Response.setValues ((string) __FUNCTION__, result);
};

void BasexClient::Create (const string & dbName, const string & content) {
  ByteVect        exec, result;
  pushByte (0x08, exec).addVoid (dbName, exec).addVoid (content, exec);
  handShake (exec, result);
  Response.setValues ((string) __FUNCTION__, result);
}

void BasexClient::Add (const string & path, const string & input) {
  ByteVect exec,
    result;
  if (path.empty () || input.empty ())
    perror ("Path and input can not be empty");
  pushByte (0x09, exec).addVoid (path, exec);
  addRawInput (input, exec);
  handShake (exec, result);
  Response.setValues ((string) __FUNCTION__, result);
};

void BasexClient::Put (const string & path, const string & input) {
  ByteVect exec, result;
  if (path.empty () || input.empty ())
    perror ("Path and input can not be empty");
  pushByte (0x0c, exec).addVoid (path, exec);
  addRawInput (input, exec);
  handShake (exec, result);
  Response.setValues ((string) __FUNCTION__, result);
};

void BasexClient::PutBinary (const string & path, const ByteVect & binContent) {
  ByteVect        exec, result;
  if (path.empty () || binContent.empty ())
    perror ("Path and input can not be empty");
  pushByte (0x0d, exec).addVoid (path, exec);
  addRawInput (binContent, exec);
  handShake (exec, result);
  Response.setValues ((string) __FUNCTION__, result);
};

QueryObj_Uptr BasexClient::Query (const string & query) {
  return Query (Socket, query);
};

QueryObj_Uptr BasexClient::Query (BasexSock_Sptr socket, const string & query) {
  return std::unique_ptr < QueryObject > (new QueryObject (socket, query));
};

string BasexClient::getCaller () {
  return Response.getCaller ();
};

string BasexClient::getInfo () {
  return Response.getInfo ();
};

string BasexClient::getStatus () {
  return Response.getStatus ();
};

string BasexClient::getResultString () {
  return Response.getResultString ();
};

ByteVect BasexClient::getResultBytes () {
  return Response.getResultBytes ();
};

ByteVect BasexClient::getResult () {
  return Response.getResult ();
};

BasexClient & BasexClient::addRawInput (const string & Add, ByteVect & To) {
	ByteVect newBytes;
	FILE * fileTest;
	bool isValid = false;
	string openTest;

	if (Add.at (0) == '~')
		openTest = pathExpander (Add);
	else
		openTest = Add;

	fileTest = std::fopen (openTest.c_str (), "r+");
	isValid = isValidURL (Add.c_str ());
	if (fileTest) {		// 'Add' corresponds to a file
		int c;
		string readAdd;
		while ((c = std::fgetc (fileTest)) != EOF) {	// standard C I/O file reading loop
			readAdd.push_back ((char) c);
		}				// Add 'c' to readAdd
		if (std::ferror (fileTest)) {
			std::puts ("I/O error when reading");
		} else if (std::feof (fileTest)) {	            // std::puts("End of file reached successfully");
			newBytes = getBytes (readAdd);	            // Convert readAdd to vector<std::byte>
		}
		std::fclose(fileTest);
	} else if (isValid) {		// 'Add' is a URL
		try {
			std::ostringstream result;
			curlpp::Cleanup cleaner;
			curlpp::Easy request;

			request.setOpt (curlpp::options::Url (Add));
			request.setOpt (new curlpp::Options::WriteStream (&result));
			request.perform ();

			newBytes = getBytes (result.str ());
		}
		catch (curlpp::RuntimeError & e) {
			std::cout << e.what () << std::endl;
		}
		catch (curlpp::LogicError & e) {
			std::cout << e.what () << std::endl;
		}
		catch (std::bad_alloc & all) {
			std::cerr << "bad_alloc exception occurred: " << all.what () << '\n';
		}
	} else {			// 'Add' is a string
		newBytes = getBytes (Add);
	};
	addVoid (newBytes, To);	// Call addRawInput(bytes)
	return *this;
};

BasexClient & BasexClient::addRawInput (const ByteVect & Add, ByteVect & To) {
	ByteVect newAdd = encode (Add);
	addVoid (newAdd, To);		// Call addRawInput(bytes)
	return *this;
};

} // End namespace BasexCpp

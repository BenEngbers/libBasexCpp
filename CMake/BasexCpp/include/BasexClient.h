/*
 * CBasexClient.h
 *
 *  Created on: 28 apr. 2020
 *      Author: Ben Engbers
 */

#ifndef BASEXCLIENT_H_
#define BASEXCLIENT_H_

class           BasexSocket;
class           ResponseObj;
class           QueryObject;

#include <BasexCpp/ResponseObj.h>
#include <BasexCpp/QueryObject.h>
#include <BasexCpp/Base.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <regex>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <cstdio>

#include <BasexCpp/export.h>

using namespace std;

namespace BasexCpp {

class           QueryObject;

typedef std::unique_ptr < QueryObject > QueryObj_Uptr;
typedef std::vector < std::byte > ByteVect;

/**
 * BasexClient
 *
 * Each client provides a session class or script with methods to
 * connect to and communicate with the database server.
 * A socket connection will be established by the constructor,
 * which expects a host, port, username and password as arguments.
 * The port number can be offered as a string or as an integer.
 */
class BasexClient: public Base {
public:
	BasexClient();
	/**
	 * @brief Initiate a BasexClient
	 *
	 * @param DBHOST String
	 * @param DBPORT Can be offered as string or integer
	 * @param DBUSERNAME String
	 * @param DBPASSWORD String
	 *
	 */
	BASEXCPP_EXPORT 	BasexClient (const string &, const int &, const string &, const string &);
	BASEXCPP_EXPORT 	BasexClient (const string &, const string &, const string &, const string &);
	BASEXCPP_EXPORT 	~ BasexClient ();
//	BASEXCPP_EXPORT 	virtual ~ BasexClient ();

	/**
	 * @brief Send a command to the server
	 *
	 * @param command Command to be executed
	 */
	BASEXCPP_EXPORT 	void Execute (const string & command);

	/**
	 * @brief Create a new database
	 *
	 * @param dbName The name from the database
	 * @param content Optional content
	 */
	BASEXCPP_EXPORT 	void Create (const string & dbName, const string & content = "");

	/**
	 * @brief Adds a new document to the opened database.
	 *
	 * @param path The name from the document.
	 * @param input The input (content) can be a UTF-8 encoded XML document, a binary resource, or any other data (such as JSON or CSV) that can be successfully converted to a resource by the server.
	 */
	BASEXCPP_EXPORT 	void Add (const string & path, const string & input);

	/**
	 * @brief Puts (adds or replaces) an XML document resource in the opened database.
	 *
	 * @param path The name from the document.
	 * @param input The input (content) can be a UTF-8 encoded XML document, a binary resource, or any other data (such as JSON or CSV) that can be successfully converted to a resource by the server.
	 */
	BASEXCPP_EXPORT 	void Put (const string & path, const string & input);

	/**
	 * @brief  	Puts (adds or replaces) a binary resource in the opened database.
	 *
	 * @param path The name from the document.
	 * @param input The input (content) can be a UTF-8 encoded XML document, a binary resource, or any other data (such as JSON or CSV) that can be successfully converted to a resource by the server.
	 */
	BASEXCPP_EXPORT 	void PutBinary (const string & path, const ByteVect & input);

	/**
	 * @brief Creates a new query instance and returns its id. The id is used to initiate a unique pointer.
	 *
	 * @return Unique pointer to a QueryObject.
	 */
	BASEXCPP_EXPORT 	QueryObj_Uptr Query (const string & query);

	/**
	 * @brief The name from the command that is send to the BasexClient object.
	 *
	 * @return string
	 */
	BASEXCPP_EXPORT 	string getCaller ();

	/**
	 * @brief The info that the server creates after executing the command.
	 *
	 * Upon successful execution of the command, 'Success' is returned.
	 * In case an error occurred while executing the command, the text 'Error: ' is completed with specific information about the error if possible.
	 *
	 * @return string Either 'Success' or 'Error: \<error feedback\>'.
	 */
	BASEXCPP_EXPORT 	string getInfo ();

	  /**
	   * @brief	String representation from status
	   *
	   * All server responses end with a single \00 byte, which indicates that the process was successful.
	   * If an error occurs, an additional byte \01 is sent, which is then followed by the error message string.
	   *
	   * @ return string "Status: Success" : "Status: Failed"
	   */
	BASEXCPP_EXPORT 	string getStatus ();

	/**
	 * @brief The string representation from the result of a call.
	 *
	 * @return String.
	 */
	BASEXCPP_EXPORT 	string getResultString ();

	/**
	 * @brief The server returns the result of a call as a vector of bytes.
	 *
	 * @return ByteVect.
	 */
	BASEXCPP_EXPORT 	ByteVect getResultBytes ();

	/**
	 * @brief The relevant part of the `resultbytes`, converted to a string.
	 *
	 * @return string Either 'Success' or 'Error'.
	 */
	ByteVect getResult ();

	ResponseObj Response;
protected:
	QueryObj_Uptr Query (BasexSock_Sptr socket, const string & query);
private:
	BasexClient & addRawInput (const string & Add, ByteVect & To);
	BasexClient & addRawInput (const ByteVect & Add, ByteVect & To);
};

}

#endif /* BASEXCLIENT_H_ */

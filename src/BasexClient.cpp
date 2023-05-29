/*
 * BasexClient.cpp
 *
 *  Created on: 7 dec. 2022
 *      Author: bengbers
 */

#include <BasexClient.h>

using namespace std;
using namespace curlpp;
using namespace curlpp::options;

BasexClient::BasexClient(const std::string& DBHOST, const std::string & DBPORT,
                         const std::string & DBUSERNAME, const std::string & DBPASSWORD)
           : Base{DBHOST, DBPORT, DBUSERNAME, DBPASSWORD}{};
BasexClient::~BasexClient(){ delete Socket; };

void          BasexClient::Command(const std::string & command) {
	ByteVect exec, result;
	addVoid(command, exec).handShake(exec, result);
	Response.setValues((std::string) __FUNCTION__, result);
};
void          BasexClient::Create(const std::string & dbName, const std::string & content) {
	ByteVect exec, result;
	pushByte(0x08, exec).addVoid(dbName, exec).addVoid(content, exec);
	handShake(exec, result);
	Response.setValues((std::string) __FUNCTION__, result);
}
void          BasexClient::Add(const std::string & path, const std::string & input) {
	ByteVect exec, result;
	if (path.empty() || input.empty()) perror("Path and input can not be empty");
	pushByte(0x09, exec).addVoid(path, exec);
	addRawInput(input, exec);
	handShake(exec, result);
	Response.setValues((std::string) __FUNCTION__, result);
}
void          BasexClient::Put(const std::string & path, const std::string & input) {
	ByteVect exec, result;
	if (path.empty() || input.empty()) perror("Path and input can not be empty");
	pushByte(0x0c, exec).addVoid(path, exec);
	addRawInput(input, exec);
	handShake(exec, result);
	Response.setValues((std::string) __FUNCTION__, result);
}
void          BasexClient::PutBinary(const std::string & path, const ByteVect & binContent) {
	ByteVect exec, result;
	if (path.empty() || binContent.empty()) perror("Path and input can not be empty");
	pushByte(0x0d, exec).addVoid(path, exec);
	addRawInput(binContent, exec);
	handShake(exec, result);
	Response.setValues((std::string) __FUNCTION__, result);
}
QueryObject*  BasexClient::Query(const std::string & query) { return Query(query, Socket); };
QueryObject*  BasexClient::Query(const std::string & query, BasexSocket * socket) { return new QueryObject(query, socket); };

BasexClient&  BasexClient::addRawInput(const std::string & Add, ByteVect &To) {
	ByteVect newBytes;
	FILE* fileTest;
	bool isValid = false;

	std::string openTest;
	if (Add.at(0) == '~') openTest = pathExpander(Add);
	else openTest = Add;

	fileTest = std::fopen( openTest.c_str(), "r+");
	isValid  = isValidURL(Add.c_str());
	if (fileTest) {                                          // 'Add' corresponds to a file
		int c;
		std::string readAdd;
		while ((c = std::fgetc(fileTest)) != EOF) {            // standard C I/O file reading loop
			// std::putchar(c);
			readAdd.push_back(c); }                              // Add 'c' to readAdd
		if (std::ferror(fileTest)) {
			std::puts("I/O error when reading");
		} else if (std::feof(fileTest)) {
			// std::puts("End of file reached successfully");
			newBytes = getBytes( readAdd);                       // Convert readAdd to vector<std::byte>
		}
		std::fclose(fileTest);
	} else if (isValid) {                                    // 'Add' is a URL
		try
		{
			std::ostringstream result;
			curlpp::Cleanup cleaner;
			curlpp::Easy request;

			request.setOpt(curlpp::options::Url(Add));
			request.setOpt(new curlpp::Options::WriteStream(&result));
			request.perform();

			newBytes = getBytes( result.str());
		}	catch(curlpp::RuntimeError & e)	{
			std::cout << e.what() << std::endl;
		}	catch(curlpp::LogicError & e)	{
			std::cout << e.what() << std::endl;
		} catch (std::bad_alloc& all) {
			std::cerr << "bad_alloc exception occurred: " << all.what() << '\n';
		}
	}	else {                                                 // 'Add' is a string
		newBytes = getBytes( Add);
	};
	addVoid(newBytes, To);                                   // Call addRawInput(bytes)
	return *this;
};
BasexClient&  BasexClient::addRawInput(const ByteVect & Add, ByteVect &To) {
	ByteVect newAdd = encode(Add);
	addVoid(newAdd, To);                                     // Call addRawInput(bytes)
	return *this;
};

ByteVect      BasexClient::encode(const ByteVect & vect) {
	ByteVect encoded;
	for (auto&& ch : vect) {
		if (ch == (byte)0xff) {
			encoded.push_back((byte)0xff);                       // (byte)0xff en '\xff' zijn synoniem!?
			encoded.push_back((byte)0xff);
		} else if (ch == (byte)0x00) {
			encoded.push_back((byte)0xff);
			encoded.push_back((byte)0x00);
		} else {
			encoded.push_back(ch);
		}
	}
	return encoded;
}
bool          BasexClient::isValidURL(const string & url) {  // Regex to check valid URL
	std::string pattern = "((http|https)://)(www.)?[a-zA-Z0-9@:%._\\+~#?&//=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)";
	// Construct regex object
	std::regex url_regex(pattern);
	if (std::regex_match(url, url_regex) == true) {
		return true;
	} else {
		return false;
	}
}
std::string   BasexClient::getCaller()      { return Response.getCaller(); };
std::string   BasexClient::getInfo()        { return Response.getInfo(); };
std::string   BasexClient::getStatus()      { return Response.getStatus(); };
ByteVect      BasexClient::getResultBytes() { return Response.getResultBytes(); };
ByteVect      BasexClient::getResult()      { return Response.getResult(); };

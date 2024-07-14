#ifndef LIBBASEXCPP_H
#define LIBBASEXCPP_H

#include <string>
#include <vector>
#include <list>
#include <cstddef>
#include <ostream>
#include <iostream>
#include <sys/socket.h>

typedef std::vector<std::vector<std::byte>> VectOfByteVect;
typedef std::vector<std::byte>              ByteVect;

std::vector<std::byte> getBytes(std::string const &s);
std::string            getChar (ByteVect const &b);
std::string            pathExpander(const std::string & s, int flags = 0);
std::ostream&          debug_dump(const ByteVect& str, std::ostream& stm = std::cout );
std::ostream&          debug_dump(const std::string& str, std::ostream& stm = std::cout );

class BasexSocket {
public:
  BasexSocket (const std::string, const std::string, const std::string, const std::string);
  ~BasexSocket();
  // ~BasexSocket() { close( Master_sfd);}
   BasexSocket(const BasexSocket&) = delete;
  BasexSocket& operator=(const BasexSocket&) = delete;

  int basex_status();                                      // moet nog geimplementeerd worden

  int get_Socket();
  bool wait();

  int readSocket( std::string &sockResponseString);
  int writeData(const std::string & input);
  int writeData(const std::vector<std::byte> & input);
  int writeData(const char* input);
  int writeByte( unsigned char rawNum);
  std::vector<std::byte> decode(const std::vector<std::byte>& encoded);
} ;

class Base {
public:
  Base(const std::string&, const std::string&, const std::string&, const std::string&);
  Base(BasexSocket * socket);
  virtual ~Base();

  Base& addVoid(std::string Add, std::vector<std::byte> & To);
  Base& addVoid(std::vector<std::byte> Add, std::vector<std::byte> &To);
  Base& pushByte(unsigned char Oct, std::vector<std::byte> &To);
  Base& handShake(std::vector<std::byte> Input, std::vector<std::byte> & Result);

  BasexSocket * Socket;
  BasexSocket * getSocket();
};

class ResponseObj {
public:
	ResponseObj() ;
	ResponseObj(const std::string &, ByteVect &);
	virtual ~ResponseObj();

	void setValues(const std::string &, ByteVect &);

	std::string                getStatus();
	std::string                getCaller() {return CallFunction.c_str();}; // Name from 'calling' function
	ByteVect              getResultBytes() {return ResultBytes;};     // Result without inserted \FF
	std::string                getInfo();                                  // info or error message
	ByteVect              getResult();                                // Result from Command
	std::string           getResultString();                          // Result converted to string

	std::byte                  StatusByte;
	std::string           CallFunction;
	ByteVect              ResultBytes;
	VectOfByteVect        Splitted;

	ByteVect              decode(const ByteVect& encoded);
	static VectOfByteVect splitter(ByteVect & BytesVector, std::byte By = (std::byte) 0x00);
};
class QueryObject : public Base {
public:
	QueryObject(const std::string &, BasexSocket *);
	virtual ~QueryObject();

  void            Close();
  void            Bind(const std::string & name, const std::string & value, const std::string & type = "");
  void            Bind(const std::string & name, const std::list<std::string> & valueList, const std::string & type = "");
  void            Bind(const std::string & name, const std::list<std::string> & valueList, const std::list<std::string> & typeList);
  ByteVect        Next();
  bool            More();
  void            Execute();
  void            Info();
  void            Options();
  void            Context(const std::string & value, const std::string & type = "");
  void            Updating();
  void            Full();

  std::string     queryString {};
  std::string     getQueryString();
  std::string     getStatus();
  ByteVect        getResultBytes();
  ByteVect        getResult();
  std::string     getResultString();
  std::string     asString(const ByteVect & vect);

  ResponseObj     Response;
  VectOfByteVect* Cache { nullptr };
};


class BasexClient : public Base {
  public:
    BasexClient (const std::string&, const std::string&, const std::string&, const std::string&);
    virtual ~BasexClient();
    void Command(const std::string & command);
    void Create(const std::string & dbName, const std::string & content = "");
    void Add(const std::string & path, const std::string & input);
    void Put(const std::string & path, const std::string & input);
    void PutBinary(const std::string & path, const std::vector<std::byte>& input);
    QueryObject * Query(const std::string & query);
    QueryObject * Query(const std::string & query, BasexSocket * socket);

    std::string            getCaller();
    std::string            getInfo();
    std::string            getStatus();
    std::vector<std::byte> getResultBytes();
    std::vector<std::byte> getResult();
};

/*    BasexClient (const std::string&, const std::string&, const std::string&, const std::string&);
    // virtual ~BasexClient();
    void Command(const std::string & command);
    void Create(const std::string & dbName, const std::string & content = "");
    void Add(const std::string & path, const std::string & input);
    void Put(const std::string & path, const std::string & input);
    void PutBinary(const std::string & path, const std::vector<std::byte>& input);
    QueryObject * Query(const std::string & query);
    QueryObject * Query(const std::string & query, BasexSocket * socket);

ResponseObj() ;
ResponseObj(const std::string &, ByteVect &);
    std::string            getCaller();
    std::string            getInfo();
    std::string            getStatus();
    std::vector<std::byte> getResultBytes();
    std::vector<std::byte> getResult();

QueryObject(const std::string &, BasexSocket *);
    void            Close();
    void            Bind(const std::string & name, const std::string & value, const std::string & type = "");
    void            Bind(const std::string & name, const std::list<std::string> & valueList, const std::string & type = "");
    void            Bind(const std::string & name, const std::list<std::string> & valueList, const std::list<std::string> & typeList);
    ByteVect        Next();
    bool            More();
    void            Execute();
    void            Info();
    void            Options();
    void            Context(const std::string & value, const std::string & type = "");
    void            Updating();
    void            Full();

    std::string     queryString {};
    std::string     getQueryString();
    std::string     getStatus();
    ByteVect        getResultBytes();
    ByteVect        getResult();
    std::string     getResultString();
    std::string     asString(const ByteVect & vect);

    ResponseObj     Response;
    VectOfByteVect* Cache { nullptr };
*/
#endif // LIBBASEXCPP_H

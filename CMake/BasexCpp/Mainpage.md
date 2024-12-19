\mainpage libBasexCpp

## Introduction

 'BaseX' is a robust, high-performance XML database engine and a highly 
 compliant XQuery 3.1 processor with full support of the W3C Update and 
 Full Text extensions.

All clients are based on the client/server architecture. Hence, a BaseX 
database server must be started first.  
Each client provides a session class or script with methods to connect to and 
communicate with the database server. A socket connection will be established 
by the constructor, which expects a host, port, username and password as 
arguments. libBasexCpp uses the BasexClient class to instantiate a websocket 
to the Basex web server .  

In the [server protocol](https://docs.basex.org/main/Server_Protocol) for BaseX, 2 sub-protocols are distinguished:
 * 	The command protocol applies to commands sent to the server.
 	+ Command() executes a database command. 
 	+ Create() creates a new database.
 	+ Create(), Add(), Put() and Putbinary() methods pass on input streams 
 		to the corresponding database commands. The input can be a UTF-8 encoded 
 		XML document, a binary resource, or any other data (such as JSON or CSV) 
 		that can be successfully converted to a resource by the server.  
 	+ Query() method creates a query instance and returns an id.  	
 * 	The query command protocol applies to commands sent to the Query object which 
 		is identified by the id as returned by the Query() method.  
 	  Variables and the context item can be bound to that instance. The result 
 	  can either be requested via execute(), or in an iterative manner with the 
 	  more() and next() functions. If an error occurs, an exception will be thrown.  

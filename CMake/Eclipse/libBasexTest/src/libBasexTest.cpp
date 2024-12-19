//============================================================================
// Name        : libBasexTest.cpp
// Author      : Ben Engbers
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <memory>
#include <cstddef>
#include <string>
#include <list>

#include <BasexCpp.h>

#include <iostream>
#include <filesystem>

using namespace std;

using namespace BasexCpp;

int main() {
	std::filesystem::path exe_path = "/proc/self/exe";

	string DBHOST("localhost");
	int DBPORT(1984);
	string DBUSERNAME("Test");
	string DBPASSWORD("testBaseX");

	BasexClient Session (DBHOST, DBPORT, DBUSERNAME, DBPASSWORD);

	Session.Create("TestCpp");

	Session.Add("Add_XML", "https://raw.githubusercontent.com/BaseXdb/basex/master/basex-api/src/test/resources/first.xml");

	string Articles;
	Articles.append(filesystem::read_symlink(exe_path).parent_path()).append("/src/extdata/Articles.xml");
	Session.Add("Add_XML_2", Articles);
	cout << Session.getStatus() << endl;
	cout << Session.getResultString() << endl;

  string path;
  path.append(filesystem::read_symlink(exe_path).parent_path()).append("/src/extdata/xml_files");
  for (const auto & entry : filesystem::directory_iterator(path)) {
  	Session.Add("Add_XML_3", entry.path());
  };
	cout << Session.getStatus() << endl;
	cout << Session.getResultString() << endl;

	Session.Add("Test.xml", "<Line_1 line='1'>Content 1</Line_1>");
	Session.Add("Test.xml", "<Line_2 line='2'>Content 2</Line_2>");
	Session.Add("Books", "<book title='XQuery' author='Walmsley'/>");
	string AddBook { "let $book := <book title='Advanced R' author='Wickham'/>"
		"return insert node $book as last into collection('TestCpp/Books')" };
	QueryObj_Uptr Q_Object = Session.Query(AddBook);
	Q_Object -> Exec();
	cout << Q_Object -> getStatus() << endl;
	cout << Q_Object -> getQueryString() << endl;
	cout << Session.getResultString() << endl;

	string Path = {"Add_XML_4	"};
	string Rep  = {"<x>Hi Friends!</x>"};
	Session.Put(Path, Rep);

	std::vector<int> binTest = {252, 255, 255, 000, 255, 254, 255, 254, 000, };
	std::vector<std::byte> bytes;
	for (int c: binTest) bytes.push_back((std::byte)c);
	Session.PutBinary("test.bin", bytes);
	Session.Execute("binary get test.bin");

	string query { "for $i in 1 to 2 return $i" };
	Q_Object = Session.Query(query);
	cout << Q_Object -> getQueryString() << endl;
	Q_Object -> Exec();
	cout << Q_Object -> getResultString() << endl;

	cout << "\n1 Bind variable \"$name\" to \"number\"" << endl;
	query = "declare variable $name external; for $i in 1 to 2 return element { $name } { $i }";
	Q_Object = Session.Query(query);
	Q_Object -> Bind("$name", "number");
	Q_Object -> Exec();
	cout << Q_Object -> getResultString() << endl;

	cout << "\n1 Bind variable \"$name\" to \"number\". Specify type" << endl;
	query = "declare variable $name external; for $i in 3 to 4 return element { $name } { $i}";
	Q_Object = Session.Query(query);
	cout << Q_Object -> getQueryString() << endl;
	Q_Object -> Bind("$name", "number", "xs:string");
	Q_Object -> Exec();
	cout << Q_Object -> getResultString() << endl;

	cout << "\n3 Bind variable \"$name\" to a list of names." << endl;
	query = "declare variable $name external; "
			"for $t in collection('TestCpp/Books')/book "
			"where $t/@author = $name "
			"return $t/@title/string()";
	Q_Object = Session.Query(query);
	cout << Q_Object -> getQueryString() << endl;
	std::list<string> names { "Walmsley", "Wickham" };
	Q_Object -> Bind("$name", names);
	Q_Object -> Exec();
	cout << Q_Object -> getResultString() << endl;

	cout << "\n4 Bind variable \"$name\" to a list of names. Specify 1 type." << endl;
	Q_Object = Session.Query(query);
	cout << Q_Object -> getQueryString() << endl;
	Q_Object -> Bind("$name", names, "xs:string");
	Q_Object -> Exec();
	cout << Q_Object -> getResultString() << endl;

	cout << "\n5 Bind variable \"$name\" to a list of names and provide a list of types." << endl;
	Q_Object = Session.Query(query);
	cout << Q_Object -> getQueryString() << endl;
	std::list<string> types{2, "xs:string"};
	Q_Object -> Bind("$name", names, types);
	Q_Object -> Exec();
	cout << Q_Object -> getResultString() << endl;

	cout << "\n6 Info" << endl;
	query = "for $i in 1 to 2 return $i";
	Q_Object = Session.Query(query);
	Q_Object -> Exec();
	Q_Object -> Info();
	cout << Q_Object -> getResultString() << endl;

	cout << "\n7 Options" << endl;
	Q_Object -> Options();
	cout << Q_Object -> getResultString() << endl;

	cout << "\n8 Updating" << endl;
	Q_Object -> Updating();
	cout << Q_Object -> getResultString() << endl;

	cout << "\n9 Full" << endl;
	query = "collection('/TestCpp/Test.xml')";
	Q_Object = Session.Query(query);
	Q_Object -> Exec();
	cout << Q_Object -> getResultString() << endl;

	cout << "\n10 Context" << endl;
	query = "for $t in .//text() return string-length($t)";
	string ctxt_val  { "<xml><txt>Hi</txt><txt>World</txt></xml>" };
	string ctxt_type { "document-node()" };
	Q_Object = Session.Query(query);
	Q_Object -> Context(ctxt_val, ctxt_type);
	Q_Object -> Exec();
	cout << Q_Object -> getResultString() << endl;

	cout << "\n11 Results and More()" << endl;
	query = "collection('/TestCpp/Test.xml')";
	Q_Object = Session.Query(query);
	Q_Object -> Exec();
	while (Q_Object -> More())
		debug_dump(Q_Object -> Next());

	Session.Execute("Close");
	cout <<  Session.getInfo() << endl;
	cout << "Ready!" << endl;

	return 0;
};

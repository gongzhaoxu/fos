#pragma once
#include <sstream>
#include "base.h"
using namespace std;
class Menu {
public:
	static void start();
	static vector<string>getLine();
	static void debug(bool flag);
	static void signin();
	static void signup();
	static void mkdir(vector<string> instruction);
	static void touch(vector<string> instruction);
	static void cd(vector<string> instruction);
	static void ls(vector<string> instruction);
	static void debug(vector<string> instruction);
	//shutdown format clear
	static void rename(vector<string> instruction);
	static void rm(vector<string> instruction);
	static void cp(vector<string> instruction);
	static int cpDir(vector<string> instruction);
	static int cpFile(vector<string> instruction);
	static void paste(vector<string> instruction);
	static void mv(vector<string> instruction);
	static void cut(vector<string> instruction);
	static int cutDir(vector<string> instruction);
	static int cutFile(vector<string> instruction);
	static void ln(vector<string> instruction);
	static void open(vector<string> instruction);
	static void close(vector<string> instruction);
	static void read(vector<string> instruction);
	static void write(vector<string> instruction);
	static void help();
	static void pass(vector<string> instruction);
	static void su();
	static void userlist();
	static void disklist(vector<string> instruction);
	static void delUser(vector<string> instruction);
	static void test(vector<string> instruction);
	static void find(vector<string> instruction);
};

#pragma once
#include "base.h"
#include <fstream>
class Disk {
public:
	static void init();
	static byte* read(short block);
	static void write(short block,string str);
	static void write(short block, byte* str);
	static bool isEmpty(short block);
	static short getNext(short block);


	/*
	* mode == 0: 返回空闲块
	* mode == 1: 返回占用块
	* mode == 2: 返回所有块
	*/
	static vector<pair<short, bool>> list(int mode);


};

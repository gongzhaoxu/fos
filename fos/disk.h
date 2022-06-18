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
	* mode == 0: ���ؿ��п�
	* mode == 1: ����ռ�ÿ�
	* mode == 2: �������п�
	*/
	static vector<pair<short, bool>> list(int mode);


};

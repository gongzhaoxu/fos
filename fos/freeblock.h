#pragma once
#include "base.h"
#include <fstream>
class FreeBlock {
	static vector<short>S;//表示空闲块号栈
	static short allocateOneBlock();
	static void freeOneBlock(int BlockNo);
	static vector<short> readFreeBlock(short block);
	static void writeFreeBlock(short block, vector<short> blocks);
public:
	static void format();
	static void init();//基本想法就是吧空闲块号栈读入内存
	static void over();//把空闲块号栈写入外存
	static vector<short>allocateBlocks(int num);
	static int occupy;
	static void freeBlocks(vector<short>Blocks);
};
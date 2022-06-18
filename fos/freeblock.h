#pragma once
#include "base.h"
#include <fstream>
class FreeBlock {
	static vector<short>S;//��ʾ���п��ջ
	static short allocateOneBlock();
	static void freeOneBlock(int BlockNo);
	static vector<short> readFreeBlock(short block);
	static void writeFreeBlock(short block, vector<short> blocks);
public:
	static void format();
	static void init();//�����뷨���ǰɿ��п��ջ�����ڴ�
	static void over();//�ѿ��п��ջд�����
	static vector<short>allocateBlocks(int num);
	static int occupy;
	static void freeBlocks(vector<short>Blocks);
};
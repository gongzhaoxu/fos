#include"freeblock.h"
#include"catalog.h"
#include"inode.h"
#include"base.h"
using namespace std;

vector<short>FreeBlock::S;//表示空闲块号栈
int FreeBlock::occupy = 0;
void FreeBlock::format() {
	for (int i = 0; i < BLOCK_COUNT; i++) {
		if (i == 0) {
			vector<short> input;
			input.push_back(48);
			for (short j = 50; j >= 3; j--) {
				input.push_back(j);
			}
			writeFreeBlock(0, input);//第二个参数是一个数组，第一个数据表示后面跟着多少个数据
			
		}//表示当前块是超级块
		else if (i % 50 == 0) {
			if (i == 4050) {
				vector<short>input;
				input.push_back(46);
				input.push_back(0);
				for (short j = i + 45; j>=i + 1; j--) {
					input.push_back(j);
				}
				writeFreeBlock(i, input);

			}
			else {
				vector<short>input;
				input.push_back(50);
				for (short j = i+ 50; j >= i + 1; j--) {
					input.push_back(j);
				}
				writeFreeBlock(i, input);//第二个参数是一个数组，第一个数据表示后面跟着多少个数据
			}
		}
	}
	init();
	//写初始化1号码盘块。
	Catalog Root;
	Root.save(1);
	Catalog Home;
	int addr = Home.save();
	Inode inode2 = InodeController::create(addr, {});
	Root.decode(1);
	Root.recodes.push_back({ "home",FileType::dir,inode2.block,inode2.id });
	Root.save(1);
}
void FreeBlock::init() {
	vector<short>tmp=readFreeBlock(0);
	for (auto item = tmp.begin(); item != tmp.end(); item++) {
		S.push_back(*item);
	}
	occupy = 0;
}
void FreeBlock::over() {
	writeFreeBlock(0, S);
}
short FreeBlock::allocateOneBlock() {
	short block=-1;
	if (S[0] == 1) {
		if (S[1] == 0) {
			return block;//内存分配失败了
		}
		else {
			block = S[1];
			vector<short>tmp = readFreeBlock(block);
			S.clear();
			for (auto item = tmp.begin(); item != tmp.end(); item++) {

				S.push_back(*item);
			}
			occupy += 1;
			return block;


		}
	}
	else {
		block = S[S[0]];
		S[0]--;
		occupy += 1;
		return block;
	}
}
vector<short>FreeBlock::allocateBlocks(int num) {
	vector<short>tmp;
	for (int i = 0; i < num; i++) {
		int block = allocateOneBlock();
		if (block == -1) {
			//把tmp再次回收
			for (auto _ = tmp.begin(); _ != tmp.end(); _++) {
				freeOneBlock(*_);
			}
			tmp.clear();
			tmp.push_back(-1);
			return tmp;
		}
		else {
			tmp.push_back(block);
		}
	}
	return tmp;
}
void FreeBlock::freeOneBlock(int BlockNo) {
	if (occupy==4048||S[0] == 50) {
		writeFreeBlock(BlockNo, S);
		S.clear();
		S[0] = 1;
		S[1] = BlockNo;
	}
	else {
		S[0]++;
		//S[S[0]] = BlockNo;
		S.push_back(BlockNo);
	}
	occupy--;
}
void FreeBlock::freeBlocks(vector<short>Blocks) {
	for (auto item = Blocks.begin(); item != Blocks.end(); item++) {
		freeOneBlock(*item);
	}
}
vector<short> FreeBlock::readFreeBlock(short block)
{
	if (block > BLOCK_COUNT) {
		exit(Status::diskBlockIdOverflow);
	}
	ifstream fin(".disk", ios::in | ios::binary);
	if (!fin.is_open()) {
		cout << "打开文件失败" << endl;
		exit(Status::openFileFailed);
	}
	vector<short>vec;
	short t;
	try {
		byte* str = new byte[BLOCK_SIZE-1]();
		fin.seekg(block * BLOCK_SIZE+1, ios::beg);
		fin.read((char*)str, sizeof(byte) * (BLOCK_SIZE-1));
		int ptr = 0;
		short num = 0;
		num = str[ptr++] * 256;
		num += str[ptr++];
		vec.push_back(num);
		for (int _ = 0; _ < num; _++) {
			t = str[ptr++] * 256;
			t += str[ptr++];
			vec.push_back(t);
		}
	}
	catch (string e) {
		exit(Status::diskReadFailed);
	}
	return vec;
}
void FreeBlock::writeFreeBlock(short block, vector<short> blocks) {
	//就顺次写就行把blocks里面东西全写进去，打头写
	if (block > BLOCK_COUNT) {
		exit(Status::diskBlockIdOverflow);
	}
	fstream fout(".disk", ios::in | ios::out | ios::binary);
	if (!fout.is_open()) {
		cout << "打开文件失败" << endl;
		exit(Status::openFileFailed);
	}
	byte* str = new byte[BLOCK_SIZE-1]();
	fout.seekp(block * BLOCK_SIZE+1);
	short ptr = 0;
	for (const short& b : blocks) {
		str[ptr++] = b / 256;
		str[ptr++] = b % 256;
	}
	fout.write((char*)str, sizeof(byte)* (BLOCK_SIZE-1));
}

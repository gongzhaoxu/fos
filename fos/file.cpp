#include "file.h"

umap<short, FileOpen>FileOpenController::buffer;

EncodeBytes File::encode()
{
	vector<byte*>vec;
	// 内容（编码）
	byte* cont = contentEncode();
	int size = this->sizeOf();
	int contPtr = 0;
	while (size == 0 || contPtr < size) {
		byte* c = new byte[BLOCK_SIZE]();
		// 占用
		c[0x000] = 255;
		// 计数器
		c[0x001] = counter % 256;
		// 下地址
		c[0x002] = 255;
		c[0x003] = 255;
		// 创建时间
		int created = this->created;
		c[0x004] = created / 256 / 256 / 256;
		c[0x005] = created / 256 / 256 % 256;
		c[0x006] = created / 256 % 256;
		c[0x007] = created % 256;
		// 修改时间
		int modified = this->modified;
		c[0x008] = modified / 256 / 256 / 256;
		c[0x009] = modified / 256 / 256 % 256;
		c[0x00A] = modified / 256 % 256;
		c[0x00B] = modified % 256;
		int ptr = 0x00C;
		int i;
		for (i = 0; contPtr + i < size && ptr + i + 1 < BLOCK_SIZE; i++) {
			c[ptr + i] = cont[contPtr + i];
		}
		vec.push_back(c);
		contPtr += i;
		if (size == 0) {
			break;
		}
	}
	return vec;
}

void File::decode(short block)
{
	byte* c = Disk::read(block);
	this->counter = c[0x001];
	short next = c[0x002] * 256 + c[0x003];
	this->created = c[0x004] * 256 * 256 * 256 + c[0x005] * 256 * 256 + c[0x006] * 256 + c[0x007];
	this->modified = c[0x008] * 256 * 256 * 256 + c[0x009] * 256 * 256 + c[0x00A] * 256 + c[0x00B];
	int block_count = 1;
	int content_size = BLOCK_SIZE - 12;
	byte* bytes = new byte[content_size]();
	for (int i = 0; i < BLOCK_SIZE - 12; i++) {
		bytes[i] = c[0x0C + i];
	}
	delete[] c;
	while (next != -1) {
		int pre_size = content_size;
		content_size += BLOCK_SIZE - 13;
		byte* pre = bytes;
		bytes = new byte[content_size]();
		for (int i = 0; i < pre_size - 1; i++) {
			bytes[i] = pre[i];
		}
		delete[] pre;
		byte* c = Disk::read(next);
		next = c[0x002] * 256 + c[0x003];
		for (int i = 0; i < BLOCK_SIZE - 12; i++) {
			bytes[pre_size - 1 + i] = c[0x0C + i];
		}
		delete[] c;
	}
	contentDncode(bytes, 0);
}

short File::save(short block)
{
	EncodeBytes encodeBytes = encode();
	vector<short>freeBlocks;		// 用于最终分配
	vector<short>blocks;			// 原块
	vector<short>surplusBlocks;		// 剩余块
	while (block != -1) {
		blocks.push_back(block);
		block = Disk::getNext(block);
	}
	if (blocks.size() < encodeBytes.size()) {
		vector<short>vec = FreeBlock::allocateBlocks(encodeBytes.size() - blocks.size());
		freeBlocks = blocks;
		for (short b : vec) {
			freeBlocks.push_back(b);
		}
	}
	else if (blocks.size() > encodeBytes.size()) {
		int i;
		for (i = 0; i < encodeBytes.size(); i++) {
			freeBlocks.push_back(blocks[i]);
		}
		for (; i < blocks.size(); i++) {
			surplusBlocks.push_back(blocks[i]);
		}
		FreeBlock::freeBlocks(surplusBlocks);
	}
	else {
		freeBlocks = blocks;
	}
	if (freeBlocks[0] != -1) {
		for (int i = 0; i < freeBlocks.size() - 1; i++) {
			Disk::write(freeBlocks[i], encodeBytes.get(i, freeBlocks[i + 1]));
		}
		Disk::write(freeBlocks[freeBlocks.size() - 1], encodeBytes.get(freeBlocks.size() - 1, -1));
	}
	return freeBlocks[0];
}

int File::remove(short block)
{
	if (FileOpenController::getCount(block)) {
		return 9;
	}
	vector<short>freeVec;
	short next = block;
	bool isRemove = 0;
	while (next != -1) {
		fstream fio(".disk", ios::in | ios::out | ios::binary);
		if (!fio.is_open()) {
			cout << "打开文件失败" << endl;
			exit(Status::openFileFailed);
		}
		fio.seekg(next * BLOCK_SIZE);
		fio.seekp(next * BLOCK_SIZE);
		byte* bytes = new byte[2]();
		fio.read((char*)bytes, sizeof(byte) * 2);
		bytes[1]--;
		uint8 count = bytes[1];
		if (next == block && count <= 0) {
			isRemove = 1;
		}
		if (isRemove) {
			bytes[0] = 0x00;
			freeVec.push_back(next);
		}
		fio.write((char*)bytes, sizeof(byte) * 2);
		next = Disk::getNext(next);
	}
	if (isRemove) {
		FreeBlock::freeBlocks(freeVec);
		return 1;
	}
	return 0;
}


void File::addCounter(short block)
{
	if (block > BLOCK_COUNT) {
		exit(Status::diskBlockIdOverflow);
	}
	fstream fout(".disk", ios::in | ios::out | ios::binary);
	if (!fout.is_open()) {
		debug("打开文件失败");
		exit(Status::openFileFailed);
	}
	fout.seekg(block * BLOCK_SIZE + 1);
	fout.seekp(block * BLOCK_SIZE + 1);
	byte* counter = new byte;
	fout.read((char*)counter, sizeof(byte));
	(*counter)++;
	fout.write((char*)counter, sizeof(byte));
	delete counter;
}


int FileOpenController::getCount(short block)
{
	FileOpen fileOpen = buffer[block];
	return fileOpen.counter;
}

bool FileOpenController::canRead(short block)
{

	FileOpen fileOpen = buffer[block];
	return fileOpen.mode == FileOpenStatus::read || fileOpen.mode == FileOpenStatus::write;
}

bool FileOpenController::canWrite(short block)
{
	FileOpen fileOpen = buffer[block];
	return fileOpen.mode == FileOpenStatus::write;
}

void FileOpenController::open(short block, FileOpenStatus mode)
{
	FileOpen fileOpen;
	fileOpen.counter = 1;
	fileOpen.mode = mode;
	buffer[block] = fileOpen;
}

FileOpenStatus FileOpenController::close(short block)
{
	if (getCount(block) == 0) {
		return FileOpenStatus::FileCloseFailNotOpen;
	}
	FileOpen fileOpen;
	buffer[block] = fileOpen;
	return FileOpenStatus::FileCloseSuccess;
}

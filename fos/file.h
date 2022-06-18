#pragma once
#include "base.h"
#include "disk.h"
#include "encodebytes.h"
#include "freeblock.h"
class File {
public:
	FileType type;							// 文件类型
	int created;							// 创建时间
	int modified;							// 修改时间
	short counter = 1;						// 计数器
	File(FileType t = FileType::ftxt) :type(t) {}
	File(FileType t, int c, int m) :type(t), created(c), modified(m) {}
	virtual EncodeBytes encode();			// 编码
	virtual void decode(short block);		// 解码
	virtual int sizeOf() = 0;				// 内容大小
	virtual byte* contentEncode() = 0;		// 内容编码
	virtual void contentDncode(byte* bytes, int size) = 0;	// 内容解码
	short save(short block = -1);
	static int remove(short block);
	static void addCounter(short block);
};

enum FileOpenStatus {
	read,
	write,
	none,
	FileCloseFailNotOpen,
	FileCloseSuccess,
};

class FileOpen {
public:
	FileOpenStatus mode = FileOpenStatus::none;
	int counter = 0;
};

class FileOpenController {
public:
	static umap<short, FileOpen>buffer;
	static int getCount(short block);
	static bool canRead(short block);
	static bool canWrite(short block);
	static void open(short block, FileOpenStatus mode);
	static FileOpenStatus close(short block);
};
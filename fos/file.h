#pragma once
#include "base.h"
#include "disk.h"
#include "encodebytes.h"
#include "freeblock.h"
class File {
public:
	FileType type;							// �ļ�����
	int created;							// ����ʱ��
	int modified;							// �޸�ʱ��
	short counter = 1;						// ������
	File(FileType t = FileType::ftxt) :type(t) {}
	File(FileType t, int c, int m) :type(t), created(c), modified(m) {}
	virtual EncodeBytes encode();			// ����
	virtual void decode(short block);		// ����
	virtual int sizeOf() = 0;				// ���ݴ�С
	virtual byte* contentEncode() = 0;		// ���ݱ���
	virtual void contentDncode(byte* bytes, int size) = 0;	// ���ݽ���
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
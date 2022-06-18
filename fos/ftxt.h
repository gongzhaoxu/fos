#pragma once
#include "base.h"
#include "file.h"

class Ftxt :File {
public:
	string content;
	byte* contentEncode() override;
	void contentDncode(byte*, int) override;
	EncodeBytes encode() override;
	void decode(short block) override;
	int sizeOf() override;
	short save(short block = -1) {
		return File::save(block);
	}
	Ftxt() :File(FileType::ftxt) {}
	Ftxt(int c, int m, string cont,FileType type)
		:File(type, c, m), content(cont) {}
};
#pragma once
#pragma once
#include "base.h"
#include "ftxt.h"
class Link :public Ftxt
{
public:
	byte* contentEncode() override {
		return Ftxt::contentEncode();
	}
	void contentDncode(byte* bytes, int l) override {
		Ftxt::contentDncode(bytes, l);
	}
	EncodeBytes encode() override {
		return Ftxt::encode();
	}
	void decode(short block) override {
		Ftxt::decode(block);
	}
	int sizeOf() override {
		return Ftxt::sizeOf();
	}
	short save(short block = -1) {
		return Ftxt::save(block);
	}
	Link(string content) :Ftxt(0, 0, content, FileType::link) {}
	string getDir() {
		return Ftxt::content;
	}
};

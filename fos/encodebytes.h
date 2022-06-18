#pragma once
#include "base.h"
class EncodeBytes:vector<byte*> {
public:
	EncodeBytes(initializer_list<byte*>l) :vector<byte*>(l) {}
	EncodeBytes(vector<byte*>l) :vector<byte*>(l) {}
	byte* get(short index, short next);
	int size() {
		return vector<byte*>::size();
	}
};
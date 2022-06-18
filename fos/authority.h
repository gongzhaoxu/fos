#pragma once
#include "base.h"
enum class Auth :uint8 {
	none = 0b00000000,
	write = 0b00000100,
	run = 0b00001000,
	read = 0b00010000,
	all = 0b00100000,
	creator = 0b01000000,
};

class Authority {
public:
	uint8 authority = 0;
	uint8 user;
	Authority() :authority((uint8)Auth::creator), user(USER) {}
	Authority(uint8 auth) :authority((uint8)auth), user(USER) {}
	Authority(uint8 authority, uint8 user) :authority(authority), user(user) {}
	byte* encode();
	static int sizeOf();
	static Authority decode(byte* bytes);
	friend ostream& operator << (ostream& out, Authority authortiy) {
		out << "(" << (int)authortiy.authority << "," << (int)authortiy.user << ")";
		return out;
	}
	static bool isCreator(uint8 auth) {
		return auth & (uint8)Auth::creator;
	}
	static bool isAll(uint8 auth) {
		return auth & (uint8)Auth::all;
	}
	static bool isRead(uint8 auth) {
		return auth & (uint8)Auth::read;
	}
	static bool isRun(uint8 auth) {
		return auth & (uint8)Auth::run;
	}
	static bool isWrite(uint8 auth) {
		return auth & (uint8)Auth::write;
	}
};

class Authorities {
public:
	umap<uint8, Authority>authorities;
	int sizeOf();
	byte* encode();
	Authorities() {
		authorities[USER] = Authority();
	};
	Authorities(umap<uint8, Authority> l) :authorities(l) {};
	static Authorities decode(byte* bytes);
};


#pragma once
#include "base.h"
#include "authority.h"
#include "disk.h"
class Inode {
public:
	uint8 block;
	uint8 id;
	pointer addr;
	Authorities authorities;
	uint8 getAuthority(uint8 user = USER);
	void addAuthority(uint8 user, uint8 auth);
	void delAuthority(uint8 user, uint8 auth);
	Inode() = default;
	Inode(uint8 id, pointer addr, Authorities authorities) :id(id), addr(addr), authorities(authorities) {}
	Inode(uint8 id, pointer addr) :Inode(id, addr, {}) {}
	int sizeOf();
	byte* encode();
	static Inode decode(byte* bytes);
};

class InodeBlock {
public:
	uint8 block;
	pointer next;
	umap<uint8, Inode>inodes;
	InodeBlock() = default;
	InodeBlock(uint8 block, pointer next, umap<uint8, Inode>inodes) :block(block), next(next), inodes(inodes) {}
	InodeBlock(pointer next, umap<uint8, Inode>inodes) :InodeBlock(1, next, inodes) {}
	InodeBlock(uint8 block, pointer next) :InodeBlock(block, next, {}) {}
	int sizeOf();
	byte* encode();
	static InodeBlock decode(short block);
	void write();
	Inode& operator [](int i) {
		return inodes[i];
	}
};

class InodeController {
public:
	static umap<uint8, InodeBlock>_map;
	static void getBlock(short block);
	static Inode create(pointer addr, Authorities authorities);
	static Inode getInode(short block, uint8 id);
	static void remove(short block, uint8 id);
	static void over();
};
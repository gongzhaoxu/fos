#include "inode.h"
#include "freeblock.h"
#include "disk.h"

umap<uint8, InodeBlock>InodeController::_map;

int InodeBlock::sizeOf()
{
	int size = 5;
	for (auto p : inodes) {
		Inode inode = p.second;
		size += inode.sizeOf();
	}
	return size;
}

byte* InodeBlock::encode()
{
	byte* bytes = new byte[BLOCK_SIZE]();
	// 占用
	bytes[0x000] = 255;
	// 下地址
	bytes[0x001] = next / 256;
	bytes[0x002] = next % 256;
	// Inode
	int ptr = 0x003;
	for (auto p : inodes) {
		Inode inode = p.second;
		int len = inode.sizeOf();
		byte* nodeb = inode.encode();
		for (int i = 0; i < len; i++) {
			bytes[ptr++] = nodeb[i];
		}
		delete[] nodeb;
	}
	bytes[ptr++] = 0x00;
	bytes[ptr++] = 0xFF;
	return bytes;
}

InodeBlock InodeBlock::decode(short block)
{
	byte* bytes = Disk::read(block);
	umap<uint8, Inode>inodes;
	int ptr = 0x003;
	while (ptr <= BLOCK_SIZE - 3 && !(bytes[ptr] == 0x00 && bytes[ptr + 1] == 0xFF)) {
		Inode inode = Inode::decode(bytes + ptr);
		inode.block = block;
		inodes[inode.id] = inode;
		ptr += inode.sizeOf();
	}
	return InodeBlock(block, bytes[1] * 256 + bytes[2], inodes);
}

void InodeBlock::write()
{
	Disk::write(block, encode());
}

uint8 Inode::getAuthority(uint8 user)
{
	return authorities.authorities[user].authority;
}

void Inode::addAuthority(uint8 user, uint8 auth)
{
	Authority authority = authorities.authorities[user];
	authority.authority = authority.authority | auth;
	authorities.authorities[user] = authority;
}

void Inode::delAuthority(uint8 user, uint8 auth)
{
	Authority authority = authorities.authorities[user];
	authority.authority = authority.authority & (~auth);
	authorities.authorities[user] = authority;
}

int Inode::sizeOf()
{
	return authorities.sizeOf() + 3;
}

byte* Inode::encode()
{
	int ll = sizeOf();
	byte* bytes = new byte[sizeOf()]();
	short ptr = 0;
	bytes[ptr++] = id;
	bytes[ptr++] = addr / 256;
	bytes[ptr++] = addr % 256;
	byte* aub = authorities.encode();
	int len = authorities.sizeOf();
	for (int i = 0; i < len; i++) {
		bytes[ptr++] = aub[i];
	}
	delete[] aub;
	return bytes;
}

Inode Inode::decode(byte* bytes)
{
	Inode inode;
	inode.id = bytes[0];
	inode.addr = bytes[1] * 256 + bytes[2];
	inode.authorities = Authorities::decode(bytes + 3);
	return inode;
}

void InodeController::getBlock(short block)
{
	_map[block] = InodeBlock::decode(block);
}

Inode InodeController::create(pointer addr, Authorities authorities)
{
	Inode inode(-1, addr, authorities);
	int len = inode.sizeOf();
	for (auto& p : _map) {
		InodeBlock& inodeBlock = p.second;
		if (inodeBlock.sizeOf() + len <= BLOCK_SIZE) {
			int id = 0;
			while (inodeBlock.inodes.count(id))id++;
			inode.id = id;
			inode.block = p.first;
			inodeBlock[id] = inode;
			_map[p.first] = inodeBlock;
			debug("创建i节点：(" + to_string(inode.block) + "," + to_string(inode.id) + ")");
			return inode;
		}
	}
	vector<short>vec = FreeBlock::allocateBlocks(1);
	// 新建块
	inode.id = 0;
	inode.block = vec[0];
	InodeBlock inodeBlock(inode.block, -1);
	inodeBlock[inode.id] = inode;
	for (auto& p : _map) {
		InodeBlock& inodeBlock = p.second;
		if (inodeBlock.next == -1) {
			inodeBlock.next = inode.block;
			_map[p.first] = inodeBlock;
		}
	}
	_map[inode.block] = inodeBlock;
	debug("创建i节点：(" + to_string(inode.block) + "," + to_string(inode.id) + ")");
	return inode;
}

Inode InodeController::getInode(short block, uint8 id)
{
	if (block == 0 && id == 0) {
		Inode inode(0, 1);
		return inode;
	}
	else if (_map.count(block) == 0) {
		debug("从磁盘获取i节点：(" + to_string(block) + "," + to_string(id) + ")");
		getBlock(block);
		return _map[block][id];
	}
	else {
		debug("从缓存获取i节点：(" + to_string(block) + "," + to_string(id) + ")");
		return _map[block][id];
	}
}

void InodeController::remove(short block, uint8 id)
{
	if (_map.count(block) == 0) {
		debug("删除i节点：(" + to_string(block) + "," + to_string(id) + ")");
		getBlock(block);
	}
	InodeBlock inodeBlock = _map[block];
	inodeBlock.inodes.erase(id);
	_map[block] = inodeBlock;
}

void InodeController::over()
{
	for (auto p : _map) {
		p.second.write();
	}
}

#include "catalog.h"
#include "base.h"
#include "inode.h"
#include "freeblock.h"
#include"ftxt.h"
#include"file.h"
#include "vimw.h"
#define FILE "catalog.cpp"
CatalogRecord CatalogController::cur;
string CatalogController::path;
CatalogRecord CatalogController::ROOT;
map<short, Catalog> CatalogController::cata;
map<string, FileType> CatalogController::type_map;
cpType CatalogController::cpStatus;
pair<pair<short, Catalog>, string> CatalogController::cpStorage;
int CatalogRecord::sizeOf()
{
	return name.size() + 5;
}

byte* CatalogRecord::encode()
{
	byte* bytes = new byte[sizeOf()]();
	bytes[0] = type;
	bytes[1] = block / 256;
	bytes[2] = block % 256;
	bytes[3] = id;
	int ptr = 4;
	char* chars = (char*)bytes;
	for (char c : name) {
		chars[ptr++] = c;
	}
	chars[ptr] = 0xFF;
	return bytes;
}

CatalogRecord CatalogRecord::decode(byte* bytes)
{
	CatalogRecord record;
	record.type = (FileType)bytes[0];
	record.block = bytes[1] * 256 + bytes[2];
	record.id = bytes[3];
	string name;
	int ptr = 4;
	while (bytes[ptr] != 0xFF) {
		name += ((char*)bytes)[ptr++];
	}
	record.name = name;
	return record;
}

int Catalog::sizeOf()
{
	int size = 2;
	for (CatalogRecord record : recodes) {
		size += record.sizeOf();
	}
	return size;
}

byte* Catalog::contentEncode()
{
	byte* bytes = new byte[sizeOf()]();
	int ptr = 0;
	for (CatalogRecord record : recodes) {
		byte* recodeb = record.encode();
		for (int i = 0; i < record.sizeOf(); i++) {
			bytes[ptr++] = recodeb[i];
		}
		delete[] recodeb;
	}
	bytes[ptr++] = 0xFF;
	bytes[ptr++] = 0xFF;
	return bytes;
}
void Catalog::contentDncode(byte* bytes, int len)
{
	recodes.clear();
	int ptr = 0;
	while (bytes[ptr] != 0xFF && bytes[ptr + 1] != 0xFF) {
		CatalogRecord record = CatalogRecord::decode(bytes + ptr);
		recodes.push_back(record);
		ptr += record.sizeOf();
	}
}
void CatalogController::readRootCatalog() {
	Catalog catelog;
	catelog = getStoreCatelog(1, -1);
	CatalogRecord USER;
	for (auto item : catelog.recodes) {
		if (item.name == "home") {
			USER = item;
			break;
		}
	}
	Inode inode = InodeController::getInode(USER.block, USER.id);
	getStoreCatelog(inode.addr, 1);
}
Catalog CatalogController::getStoreCatelog(short addr, short fa) {
	if (cata.find(addr) == cata.end()) {
		Catalog catalog;
		catalog.decode(addr);
		catalog.parent = fa;
		cata[addr] = catalog;
	}
	return cata[addr];
}
void CatalogController::init()
{
	debug("test", FILE, LINE);
	cpStatus = cpType::origin;
	CatalogController::ROOT.block = 0;
	CatalogController::ROOT.id = 0;
	CatalogController::ROOT.name = "";
	CatalogController::ROOT.type = FileType::dir;

	type_map[".txt"] = FileType::ftxt;
	type_map[".link"] = FileType::link;
	cur = ROOT;
	path = "/";
	readRootCatalog();

}
FileType CatalogController::distinguishFileType(string name) {
	int l = name.size() - 1;
	for (; l >= 0; l--) {
		if (name[l] == '.') {
			break;
		}
	}
	string s;
	for (; l < name.size(); l++) {
		s += name[l];

	}
	if (type_map.count(s)) {
		return type_map[s];
	}
	else {
		return FileType::undefined;
	}
}

vector<string> CatalogController::splitDirectory(string dir) {
	vector<string>level;
	if (dir.size() == 0) {
		return level;
	}
	while (1) {
		int index = dir.find('/');
		if (index == string::npos)break;
		if (dir[0] == '/')dir.erase(index, 1);
		else {
			level.push_back(dir.substr(0, index));
			dir.erase(dir.begin(), dir.begin() + index + 1);
		}
	}
	if (dir.size() > 0)
		level.push_back(dir);
	return level;
}
short CatalogController::getAddr(CatalogRecord cur) {
	Inode inode = InodeController::getInode(cur.block, cur.id);
	return inode.addr;
}
short CatalogController::rtnLevel(string dir) {
	vector<string>level = splitDirectory(dir);
	int cnt = 0;
	for (auto item : level) {
		if (item == "..")cnt++;
		else break;
	}
	return cnt;
}
void CatalogController::touchCascade(Catalog catelog, vector<string>level, int fa, int index, string cont) {

	if (index >= level.size())return;
	if (index == level.size() - 1) {
		Ftxt new_file;
		new_file.content = cont;
		int addr = new_file.save();
		Inode inode = InodeController::create(addr, {});
		catelog.recodes.push_back({ level.at(index),distinguishFileType(level.at(index)),inode.block,inode.id });
		catelog.save(fa);
		cata[fa] = catelog;
		return;
	}
	else {
		Catalog new_cata;
		int addr = new_cata.save();
		cata[addr] = new_cata;
		Inode inode = InodeController::create(addr, {});
		catelog.recodes.push_back({ level.at(index),FileType::dir,inode.block,inode.id });
		catelog.save(fa);
		cata[fa] = catelog;
		touchCascade(new_cata, level, addr, index + 1, cont);
		return;
	}
}
void CatalogController::touchPasteCutCascade(Catalog catelog, vector<string>level, int fa, int index, short block, uint8 id) {
	if (index >= level.size())return;
	if (index == level.size() - 1) {
		catelog.recodes.push_back({ level.at(index),distinguishFileType(level.at(index)),block,id });
		catelog.save(fa);
		cata[fa] = catelog;
		return;
	}
	else {
		Catalog new_cata;
		int addr = new_cata.save();
		cata[addr] = new_cata;
		Inode inode = InodeController::create(addr, {});
		catelog.recodes.push_back({ level.at(index),FileType::dir,inode.block,inode.id });
		catelog.save(fa);
		cata[fa] = catelog;
		touchPasteCutCascade(new_cata, level, addr, index + 1, block, id);
		return;
	}
}
void CatalogController::fileLinkCascade(Catalog catelog, vector<string>level, int fa, int index, short block, uint8 id) {
	if (index >= level.size())return;
	if (index == level.size() - 1) {

		Inode inode = InodeController::getInode(block, id);
		File::addCounter(inode.addr);
		catelog.recodes.push_back({ level.at(index),distinguishFileType(level.at(index)),block,id });
		catelog.save(fa);
		cata[fa] = catelog;
		return;
	}
	else {
		Catalog new_cata;
		int addr = new_cata.save();
		cata[addr] = new_cata;
		Inode inode = InodeController::create(addr, {});
		catelog.recodes.push_back({ level.at(index),FileType::dir,inode.block,inode.id });
		catelog.save(fa);
		cata[fa] = catelog;
		fileLinkCascade(new_cata, level, addr, index + 1, block, id);
		return;
	}
}
void CatalogController::fileSoftLinkCascade(Catalog catelog, vector<string> level, int fa, int index, string dir)
{
	if (index >= level.size())return;
	if (index == level.size() - 1) {

		Link link(dir);
		int addr = link.save();
		Inode inode = InodeController::create(addr, {});
		catelog.recodes.push_back({ level.at(0),FileType::link,inode.block,inode.id });
		catelog.save(fa);
		cata[fa] = catelog;
		return;
	}
	else {
		Catalog new_cata;
		int addr = new_cata.save();
		cata[addr] = new_cata;
		Inode inode = InodeController::create(addr, {});
		catelog.recodes.push_back({ level.at(index),FileType::dir,inode.block,inode.id });
		catelog.save(fa);
		cata[fa] = catelog;
		fileSoftLinkCascade(new_cata, level, addr, index + 1, dir);
		return;
	}

}
void CatalogController::touchPasteCopyCascade(Catalog catelog, vector<string>level, int fa, int index, Ftxt ftxt) {
	if (index >= level.size())return;
	if (index == level.size() - 1) {
		int addr = ftxt.save();
		Inode inode = InodeController::create(addr, {});
		catelog.recodes.push_back({ level.at(index),distinguishFileType(level.at(index)),inode.block,inode.id });
		catelog.save(fa);
		cata[fa] = catelog;
		return;
	}
	else {
		Catalog new_cata;
		int addr = new_cata.save();
		cata[addr] = new_cata;
		Inode inode = InodeController::create(addr, {});
		catelog.recodes.push_back({ level.at(index),FileType::dir,inode.block,inode.id });
		catelog.save(fa);
		cata[fa] = catelog;
		touchPasteCopyCascade(new_cata, level, addr, index + 1, ftxt);
		return;
	}
}
bool CatalogController::touch(string dir, string cont) {

	pair<pair<short, Catalog>, vector<string>> catalog = Find(dir);
	if (catalog.second.size() == 1) {
		for (auto item : catalog.first.second.recodes) {
			if (item.name == catalog.second.at(0) && item.type != FileType::dir) {
				return 1;
			}
		}
		Ftxt new_file;
		new_file.content = cont;
		int addr = new_file.save();
		Inode inode = InodeController::create(addr, {});
		catalog.first.second.recodes.push_back({ catalog.second.at(0),distinguishFileType(catalog.second.at(0)),inode.block,inode.id });
		catalog.first.second.save(catalog.first.first);
		cata[catalog.first.first] = catalog.first.second;
		return 0;
	}
	else {
		touchCascade(catalog.first.second, catalog.second, catalog.first.first, 0, cont);
		return 0;
	}



}
short CatalogController::getFa(CatalogRecord cur) {
	short fa;
	if (cata.find(getAddr(cur)) != cata.end()) {
		fa = cata[getAddr(cur)].parent;
	}
	else {
		return -2;
	}
}
string CatalogController::getDir(string dir) {
	if (dir.empty())return path;
	if (dir.at(0) == '~') {
		dir.erase(0, 1);
		dir = "/home/" + USER_NAME + dir;
	}
	else if (dir.at(0) != '/' && dir.at(0) != '~' && !(dir.size() >= 2 && dir.at(0) == '.' && dir.at(1) == '.')) {
		dir = path + dir;
	}
	else if (dir.size() >= 2 && dir.at(0) == '.' && dir.at(1) == '.') {
		int rtn_num = rtnLevel(dir);
		vector<string>one = splitDirectory(dir);
		vector<string>two = splitDirectory(path);
		two.erase(two.end() - rtn_num, two.end());
		for (auto item : one) {
			if (item == "..")continue;
			two.push_back(item);
		}
		dir = "/";
		for (auto item : two) {
			dir += item + "/";
		}
	}
	vector<string>level = splitDirectory(dir);
	string res = "/";
	for (auto item : level) {
		res += item + '/';
	}
	return res;
}
pair<pair<short, Catalog>, vector<string>> CatalogController::Find(string dir) {//不判断dir是不是空，直接当成非空处理

	if (dir.at(0) == '~') {
		dir.erase(0, 1);
		dir = "/home/" + USER_NAME + dir;
	}
	else if (dir.empty())dir = path;
	else if (dir.at(0) != '/' && dir.at(0) != '~' && !(dir.size() >= 2 && dir.at(0) == '.' && dir.at(1) == '.')) {
		dir = path + dir;
	}
	else if (dir.size() >= 2 && dir.at(0) == '.' && dir.at(1) == '.') {
		int rtn_num = rtnLevel(dir);
		vector<string>one = splitDirectory(dir);
		vector<string>two = splitDirectory(path);
		//two.erase(two.end() - rtn_num, two.end());
		for (int i = 0; i < rtn_num; i++) {
			two.pop_back();
		}
		for (auto item : one) {
			if (item == "..")continue;
			two.push_back(item);
		}
		dir = "/";
		for (auto item : two) {
			dir += item + "/";
		}
	}
	Catalog catalog;
	vector<string>level = splitDirectory(dir);
	vector<string>level_tmp = level;
	int fa = -1;
	int cur = 1;
	catalog = getStoreCatelog(cur, fa);
	if (level_tmp.size() == 0) {
		return make_pair(make_pair(cur, catalog), level);
	}
	for (auto item = level_tmp.begin(); item < level_tmp.end() - 1; item++) {
		fa = cur;
		bool flag = 0;
		for (auto _ : catalog.recodes) {
			if (_.name == *item && _.type == FileType::dir) {
				Inode inode = InodeController::getInode(_.block, _.id);
				catalog = getStoreCatelog(inode.addr, cur);
				cur = inode.addr;
				level.erase(level.begin(), level.begin() + 1);
				flag = 1;
				break;

			}

		}
		if (flag == 0) {
			return make_pair(make_pair(cur, catalog), level);
		}
	}
	return make_pair(make_pair(cur, catalog), level);
}
int CatalogController::rmCascade(Catalog tmp, short tmp_addr) {
	int ok_delete = 0;
	for (auto item = tmp.recodes.begin(); item != tmp.recodes.end(); item++) {
		if (item->type == FileType::dir) {
			Catalog catalog;
			Inode inode = InodeController::getInode(item->block, item->id);
			catalog = getStoreCatelog(inode.addr, tmp_addr);
			int k = rmCascade(catalog, inode.addr);
			if (k == 9) {
				ok_delete = 1;
				continue;
			}
			File::remove(inode.addr);
			InodeController::remove(item->block, item->id);
			Catalog tmp2 = tmp;
			for (auto _ = tmp2.recodes.begin(); _ != tmp2.recodes.end(); _++) {
				if (_->name == item->name) {
					tmp2.recodes.erase(_);
					tmp2.save(tmp_addr);
					cata[tmp_addr] = tmp2;
					break;
				}
			}
		}
		else {
			Inode inode = InodeController::getInode(item->block, item->id);
			if (FileOpenController::getCount(inode.addr)) {
				ok_delete = 1;
				continue;
			}
			File::remove(inode.addr);
			InodeController::remove(item->block, item->id);
			Catalog tmp2 = tmp;
			for (auto _ = tmp2.recodes.begin(); _ != tmp2.recodes.end(); _++) {
				if (_->name == item->name) {
					tmp2.recodes.erase(_);
					tmp2.save(tmp_addr);
					cata[tmp_addr] = tmp2;
					break;
				}
			}

		}

	}
	if (ok_delete == 1)return 9;
	else return 0;
}
int CatalogController::rm(string dir, bool isDir) {
	pair<pair<short, Catalog>, vector<string>>catalog = Find(dir);
	if (catalog.second.size() != 1)return 2;
	else if (catalog.second.size() == 1) {
		if (isDir == false) {
			bool flag = 0;
			for (auto item = catalog.first.second.recodes.begin(); item != catalog.first.second.recodes.end(); item++) {
				if (item->name == catalog.second.at(0) && item->type != FileType::dir) {
					flag = 1;
					//删除一下
					Inode inode = InodeController::getInode(item->block, item->id);
					if (FileOpenController::getCount(inode.addr)) {
						return 9;
					}
					File::remove(inode.addr);
					InodeController::remove(item->block, item->id);
					catalog.first.second.recodes.erase(item);
					catalog.first.second.save(catalog.first.first);
					cata[catalog.first.first] = catalog.first.second;
					return 0;
				}
			}
			if (flag == 0)return 2;
		}
		else {
			bool flag = 0;
			for (auto item = catalog.first.second.recodes.begin(); item != catalog.first.second.recodes.end(); item++) {
				if (item->name == catalog.second.at(0) && item->type == FileType::dir) {
					flag = 1;
					//删除一下
					Inode inode = InodeController::getInode(item->block, item->id);
					Catalog catelog = getStoreCatelog(inode.addr, catalog.first.first);
					if (rmCascade(catelog, inode.addr) == 9) {
						return 8;
					}
					File::remove(inode.addr);
					InodeController::remove(item->block, item->id);
					catalog.first.second.recodes.erase(item);
					catalog.first.second.save(catalog.first.first);
					cata[catalog.first.first] = catalog.first.second;
					return 1;
				}
			}
			if (flag == 0)return 2;
		}
	}
}
int CatalogController::rename(string a, string b, bool isDir) {
	pair<pair<short, Catalog>, vector<string>>catalog = Find(a);
	if (catalog.second.size() > 1)return 2;
	else if (catalog.second.size() == 1) {
		for (auto item : b) {
			if (item == '/') {
				return 1;
			}
		}
		bool exist = 0;
		for (auto item : catalog.first.second.recodes) {
			if (isDir) {
				if (item.name == catalog.second.at(0) && item.type == FileType::dir) {
					exist = 1;
					break;
				}
			}
			else {
				if (item.name == catalog.second.at(0) && item.type != FileType::dir) {
					exist = 1;
					break;
				}
			}
		}
		if (exist == 0)return 2;
		exist = 0;
		for (auto item : catalog.first.second.recodes) {
			if (isDir) {
				if (item.name == b && item.type == FileType::dir) {
					exist = 1;
					break;
				}
			}
			else {
				if (item.name == b && item.type != FileType::dir) {
					exist = 1;
					break;
				}
			}
		}
		if (exist == 1)return 3;
		for (auto item = catalog.first.second.recodes.begin(); item != catalog.first.second.recodes.end(); item++) {
			if (isDir) {
				if (item->name == catalog.second.at(0) && item->type == FileType::dir) {
					item->name = b;
					catalog.first.second.save(catalog.first.first);
					cata[catalog.first.first] = catalog.first.second;
				}
			}
			else {
				if (item->name == catalog.second.at(0) && item->type != FileType::dir) {
					item->name = b;
					item->type = distinguishFileType(b);
					catalog.first.second.save(catalog.first.first);
					cata[catalog.first.first] = catalog.first.second;
				}
			}
		}
		return 0;
	}
	else return 2;
}
Catalog CatalogController::ls(string dir) {
	if (dir == "")dir = path;

	pair<pair<short, Catalog>, vector<string>>catalog = Find(dir);
	Catalog Fail;
	Fail.parent = -2;
	if (catalog.second.size() > 1) {
		return Fail;
	}
	else  if (catalog.second.size() == 0 && catalog.first.first == 1) {
		return catalog.first.second;
	}
	else if (catalog.second.size() == 1) {
		bool exist = 0;
		for (auto item : catalog.first.second.recodes) {
			if (item.name == catalog.second.at(0) && item.type == FileType::dir) {
				Inode inode = InodeController::getInode(item.block, item.id);
				Catalog catelog = getStoreCatelog(inode.addr, catalog.first.first);
				exist = 1;
				return catelog;
			}
		}
		if (exist == 0)return Fail;
	}


}
bool CatalogController::cd(string dir) {

	pair<pair<short, Catalog>, vector<string>>catalog = Find(dir);

	if (catalog.second.size() > 1) {

		return 1;
	}
	else if (catalog.second.size() == 0 && catalog.first.first == 1) {
		path = "/";
		return 0;
	}
	else if (catalog.second.size() == 1) {

		bool exist = 0;
		for (auto item : catalog.first.second.recodes) {
			if (item.name == catalog.second.at(0) && item.type == FileType::dir) {

				exist = 1;
				path = getDir(dir);
				return 0;
			}
		}
		if (exist == 0)return 1;
	}
}
void CatalogController::mkdirCascade(Catalog catelog, vector<string>level, int fa, int index) {
	if (index >= level.size())return;
	Catalog new_cata;
	int addr = new_cata.save();
	cata[addr] = new_cata;
	Inode inode = InodeController::create(addr, {});
	catelog.recodes.push_back({ level.at(index),FileType::dir,inode.block,inode.id });
	catelog.save(fa);
	cata[fa] = catelog;
	mkdirCascade(new_cata, level, addr, index + 1);
	return;
}
void CatalogController::mkdirCutCascade(Catalog catelog, vector<string>level, int fa, int index, short block, uint8 id) {
	if (index >= level.size())return;
	if (index == level.size() - 1) {
		catelog.recodes.push_back({ level.at(index),FileType::dir,block,id });
		catelog.save(fa);
		cata[fa] = catelog;
		return;
	}
	Catalog new_cata;
	int addr = new_cata.save();
	cata[addr] = new_cata;
	Inode inode = InodeController::create(addr, {});
	catelog.recodes.push_back({ level.at(index),FileType::dir,inode.block,inode.id });
	catelog.save(fa);
	cata[fa] = catelog;
	mkdirCutCascade(new_cata, level, addr, index + 1, block, id);
	return;
}
bool CatalogController::mkdir(string dir) {
	pair<pair<short, Catalog>, vector<string>>catalog = Find(dir);

	if (catalog.second.size() == 0)return 1;
	if (catalog.second.size() == 1 && findNameIn(catalog.first.second, catalog.second.at(0), true) == 1)return 1;
	mkdirCascade(catalog.first.second, catalog.second, catalog.first.first, 0);
	return 0;
}
bool CatalogController::findNameIn(Catalog cata, string name, bool isDir) {
	bool exist = 0;
	for (auto item : cata.recodes) {
		if (item.name == name) {
			if (isDir) {
				if (item.type == FileType::dir) {
					exist = 1;
					break;
				}
			}
			else {
				if (item.type != FileType::dir) {
					exist = 1;
					break;
				}
			}
		}
	}
	if (exist == 0)return 0;
	else return 1;

}
bool CatalogController::findNameDir(Catalog cata, string name) {
	bool exist = 0;
	bool has_dir = 0;
	for (auto item : cata.recodes) {
		if (item.name == name) {
			exist = 1;
			if (item.type == FileType::dir) {
				has_dir = 1;
			}

		}
	}
	if (exist && !has_dir)return 0;
	else return 1;


}
void CatalogController::fdCascade(pair<short, Catalog> cur, string name, string dir, vector<pair<Ftxt, string>>& res)
{
	for (auto item : cur.second.recodes) {
		if (item.type == FileType::dir) {
			Inode inode = InodeController::getInode(item.block, item.id);
			Catalog cat = getStoreCatelog(inode.addr, cur.first);
			fdCascade(make_pair(inode.addr, cat), name, dir + item.name + '/', res);

		}
		else {
			if (item.name == name) {
				Ftxt file;
				Inode inode = InodeController::getInode(item.block, item.id);
				file.decode(inode.addr);
				res.push_back(make_pair(file, dir + item.name));
			}
		}
	}
}

void CatalogController::fdSimilarCascade(pair<short, Catalog> cur, string name, string dir, vector<pair<Ftxt, string>>& res)
{
	for (auto item : cur.second.recodes) {
		if (item.type == FileType::dir) {
			Inode inode = InodeController::getInode(item.block, item.id);
			Catalog cat = getStoreCatelog(inode.addr, cur.first);
			fdSimilarCascade(make_pair(inode.addr, cat), name, dir + item.name + '/', res);

		}
		else {
			if (similar(name, item.name)) {
				Ftxt file;
				Inode inode = InodeController::getInode(item.block, item.id);
				file.decode(inode.addr);
				res.push_back(make_pair(file, dir + item.name));
			}
		}
	}
}


int CatalogController::cp(string dir, bool isDir, bool isCut) {
	if (isCut && isDir)
		cpStatus = cpType::CutDir;
	else if (isCut && !isDir)
		cpStatus = cpType::CutTxt;
	else if (!isCut && isDir)
		cpStatus = cpType::CopyDir;
	else
		cpStatus = cpType::CopyTxt;
	pair<pair<short, Catalog>, vector<string>>catalog = Find(dir);
	if (catalog.second.size() != 1)return 1;
	else {
		if (!findNameIn(catalog.first.second, catalog.second.at(0), isDir))return 1;
		cpStorage.first = catalog.first;
		cpStorage.second = catalog.second.at(0);
		return 0;
	}
}
Catalog eraseOneCatalogRecord(Catalog catalog, int block, int id) {
	for (auto _ = catalog.recodes.begin(); _ != catalog.recodes.end(); _++) {
		if (_->block == block && _->id == id) {
			catalog.recodes.erase(_);
			return catalog;
		}
	}
	return catalog;
}
pair<short, uint8> CatalogController::mkdirCopyCascade(pair<short, Catalog>catalog) {

	Catalog my;
	for (auto item : catalog.second.recodes) {
		if (item.type == FileType::dir) {
			Inode inode = InodeController::getInode(item.block, item.id);
			Catalog child = getStoreCatelog(inode.addr, catalog.first);
			pair<short, Catalog>pr;
			pr.first = inode.addr;
			pr.second = child;
			pair<short, uint8> g = mkdirCopyCascade(pr);
			my.recodes.push_back({ item.name, item.type, g.first, g.second });
		}
		else {
			Inode inode = InodeController::getInode(item.block, item.id);
			Ftxt file;
			file.decode(inode.addr);
			Ftxt file_copy = file;
			int addr = file_copy.save();
			inode = InodeController::create(addr, {});
			my.recodes.push_back({ item.name, item.type, inode.block, inode.id });
		}

	}
	int my_addr = my.save();
	Inode inode = InodeController::create(my_addr, {});
	cata[my_addr] = my;
	return make_pair(inode.block, inode.id);
}
int CatalogController::paste(string dir, bool ismv) {

	if (cpStatus == cpType::origin)return 1;
	bool isDir = 0;
	bool isCut = 0;
	if (cpStatus == cpType::CopyDir || cpStatus == cpType::CutDir)isDir = 1;
	if (cpStatus == cpType::CutDir || cpStatus == cpType::CutTxt)isCut = 1;

	dir = getDir(dir);
	pair<pair<short, Catalog>, vector<string>>test_dir = Find(dir);
	if (!ismv && test_dir.second.size() == 1 && !findNameDir(test_dir.first.second, test_dir.second.at(0)))return 3;

	if (!ismv && dir.at(dir.size() - 1) != '/')dir += "/" + cpStorage.second + "/";
	else if (!ismv) dir += cpStorage.second + "/";
	pair<pair<short, Catalog>, vector<string>>catalog = Find(dir);
	if (catalog.second.size() == 1 && findNameIn(catalog.first.second, catalog.second.at(0), isDir) == 1)return 2;

	if (isDir && isCut) {
		//是一个路径，但是我剪切
		short block;
		uint8 id;
		for (auto item : cpStorage.first.second.recodes) {
			if (item.name == cpStorage.second && item.type == FileType::dir) {
				block = item.block;
				id = item.id;
				Catalog cata_tmp = cpStorage.first.second;
				cata_tmp = eraseOneCatalogRecord(cata_tmp, block, id);
				cata_tmp.save(cpStorage.first.first);
				cata[cpStorage.first.first] = cata_tmp;
				break;
			}
		}
		mkdirCutCascade(catalog.first.second, catalog.second, catalog.first.first, 0, block, id);

	}
	else if (isDir && !isCut) {

		pair<short, Catalog> copysrc;
		for (auto item : cpStorage.first.second.recodes) {
			if (item.name == cpStorage.second && item.type == FileType::dir) {
				Inode inode = InodeController::getInode(item.block, item.id);
				Catalog icata = getStoreCatelog(inode.addr, cpStorage.first.first);
				copysrc.first = inode.addr;
				copysrc.second = icata;
				break;
			}
		}
		pair<short, uint8> num_need;
		num_need = mkdirCopyCascade(copysrc);
		mkdirCutCascade(catalog.first.second, catalog.second, catalog.first.first, 0, num_need.first, num_need.second);
	}
	else if (!isDir && isCut) {
		//是一个文件，但是我只做剪切操作
		short block;
		uint8 id;
		for (auto item : cpStorage.first.second.recodes) {
			if (item.name == cpStorage.second && item.type != FileType::dir) {
				block = item.block;
				id = item.id;
				Catalog cata_tmp = cpStorage.first.second;
				cata_tmp = eraseOneCatalogRecord(cata_tmp, block, id);
				cata_tmp.save(cpStorage.first.first);
				cata[cpStorage.first.first] = cata_tmp;
				break;
			}
		}
		touchPasteCutCascade(catalog.first.second, catalog.second, catalog.first.first, 0, block, id);
	}
	else {
		//是一个文件，我做复制操作
		Ftxt  copy_txt;
		for (auto item : cpStorage.first.second.recodes) {
			if (item.name == cpStorage.second && item.type != FileType::dir) {
				Inode inode = InodeController::getInode(item.block, item.id);
				copy_txt.decode(inode.addr);
				break;
			}
		}
		touchPasteCopyCascade(catalog.first.second, catalog.second, catalog.first.first, 0, copy_txt);
	}

	//最后修改缓存的东西
	cpStatus = cpType::origin;
	return 0;
}
int CatalogController::mv(string src, string dest, bool isDir) {

	pair<pair<short, Catalog>, vector<string>>Src = Find(src);
	pair<pair<short, Catalog>, vector<string>>Dest = Find(dest);
	if (Src.second.size() > 1)return 1;
	if (Src.second.size() == 1 && !findNameIn(Src.first.second, Src.second.at(0), isDir))return 1;
	if (Dest.second.size() == 0 && Dest.first.first == 1) {
		dest = "/" + Src.second.at(0) + "/";
		Dest = Find(dest);
		if (Dest.second.size() == 1 && findNameIn(Dest.first.second, Dest.second.at(0), isDir))return 2;
		cp(src, isDir, true);
		paste(dest, true);
		return 0;
	}
	else if (Dest.second.size() == 1 && !findNameDir(Dest.first.second, Dest.second.at(0)))return 3;
	dest = getDir(dest) + Src.second.at(0) + "/";
	Dest = Find(dest);
	if (Dest.second.size() == 1 && findNameIn(Dest.first.second, Dest.second.at(0), isDir))return 2;
	cp(src, isDir, true);
	paste(dest, true);
	return 0;
}
int CatalogController::link(string src, string dest, int mode) {

	if (mode == 0) {
		pair<pair<short, Catalog>, vector<string>>Src = Find(src);
		pair<pair<short, Catalog>, vector<string>>Dest = Find(dest);
		if (Src.second.size() > 1)return 1;
		if (Src.second.size() == 1 && !findNameIn(Src.first.second, Src.second.at(0), false))return 1;
		if (Dest.second.size() == 0 && Dest.first.first == 1) {
			dest = "/" + Src.second.at(0) + "/";
			Dest = Find(dest);
			if (Dest.second.size() == 1 && findNameIn(Dest.first.second, Dest.second.at(0), false))return 2;
			short block;
			uint8 id;
			for (auto item : Src.first.second.recodes) {
				if (item.name == Src.second.at(0) && item.type != FileType::dir) {
					block = item.block;
					id = item.id;
					break;
				}
			}
			fileLinkCascade(Dest.first.second, Dest.second, Dest.first.first, 0, block, id);
			return 0;
		}
		else if (Dest.second.size() == 1 && !findNameDir(Dest.first.second, Dest.second.at(0)))return 3;
		dest = getDir(dest) + Src.second.at(0) + "/";
		Dest = Find(dest);
		if (Dest.second.size() == 1 && findNameIn(Dest.first.second, Dest.second.at(0), false))return 2;
		short block;
		uint8 id;
		for (auto item : Src.first.second.recodes) {
			if (item.name == Src.second.at(0) && item.type != FileType::dir) {
				block = item.block;
				id = item.id;
				break;
			}
		}
		fileLinkCascade(Dest.first.second, Dest.second, Dest.first.first, 0, block, id);
		return 0;
	}
	else if (mode == 1) {
		pair<pair<short, Catalog>, vector<string>>Src = Find(src);
		pair<pair<short, Catalog>, vector<string>>Dest = Find(dest);
		if (Src.second.size() > 1)return 1;
		if (Src.second.size() == 1 && !findNameIn(Src.first.second, Src.second.at(0), false))return 1;
		if (Dest.second.size() == 0 && Dest.first.first == 1) {
			dest = "/" + Src.second.at(0) + "/";
			Dest = Find(dest);
			if (Dest.second.size() == 1 && findNameIn(Dest.first.second, Dest.second.at(0), false))return 2;
			string src_chuan = getDir(src);
			src_chuan.erase(src_chuan.end() - 1);
			fileSoftLinkCascade(Dest.first.second, Dest.second, Dest.first.first, 0, src_chuan);
			return 0;
		}
		else if (Dest.second.size() == 1 && !findNameDir(Dest.first.second, Dest.second.at(0)))return 3;
		dest = getDir(dest) + Src.second.at(0) + "/";
		Dest = Find(dest);
		if (Dest.second.size() == 1 && findNameIn(Dest.first.second, Dest.second.at(0), false))return 2;
		string src_chuan = getDir(src);
		src_chuan.erase(src_chuan.end() - 1);
		fileSoftLinkCascade(Dest.first.second, Dest.second, Dest.first.first, 0, src_chuan);
		return 0;
	}
}
bool CatalogController::open(string src, FileOpenStatus mode) {

	pair<pair<short, Catalog>, vector<string>>Src = Find(src);
	if (Src.second.size() != 1)return 1;
	if (!findNameIn(Src.first.second, Src.second.at(0), false))return 1;
	Inode inode;
	for (auto item : Src.first.second.recodes) {
		if (item.name == Src.second.at(0) && item.type != FileType::dir) {
			inode = InodeController::getInode(item.block, item.id);
			break;
		}
	}
	FileOpenController::open(inode.addr, mode);
	return 0;
}
bool CatalogController::close(string src) {
	pair<pair<short, Catalog>, vector<string>>Src = Find(src);
	if (Src.second.size() != 1)return 1;
	if (!findNameIn(Src.first.second, Src.second.at(0), false))return 1;
	Inode inode;
	for (auto item : Src.first.second.recodes) {
		if (item.name == Src.second.at(0) && item.type != FileType::dir) {
			inode = InodeController::getInode(item.block, item.id);
			break;
		}
	}
	FileOpenController::close(inode.addr);
	return 0;
}

int CatalogController::write(string src)
{
	pair<pair<short, Catalog>, vector<string>>Src = Find(src);
	if (Src.second.size() != 1)return 1;
	if (!findNameIn(Src.first.second, Src.second.at(0), false))return 1;
	Inode inode;
	string file_name;
	for (auto item : Src.first.second.recodes) {
		if (item.name == Src.second.at(0) && item.type != FileType::dir) {
			inode = InodeController::getInode(item.block, item.id);
			file_name = item.name;
			break;
		}
	}
	if (!FileOpenController::canRead(inode.addr))return 2;
	if (!FileOpenController::canWrite(inode.addr))return 3;

	//写文件
	Ftxt file;
	file.decode(inode.addr);
	string content = file.content;
	vimw texteditor; //创建文件编辑器类
	texteditor.editable(true);//设置content是否可编辑 true 可编辑,false不可编辑
	texteditor.setFileName(file_name);//设置文件名
	texteditor.setContent(content);//设置编辑的最初内容
	texteditor.runProg();//显示、编辑循环
	file.content = texteditor.getResult();
	file.save(inode.addr);
	return 0;
}

int CatalogController::read(string src)
{
	pair<pair<short, Catalog>, vector<string>>Src = Find(src);
	if (Src.second.size() != 1)return 1;
	if (!findNameIn(Src.first.second, Src.second.at(0), false))return 1;
	Inode inode;
	string file_name;
	for (auto item : Src.first.second.recodes) {
		if (item.name == Src.second.at(0) && item.type != FileType::dir) {
			inode = InodeController::getInode(item.block, item.id);
			file_name = item.name;
			break;
		}
	}
	if (!FileOpenController::canRead(inode.addr))return 2;

	//读取文件
	Ftxt file;
	file.decode(inode.addr);
	string content = file.content;
	vimw texteditor; //创建文件编辑器类

	texteditor.editable(false);//设置content是否可编辑 true 可编辑,false不可编辑

	texteditor.setFileName(file_name);//设置文件名

	texteditor.setContent(content);//设置编辑的最初内容

	texteditor.runProg();//显示、编辑循环
	return 0;
}

vector<pair<Ftxt, string>> CatalogController::fd(string name, string dir, bool cascade)
{
	vector<pair<Ftxt, string>>res;
	pair<pair<short, Catalog>, vector<string>>Dir = Find(dir);
	if (Dir.second.size() > 1)return res;
	else if (Dir.second.size() == 0 && Dir.first.first == 1)
	{
		Catalog cur = cata[1];
		short fa = 1;
		string res_dir = getDir(dir);
		if (!cascade) {
			for (auto item : Dir.first.second.recodes) {
				if (item.name == name && item.type != FileType::dir) {
					Inode  inode = InodeController::getInode(item.block, item.id);
					Ftxt file;
					file.decode(inode.addr);
					res.push_back(make_pair(file, res_dir + item.name));
				}
			}
		}
		else {
			fdCascade(make_pair(fa, cur), name, res_dir, res);
		}

	}
	else if (Dir.second.size() == 1 || (Dir.second.size() == 0 && Dir.first.first == 1)) {

		Catalog cur;
		short fa;
		string res_dir = getDir(dir);
		if (!findNameIn(Dir.first.second, Dir.second.at(0), true))return res;
		for (auto item : Dir.first.second.recodes) {
			if (item.name == Dir.second.at(0) && item.type == FileType::dir) {
				Inode inode = InodeController::getInode(item.block, item.id);
				getStoreCatelog(inode.addr, Dir.first.first);
				cur = getStoreCatelog(inode.addr, Dir.first.first);
				fa = inode.addr;
				break;
			}
		}
		if (!cascade) {
			for (auto item : cur.recodes) {
				if (item.name == name && item.type != FileType::dir) {
					Inode  inode = InodeController::getInode(item.block, item.id);
					Ftxt file;
					file.decode(inode.addr);
					res.push_back(make_pair(file, res_dir + item.name));
				}
			}
		}
		else {
			fdCascade(make_pair(fa, cur), name, res_dir, res);
		}
	}
	return res;
}
bool CatalogController::similar(string a, string b) {
	if (a.size() > b.size())return 0;
	else {
		bool exist = 0;
		for (int i = 0; i < b.size(); i++) {
			int j;
			for (j = 0; j < a.size(); j++) {
				if (i + j >= b.size())break;
				if (a[j] != b[i + j])break;
			}
			if (j == a.size()) {
				exist = 1;
				break;
			}
		}
		return exist;
	}
}
vector<pair<Ftxt, string>> CatalogController::similarFd(string name, string dir, bool cascade)
{
	vector<pair<Ftxt, string>>res;
	pair<pair<short, Catalog>, vector<string>>Dir = Find(dir);
	if (Dir.second.size() > 1)return res;
	else if (Dir.second.size() == 0 && Dir.first.first == 1)
	{
		Catalog cur = cata[1];
		short fa = 1;
		string res_dir = getDir(dir);
		if (!cascade) {
			for (auto item : Dir.first.second.recodes) {
				if (similar(name, item.name) && item.type != FileType::dir) {
					Inode  inode = InodeController::getInode(item.block, item.id);
					Ftxt file;
					file.decode(inode.addr);
					res.push_back(make_pair(file, res_dir + item.name));
				}
			}
		}
		else {
			fdSimilarCascade(make_pair(fa, cur), name, res_dir, res);
		}

	}
	else if (Dir.second.size() == 1 || (Dir.second.size() == 0 && Dir.first.first == 1)) {

		Catalog cur;
		short fa;
		string res_dir = getDir(dir);
		if (!findNameIn(Dir.first.second, Dir.second.at(0), true))return res;
		for (auto item : Dir.first.second.recodes) {
			if (item.name == Dir.second.at(0) && item.type == FileType::dir) {
				Inode inode = InodeController::getInode(item.block, item.id);
				getStoreCatelog(inode.addr, Dir.first.first);
				cur = getStoreCatelog(inode.addr, Dir.first.first);
				fa = inode.addr;
				break;
			}
		}
		if (!cascade) {
			for (auto item : cur.recodes) {
				if (similar(name, item.name) && item.type != FileType::dir) {
					Inode  inode = InodeController::getInode(item.block, item.id);
					Ftxt file;
					file.decode(inode.addr);
					res.push_back(make_pair(file, res_dir + item.name));
				}
			}
		}
		else {
			fdSimilarCascade(make_pair(fa, cur), name, res_dir, res);
		}
	}
	return res;
}


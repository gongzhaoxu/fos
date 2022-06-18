#pragma once
#include "base.h"
#include "file.h"
#include"ftxt.h"
#include"link.h"
class CatalogRecord {
public:
	string name;
	FileType type;
	// 块号
	short block;
	uint8 id;
	byte* encode();
	CatalogRecord(string name = "", FileType type = FileType::undefined, short block = -1, uint8 id = -1) :
		name(name), type(type), block(block), id(id) {}
	static CatalogRecord decode(byte* bytes);
	int sizeOf();
	friend ostream& operator << (ostream& out, CatalogRecord record) {
		out << "(" << record.name << "," << FileTypeString(record.type) << "," << (short)record.block << "," << (short)record.id << ")";
		return out;
	}
};

class Catalog :File {
public:
	short parent;
	vector<CatalogRecord>recodes;
	EncodeBytes encode() override {
		return File::encode();
	}
	void decode(short block) override {

		File::decode(block);
	}
	Catalog(initializer_list<CatalogRecord>l = {}) :recodes(l) {}
	Catalog(vector<CatalogRecord>l) :recodes(l) {}
	int sizeOf() override;						// 内容大小
	byte* contentEncode() override;				// 内容编码
	void contentDncode(byte*, int) override;	// 内容解码
	short save(short block = -1) {
		return File::save(block);
	}
	friend ostream& operator << (ostream& out, Catalog catalog) {
		out << "[";
		if (catalog.recodes.size() > 0) {
			out << catalog.recodes[0];
		}
		for (int i = 1; i < catalog.recodes.size(); i++) {
			out << "," << catalog.recodes[i];
		}
		out << "]";
		return out;
	}
};

class CatalogController {
private:
	static vector<string> splitDirectory(string dir);
	static map<string, FileType>type_map;
	static pair<pair<short, Catalog>, vector<string>> Find(string dir);
	static string getDir(string dir);
	static short getAddr(CatalogRecord cur);
	static short getFa(CatalogRecord cur);
	static void readRootCatalog();
	static Catalog getStoreCatelog(short addr, short fa);
	static int rmCascade(Catalog tmp, short fa);
	static map<short, Catalog>cata;
	static short rtnLevel(string dir);
	static void touchCascade(Catalog catelog, vector<string>level, int fa, int index,string cont);
	static void touchPasteCutCascade(Catalog catelog, vector<string>level, int fa, int index, short block, uint8 id);
	static void fileLinkCascade(Catalog catelog, vector<string>level, int fa, int index, short block, uint8 id);
	static void fileSoftLinkCascade(Catalog catelog, vector<string>level, int fa, int index, string dir);
	static void touchPasteCopyCascade(Catalog catelog, vector<string>level, int fa, int index, Ftxt ftxt);
	static void mkdirCascade(Catalog catelog, vector<string>level, int fa, int index);
	static FileType distinguishFileType(string name);
	static pair<short, uint8> mkdirCopyCascade(pair<short, Catalog>catalog);
	static void mkdirCutCascade(Catalog catelog, vector<string>level, int fa, int index, short block, uint8 id);
	static bool findNameIn(Catalog cata, string name, bool isDir);
	static pair<pair<short, Catalog>, string> cpStorage;
	static bool findNameDir(Catalog cata, string name);
	static void fdCascade(pair<short, Catalog> cur, string name, string dir, vector<pair<Ftxt, string>>& res);
	static void fdSimilarCascade(pair<short, Catalog> cur, string name, string dir, vector<pair<Ftxt, string>>& res);

	static cpType cpStatus;
public:
	static void init();
	static CatalogRecord ROOT;
	static bool similar(string a, string b);
	static string path;
	static int rm(string dir, bool isDir = false);
	static int rename(string a, string b, bool isDir = false);
	static bool touch(string dir,string cont="");
	static Catalog  ls(string dir = "");
	static bool mkdir(string dir);
	static int cp(string dir, bool isDir, bool isCut);
	static int paste(string dir, bool mv);
	static bool cd(string dir);
	static int mv(string src, string dest, bool isDir);
	static int link(string src, string dest, int mode);
	static bool open(string src, FileOpenStatus mode);
	static bool close(string src);
	static int  write(string src);
	static int read(string src);
	static vector <pair<Ftxt, string>> fd(string name, string dir, bool cascade);
	static vector <pair<Ftxt, string>> similarFd(string name, string dir, bool cascade);
	static CatalogRecord cur;
};
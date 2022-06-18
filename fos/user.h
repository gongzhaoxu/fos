#pragma once
#include "base.h"
#include "file.h"
class User {
public:
	uint8 _id;
	string name;
	string pass;
	int sizeOf();
	byte* encode();						// ���ݱ���
	static User decode(byte* bytes);	// ���ݽ���
	User() :_id(255) {}
	User(uint8 _id, string name, string pass) :_id(_id), name(name), pass(pass) {}
	friend ostream& operator << (ostream& out, User user) {
		out << "(" << (short)user._id << "," << user.name << "," << user.pass << ")";
		return out;
	}
};

class UserFile :File {
public:
	umap<short, User>users;
	EncodeBytes encode() override {
		return File::encode();
	}
	void decode(short block) override {
		File::decode(block);
	}
	UserFile(umap<short, User>l = {}) :users(l) {}
	int sizeOf() override;						// ���ݴ�С
	byte* contentEncode() override;				// ���ݱ���
	void contentDncode(byte*, int) override;	// ���ݽ���
	short save(short block = -1) {
		return File::save(block);
	}
	friend ostream& operator << (ostream& out, UserFile userFile) {
		out << "[";
		if (userFile.users.size() > 0) {
			out << userFile.users[0];
		}
		for (int i = 1; i < userFile.users.size(); i++) {
			out << "," << userFile.users[i];
		}
		out << "]";
		return out;
	}
};

class UserController {
	static UserFile userFile;
	static void signUpSucceced(User user);
public:
	static void format();
	static void init();
	static User getUserById(uint8 _id);
	static User getUserByName(string name);
	static Status signIn(string name, string pass);		// ע��
	static Status signUp(string name, string pass);		// ��¼
	static Status su(string pass);						// �л�����Ա
	static Status delUser(string name);						// �л�����Ա
	static int newPass(string pass);
	static int newPass(string name, string pass);
	static vector<User> getList();
	static void over();
};

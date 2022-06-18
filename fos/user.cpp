#include "user.h"
#include "catalog.h"

UserFile UserController::userFile;

int User::sizeOf()
{
	return name.size() + pass.size() + 3;
}

byte* User::encode()
{
	byte* bytes = new byte[sizeOf()]();
	bytes[0] = _id;
	int ptr = 1;
	for (int i = 0; i < name.size(); i++) {
		((char*)bytes)[ptr++] = name[i];
	}
	bytes[ptr++] == 0xFF;
	for (int i = 0; i < pass.size(); i++) {
		((char*)bytes)[ptr++] = pass[i];
	}
	bytes[ptr] == 0xFF;
	return bytes;
}

User User::decode(byte* bytes)
{
	User user;
	user._id = bytes[0];
	user.name = string((char*)(bytes + 1));
	user.pass = string((char*)(bytes + user.name.size() + 2));
	return user;
}

int UserFile::sizeOf()
{
	int size = 2;
	for (auto p : users) {
		User user = p.second;
		size += user.sizeOf();
	}
	return size;
}

byte* UserFile::contentEncode()
{
	byte* bytes = new byte[sizeOf()]();
	int ptr = 0;
	for (auto p : users) {
		User user = p.second;
		byte* userb = user.encode();
		for (int i = 0; i < user.sizeOf(); i++) {
			bytes[ptr++] = userb[i];
		}
		delete[] userb;
	}
	bytes[ptr++] = 0x00;
	return bytes;
}

void UserFile::contentDncode(byte* bytes, int block)
{
	users.clear();
	int ptr = 0;
	while (!(bytes[ptr] == 0x00 && bytes[ptr + 1] == 0x00)) {
		User user = User::decode(bytes + ptr);
		users[user._id] = user;
		ptr += user.sizeOf();
	}
}

void UserController::signUpSucceced(User user)
{
	USER = user._id;
	USER_NAME = user.name;
	CatalogController::mkdir("~");
	CatalogController::cd("~");
}

void UserController::format()
{
	UserFile file;
	userFile = file;
	userFile.save(USER_BLOCK);
}

void UserController::init()
{
	userFile.decode(USER_BLOCK);
}

User UserController::getUserById(uint8 _id)
{
	return userFile.users[_id];	// _id == 255
}

User UserController::getUserByName(string name)
{
	for (auto p : userFile.users) {
		User user = p.second;
		if (user.name == name) {
			return user;
		}
	}
	return User();	// _id == 255
}

Status UserController::signIn(string name, string pass)
{
	if (name.size() == 0) {
		return Status::userSignInFailedNameNull;
	}
	if (pass.size() == 0) {
		return Status::userSignInFailedPassNull;
	}
	if (name.size() > 32) {
		return Status::userSignInFailedNameOverflow;
	}
	if (pass.size() > 32) {
		return Status::userSignInFailedPassOverflow;
	}
	if (getUserByName(name)._id != 255) {
		return Status::userSignInFailedUserExisted;
	}
	uint8 id = 1;
	while (userFile.users.count(id)) id++;
	if (id >= 255) {
		return Status::userSignInFailedUserOverflow;
	}
	User user(id, name, pass);
	userFile.users[id] = user;
	signUpSucceced(user);
	return Status::userSignInSucceced;
}

Status UserController::signUp(string name, string pass)
{
	User user = getUserByName(name);
	if (user._id == 255) {
		return Status::userSignUpFailedUserNotFound;
	}
	if (user.pass != pass) {
		return Status::userSignUpFailedPassError;
	}
	signUpSucceced(user);
	return Status::userSignUpSucceced;
}

int UserController::newPass(string pass)
{
	User user = userFile.users[USER];
	user.pass = pass;
	userFile.users[USER] = user;
	return true;
}

Status UserController::su(string pass)
{
	User user = getUserByName("root");
	if (user._id == 255) {
		return Status::userSignUpFailedUserNotFound;
	}
	if (user.pass != pass) {
		return Status::userSignUpFailedPassError;
	}
	USER = user._id;
	USER_NAME = user.name;
	return Status::userSignUpSucceced;
}

Status UserController::delUser(string name)
{
	User user = getUserByName(name);
	if (user._id == 255) {
		return Status::userSignUpFailedUserNotFound;
	}
	userFile.users.erase(user._id);
	return Status::deleteUserSucceced;
}

int UserController::newPass(string name, string pass)
{
	if (USER_NAME != "root") {
		return 2;
	}
	User user = getUserByName(name);
	if (user._id == 255) {
		return 1;
	}
	user.pass = pass;
	userFile.users[user._id] = user;
	return 0;
}


vector<User> UserController::getList()
{
	if (USER_NAME != "root") {
		return { {255,0,0} };
	}
	vector<User> users;
	for (auto p : userFile.users) {
		users.push_back(p.second);
	}
	return users;
}

void UserController::over()
{
	userFile.save(USER_BLOCK);
}

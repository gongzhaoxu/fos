#include "disk.h"
#define FILE "disk.cpp"

void Disk::init() {
	Timer timer;
	ofstream fout(".disk", ios::out | ios::binary);
	if (!fout.is_open()) {
		debug("���ļ�ʧ��");
		exit(Status::openFileFailed);
	}
	try {
		for (int i = 0; i < DISK_SIZE; i++) {
			char c = 0;
			fout.write((char*)&c, sizeof(char));
		}
	}
	catch (string e) {
		exit(Status::diskClearFailed);
	}
	debug(timer.end("���̳�ʼ�����"));
}

byte* Disk::read(short block)
{
	Timer timer;
	if (block > BLOCK_COUNT) {
		exit(Status::diskBlockIdOverflow);
	}
	ifstream fin(".disk", ios::in | ios::binary);
	if (!fin.is_open()) {
		debug("���ļ�ʧ��");
		exit(Status::openFileFailed);
	}
	byte* temp;
	try {
		temp = new byte[BLOCK_SIZE]();
		fin.seekg(block * BLOCK_SIZE, ios::beg);
		fin.read((char*)temp, sizeof(byte) * BLOCK_SIZE);
	}
	catch (string e) {
		exit(Status::diskReadFailed);
	}
	debug(timer.end("���̿�" + to_string(block) + "��ȡ���"));
	return temp;
}

void Disk::write(short block, byte* str)
{
	Timer timer;
	if (block > BLOCK_COUNT) {
		exit(Status::diskBlockIdOverflow);
	}
	fstream fout(".disk", ios::in | ios::out | ios::binary);
	if (!fout.is_open()) {
		debug("���ļ�ʧ��");
		exit(Status::openFileFailed);
	}
	try {
		fout.seekp(block * BLOCK_SIZE);
		fout.write((char*)str, sizeof(byte) * BLOCK_SIZE);
	}
	catch (string e) {
		exit(Status::diskWriteFailed);
	}
	debug(timer.end("���̿�" + to_string(block) + "д�����"));
}

bool Disk::isEmpty(short block)
{
	ifstream fin(".disk", ios::in | ios::binary);
	if (!fin.is_open()) {
		debug("���ļ�ʧ��");
		exit(Status::openFileFailed);
	}
	char* t;
	t = new char();
	fin.seekg(block * BLOCK_SIZE, ios::beg);
	fin.read(t, sizeof(char));
	if (*t == -1) {
		return false;
	}
	else {
		return true;
	}
}

short Disk::getNext(short block)
{
	ifstream fin(".disk", ios::in | ios::binary);
	if (!fin.is_open()) {
		debug("���ļ�ʧ��");
		exit(Status::openFileFailed);
	}
	byte* t;
	t = new byte[4]();
	fin.seekg(block * BLOCK_SIZE, ios::beg);
	fin.read((char*)t, sizeof(char) * 4);
	if (t[0] == 0x00) {
		return -1;
	}
	return t[2] * 256 + t[3];
}

void Disk::write(short block, string str)
{
	write(block, str.c_str());
}

/*
* mode == 0: ���ؿ��п�
* mode == 1: ����ռ�ÿ�
* mode == 2: �������п�
*/
vector<pair<short, bool>> Disk::list(int mode)
{
	vector<pair<short, bool>>vec;
	for (short i = 0; i < BLOCK_COUNT; i++) {
		if (mode == 0) {
			if (isEmpty(i)) {
				vec.emplace_back(i, false);
			}
		}
		else if (mode == 1) {
			if (!isEmpty(i)) {
				vec.emplace_back(i, true);
			}
		}
		else {
			if (isEmpty(i)) {
				vec.emplace_back(i, false);
			}
			else {
				vec.emplace_back(i, true);
			}
		}
	}
	return vec;
}




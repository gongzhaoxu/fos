#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <map>
#include <unordered_map>
#include "timer.h"
#define LINE __LINE__
using namespace std;
template<typename T, typename P>
using umap = unordered_map<T, P>;
using byte = unsigned char;
using int8 = char;
using uint8 = byte;
using pointer = unsigned short;
const pointer null = 0xffff;
const short BLOCK_COUNT = 4096;
const short BLOCK_SIZE = 1024;
const int DISK_SIZE = BLOCK_COUNT * BLOCK_SIZE;
const int CUSTOM = 10;
const short USER_BLOCK = 2;
extern bool DEBUG;
extern uint8 USER;
extern string USER_NAME;
extern bool isFormat;
enum Status {
	openFileFailed = 90000,
	diskClearFailed,
	diskBlockIdOverflow,
	diskBlockSizeOverflow,
	diskReadFailed,
	diskWriteFailed,
	diskInputFailed,
	diskOutputFailed,
	userSignInFailedNameNull,
	userSignInFailedPassNull,
	userSignInFailedNameOverflow,
	userSignInFailedPassOverflow,
	userSignInFailedUserExisted,
	userSignInFailedUserOverflow,
	userSignInSucceced,
	userSignUpFailedUserNotFound,
	userSignUpFailedPassError,
	userSignUpSucceced,
	deleteUserSucceced,
};
enum FileType {
	dir,
	ftxt,
	link,
	undefined,
};
enum cpType {
	origin,
	CopyTxt,
	CopyDir,
	CutTxt,
	CutDir,
};
string FileTypeString(FileType fileType);
template<typename A, typename B, typename C>
void debug(A out, B file, C line);
template<typename A, typename B>
void debug(A out);

template<typename A, typename B, typename C>
inline void debug(A out, B file, C line)
{
	HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (DEBUG) {
		SetConsoleTextAttribute(outHandle, 0x08);
		cout << file << "(" << line << ") " << out << endl;
		SetConsoleTextAttribute(outHandle, 0x07);
	}
}

template<typename A>
inline void debug(A out)
{
	HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (DEBUG) {
		SetConsoleTextAttribute(outHandle, 0x08);
		cout << out << endl;
		SetConsoleTextAttribute(outHandle, 0x07);
	}
}

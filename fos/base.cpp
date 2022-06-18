#include "base.h"

bool DEBUG = true;
uint8 USER;
string USER_NAME;
bool isFormat = false;
string FileTypeString(FileType fileType) {
	switch (fileType)
	{
	case FileType::dir:
		return "目录";
	case FileType::ftxt:
		return "文本文件";
	default:
		return "未知类型";
	}
}

#include "base.h"

bool DEBUG = true;
uint8 USER;
string USER_NAME;
bool isFormat = false;
string FileTypeString(FileType fileType) {
	switch (fileType)
	{
	case FileType::dir:
		return "Ŀ¼";
	case FileType::ftxt:
		return "�ı��ļ�";
	default:
		return "δ֪����";
	}
}

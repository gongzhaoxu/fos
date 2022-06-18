#include "ftxt.h"

byte* Ftxt::contentEncode()
{
	char* c = new char[content.size() + 1]();
	strcpy(c, content.c_str());
	return (byte*)c;
}

void Ftxt::contentDncode(byte* c, int offset)
{
	content = string((char*)c);
}

EncodeBytes Ftxt::encode()
{
	return File::encode();
}

void Ftxt::decode(short block)
{
	File::decode(block);
}

int Ftxt::sizeOf()
{
	return content.size();
}

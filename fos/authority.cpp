#include "authority.h"
int Authority::sizeOf()
{
	return 2;
}

byte* Authority::encode()
{
	byte* bytes = new byte[2]();
	bytes[0] = authority;
	bytes[1] = user;
	return bytes;
}

Authority Authority::decode(byte* bytes)
{
	return Authority(bytes[0], bytes[1]);
}

int Authorities::sizeOf()
{
	return authorities.size() * 2;
}

byte* Authorities::encode()
{
	byte* bytes = new byte[sizeOf()]();
	int ptr = 0;
	for (auto p : authorities) {
		Authority au = p.second;
		byte* bau = au.encode();
		for (int j = 0; j < au.sizeOf(); j++) {
			bytes[ptr++] = bau[j];
		}
		delete[] bau;
	}
	return bytes;
}

Authorities Authorities::decode(byte* bytes)
{
	int ptr = 0;
	Authorities authorities;
	while (bytes[ptr] & 0x80) {
		Authority authority = Authority::decode(bytes + ptr);
		authorities.authorities[authority.user] = authority;
		ptr += Authority::sizeOf();
	}
	return authorities;
}

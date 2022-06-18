#include "encodebytes.h"
byte* EncodeBytes::get(short index,short next)
{
	byte* c = this->at(index);
	if (next == -1) {
		c[2] = c[3] = 255;
	}
	else {
		c[2] = next / 256;
		c[3] = next % 256;
	}
	return c;
}
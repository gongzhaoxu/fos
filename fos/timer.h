#pragma once
#include <windows.h>
#include "base.h"
using namespace std;
class Timer
{
private:
	LARGE_INTEGER begin_time;
	LARGE_INTEGER end_time;
	LARGE_INTEGER cpu;
public:
	Timer();
	string end(string tips = "ºÄÊ±");
};
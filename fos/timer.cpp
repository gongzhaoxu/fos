#include "timer.h"
#include <sstream>
Timer::Timer() :begin_time({ 0 }), end_time({ 0 })
{
	QueryPerformanceCounter(&begin_time);
	QueryPerformanceFrequency(&cpu);
}

string Timer::end(string tips)
{
	QueryPerformanceCounter(&end_time);
	double diff = ((double)end_time.QuadPart - (double)begin_time.QuadPart) / (double)cpu.QuadPart * 1000;
	stringstream ss;
	ss << tips << ": " << diff << "ms";
	string str1,str2;
	ss >> str1;
	ss >> str2;
	return str1 + " " + str2;
}
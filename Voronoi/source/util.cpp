#include "util.h"
#include <string>

#ifdef WIN32
#include <Windows.h>
string path="./data/";
double gettime(){
	static LARGE_INTEGER frequency,counter;
	if(frequency.QuadPart==0)
		QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&counter);
	return counter.QuadPart*1e9/frequency.QuadPart;
}
#else
string path="../mat/data/";
double gettime(){
    timespec t;
    clock_gettime(CLOCK_MONOTONIC,&t);
    return t.tv_sec*1e9+t.tv_nsec;
}
#endif

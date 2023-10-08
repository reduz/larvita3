#ifdef WINDOWS_ENABLED

#include "time_windows.h"

#include <windows.h>

void TimeWindows::get_date(int& r_year,Month& r_month,int& r_day,Weekday& r_weekday,bool& r_dst) const {

};

void TimeWindows::get_time(int& r_hour, int& r_min,int& r_sec) const {

};

void TimeWindows::delay_usec(Uint32 p_usec) const {

	if (p_usec < 1000)
		Sleep(1);
	else
		Sleep(p_usec / 1000);
};

Uint64 TimeWindows::get_ticks_usec() const {

	Uint64 ticks;
	Uint64 time;
	// This is the number of clock ticks since start
	if( !QueryPerformanceCounter((LARGE_INTEGER *)&ticks) )
		ticks = (UINT64)timeGetTime();
	// Divide by frequency to get the time in seconds
	time = ticks * 1000000L / ticks_per_second;
	// Subtract the time at game start to get
	// the time since the game started
	time -= ticks_start;
	return time;
};

TimeWindows::TimeWindows() {

	// We need to know how often the clock is updated
	if( !QueryPerformanceFrequency((LARGE_INTEGER *)&ticks_per_second) )
		ticks_per_second = 1000;
	// If timeAtGameStart is 0 then we get the time since
	// the start of the computer when we call GetGameTime()
	ticks_start = 0;
	ticks_start = get_ticks_usec();
};

TimeWindows::~TimeWindows() {

};

#endif

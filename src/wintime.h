// SPDX-License-Identifier: MIT
// clN22 v0.98-230520 (c)2020-2019 ~EM eetu@kkona.xyz

#include <Windows.h>
#include <time.h>
struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};
static int gettimeofday(struct timeval* tv, struct timezone* tz)
{
	if (tv) {
		FILETIME filetime;
		ULARGE_INTEGER x;
		ULONGLONG usec;
		static const ULONGLONG epoch_offset_us = 11644473600000000ULL;
#if _WIN32_WINNT >= _WIN32_WINNT_WIN8
		GetSystemTimePreciseAsFileTime(&filetime);
#else
		GetSystemTimeAsFileTime(&filetime);
#endif
		x.LowPart = filetime.dwLowDateTime;
		x.HighPart = filetime.dwHighDateTime;
		usec = x.QuadPart / 10 - epoch_offset_us;
		tv->tv_sec = (time_t)(usec / 1000000ULL);
		tv->tv_usec = (long)(usec % 1000000ULL);
	}
	if (tz) {
		TIME_ZONE_INFORMATION timezone;
		GetTimeZoneInformation(&timezone);
		tz->tz_minuteswest = timezone.Bias;
		tz->tz_dsttime = 0;
	}
	return 0;
}
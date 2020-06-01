// SPDX-License-Identifier: MIT
// clN22-worker / common.h
// clN22 v0.93-310520 (c)2020-2019 ~EM eetu@kkona.xyz

#pragma once
#define CL_TARGET_OPENCL_VERSION 120
#if CL_TARGET_OPENCL_VERSION == 120
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifdef __cplusplus
extern "C" {
#endif
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "assert.h"
	void zoldhash(char*, char*, int);
	void initialization(char*, int);
	static size_t clValSize, source_size, binary_size, maxWorkgroupsize, perferredMultiple;
	static cl_uint clDevCount, ret_clplatforminfo, ret_num_platforms, ret_num_devices;
	char* devInfo;
	static cl_platform_id platform_id = NULL;
	static cl_device_id* deviceId;
	static cl_context context;
	static cl_int ret, binary_status;
	static cl_program program;
	static cl_kernel kernel22;
	static cl_command_queue command_queue, command_queueL;
	static cl_mem dBufIn, dBufInL, pinBufIn, pinBufInL, dValidKey, pinValidKey, pinSeedToCl, bufSeedToCl;
	bool kernelLoad(char[18], int);
	void clInitObj(char*, int);
	bool testRun;
	unsigned int testRound;
	char logHelper[90];
	char logEntry[420];
#ifdef _WIN32
#include <Windows.h>
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
			srand(tv->tv_sec = (long)(time_t)(usec / 1000000ULL));
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
#else
#include <sys/time.h>
#endif
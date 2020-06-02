// SPDX-License-Identifier: MIT
// clN22-worker / main.h
// v0.931-020620 (c)2019-2020 ~EM eetu@kkona.xyz

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
#include <stdbool.h>
#include "common.h"
	bool logToFile;
	static char testExpect[7];
	static char testString[70];
	char logFile[32];
#define Strength 8
#define lenDateTimeFormat 19 // "dd.mm.yy HH:MM:SS\0"
	char inTimeAsStr[lenDateTimeFormat];
	char introTxt[] = "clN22-worker v%s (c)2019-2020 EM~ eetu@kkona.xyz";
	char useHelp[] = "\nUsage: 'clN22 <input> <output> (setdev #) (log <logfile>)\n\t(Optional) Use 'setdev <Device ID>' to select device. Use 'log \"file.log\"' to save log.\n\t 'cln22 testrun' - run a sanity check / benchmark.\n";
	int idval = 0;
	char VerInfo[] = "0.931-020620";
	char output[128];
	char reStr[128];
	char resultOut[34];
	char stringIn[90];
	void currLocalTime(char*);
#ifdef __cplusplus
}
#endif

// SPDX-License-Identifier: MIT
// clN22-worker / main.h
// v0.983-240520 (c)2019-2020 ~EM eetu@kkona.xyz

#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include "wintime.h"
bool logToFile;
static char testExpect[7];
static char benchString[65];
static char benchExpect[7];
char logFile[32];
#define Strength 8
#define lenDateTimeFormat 19 // "dd.mm.yy HH:MM:SS\0"
char inTimeAsStr[lenDateTimeFormat];
char introTxt[] = "clN22-worker v%s (c)2019-2020 EM~ eetu@kkona.xyz\n";
char useHelp[] = "\nUsage: 'clN22 <input> <output> (setdev #) (log <logfile>)\n\t(Optional) Use 'setdev <Device ID>' to select device. Use 'log \"file.log\"' to save log.";
int idval = 0;
char VerInfo[] = "0.981-230520";
char output[128];
char reStr[128];
char resultOut[34];
char stringIn[90];
void currLocalTime(char*);
#ifdef __cplusplus
}
#endif
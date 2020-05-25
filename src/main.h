#pragma once
// SPDX-License-Identifier: MIT
// clN22-worker / main.h
// v0.983-240520 (c)2019-2020 ~EM eetu@kkona.xyz

#ifdef __cplusplus
extern "C" {
#endif
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#include "worker.h"
#include <stdlib.h>
#define Strength 8
#define lenDateTimeFormat 19 // "dd.mm.yy HH:MM:SS\0"
char inTimeAsStr[lenDateTimeFormat];
const char introTxt[] = "clN22-worker v%s (c)2019-2020 EM~ eetu@kkona.xyz\n";
const char useHelp[] = "\nUsage: 'clN22 <input> <output> (setdev #) (log <logfile>)\n\t(Optional) Use 'setdev <Device ID>' to select device. Use 'log \"file.log\"' to save log.";
int idval = 0;
char VerInfo[] = "0.981-230520";
char output[128];
char reStr[128];
char resultOut[34];
char stringIn[90];
bool logToFile;
void currLocalTime(char*);
#ifdef __cplusplus
}
#endif
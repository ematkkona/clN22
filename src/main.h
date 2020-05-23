// SPDX-License-Identifier: MIT
// clN22 v0.98-230520 (c)2020-2019 ~EM eetu@kkona.xyz

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#ifdef __cplusplus
extern "C" {
#endif
#ifdef _MSC_VER
#include "wintime.h"
#endif
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>
#define Strength 8
#define lenDateTimeFormat 19 // "dd.mm.yy HH:MM:SS"

bool logToFile;
char logHelper[90];
char logFile[32];
char logEntry[256];
void currLocalTime(char*);
void zoldhash(char*, char*, int, unsigned int);
unsigned int initialization(char*, char*, int);
#ifdef __cplusplus
}
#endif
// SPDX-License-Identifier: MIT
// clN22-worker / worker.h
// v0.983-240520 (c)2019-2020 ~EM eetu@kkona.xyz

#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifdef __cplusplus
extern "C" {
#endif
#define MAX_SOURCE_SIZE	0x10000000
#define MAX_BINARY_SIZE 0x10000000
#include "wintime.h"
unsigned int clSelectDevice(int);
static unsigned char* source_str, *binary_str;
char* devInfo;
unsigned short* dBuf_i;
static char* seedToCl, *hValidKey;
double gHashrate, gRoundtime;
static unsigned int* string_len;
char const* clGetErrorString(cl_int const err);
static volatile sig_atomic_t keep_running = 1;
void ctrlc(int);
unsigned long djb2(char*);
void kernelBuild(char*, int);
void clInitObj(char*, int);
unsigned int calcSizeMltPr(unsigned int);
void resPrintout(char[2], char[8], int, int);
void ReleaseAndFlush();
#ifdef __cplusplus
}
#endif
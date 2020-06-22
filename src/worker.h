// SPDX-License-Identifier: MIT
// clN22-worker / worker.h
// v0.933-220620 (c)2019-2020 ~EM eetu@kkona.xyz

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
#define MAX_SOURCE_SIZE	0x10000000
#define MAX_BINARY_SIZE 0x10000000
#include "common.h"
void clSelectDevice(int);
static unsigned char* source_str, *binary_str;
char* devInfo;
short* dBuf_i;
char* seedToCl, *hValidKey;
double gHashrate, gRoundtime;
char const* clGetErrorString(cl_int const err);
volatile sig_atomic_t keep_running = 1;
struct timeval start, end;
double time_taken;
unsigned int rCount, nSpace, hId0, hId1, lCounter, hId0m; 
int WSMcount;
void ctrlc(int);
unsigned long djb2(char*);
void kernelBuild(char*, int);
int calcSizeMltPr(unsigned int);
void resPrintout(char[2], char[8], int, int, int);
void ReleaseAndFlush();
#ifdef __cplusplus
}
#endif
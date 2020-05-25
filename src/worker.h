#pragma once
// SPDX-License-Identifier: MIT
// clN22-worker / worker.h
// v0.983-240520 (c)2019-2020 ~EM eetu@kkona.xyz

#ifdef __cplusplus
extern "C" {
#endif
#define MAX_SOURCE_SIZE	0x10000000
#define MAX_BINARY_SIZE 0x10000000
#if CL_TARGET_OPENCL_VERSION == 120
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#elif CL_TARGET_OPENCL_VERSION == 110
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#elif CL_TARGET_OPENCL_VERSION == 100
#define CL_USE_DEPRECATED_OPENCL_1_0_APIS
#endif
#define cl(...)    cl_assert((cl##__VA_ARGS__), __FILE__, __LINE__, true);
#define cl_ok(err) cl_assert(err, __FILE__, __LINE__, true);
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "wintime.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <math.h>
#include <stdlib.h>
void zoldhash(char*, char*, int, unsigned int);
unsigned int initialization(char*, int);
unsigned int clSelectDevice(int);
char logHelper[90];
char logFile[32];
char logEntry[256];
static unsigned char* source_str, *binary_str;
static size_t clValSize, source_size, binary_size;
static cl_uint clDevCount, maxComputeUnits, ret_clplatforminfo, ret_num_platforms, ret_num_devices;
char* devInfo;
static cl_platform_id platform_id = NULL;
static cl_device_id* deviceId;
static cl_context context;
static cl_int ret, binary_status;
static cl_program program;
static cl_kernel kernel22;
static cl_command_queue command_queue, command_queueL;
static cl_mem dBufIn, dBufInL, pinBufIn, pinBufInL, dValidKey, pinValidKey, pinSeedToCl, bufSeedToCl;
unsigned short* dBuf_i;
static char* seedToCl, *hValidKey;
double gHashrate, gRoundtime;
static unsigned int* string_len;
char const* clGetErrorString(cl_int const err);
static volatile sig_atomic_t keep_running = 1;
cl_int cl_assert(cl_int const code, char const* const file, int const line, bool const abort);
bool kernelLoad(char[18], int);
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
// SPDX-License-Identifier: GPL-3.0-only
// clNuulo.h @clN22-worker
// v0.90-061219 (c)2019 ~EM eetu@kkona.xyz

#pragma once
#define CL_TARGET_OPENCL_VERSION	210
#define SHA256_FORMAT_LABEL			"sha256-opencl"
#define SHA256_FORMAT_NAME			"SHA-256"
#define SHA256_ALGORITHM_NAME		"Zold PoW"
#define MAX_SOURCE_SIZE				0x10000000
#define Strength					8
#define SHA256_BINARY_SIZE			32
#define SHA256_RESULT_SIZE			8
#define cl(...)    cl_assert((cl##__VA_ARGS__), __FILE__, __LINE__, true);
#define cl_ok(err) cl_assert(err, __FILE__, __LINE__, true);
#define nl		
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef uint32_t
#define uint32_t unsigned int
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <locale.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

static cl_platform_id platform_id = NULL;
static cl_device_id device_id = NULL;
static cl_uint ret_clplatforminfo, ret_num_platforms, ret_num_devices;
static cl_context context;
static cl_int ret;
static unsigned char* source_str;
static size_t source_size;
static cl_program program;
static cl_kernel kernel22;
static cl_command_queue command_queue;
static cl_mem dBufIn, dValidKey, pinSeedToCl, bufSeedToCl;
static unsigned int dBuf_i[3];
static char* seedToCl;
static char hValidKey[6];
static int lCounter = 0;
static size_t string_len;
char const* clGetErrorString(cl_int const err);
cl_int cl_assert(cl_int const code, char const* const file, int const line, bool const abort);

void kernelLoad(char*);
void clInitObj(char*);
void initKernel22(char*, char*);
void zoldhash(char*, char*);
void ReleaseAndFlush();

#ifdef __cplusplus
}
#endif

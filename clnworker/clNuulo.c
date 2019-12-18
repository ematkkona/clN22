// SPDX-License-Identifier: GPL-3.0-only
// clNuulo.c @clN22-worker
// v0.93-131219 (c)2019 ~EM eetu@kkona.xyz

#include "clNuulo.h"

void ctrlc(int sig) {
	signal(sig, SIG_IGN);
	printf("\n<CTRL-C> Exiting");
	ReleaseAndFlush();
	exit(1);
}

void zoldhash(char* prefixIn, char* resultOut) {
	int isSolved = 0, rCount = 0, nSpace = 0;
	unsigned int hId0 = 0;
	unsigned int hId1 = 0;
	time_t ltime;
	size_t local_work_size = 0;
	size_t global_work_size[3] = { 64, 64, 64 };
	cl_event clEvent;
	string_len = (unsigned int)strlen(prefixIn);
	dBuf_i[0] = (unsigned int)string_len;
	memcpy(seedToCl, prefixIn, string_len);
	ltime = time(NULL);
	printf("[k22]Starting worker\n");
	lCounter = 0;
	cl(EnqueueWriteBuffer(command_queue, bufSeedToCl, CL_FALSE, 0, string_len, seedToCl, 0, NULL, &clEvent));
	cl(WaitForEvents(1, &clEvent));
	cl(ReleaseEvent(clEvent));
	cl(EnqueueUnmapMemObject(command_queue, pinSeedToCl, seedToCl, 0, NULL, &clEvent));
	cl(WaitForEvents(1, &clEvent));
	cl(ReleaseEvent(clEvent));
	seedToCl = NULL;
	free(seedToCl);
	clock_t t;

	while (!isSolved) {
		signal(SIGINT, ctrlc);
		if (!lCounter)
			t = clock();
		dBuf_i[1] = hId0; dBuf_i[2] = hId1;
		cl(EnqueueWriteBuffer(command_queue, dBufIn, CL_FALSE, 0, sizeof(unsigned int) * 3, dBuf_i, 0, NULL, NULL));
		cl(EnqueueNDRangeKernel(command_queue, kernel22, 3, NULL, global_work_size, NULL, 0, NULL, NULL));
		cl(EnqueueReadBuffer(command_queue, dValidKey, CL_TRUE, 0, sizeof(cl_char) * 6, hValidKey, 0, NULL, NULL));
		if (strcmp("######", hValidKey)) {
			sprintf(resultOut, "%s", hValidKey);
			printf("\n[k22]Solultion! Round:%d\n[k22]Result:'%s'", rCount, resultOut);
			ReleaseAndFlush();
			break; 
		}

		if (hId0 < (unsigned int)61)
			hId0++;
		else if (hId1 < (unsigned int)61) {
			hId0 = 0;
			hId1++; }
		else {
			sprintf(resultOut, "failure");
			ReleaseAndFlush();
			break; }
		rCount++;
		lCounter++;
		if (lCounter == 62) {
			nSpace++;
			t = clock() - t;
			double time_taken = ((double)t) / CLOCKS_PER_SEC;
			ltime = time(NULL);
			double hashcntTmp = 62 * 62 * 62 * 62;
			double hashcnt = (((hashcntTmp * 62) / time_taken) / 1000000); // gidx * gidy * gidz * hostGen0 * hostGen1 * lCounter >>> count MH/s
			printf("\n[%d/62]Batch done in %lfs. Speed:%lfMH/s ", nSpace, time_taken, hashcnt);
			lCounter = 0;
		}
		if (nSpace == 62) {
			sprintf(resultOut, "failure");
			ReleaseAndFlush();
			break;
		}
	}
}

void kernelLoad(char* kernelV) {
	FILE* fp = fopen(kernelV, "r");
	if (!fp) {
		fprintf(stderr, "\nFailed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	if (source_str)
		source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	else {
		printf("\nError loading kernel.");
		exit(1);
	}

	fclose(fp);
}

void initKernel22(char* kernelV, char* strIn) {
	kernelLoad(kernelV);
	clInitObj(strIn);
}

void clInitObj(char* strIn) {
	size_t ValidKLen = 6;
	size_t dBufLen = 3;
	size_t strLen = strlen(strIn);
	cl(GetPlatformIDs(1, &platform_id, &ret_num_platforms));
	cl(GetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices));
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	cl_ok(ret);
	program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);
	cl_ok(ret);
	cl(BuildProgram(program, 1, &device_id, "-cl-mad-enable -cl-fast-relaxed-math", NULL, NULL));
	kernel22 = clCreateKernel(program, "kernel22", &ret);
	cl_ok(ret);
	free(source_str);
#if CL_TARGET_OPENCL_VERSION < 200
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
#else
	command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
#endif
	cl_ok(ret);
	pinBufIn = clCreateBuffer(context, CL_MEM_READ_ONLY, (sizeof(unsigned int) * 3), NULL, &ret);
	cl_ok(ret);
	*dBuf_i = (unsigned int)clEnqueueMapBuffer(command_queue, pinBufIn, CL_FALSE, CL_MAP_READ, 0, (sizeof(unsigned int) * 3), 0, NULL, NULL, &ret);
	cl_ok(ret);
	memset(dBuf_i, 0, dBufLen);
	dBufIn = clCreateBuffer(context, CL_MEM_READ_ONLY, (sizeof(unsigned int) * 3), NULL, &ret);
	cl_ok(ret);
	pinSeedToCl = clCreateBuffer(context, CL_MEM_READ_ONLY, strLen, NULL, &ret);
	cl_ok(ret);
	seedToCl = (char*)clEnqueueMapBuffer(command_queue, pinSeedToCl, CL_FALSE, CL_MAP_READ, 0, strLen, 0, NULL, NULL, &ret);
	cl_ok(ret);
	memset(seedToCl, 0, strLen);
	bufSeedToCl = clCreateBuffer(context, CL_MEM_READ_ONLY, strLen, NULL, &ret);
	cl_ok(ret);
	pinValidKey = clCreateBuffer(context, CL_MEM_WRITE_ONLY, (sizeof(cl_char) * 6), NULL, &ret);
	cl_ok(ret);
	hValidKey = (char*)clEnqueueMapBuffer(command_queue, pinValidKey, CL_FALSE, CL_MAP_WRITE, 0, (sizeof(cl_char) * 6), 0, NULL, NULL, &ret);
	cl_ok(ret);
	memset(hValidKey, 0, ValidKLen);
	dValidKey = clCreateBuffer(context, CL_MEM_WRITE_ONLY, (sizeof(cl_char) * 6), NULL, &ret);
	cl_ok(ret);
	cl(SetKernelArg(kernel22, 0, sizeof(dBufIn), (void*)&dBufIn));
	cl(SetKernelArg(kernel22, 1, sizeof(bufSeedToCl), (void*)&bufSeedToCl));
	cl(SetKernelArg(kernel22, 2, sizeof(dValidKey), (void*)&dValidKey));
}

void ReleaseAndFlush() {
	cl_event clEvent;
	cl(Finish(command_queue));
	cl(EnqueueUnmapMemObject(command_queue, pinBufIn, dBuf_i, 0, NULL, &clEvent));
	cl(WaitForEvents(1, &clEvent));
	cl(ReleaseEvent(clEvent));
	cl(EnqueueUnmapMemObject(command_queue, pinValidKey, hValidKey, 0, NULL, &clEvent));
	cl(WaitForEvents(1, &clEvent));
	cl(ReleaseEvent(clEvent));
	cl(ReleaseMemObject(pinSeedToCl));
	cl(ReleaseMemObject(bufSeedToCl));
	cl(ReleaseMemObject(pinBufIn));
	cl(ReleaseMemObject(pinValidKey));
	cl(ReleaseMemObject(dBufIn));
	cl(ReleaseMemObject(dValidKey));
	cl(ReleaseCommandQueue(command_queue));
	cl(ReleaseContext(context));
	cl(ReleaseKernel(kernel22));
	cl(ReleaseProgram(program));
}
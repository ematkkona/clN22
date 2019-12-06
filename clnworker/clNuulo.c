// SPDX-License-Identifier: GPL-3.0-only
// clNuulo.c @clN22-worker
// v0.90-061219 (c)2019 ~EM eetu@kkona.xyz

#include "clNuulo.h"

void ctrlc(int sig) {
	signal(sig, SIG_IGN); 
	printf("\n<CTRL-C> Exiting");
		ReleaseAndFlush();
		exit(1);
}

void zoldhash(char* prefixIn, char* resultOut) {
	int isSolved = 0, rCount = 0, nSpace = 0, hId0 = 0, hId1 = 0;
	time_t ltime;
	size_t local_work_size = 0;
	size_t global_work_size[3] = { 64, 64, 64 };
	cl_event clEvent;
	string_len = strlen(prefixIn);
	dBuf_i[0] = string_len; dBuf_i[1] = 0; dBuf_i[2] = 0;
	memcpy(seedToCl, prefixIn, string_len);
	ltime = time(NULL);
	printf("[k22]Starting worker\n",string_len);
	lCounter = 0;
	cl(EnqueueWriteBuffer(command_queue, bufSeedToCl, CL_TRUE, 0, string_len, seedToCl, 0, NULL, &clEvent));
	cl(WaitForEvents(1, &clEvent));
	cl(ReleaseEvent(clEvent));
	seedToCl = NULL;
	free(seedToCl);
	clock_t t;
	while(!isSolved) {
		signal(SIGINT, ctrlc);
		dBuf_i[1] = hId0;
		dBuf_i[2] = hId1;
		cl(EnqueueWriteBuffer(command_queue, dBufIn, CL_TRUE, 0, sizeof(unsigned int) * 3, dBuf_i, 0, NULL, NULL));
		if (!lCounter) {
			t = clock(); }
		cl(EnqueueNDRangeKernel(command_queue, kernel22, 3, NULL, global_work_size, NULL, 0, NULL, NULL));  //launch kernel22
		cl(EnqueueReadBuffer(command_queue, dValidKey, CL_TRUE, 0, sizeof(cl_char) * 6, hValidKey, 0, NULL, NULL)); //see if solved, ###### = unsolved
		if (+hValidKey[0] != 35) {
			sprintf(resultOut, "%c%c%c%c%c%c\0", hValidKey[0], hValidKey[1], hValidKey[2], hValidKey[3], hValidKey[4], hValidKey[5]);
			printf("\n[k22]Solution found! Round:%d\n[k22]Suffix:'%s'", rCount, resultOut);
			ReleaseAndFlush();
			break;
		}
		if (hId0 < 61) {
			hId0++;
		}
		else if (hId1 < 61) {
			hId0 = 0;
			hId1++;
		}
		else {
			sprintf(resultOut, "NONE");
			ReleaseAndFlush();
			break;
		}
		rCount++;
		lCounter++;
		//spit out hashspeed every 62nd round
		if (lCounter == 62) {
			nSpace++;
			t = clock() - t;
			double time_taken = ((double)t) / CLOCKS_PER_SEC;
			ltime = time(NULL);
			double hashcnt = (((62*62*62*62*62*62) / time_taken) / 1000000); // gidx * gidy * gidz * hostGen0 * hostGen1 * lCounter >>> count MH/s
			printf("\n[%d/62]Batch done in %lfs. Speed:%lfMH/s ", nSpace, time_taken, hashcnt);
			lCounter = 0;
		}
		if (nSpace == 62) {
			sprintf(resultOut, "NONE");
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
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
}

void initKernel22(char* kernelV, char* strIn) {
	kernelLoad(kernelV);
	clInitObj(strIn);
}

void clInitObj(char* strIn) {
	size_t strLen = strlen(strIn);
	cl(GetPlatformIDs(1, &platform_id, &ret_num_platforms));
	cl(GetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices));
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	cl_ok(ret);
	program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);
	cl_ok(ret);
	cl(BuildProgram(program, 1, &device_id, NULL, NULL, NULL));
	kernel22 = clCreateKernel(program, "kernel22", &ret);
	cl_ok(ret);
	free(source_str);
	#if CL_TARGET_OPENCL_VERSION < 200
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	#else
	command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
	#endif
	cl_ok(ret);
	pinSeedToCl = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, strLen, NULL, &ret);
	cl_ok(ret);
	seedToCl = (char*)clEnqueueMapBuffer(command_queue, pinSeedToCl, CL_TRUE, CL_MAP_READ, 0, strLen, 0, NULL, NULL, &ret);
	cl_ok(ret);
	memset(seedToCl, 0, strLen);
	bufSeedToCl = clCreateBuffer(context, CL_MEM_READ_ONLY, strLen, NULL, &ret);
	cl_ok(ret);
	dBufIn = clCreateBuffer(context, CL_MEM_READ_ONLY, (sizeof(unsigned int) * 3), NULL, &ret);
	cl_ok(ret);
	dValidKey = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, (sizeof(cl_char) * 6), NULL, &ret);
	cl_ok(ret);
	cl(SetKernelArg(kernel22, 0, sizeof(dBufIn), (void*)&dBufIn));
	cl(SetKernelArg(kernel22, 1, sizeof(bufSeedToCl), (void*)&bufSeedToCl));
	cl(SetKernelArg(kernel22, 2, sizeof(dValidKey), (void*)&dValidKey)); }

void ReleaseAndFlush() {
	cl(Finish(command_queue));
	cl(ReleaseCommandQueue(command_queue));
	cl(ReleaseMemObject(pinSeedToCl));
	cl(ReleaseMemObject(bufSeedToCl));
	cl(ReleaseMemObject(dBufIn));
	cl(ReleaseMemObject(dValidKey));
	cl(ReleaseContext(context));
	cl(ReleaseKernel(kernel22));
	cl(ReleaseProgram(program));
}
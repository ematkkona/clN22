// SPDX-License-Identifier: GPL-3.0-only
// clNuulo.c @clN22-worker
// v0.98-150520 (c)2019-2020 ~EM eetu@kkona.xyz

#include "clNuulo.h"

void clSelectDevice(int idval) {
	size_t clValSize;
	cl_uint clDevCount, maxComputeUnits;
	char* devInfo;
	cl(GetPlatformIDs(1, &platform_id, &ret_num_platforms));
	cl(GetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 0, NULL, &clDevCount));
	if (clDevCount <= (unsigned int)idval) {
		printf("\nError: Device #%d not found. Number of device(s): %d", idval + 1, clDevCount);
		exit(1); }
	deviceId = (cl_device_id*)malloc(sizeof(cl_device_id) * clDevCount);
	cl(GetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, clDevCount, deviceId, NULL));
	cl(GetDeviceInfo(deviceId[idval], CL_DEVICE_NAME, 0, NULL, &clValSize));
	devInfo = (char*)malloc(clValSize);
	cl(GetDeviceInfo(deviceId[idval], CL_DEVICE_NAME, clValSize, devInfo, NULL));
	printf("\n[init]#%d/%d: %s ", idval + 1, clDevCount, devInfo);
	sprintf(logHelper, "dev:%s; ", devInfo);
	strcat(logEntry, logHelper);
	free(devInfo);
	cl(GetDeviceInfo(deviceId[idval], CL_DEVICE_VERSION, 0, NULL, &clValSize));
	devInfo = (char*)malloc(clValSize);
	cl(GetDeviceInfo(deviceId[idval], CL_DEVICE_VERSION, clValSize, devInfo, NULL));
	printf("%s ", devInfo);
	free(devInfo);
	cl(GetDeviceInfo(deviceId[idval], CL_DRIVER_VERSION, 0, NULL, &clValSize));
	devInfo = (char*)malloc(clValSize);
	cl(GetDeviceInfo(deviceId[idval], CL_DRIVER_VERSION, clValSize, devInfo, NULL));
	printf("%s ", devInfo);
	free(devInfo);
	cl(GetDeviceInfo(deviceId[idval], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &clValSize));
	devInfo = (char*)malloc(clValSize);
	cl(GetDeviceInfo(deviceId[idval], CL_DEVICE_OPENCL_C_VERSION, clValSize, devInfo, NULL));
	printf("%s ", devInfo);
	free(devInfo);
	cl(GetDeviceInfo(deviceId[idval], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, NULL));
	printf("CUs: %d\n", maxComputeUnits);
}

void ctrlc(int sig) {
	signal(sig, SIG_IGN);
	printf("\nCTRL-C detected. Aborting work and exiting ...\n");
	keep_running = 0;
}

void zoldhash(char* prefixIn, char* resultOut) {
	int isSolved = 0, rCount = 0, nSpace = 0; unsigned int hId0 = 0; unsigned int hId1 = 0;
	clock_t t;
	double time_taken;
	size_t local_work_size = 0;
	size_t global_work_size[3] = { 64, 64, 64 };
	cl_event clEvent;
	string_len = (unsigned int*)strlen(prefixIn);
	dBuf_i[0] = (unsigned int)strlen(prefixIn);
	memcpy(seedToCl, prefixIn, (size_t)string_len);
	gHashrate = 0;
	gRoundtime = 0;
	lCounter = 0;
	printf("[clN22]Starting worker - Press CTRL-C to exit\n   0  abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789  3844\n   >  ^");
	cl(EnqueueWriteBuffer(command_queue, bufSeedToCl, CL_FALSE, 0, (size_t)string_len, seedToCl, 0, NULL, &clEvent));
	cl(WaitForEvents(1, &clEvent));
	cl(ReleaseEvent(clEvent));
	cl(EnqueueUnmapMemObject(command_queue, pinSeedToCl, seedToCl, 0, NULL, &clEvent));
	cl(WaitForEvents(1, &clEvent));
	cl(ReleaseEvent(clEvent));
	seedToCl = NULL;
	free(seedToCl);
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
			resPrintout("*", resultOut, nSpace, rCount);
			ReleaseAndFlush();
			break; }
		else if (!keep_running) {
			sprintf(resultOut, "userexit");
			resPrintout("!", "userexit", nSpace, rCount);
			ReleaseAndFlush();
			break; }
		if (hId0 < 61)
			hId0++;
		else if (hId1 < 61) {
			hId0 = 0;
			hId1++; }
		else {
			sprintf(resultOut, "failure");
			resPrintout("?", "failure", nSpace, rCount);
			ReleaseAndFlush();
			break; }
		rCount++;
		lCounter++;
		if (lCounter == 62) {
			nSpace++;
			t = clock() - t;
			time_taken = ((double)t) / CLOCKS_PER_SEC;
			double hashcntTmp = 62 * 62 * 62 * 62;
			double hashcnt = (((hashcntTmp * 62) / time_taken) / 1000000);
			printf("\b'^");
			gHashrate += hashcnt;
			gRoundtime += time_taken;
			lCounter = 0; }
		if (nSpace > 62) {
			sprintf(resultOut, "failure");
			resPrintout(">", "failure", nSpace, rCount);
			ReleaseAndFlush();
			break; }
	}
}

void resPrintout(char bEndChar[2], char resOut[8], int nSpace, int rCount) {
	printf("\b%s", bEndChar);
	if (gHashrate > 0 && gRoundtime > 0) {
		gHashrate = gHashrate / nSpace;
		gRoundtime = gRoundtime / nSpace; }
	sprintf(logHelper, "result:'%s'; flag:%s; round:%d; nspace:%d; speed:%.2lf; roundtime:%.2lf; ", resOut, bEndChar, rCount, nSpace, gHashrate, gRoundtime);
	strcat(logEntry, logHelper);
	for (;nSpace < 61;nSpace++)
		printf(".");
	printf("  %d\n[finish]Speed:%.2lfMH/s Avg.time per round:%.3lfs ",rCount, gHashrate, gRoundtime/62);
}

void kernelLoad(char* kernelV) {
	FILE* fp = fopen(kernelV, "r");
	if (!fp) {
		fprintf(stderr, "\nError loading kernel: Unable to open file\n");
		exit(1); }
	source_str = (unsigned char*)malloc(MAX_SOURCE_SIZE);
	if (source_str)
		source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	else {
		fprintf(stderr, "\nError loading kernel: File empty\n");
		exit(1); }
	fclose(fp);
}


void initialization(char* kernelV, char* strIn, int idval) {
	kernelLoad(kernelV);
	clSelectDevice(idval);
	clInitObj(strIn, idval);
}

void clInitObj(char* strIn, int idval) {
	size_t ValidKLen = 6;
	size_t dBufLen = 3;
	size_t strLen = strlen(strIn);
	context = clCreateContext(NULL, 1, &deviceId[idval], NULL, NULL, &ret);
	cl_ok(ret);
	program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);
	cl_ok(ret);
	cl(BuildProgram(program, 1, &deviceId[idval], "-cl-unsafe-math-optimizations -cl-mad-enable", NULL, NULL));
	kernel22 = clCreateKernel(program, "kernel22", &ret);
	cl_ok(ret);
	free(source_str);
#if CL_TARGET_OPENCL_VERSION < 200
	command_queue = clCreateCommandQueue(context, deviceId[idval], 0, &ret);
#else
	command_queue = clCreateCommandQueueWithProperties(context, deviceId[idval], 0, &ret);
#endif
	cl_ok(ret);
	pinBufIn = clCreateBuffer(context, CL_MEM_READ_ONLY, (sizeof(unsigned int) * 3), NULL, &ret);
	cl_ok(ret);
	dBuf_i = (unsigned int*)clEnqueueMapBuffer(command_queue, pinBufIn, CL_FALSE, CL_MAP_READ, 0, (sizeof(unsigned int) * 3), 0, NULL, NULL, &ret);
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
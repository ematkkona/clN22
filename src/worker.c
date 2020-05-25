// SPDX-License-Identifier: MIT
// clN22-worker / worker.c
// v0.983-240520 (c)2019-2020 ~EM eetu@kkona.xyz

#include "worker.h"
size_t ValidKLen = 6;
size_t dBufLen = 5;

void zoldhash(char* prefixIn, char* resultOut, int idval, unsigned int WGs) {
	unsigned int isSolved = 0, rCount = 0, nSpace = 0, hId0 = 0, hId1 = 0, lCounter = 0;
	unsigned int limtdWSizeMltplr = (WGs < 62) ? calcSizeMltPr(WGs) : 0;
	printf("\ndebug SizeMultiplier:%d WG:%d", limtdWSizeMltplr, WGs);
	struct timeval start, end;
	double time_taken;
	size_t global_work_size[3] = { WGs, WGs, WGs };
	cl_event clEvent;
	string_len = (unsigned int*)strlen(prefixIn);
	dBuf_i[0] = (unsigned short)strlen(prefixIn);
	dBuf_i[3] = 0;
	memcpy(seedToCl, prefixIn, (size_t)string_len);
	gHashrate = 0;
	gRoundtime = 0;
	printf("\n[clN22]Starting worker - Press CTRL-C to exit\n   0  abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789  3844\n   >  ^");
	cl(EnqueueWriteBuffer(command_queue, bufSeedToCl, CL_FALSE, 0, (size_t)string_len, seedToCl, 0, NULL, &clEvent));
	cl(WaitForEvents(1, &clEvent));
	cl(ReleaseEvent(clEvent));
	cl(EnqueueUnmapMemObject(command_queue, pinSeedToCl, seedToCl, 0, NULL, &clEvent));
	cl(WaitForEvents(1, &clEvent));
	cl(ReleaseEvent(clEvent));
	seedToCl = NULL;
	free(seedToCl);
	unsigned int WSMcount = 0;
	while (!isSolved) {
		signal(SIGINT, ctrlc);
		if (!lCounter)
			gettimeofday(&start, NULL);
		dBuf_i[1] = (unsigned short)hId0; dBuf_i[2] = (unsigned short)hId1; dBuf_i[3] = (unsigned short)WSMcount; dBuf_i[4] = (unsigned short)WGs;
		cl(EnqueueWriteBuffer(command_queue, dBufIn, CL_FALSE, 0, sizeof(cl_ushort) * dBufLen, dBuf_i, 0, NULL, NULL));
		cl(EnqueueNDRangeKernel(command_queue, kernel22, 3, NULL, global_work_size, NULL, 0, NULL, NULL));
		cl(EnqueueReadBuffer(command_queue, dValidKey, CL_TRUE, 0, sizeof(cl_char) * ValidKLen, hValidKey, 0, NULL, NULL));
		if (strcmp("######", hValidKey)) {
			sprintf(resultOut, "%s", hValidKey);
			resPrintout("*", resultOut, nSpace, rCount);
			ReleaseAndFlush();
			break;
		}
		else if (!keep_running) {
			sprintf(resultOut, "failure");
			resPrintout("!", "userexit", nSpace, rCount);
			ReleaseAndFlush();
			break;
		}
		if (WSMcount >= limtdWSizeMltplr || limtdWSizeMltplr == 0) {
			WSMcount = 0;
			if (hId0 < 61)
				hId0++;
			else if (hId1 < 61) {
				hId0 = 0;
				hId1++;
			}
		}
		else if (limtdWSizeMltplr != 0)
			WSMcount++;
		else {
			sprintf(resultOut, "UnSpec");
			resPrintout("?", "unknown", nSpace, rCount);
			ReleaseAndFlush();
			break;
		}
		rCount++;
		lCounter++;
		if (lCounter == 62) {
			nSpace++;
			gettimeofday(&end, NULL);
			time_taken = ((double)end.tv_sec - (double)start.tv_sec) * 1e6;
			time_taken = (time_taken + ((double)end.tv_usec - (double)start.tv_usec)) * 1e-6;
			double hashcntTmp = 62 * 62 * 62 * 62;
			double hashcnt = (((hashcntTmp * 62) / time_taken) / 1000000);
			printf("\b'^");
			gHashrate += hashcnt;
			gRoundtime += time_taken;
			lCounter = 0;
		}
		if (nSpace > 62) {
			sprintf(resultOut, "UnSolv");
			resPrintout(">", "unsolvable", nSpace, rCount);
			ReleaseAndFlush();
			break;
		}
	}
}

void resPrintout(char bEndChar[2], char resOut[8], int nSpace, int rCount) {
	printf("\b%s", bEndChar);
	if (gHashrate > 0 && gRoundtime > 0) {
		gHashrate = gHashrate / nSpace;
		gRoundtime = gRoundtime / nSpace;
	}
	sprintf(logHelper, "result:'%s'; flag:%s; round:%d; nspace:%d; speed:%.2lf; roundtime:%.2lf; ", resOut, bEndChar, rCount, nSpace, gHashrate, gRoundtime);
	strcat(logEntry, logHelper);
	for (;nSpace < 61;nSpace++)
		printf(".");
	printf("  %d\n[finish]Speed:%.2lfMH/s Avg.time per round:%.3lfs ", rCount, gHashrate, gRoundtime / 62);
}

bool kernelLoad(char kerNHash[18], int idval) {
	FILE* fp = fopen(kerNHash, "rb");
	if (!fp) {
		printf("\n[init]No pre-compiled kernel found for selected device - building: '%s'", kerNHash);
		kernelBuild(kerNHash, idval); 
		return(1); }
	else {
		binary_str = (unsigned char*)malloc(MAX_SOURCE_SIZE);
		binary_size = fread(binary_str, 1, MAX_BINARY_SIZE, fp);
		if (!binary_str) {
			fprintf(stderr, "\nError reading pre-compiled kernel!");
			exit(1);
		}
	}
	fclose(fp);
	return(0);
}

void kernelBuild(char* binName, int idval) {
	binary_str = malloc(sizeof(unsigned char *) * MAX_BINARY_SIZE);
	FILE* fp = fopen("kernel22.cl", "r");
	if (!fp) {
		fprintf(stderr, "\nError reading kernel-file (source)!\n");
		exit(1);
	}
	source_str = (unsigned char*)malloc(MAX_SOURCE_SIZE);
	if (source_str)
		source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	else {
		fprintf(stderr, "\nError while reading kernel-file (source)!\n");
		exit(1);
	}
	fclose(fp);
	struct timeval start, end;
	gettimeofday(&start, NULL);
	context = clCreateContext(NULL, 1, &deviceId[idval], NULL, NULL, &ret);
	cl_ok(ret);
	program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);
	cl_ok(ret);
	cl(BuildProgram(program, 1, &deviceId[idval], "-cl-unsafe-math-optimizations -cl-mad-enable", NULL, NULL));
	kernel22 = clCreateKernel(program, "kernel22", &ret);
	cl_ok(ret);
	free(source_str);
	gettimeofday(&end, NULL);
	double time_taken = ((double)end.tv_sec - (double)start.tv_sec) * 1e6;
	time_taken = (time_taken + ((double)end.tv_usec - (double)start.tv_usec)) * 1e-6;
	cl(GetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &binary_size, NULL));
	cl(GetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(unsigned int**)&binary_size, &binary_str, NULL));
	if (binary_str) {
		fp = fopen(binName, "wb");
		fwrite(binary_str, 1, binary_size, fp);
		fclose(fp);
		cl(ReleaseKernel(kernel22));
		cl(ReleaseProgram(program));
		cl(ReleaseContext(context));
		printf("\n[build]Kernel compilation completed. Saved to:'%s'. Time:%.2lfs", binName, time_taken);
		free(binary_str);
	}
	else {
		printf("\n[build]Unknown error! Build completed with CL_OK but returned nothing...");
		exit(1);
	}
}

void clInitObj(char* strIn, int idval) {
	struct timeval start, buildTime, end;
	size_t strLen = strlen(strIn);
	gettimeofday(&start, NULL);
	context = clCreateContext(NULL, 1, &deviceId[idval], NULL, NULL, &ret);
	cl_ok(ret);
	program = clCreateProgramWithBinary(context, 1, &deviceId[idval], &binary_size, (const unsigned char**)&binary_str, &binary_status, &ret);
	cl_ok(ret);
	cl(BuildProgram(program, 1, &deviceId[idval], NULL, NULL, NULL));
	kernel22 = clCreateKernel(program, "kernel22", &ret);
	cl_ok(ret);
	gettimeofday(&buildTime, NULL);
#if CL_TARGET_OPENCL_VERSION < 200
	command_queue = clCreateCommandQueue(context, deviceId[idval], 0, &ret);
#else
	command_queue = clCreateCommandQueueWithProperties(context, deviceId[idval], 0, &ret);
#endif
	cl_ok(ret);
	pinBufIn = clCreateBuffer(context, CL_MEM_READ_ONLY, (sizeof(cl_ushort) * dBufLen), NULL, &ret);
	cl_ok(ret);
	dBuf_i = (unsigned short*)clEnqueueMapBuffer(command_queue, pinBufIn, CL_FALSE, CL_MAP_READ, 0, (sizeof(cl_ushort) * dBufLen), 0, NULL, NULL, &ret);
	cl_ok(ret);
	memset(dBuf_i, 0, dBufLen);
	dBufIn = clCreateBuffer(context, CL_MEM_READ_ONLY, (sizeof(cl_ushort) * dBufLen), NULL, &ret);
	cl_ok(ret);
	pinSeedToCl = clCreateBuffer(context, CL_MEM_READ_ONLY, strLen, NULL, &ret);
	cl_ok(ret);
	seedToCl = (char*)clEnqueueMapBuffer(command_queue, pinSeedToCl, CL_FALSE, CL_MAP_READ, 0, strLen, 0, NULL, NULL, &ret);
	cl_ok(ret);
	memset(seedToCl, 0, strLen);
	bufSeedToCl = clCreateBuffer(context, CL_MEM_READ_ONLY, strLen, NULL, &ret);
	cl_ok(ret);
	pinValidKey = clCreateBuffer(context, CL_MEM_WRITE_ONLY, (sizeof(cl_char) * ValidKLen), NULL, &ret);
	cl_ok(ret);
	hValidKey = (char*)clEnqueueMapBuffer(command_queue, pinValidKey, CL_FALSE, CL_MAP_WRITE, 0, (sizeof(cl_char) * ValidKLen), 0, NULL, NULL, &ret);
	cl_ok(ret);
	memset(hValidKey, 0, ValidKLen);
	dValidKey = clCreateBuffer(context, CL_MEM_WRITE_ONLY, (sizeof(cl_char) * ValidKLen), NULL, &ret);
	cl_ok(ret);
	cl(SetKernelArg(kernel22, 0, sizeof(dBufIn), (void*)&dBufIn));
	cl(SetKernelArg(kernel22, 1, sizeof(bufSeedToCl), (void*)&bufSeedToCl));
	cl(SetKernelArg(kernel22, 2, sizeof(dValidKey), (void*)&dValidKey));
	free(binary_str);
	gettimeofday(&end, NULL);
	double bTime = ((double)buildTime.tv_sec - (double)start.tv_sec) * 1e6;
	bTime = (bTime + ((double)buildTime.tv_usec - (double)start.tv_usec)) * 1e-6;
	double eTime = ((double)end.tv_sec - (double)start.tv_sec) * 1e6;
	eTime = (eTime + ((double)end.tv_usec - (double)start.tv_usec)) * 1e-6;
	printf("\n[init]Initialization done. Build time:%.2lfs From:%.2lfs total.", bTime, eTime);
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
	cl(ReleaseKernel(kernel22));
	cl(ReleaseProgram(program));
	cl(ReleaseContext(context));
}

unsigned int initialization(char* strIn, int idval) {
	unsigned int WGs = clSelectDevice(idval);
	clInitObj(strIn, idval);
	return WGs;
}

unsigned int clSelectDevice(int idval) {
	cl(GetPlatformIDs(1, &platform_id, &ret_num_platforms));
	cl(GetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 0, NULL, &clDevCount));
	if (clDevCount <= (unsigned int)idval) {
		printf("\nError: Device #%d not found. Number of device(s): %d", idval + 1, clDevCount);
		exit(1);
	}
	deviceId = (cl_device_id*)malloc(sizeof(cl_device_id) * clDevCount);
	cl(GetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, clDevCount, deviceId, NULL));
	cl(GetDeviceInfo(deviceId[idval], CL_DEVICE_NAME, 0, NULL, &clValSize));
	devInfo = (char*)malloc(clValSize);
	cl(GetDeviceInfo(deviceId[idval], CL_DEVICE_NAME, clValSize, devInfo, NULL));
	printf("\n[init]#%d/%d: %s ", idval + 1, clDevCount, devInfo);
	sprintf(logHelper, "dev:%s; ", devInfo);
	strcat(logEntry, logHelper);
	unsigned long devNameHash = djb2((char*)devInfo);
	char devNameToCh[72];
	sprintf(devNameToCh, "%lu", devNameHash);
	char binFNameFormatted[18];
	sprintf(binFNameFormatted, "%s.clbin", devNameToCh);
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
	printf("CUs: %d", maxComputeUnits);
	bool didWeBuildIt = kernelLoad(binFNameFormatted, idval);
	if (didWeBuildIt)
		kernelLoad(binFNameFormatted, idval);
	return (unsigned int)maxComputeUnits;
}

unsigned long djb2(char* str) {
	unsigned long hash = 5381;
	int c;
	while (c = *str++)
		hash = ((hash << 5) + hash) + c;
	return hash;
}

void ctrlc(int sig) {
	signal(sig, SIG_IGN);
	printf("\nCTRL-C! Aborting work and exiting ...\n");
	keep_running = 0;
}

unsigned int calcSizeMltPr(unsigned int MaxWGS) {
	return (unsigned int)ceil(62 / MaxWGS) - 1;
}
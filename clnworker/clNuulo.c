// SPDX-License-Identifier: GPL-3.0-only
//(c)2019 EM~ eetu@kkona.xyz

#include "clNuulo.h"

void ctrlc(int sig) {		//ctrl-c to allow user exit - 'rather clean' exit (1)
	signal(sig, SIG_IGN); 
	printf("\nCTRL-C! Quitting.");
		ReleaseAndFlush();
		exit(1);
}

void zoldhash(char* input, char* output, char* lastkeyM, char* reStrMc, int opMode) {

	int isSolved, rCount, nSpace, id2 = 0, id3 = 0; //rCount=kernel runs, nSpace=how much of noncespace tested (max62), id2&id3=host ctrl'd nonce chars
	time_t ltime;
	size_t local_work_size = 0;						//local work size to be autoset
	size_t global_work_size[3] = { 64, 64, 64 };	//setting to launch 3d kernel
	cl_event evWriteBuf, evReadBuf, evKernel;		//set cl events - todo: go through those events, determine necessity
	string_len = strlen(input);
	dBuf_i[0] = SHA256_PLAINTEXT_LENGTH; dBuf_i[1] = &global_work_size; dBuf_i[2] = string_len; dBuf_i[3] = 10; dBuf_i[4] = 0; lKeySize = string_len + 1; //set some initial values for kernel
	ltime = time(NULL);
	//char logFname[16] = "log/       .log";  //for logging
	//for (int nn = 0;nn < 7;nn++)			//for logging
	//	logFname[nn + 4] = input[nn + 49];	//for logging
	//FILE* fp;								//for logging
	printf("[k22]Starting worker\n");
	dBuf_i[3] = 10;							//was used to set useless runstates to kernel - priority-TODO: get rid of the whole dbufi[3] to free allocated but not-used memory...
	memcpy(seedToCl, input, string_len);	//pass preformatted prefix to be stored in buffer (ocl/read-only)
	lCounter = 0; nSpace = 0; rCount = 0; isSolved = 0;
	dBuf_i[0] = SHA256_PLAINTEXT_LENGTH; dBuf_i[1] = &global_work_size; dBuf_i[2] = string_len;
	//write prefix to buffer
	cl(EnqueueWriteBuffer(command_queue, seedToClBuf, CL_TRUE, 0, SHA256_PLAINTEXT_LENGTH * 2048, seedToCl, 0, NULL, &evWriteBuf));
	cl(WaitForEvents(1, &evWriteBuf));
	cl(ReleaseEvent(evWriteBuf));
	clock_t t;
	//enter while-loop - run until solved or if failed
	while (isSolved == 0) {
		signal(SIGINT, ctrlc); //ctrl-c signal
		dBuf_i[3] = id2;	//send host ctrl'd chars to kernel
		dBuf_i[4] = id3;
		cl(EnqueueWriteBuffer(command_queue, dBufIn, CL_TRUE, 0, sizeof(unsigned int) * 5, dBuf_i, 0, NULL, &evWriteBuf));
		cl(WaitForEvents(1, &evWriteBuf));
		cl(ReleaseEvent(evWriteBuf));
		if (lCounter == 0)		//start timer to report runspeed every 62th round
			t = clock();
		cl(EnqueueNDRangeKernel(command_queue, kernel22, 3, NULL, global_work_size, NULL, 0, NULL, &evKernel));  //launch kernel22
		cl(WaitForEvents(1, &evKernel));
		cl(ReleaseEvent(evKernel));
		cl(EnqueueReadBuffer(command_queue, dValidKey, CL_TRUE, 0, sizeof(cl_char) * 6, hValidKey, 0, NULL, &evReadBuf)); //read validkey-memory - if not yet solved, reads: ######
		cl(WaitForEvents(1, &evReadBuf));
		cl(ReleaseEvent(evReadBuf));
		if (+hValidKey[0] != 35) {		//see if # has changed indicating valid solution
			cl(EnqueueReadBuffer(command_queue, buf_bHashToHost, CL_TRUE, 0, sizeof(cl_uint) * SHA256_RESULT_SIZE, hashToHost, 0, NULL, &evReadBuf));  //read hash of the valid entry (todo: remove, no need to return hash to host...)
			cl(WaitForEvents(1, &evReadBuf));
			cl(ReleaseEvent(evReadBuf));
			for (int i = 0; i < SHA256_RESULT_SIZE; i++) { sprintf(output + i * 8, "%08x", hashToHost[i]); }	//read hash from buffer, and sprint it to 'output'
			isSolved = 1;
			int strlnTmp = string_len - nl;			//umm, this is propably some relic which is no longer used
			printf("\n[k22]Solution found! Round:%d\n[k22]Valid suffix:'%c%c%c%c%c%c'>>'%s'", rCount, hValidKey[0], hValidKey[1], hValidKey[2], hValidKey[3], hValidKey[4], hValidKey[5], output);
			sprintf(lastkeyM, "%s%c%c%c%c%c%c\0", seedToCl, hValidKey[0], hValidKey[1], hValidKey[2], hValidKey[3], hValidKey[4], hValidKey[5]);
			sprintf(reStrMc, "%c%c%c%c%c%c\0", hValidKey[0], hValidKey[1], hValidKey[2], hValidKey[3], hValidKey[4], hValidKey[5]);
			ReleaseAndFlush();  //clean exit before...
			break;	//...breaking out of the while-loop
		}
		if (id2 < 61) {
			id2++;
		}
		else if (id3 < 61) {
			id2 = 0;
			id3++;
		}
		else
		{
			printf("\n[k22]All done, no solution.");
			sprintf(reStrMc, "failure");
			ReleaseAndFlush();
			break;
		}
		rCount++;
		lCounter++;
		if (lCounter == 62) {	//reached 62 rounds, time to take a small break and report some information 
			nSpace++;
			t = clock() - t;
			double time_taken = ((double)t) / CLOCKS_PER_SEC; // time spent, in seconds
			ltime = time(NULL);
			double hashcnt = (((62 * 62 * 62 * 62 * 62) / time_taken) / 1000000); // gidx | gidy | gidz | kernGenW | lCounter - count hashrate in megahashes/s
			printf("\n[%d/62]Batch done in %lfs. Speed:%fMHs ", nSpace, time_taken, hashcnt);
			lCounter = 0; //revert our counter to 0, to restart timer next time around
		}
		if (nSpace == 62) {
			printf("\nWhole nonceSpace tested! No result. Very likely due to some error. Returning 'failure' as result.\n");
			sprintf(reStrMc, "failure\0");
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

void initKernel22(char* kernelV) {
	kernelLoad(kernelV);
	clInitObj();
}

void clInitObj() {
	//create context
	cl(GetPlatformIDs(1, &platform_id, &ret_num_platforms));
	cl(GetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices));  // look for ocl platforms, target 1st GPU device. TYPE_CPU to target CPUs (not worth it imo)
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	cl_ok(ret);
	//prepare program
	program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);
	cl_ok(ret);
	//build
	//cl(BuildProgram(program, 1, &device_id, "-g", NULL, NULL)); -g allows for (rather limited) ocl debugging etc.
	cl(BuildProgram(program, 1, &device_id, NULL, NULL, NULL));
	//create kernel
	kernel22 = clCreateKernel(program, "kernel22", &ret);
	cl_ok(ret);
	free(source_str);
	//create just a single commandqueue (tried-and-turned-out-to-suck: utilizing many separate command queues doesn't work that good here, no performance boost gained and is unstable)
	command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
	cl_ok(ret);
	//set up buffers
	bInputCl = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, (SHA256_PLAINTEXT_LENGTH)*2048, NULL, &ret); //prefix to cl
	bHashToHost = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, sizeof(cl_uint) * SHA256_RESULT_SIZE, NULL, &ret); //resulting hash back to host (TODO: this can be omitted completely methinks)
	seedToCl = (char*)clEnqueueMapBuffer(command_queue, bInputCl, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, (SHA256_PLAINTEXT_LENGTH)*2048, 0, NULL, NULL, &ret); //prefix to cl, pinned buffer
	memset(seedToCl, 0, (SHA256_PLAINTEXT_LENGTH)*2048);
	hashToHost = (cl_uint*)clEnqueueMapBuffer(command_queue, bHashToHost, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, sizeof(cl_uint) * SHA256_RESULT_SIZE, 0, NULL, NULL, &ret); //hash to host - again, to be removed
	memset(hashToHost, 0, sizeof(cl_uint) * SHA256_RESULT_SIZE); //to be removed...
	seedToClBuf = clCreateBuffer(context, CL_MEM_READ_ONLY, (SHA256_PLAINTEXT_LENGTH)*2048, NULL, &ret);
	buf_bHashToHost = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uint) * SHA256_RESULT_SIZE, NULL, &ret); //to be removed...
	dBufIn = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(unsigned int) * 5, NULL, &ret);	//some control values for kernel, incl. hash ctrl'd characters
	dValidKey = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, (sizeof(cl_char) * 6), NULL, &ret); //valid result to host - only written when valid solution found
	//kernel arguments
	cl(SetKernelArg(kernel22, 0, sizeof(dBufIn), (void*)&dBufIn));
	cl(SetKernelArg(kernel22, 1, sizeof(seedToClBuf), (void*)&seedToClBuf));
	cl(SetKernelArg(kernel22, 2, sizeof(buf_bHashToHost), (void*)&buf_bHashToHost)); //to be removed...
	cl(SetKernelArg(kernel22, 3, sizeof(dValidKey), (void*)&dValidKey)); }

void ReleaseAndFlush() {		//release everything reserved for the ocl/kernel
	clEnqueueUnmapMemObject(command_queue, bInputCl, seedToCl, 0, NULL, NULL);
	clEnqueueUnmapMemObject(command_queue, bHashToHost, hashToHost, 0, NULL, NULL); //to be removed...
	cl(Finish(command_queue));
	cl(ReleaseCommandQueue(command_queue));
	seedToCl = NULL;
	hashToHost = NULL;
	cl(ReleaseMemObject(bInputCl));
	cl(ReleaseMemObject(bHashToHost)); //to be removed...
	cl(ReleaseMemObject(dBufIn));
	cl(ReleaseMemObject(seedToClBuf));
	cl(ReleaseMemObject(buf_bHashToHost)); //to be removed...
	cl(ReleaseMemObject(dValidKey));
	cl(ReleaseContext(context));
	cl(ReleaseKernel(kernel22));
	cl(ReleaseProgram(program));
}

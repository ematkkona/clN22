// SPDX-License-Identifier: MIT
// clN22 Host program for OpenCL-accelerated "zold-score"-worker
// (c)2019-2020 EM~ eetu@kkona.xyz

#include "main.h"

int main(int argc, char* argv[]) {
	char inTimeAsStr[lenDateTimeFormat];
	const char introTxt[] = "clN22-worker v%s (c)2019-2020 EM~ eetu@kkona.xyz\n";
	const char useHelp[] = "\nUsage: 'clN22 <input> <output> (setdev #) (log <logfile>)\n\t(Optional) Use 'setdev <Device ID>' to select device. Use 'log \"file.log\"' to save log.";
	int idval = 0;
	char VerInfo[] = "0.981-230520";
	char output[128];
	char reStr[128];
	char resultOut[34];
	char stringIn[90];
	char kernelV[18];
	printf(introTxt, VerInfo);
	if (argc != 3 && argc != 5 && argc != 7) {
		printf("%s", useHelp);
		exit(0); }
	strcpy(stringIn, argv[1]);
	strcat(stringIn, " \0");
	strcpy(resultOut, argv[2]);
	strcpy(kernelV, "kernel22.cl");
	if (argc == 5 && !strcmp(argv[3], "setdev")) {
		idval = atoi(argv[4]); }
	else if (argc == 5 && !strcmp(argv[3], "log")) {
		logToFile = true;
		strcpy(logFile, argv[4]); }
	else if (argc == 7 && !strcmp(argv[5], "log")) {
		logToFile = true;
		strcpy(logFile, argv[6]); }
	else if (argc == 5 || argc == 7) {
		printf("\nError: Invalid argument(s) '%s' Expected: 'setdev' or 'log'\n%s", argv[3], useHelp);
		exit(0); }
	else { logToFile = false; }
	unsigned int inputLen = (unsigned int)strlen(stringIn);
	if (inputLen < 60 || inputLen > 90) {
		printf("\nError: Invalid input%s", useHelp);
		exit(0); }
	int opMode = 0;
	strcpy(output, stringIn);
	struct timeval start, end;
	while (!opMode) {
		currLocalTime(inTimeAsStr);
		printf("[init]{%s} In:'%s' Str>=%d", inTimeAsStr, argv[1], Strength);
		sprintf(logHelper, "\n<%s;input:'%s'; ", inTimeAsStr, argv[1]);
		strcat(logEntry, logHelper);
		unsigned int maxWgs = initialization(kernelV, stringIn, idval);
		gettimeofday(&start, NULL);
		zoldhash(output, reStr, idval, maxWgs);
		gettimeofday(&end, NULL);
		double time_taken = ((double)end.tv_sec - (double)start.tv_sec) * 1e6;
		time_taken = (time_taken + ((double)end.tv_usec - (double)start.tv_usec)) * 1e-6;
		if (strcmp(reStr, "") || strcmp(reStr, "failure")) {
			printf("Time:%.2lfs\n[finish]'%s%s'", time_taken, stringIn, reStr);
			opMode++; }
		else if (!strcmp(reStr, "UnSolv")) {
			printf("\n[finish]Unsolvable! Total time:%.2lfs", time_taken);
			opMode++; }
		else {
			printf("\n[error]Unspecified error\n");
			strcpy(reStr, "UnSpec");
			opMode++; }
		currLocalTime(inTimeAsStr);
		sprintf(logHelper, "total:%.2lf;%s>", time_taken, inTimeAsStr);
		strcat(logEntry, logHelper);
		printf("\n[finish]Save result '%s' to '%s'", reStr, resultOut);
		FILE* fp = fopen(resultOut, "w");
		fprintf(fp, reStr);
		fclose(fp);
		if (logToFile) {
			printf("\n[finish]Save log '%s'", logFile);
			FILE* fp = fopen(logFile, "a");
			fprintf(fp, logEntry);
			fclose(fp);
		}
	}
	printf("\n[finish]{%s} - clN22 out.", inTimeAsStr);
	exit(0);
}

void currLocalTime(char* timeOutStr) {
	char outBuff[lenDateTimeFormat];
	time_t rawtime;
	time(&rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	strftime(outBuff, sizeof(outBuff) - 1, "%d.%m.%y %H:%M:%S", timeinfo);
	memcpy(timeOutStr, outBuff, sizeof(outBuff));
}
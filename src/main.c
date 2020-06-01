// SPDX-License-Identifier: MIT
// clN22-worker / main.c
// v0.93-310520 (c)2019-2020 ~EM eetu@kkona.xyz

#include "main.h"

int main(int argc, char* argv[]) {
	printf(introTxt, VerInfo);
	testRun = false;
	sanityPass = false;
	idval = 0;
	if (argc == 2 && !strcmp(argv[1], "runtests")) {
		strcpy(logFile, "runtests.log");
		logToFile = true;
		testRun = true;
		benchRound = 146;
		strcpy(benchExpect, "2acA2t");
		strcpy(testExpect, "0aa92Q");
		strcpy(stringIn, "9297b24dd0251ff78c58ea06f6c2b128cd2675abb7ce93f3a2352c7c00000000 \0");
		strcpy(benchString, "9e40705bed228d6dca9d3d3514da58d1059c98d0100b9a4777c1737e00000000 \0");
		printf("\n*** Sanity check & benchmark ***\n");
	}
	if (argc != 3 && argc != 5 && argc != 7 && !testRun) {
		printf("%s", useHelp);
		exit(0); }
	else if (!testRun) {
		strcpy(stringIn, argv[1]);
		strcat(stringIn, " \0");
		strcpy(resultOut, argv[2]);
	}
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
	currLocalTime(inTimeAsStr);
	sprintf(logHelper, "\n<%s; ", inTimeAsStr);
	strcat(logEntry, logHelper);
	initialization(stringIn, idval);
	strcpy(output, stringIn);
	if (testRun) {
		zoldhash(output, reStr, idval);
		if (!strcmp(reStr, "TestFa")) {
			printf("\n[error]Uh-oh. Sanity check failed. No result.\n");
			sprintf(logHelper, "testInput:'%s'; expectedResult:'%s'; expectedRound:0; FAILED:ExpRoundExceeded>", stringIn, testExpect);
			strcat(logEntry, logHelper);
			FILE* fp = fopen(logFile, "a");
			fprintf(fp, logEntry);
			fclose(fp);
			exit(1); }
		else if (strcmp(reStr, testExpect)) {
			printf("\n[error]Uh-oh. Sanity check failed. Invalid result:'%s' Expected:'%s'\n", reStr, testExpect);
			sprintf(logHelper, "testInput:'%s'; expectedResult:'%s'; result:'%s'; expectedRound:0;FAILED:UnexpectedResult>", stringIn, testExpect, reStr);
			strcat(logEntry, logHelper);
			FILE* fp = fopen(logFile, "a");
			fprintf(fp, logEntry);
			fclose(fp);
			exit(1); }
		sanityPass = true;
	}
	struct timeval start, end;
	while (!opMode) {
		if (!testRun) {
			printf("\n[init]{%s} In:'%s' Str>=%d", inTimeAsStr, argv[1], Strength);
			sprintf(logHelper, "input:'%s'; ", argv[1]);
			strcat(logEntry, logHelper); 
		}
		else {
			strcpy(output, benchString);
			printf("\n[finish]Sanity check PASSED!\n[init]Run benchmark: Expected result:'%s' at round:%u", benchExpect, benchRound);
			sprintf(logHelper, "testString:'%s'; expectedResult:'%s'; result:'%s' expectedRound:'%s';", stringIn, testExpect, reStr, "0");
			strcat(logEntry, logHelper);
			strcpy(reStr, "");
		}
		gettimeofday(&start, NULL);
		zoldhash(output, reStr, idval);
		gettimeofday(&end, NULL);
		double time_taken = ((double)end.tv_sec - (double)start.tv_sec) * 1e6;
		time_taken = (time_taken + ((double)end.tv_usec - (double)start.tv_usec)) * 1e-6;
		if (strcmp(reStr, "") || strcmp(reStr, "UsrExit") || strcmp(reStr, "UnSolvd") || strcmp(reStr, "UnSpecd") || strcmp(reStr, "BncFail") || strcmp(reStr, "TstFail")) {
			printf("Time:%.2lfs\n[finish]'%s%s'", time_taken, stringIn, reStr);
			opMode++; }
		else {
			printf("\n[finish]FAILURE:'%s' Total time:%.2lfs",reStr, time_taken);
			if (!strcmp(reStr, "UsrExit"))
				strcpy(reStr, "bailed");
			else
				strcpy(reStr, "failed");
			opMode++; }
		currLocalTime(inTimeAsStr);
		sprintf(logHelper, "total:%.2lf; %s>", time_taken, inTimeAsStr);
		strcat(logEntry, logHelper);
		if (!testRun) {
			FILE* fp = fopen(resultOut, "w");
			fprintf(fp, reStr);
			fclose(fp);
		}
		if (logToFile) {
			FILE* fp = fopen(logFile, "a");
			fprintf(fp, logEntry);
			fclose(fp);
		}
	}
	printf("\n[finish]{%s} - clN22 out.\n", inTimeAsStr);
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
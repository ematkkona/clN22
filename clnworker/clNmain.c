// SPDX-License-Identifier: MIT
// clN22 Host program for OpenCL-accelerated "zold-score"-worker
// (c)2019-2020 EM~ eetu@kkona.xyz

#include "clNuulo.h"

int main(int argc, char* argv[]) {
	char resultOut[34];
	char stringIn[90];
	char kernelV[18];
	const char introTxt[] = "clN22-worker v%s (c)2019-2020 EM~ eetu@kkona.xyz\n";
	const char useHelp[] = "\nUsage: 'clN22 <input> <output> (setdev #) (log <logfile>)\n\t(Optional) Use 'setdev <Device ID>' to select device. Use 'log \"file.log\"' to save log.";
	time_t ltime;
	int idval = 0;
	char VerInfo[] = "0.98-150520";
	char output[128];
	char reStr[128];
	setlocale(LC_ALL, "fi-FI");
	printf(introTxt, VerInfo);
	if (argc != 3 && argc != 5 && argc != 7) {
		printf("%s", useHelp);
		exit(0); }
	strcpy(stringIn, argv[1]);
	strcat(stringIn, " \0");
	strcpy(resultOut, argv[2]);
	strcpy(kernelV, "kernel22.cl");
	if (argc == 5 && !strcmp(argv[3], "setdev")) {
		idval = atoi(argv[4]);
	}
	else if (argc == 5 && !strcmp(argv[3], "log")) {
		logToFile = true;
		strcpy(logFile, argv[4]);
	}
	else if (argc == 7 && !strcmp(argv[5], "log")) {
		logToFile = true;
		strcpy(logFile, argv[6]);
	}
	else if (argc == 5 || argc == 7) {
		printf("\nError: Invalid argument(s) '%s' Expected: 'setdev' or 'log'\n%s", argv[3], useHelp);
		exit(1);
	}
	else { logToFile = false;  }
	unsigned int inputLen = (unsigned int)strlen(stringIn);
	if (inputLen < 60 || inputLen > 90) {
		printf("\nError: Invalid input%s", useHelp);
		exit(0);
	}
	int opMode = 0;
	strcpy(output, stringIn);
	ltime = time(NULL);
	int lround = 0;
	while (!opMode) {
		printf("%s[init]Input:'%s' Str>=%d", asctime(localtime(&ltime)), argv[1], Strength);
		sprintf(logHelper, "\n%s;;;input:'%s'; ", asctime(localtime(&ltime)), argv[1]);
		strcat(logEntry, logHelper);
		initialization(kernelV, stringIn, idval);
		clock_t t = clock();
		zoldhash(output, reStr);
		t = clock() - t;
		double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
		if (strcmp(reStr, "") || strcmp(reStr, "failure")) {
			printf("Time:%.2lfs\n[finish]'%s%s'", time_taken, stringIn, reStr);
			opMode++;
		}
		else if (!strcmp(reStr, "failure")) {
			printf("\n[finish]Unsolvable! Total time:%.2lfs", time_taken);
			opMode++;
		}
		else {
			printf("\n[error]Unspecified error\n");
			strcpy(reStr, "failure");
			opMode++;
		}
		ltime = time(NULL);
		sprintf(logHelper, "total:%.2lf;;;\n%s", time_taken, asctime(localtime(&ltime)));
		strcat(logEntry, logHelper);
		printf("\n[finish]Saving result '%s' to '%s' ", reStr, resultOut);
		FILE* fp = fopen(resultOut, "w");
		fprintf(fp, reStr);
		fclose(fp);
		if (logToFile) {
			printf("\n[finish]Update log '%s'", logFile);
			FILE* fp = fopen(logFile, "a");
			fprintf(fp, logEntry);
			fclose(fp);
		}
	}
	printf(" - All done. clN22 out.\n%s", asctime(localtime(&ltime)));
	exit(0);
}
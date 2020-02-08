// SPDX-License-Identifier: GPL-3.0-only
// clN22 Host program for OpenCL-accelerated "zold-score"-worker
// (c)2019-2020 EM~ eetu@kkona.xyz

// 

#include "clNuulo.h"

int main(int argc, char* argv[]) {
	char resultOut[34];
	char stringIn[90];
	char kernelV[18];
	const char introTxt[] = "\nclN22-worker v%s (c)2019-2020 EM~ eetu@kkona.xyz\n'Älä ole ennakkoluuloton' -niilo22\n";
	const char useHelp[] = "\nUsage: 'clN22 <input> <output> (setdev #)\n\t(Optional) Use 'setdev <Device ID>' to select device";
	time_t ltime;
	int idval = 0;
	char VerInfo[] = "0.96-020220";
	char output[128];
	int exitcond = 0;
	char reStr[128];
	setlocale(LC_ALL, "fi-FI");
	printf(introTxt, VerInfo);
	if (argc != 3 && argc != 5) {
		printf("%s", useHelp);
		exit(0); }
	strcpy(stringIn, argv[1]);
	strcat(stringIn, " \0");
	strcpy(resultOut, argv[2]);
	strcpy(kernelV, "kernel22.cl");
	if (argc == 5 && !strcmp(argv[3], "setdev")) {
		idval = atoi(argv[4]);
	}
	else if (argc == 5) {
		printf("\nError: Invalid argument '%s' Expected: 'setdev'\n%s",argv[3],useHelp);
		exit(1);
	}
	unsigned int inputLen = (unsigned int)strlen(stringIn);
	if (inputLen < 60 || inputLen > 90) {
		printf("\nError: Invalid prefix%s", useHelp);
		exit(0);
	}
	int opMode = 0;
	strcpy(output, stringIn);
	ltime = time(NULL);
	int lround = 0;
	//normal run
	while (!opMode) {
		initialization(kernelV, stringIn, idval);
		printf("\n%s[input]strength>=%d prefix:'%s'\n", asctime(localtime(&ltime)), Strength, argv[1]);
		clock_t t = clock();
		zoldhash(output, reStr);
		t = clock() - t;
		double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
		ltime = time(NULL);
		if (strcmp(reStr, "") || strcmp(reStr, "NONE")) {
			printf("\n\n%s[finish]Time:%lfs\nEntry:'%s %s'\n", asctime(localtime(&ltime)), time_taken, argv[1], reStr);
			opMode++;
		}
		else if (!strcmp(reStr, "NONE")) {
			printf("\n\n%s[finish]Time:%lfs\nEntry:'%s'\nNo solution found!\n", asctime(localtime(&ltime)), time_taken, argv[1]);
			opMode++;
		}
		else {
			printf("\n[error]Unspecified error\n");
			strcpy(reStr, "failure");
			opMode++;
		}
		printf("\nSaving result '%s' to '%s'\n", reStr, resultOut);
		FILE* fp = fopen(resultOut, "w");
		fprintf(fp, reStr);
		fclose(fp);
	}
	printf("All done. clN22 out.\n");
	exit(0);
}
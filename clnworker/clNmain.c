// SPDX-License-Identifier: GPL-3.0-only
//(c)2019 EM~ eetu@kkona.xyz

//clN22 Host for OpenCL "zold-score"-grinder

#include "clNuulo.h"

int main(int argc, char* argv[]) {
	char resultOut[34];
	char stringIn[90];
	char kernelV[18];
	const char introTxt[] = "\n\nclN22 - OpenCL-replacement for 'zold-score'-worker\nv%s (c)2019 EM~ eetu@kkona.xyz\n\n'Älä ole ennakkoluuloton' -niilo22\n";
	const char useHelp[] = "\nUsage: 'clN22 <prefix> <outFile>'\n";
	time_t ltime;
	char VerInfo[] = "0.810-191119";
	char output[128];
	char solution[65];
	char lastkeyM[128];
	int exitcond = 0;
	char reStr[128];
	int nn;
	setlocale(LC_ALL, "fi-FI");
	printf(introTxt, VerInfo);
	if (argc < 3) {
		printf("%s", useHelp);
		exit(0);
	}
	strcpy(stringIn, argv[1]);
	strcat(stringIn, " ");
	strcpy(resultOut, argv[2]);
	strcpy(kernelV, "kernel22.cl");
	if (strlen(stringIn) < 60 || strlen(stringIn) > 90) {
		printf("\nInvalid prefix!%s", useHelp);
		exit(0);
	}
	int opMode = 0;
	strcpy(output, stringIn);
	int totLen = strlen(stringIn);
	ltime = time(NULL);
	char logFname[16] = "log/       .log";
	for (nn = 0;nn < 7;nn++)
		logFname[nn + 4] = output[nn + 49];
	//printf("\n[init]Session log: '%s'\n", logFname);
	/*FILE* fp = fopen(logFname, "a");
	fprintf(fp, "%s[init]Input:'%s'\n", asctime(localtime(&ltime)), output);
	fclose(fp);*/
	int lround = 0;
	if (!opMode) { //normal run
		initKernel22(kernelV);
		printf("\n%s[init]strength>=%d prefix:'%s'\n", asctime(localtime(&ltime)), Strength, argv[1]);
		clock_t t = clock();
		zoldhash(output, solution, lastkeyM, reStr, opMode);
		t = clock() - t;
		double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
		ltime = time(NULL);
		if (strcmp(reStr, "") || strcmp(reStr, "NONE")) {
			printf("\n\n%s[finish]Time:%lfs\nEntry:'%s'\nHash:'%s'\n", asctime(localtime(&ltime)), time_taken, lastkeyM, solution);
			/*fp = fopen(logFname, "a");
			fprintf(fp, "\n%s[finish]Time: %lfs\nEntry:'%s'\nHash:'%s'\n", asctime(localtime(&ltime)), time_taken, lastkeyM, solution);
			fclose(fp);*/
			exitcond++;
		}
		else if (!strcmp(reStr, "NONE")) {
			printf("\n\n%s[finish]Time:%lfs\nEntry:'%s'\nWell this was waste of time... returning workitem to be reprocessed.\n", asctime(localtime(&ltime)), time_taken, lastkeyM);
			exitcond++;
		}
		else {
			printf("\n[ERROR]reStr=NULL - This should not be possible!\n");
			/*fp = fopen(logFname, "a");
			fprintf(fp, "\n%s\n[ERROR]reStr=NULL - This should not be possible!\n", asctime(localtime(&ltime)));
			fclose(fp);*/
			exit(1);
		}
		printf("\nSaving result [%s] to: %s\n", reStr, resultOut);
		FILE* fp = fopen(resultOut, "w");
		fprintf(fp, reStr);
		fclose(fp);

		printf("All done. clN22 out.\n");
		return 0;
	}
}

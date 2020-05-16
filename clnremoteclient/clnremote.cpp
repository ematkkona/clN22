//SPDX-License-Identifier: MIT
//clN22 remote work manager client v0.410-020220
//(c) 2019-2020 EM~ eetu@kkona.xyz
#include "clnremote.h"
using json = nlohmann::json;
using namespace std;
static std::string wuID;
static std::string setDeviceId = "0";
static std::int32_t timeToWait;
const std::string wuIDFileName = "worker.uid";
const std::string execclN22 = "clN22.exe";
const std::string dLogBase = " log/";
static std::string lArgs;
static std::int32_t retryLmt = 6;
static std::string BaseUrl = "http://kkona.xyz:8080/workitem";
std::int32_t retryCnt = 0; std::int32_t proceed = 1; std::int32_t errorCond = 0; std::int32_t procRet = 0; std::string workIn = ""; std::string resultOut = ""; std::string workId = ""; std::string wOffer = ""; std::string ToWorkUrl = ""; std::string zwStartp = "cancel"; std::string zwOffer = "0"; std::string zwRes = "NONE"; std::string wkRes = "working"; std::string exRes = "expired";
std::string cancelBdy = "{ \"result\": \"NONE\",\"offer\": \"" + zwRes + "\",\"startp\": \"" + zwStartp + "\",\"wuid\": \"" + wuID + "\"  }";
void signalHandler(int signum) { exit(0); }
int main(int argc, char* argv[])
{
	std::cout << "clN22-wproxy v0.420-150520 (c)2019-2020 EM~ eetu@kkona.xyz" << endl;
	std::string lArgs = " log " + dLogBase + logdaily();
	if (argc == 2 && argv[1] != "") {
		setDeviceId = argv[1];
		std::cout << "Device #" << setDeviceId << endl; }
	else
		std::cout << "No device specified. Default: #" << setDeviceId << endl;
	wuID = checkID(wuIDFileName);
	if (wuID == "" || wuID == "fail")
		exit(1);
	BOOL regok = registerWorker(wuID, BaseUrl);
	if (!regok) { 
		cout << "Unable to register worker!" << endl;	
		return(1); }
	std::int32_t countAtt, relayStat = 0;
	std::string wuidBdy = "{ \"wuid\": \"" + wuID + "\" }";
	std::signal(SIGINT, signalHandler);
	while (proceed) {
		timeToWait = 0;
		errorCond = 0; proceed = 1; workIn = ""; workId = "";
		ToWorkUrl = BaseUrl + "/";
		countAtt = 0;
		while (countAtt < 1000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			auto r = cpr::Get(cpr::Url{ ToWorkUrl + "wreq" },
				cpr::Header{ { "Content-Type", "application/json" } },
				cpr::Body{ wuidBdy } );
			++countAtt;
			if (r.status_code == 200) {
				json retrieved = json::parse(r.text);
				auto& messages = retrieved["message"];
				auto& workitem = messages;
				if (workitem.contains("result")) {
					if (workitem["result"] == zwRes) {
						stringstream workin, workid;
						workin << workitem["workitem"];
						workid << workitem["_id"];
						workIn = workin.str();
						workId = workid.str();
						char toErase[] = "\"";
						workIn.erase(std::remove(workIn.begin(), workIn.end(), toErase[0]), workIn.end());
						workId.erase(std::remove(workId.begin(), workId.end(), toErase[0]), workId.end());
						std::string reserveBdy = "{ \"result\": \"working\", \"offer\": \"" + wOffer + "\", \"wuid\": \"" + wuID + "\" }";
						while (countAtt < 1000) {
							auto r = cpr::Put(cpr::Url{ ToWorkUrl + workId },
								cpr::Body{ reserveBdy },
								cpr::Header{ { "Content-Type", "application/json" } });
							if (r.status_code == 200) {
								std::cout << wOffer << "Got work:" << workIn << endl;
								proceed = 1;
								countAtt = 1000; }
							else if (r.status_code == 409) {
								proceed = 0;
								std::cout << r.status_code << " - Error retrieving work!" << endl;
								std::this_thread::sleep_for(std::chrono::milliseconds(50));
								countAtt = 1000; }
							else {
								cout << "Error " << r.status_code << " cnt: " << countAtt;
								proceed = 0;
								++countAtt; }
						}
					}
				}
			}
			else if (countAtt >= 999) {
				std::cout << "Error: " << r.status_code << ". Unable to get workitem. " << countAtt << " attemtps." << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				timeToWait = 10000;
				++errorCond;
				proceed = 0; }
		}
		if (workId == "" || workIn == "") {
			proceed = 0;
			std::cout << "There is no work available at the moment!" << endl;
			timeToWait = 10000; }
		else {
			if (proceed) {
				std::string rFileName = "work" + workId + ".out";
				std::string argsFor22i = execclN22 + " \"" + workIn + "\" \"" + "work/" + workId + ".out\" setdev " + setDeviceId + lArgs;
				std::cout << "Workfile: " << rFileName << endl;
				BOOL execExit = executeCommandLine(argsFor22i.c_str());
				if (!execExit) {
					auto r = cpr::Put(cpr::Url{ ToWorkUrl },
						cpr::Header{ {"Content - Type", "application / json"} },
						cpr::Body{ cancelBdy });
					if (r.status_code == 200) {
						if (remove(rFileName.c_str())) {
							std::cout << "Error starting worker. Workitem released" << endl;
							std::this_thread::sleep_for(std::chrono::milliseconds(50));
							timeToWait = 10000; }
						else {
							proceed = 0;
							errorCond++;
							std::cout << "Error starting worker. Unable to remove work-file." << endl;
							std::this_thread::sleep_for(std::chrono::milliseconds(50));
							timeToWait = 10000; }
					}
				}
				else {
					ifstream resFile("work/" + workId + ".out");
					if (resFile.is_open()) {
						getline(resFile, resultOut);
						resFile.close();
						remove(rFileName.c_str()); }
					else {
						std::cout << endl << "Unable to read result!" << endl << "Output: " << rFileName;
						std::this_thread::sleep_for(std::chrono::milliseconds(2000));
						errorCond++;
						proceed = 0; }
					if (resultOut != "" && proceed == 1) {
						std::string resultBdy = "{ \"result\": \"" + resultOut + "\",\"wuid\": \""+wuID+"\" }";
						auto r = cpr::Put(cpr::Url{ ToWorkUrl + workId },
							cpr::Body{ resultBdy },
							cpr::Header{ { "Content-Type", "application/json" } });
						if (r.status_code == 200) {
							if (resultOut != "userex") {
								std::cout << "'" << resultOut << "'" << " All done. Sleeping for 1 second..." << endl;
								std::this_thread::sleep_for(std::chrono::milliseconds(1000));
								proceed = 1;
								errorCond = 0;
								retryCnt = 0; }
							else {
								std::cout << endl << "Workitem returned unsolved. Quitting due to local request." << endl;
								exit(0); }
						}
						else {
							proceed = 0;
							errorCond++;
							std::cout << "Error returning result!" << endl;
							std::this_thread::sleep_for(std::chrono::milliseconds(50));
							timeToWait = 10000; }
					}
					else {
						cout << "Invalid result!" << endl;
						errorCond++;
						proceed = 0;
						timeToWait = 5000; }
				}
			}
		}
		if (errorCond > 0) {
			retryCnt++;
			if (retryCnt <= retryLmt) {
				std::cout << "Errors encountered. Attempt: " << retryCnt << "/" << retryLmt << endl;
				proceed = 1;
				errorCond = 0; }
			else {
				std::cout << "Error. Unable to continue. Retry in 20s." << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(20000));
				retryCnt = 0;
				errorCond = 0;
				proceed = 1; }
		}
		if (proceed == 0 || errorCond == 0) {
			if (timeToWait >= 500) {
				std::cout << "Sleeping for " << timeToWait / 1000 << " seconds..." << endl;
				proceed = 1;
				std::this_thread::sleep_for(std::chrono::milliseconds(timeToWait)); }
		}
	}
	return(0);
}

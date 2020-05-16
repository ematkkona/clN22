//SPDX-License-Identifier: MIT
//(c) 2019-2020 EM~ eetu@kkona.xyz

#include "clnremote.h"
#include<ctime>
using namespace std;

string checkID(string wuIDFileName) {
	ifstream idFile(wuIDFileName);
	string resId;
	if (idFile.is_open()) {
		getline(idFile, resId);
		idFile.close(); }
	if (resId != "") {
		cout << "Using previously saved workerID." << endl;
		return resId; }
	else if (resId == "") {
		ofstream idToFile(wuIDFileName);
		string idGen = genUID();
		if (idToFile.is_open()) {
			idToFile << genUID();
			idToFile.close();
			cout << "Using newly generated workerID." << endl;
			return idGen; } 
	}
	cout << "Error with workerID. Exiting.";
	return "fail"; }

std::string logdaily() {
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	tm local_tm = *localtime(&tt);
	std::int16_t year = local_tm.tm_year + 1900;
	std::int16_t month = local_tm.tm_mon + 1;
	std::int16_t date = local_tm.tm_mday;
	std::string returnVal = std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(date) + ".log";
	return returnVal;
}

std::string genUID() {
	unsigned char *suuid;
	std::string strOut;
	UUID genuID;
	ZeroMemory(&genuID, sizeof(UUID));
	UuidCreate(&genuID);
	static RPC_CSTR uID = NULL;
	RPC_STATUS rS = ::UuidToStringA(&genuID, &suuid);
	if (rS == RPC_S_OK) {
		strOut = (reinterpret_cast<char const*>(suuid));
		RpcStringFreeA(&uID); }
	return strOut;
}

BOOL registerWorker(std::string uID, std::string BaseUrl) {
	std::cout << "WorkerID: " << uID;
	auto r = cpr::Post(cpr::Url{ {BaseUrl + "/workitem/wreq"} },
		cpr::Header{ {"Content - Type", "application / json"} },
		cpr::Body{ {"\"wuid\": \"" + uID + "\"" } });
	if (r.status_code != 200) {
		std::cout << " - Ok! Starting workitem polling..." << std::endl;
		return(1); }
	else
		return(0);
}

BOOL executeCommandLine(CString cmdLine)
{
	PROCESS_INFORMATION processInformation = { 0 };
	STARTUPINFO startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);
	int nStrBuffer = cmdLine.GetLength() + 50;

	BOOL result = CreateProcess(NULL, cmdLine.GetBuffer(nStrBuffer),
		NULL, NULL, FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL, NULL, &startupInfo, &processInformation);
	cmdLine.ReleaseBuffer();


	if (!result)
	{
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

		CString strError = (LPTSTR)lpMsgBuf;
		ATLTRACE(_T("::executeCommandLine() failed at CreateProcess()\nCommand=%s\nMessage=%s\n\n"), cmdLine, strError);
		LocalFree(lpMsgBuf);

		return FALSE;
	}
	else
	{

		WaitForSingleObject(processInformation.hProcess, INFINITE);
		DWORD exitCode = 0;
		result = GetExitCodeProcess(processInformation.hProcess, &exitCode);

		CloseHandle(processInformation.hProcess);
		CloseHandle(processInformation.hThread);

		if (!result)
		{
			ATLTRACE(_T("Missing exit code!\n"));
			return FALSE;
		}
		return TRUE;
	}
}
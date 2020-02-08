//SPDX-License-Identifier: GPL-3.0-only
//(c) 2019-2020 EM~ eetu@kkona.xyz

#include "clnremote.h"

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
		RpcStringFreeA(&uID);
	}
	return strOut;
}

BOOL registerWorker(std::string uID, std::string BaseUrl) {
	std::cout << "Session ID: " << uID << std::endl;
	auto r = cpr::Post(cpr::Url{ {BaseUrl + "/workitem/wreq"} },
		cpr::Header{ {"Content - Type", "application / json"} },
		cpr::Body{ {"\"wuid\": \"" + uID + "\"" } });
	if (r.status_code != 200) {
		std::cout << "Ok!" << std::endl;
		return(1);
	}
	else {
		return(0);
	}
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
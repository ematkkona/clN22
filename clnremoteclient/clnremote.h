#pragma once
#pragma comment(lib, "rpcrt4.lib")
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <rpc.h>
#include <iosfwd>
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <chrono>
#include <fstream>
#include <windows.h>
#include <atlstr.h>
#include <atltrace.h>
#include <cstring>
#include <csignal>
#include <algorithm>

BOOL registerWorker(std::string, std::string);
BOOL executeCommandLine(ATL::CString);
void signalHandler(std::int32_t);
std::string genUID();
std::string checkID(std::string);
std::string logdaily();

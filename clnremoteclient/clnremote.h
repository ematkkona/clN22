#pragma once
#pragma comment(lib, "rpcrt4.lib")
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

BOOL registerWorker(std::string, std::string);
BOOL executeCommandLine(ATL::CString);
void signalHandler(std::int32_t);
std::string genUID();
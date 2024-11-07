#pragma once
#include <algorithm>
#include <thread>
#include <windows.h>
#include <vector>
#include <Psapi.h>
#include <tchar.h>

#include "minhook/MinHook.h"
#include "util.h"
typedef uint64_t QWORD;

QWORD scanmem(HANDLE process, char* markCode, int nOffset, unsigned long dwReadLen = 4, QWORD StartAddr = 0x140000000, QWORD EndAddr = 0x200000000, int InstructionLen = 0);
bool unprotect(char* ptr, int len, PDWORD oldp);
bool protect(char* ptr, int len, PDWORD oldp);
#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>

using namespace std;


MODULEINFO GetModuleInfo(char *szModule)
{
	MODULEINFO modinfo = { 0 };
	HMODULE hModule = GetModuleHandle(szModule);
	if (hModule == 0)
		return modinfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	return modinfo;
}


void WriteToMemory(uintptr_t addressToWrite, char* valueToWrite, int byteNum)
{
	unsigned long OldProtection;
	VirtualProtect((LPVOID)(addressToWrite), byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);
	memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);
	VirtualProtect((LPVOID)(addressToWrite), byteNum, OldProtection, NULL);
}


DWORD FindPattern(char *module, char *pattern, char *mask)
{
	MODULEINFO mInfo = GetModuleInfo(module);
	DWORD base = (DWORD)mInfo.lpBaseOfDll;
	DWORD size = (DWORD)mInfo.SizeOfImage;
	DWORD patternLength = (DWORD)strlen(mask);

	for (DWORD i = 0; i < size - patternLength; i++)
	{
		bool found = true;
		for (DWORD j = 0; j < patternLength; j++)
		{
			found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
		}

		if (found)
		{
			return base + i;
		}
	}

	return NULL;
}

DWORD MyFindPattern(DWORD Inicio, DWORD Tamanho, char *pattern, char *mask)
{
	DWORD base = Inicio;
	DWORD size = Tamanho;
	DWORD patternLength = (DWORD)strlen(mask);

	for (DWORD i = 0; i < size - patternLength; i++)
	{
		bool found = true;
		for (DWORD j = 0; j < patternLength; j++)
		{
			found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
		}

		if (found)
		{
			return base + i;
		}
	}

	return NULL;
}
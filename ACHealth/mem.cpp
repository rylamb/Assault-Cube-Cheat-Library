#include "stdafx.h"
#include "mem.h"

void mem::PatchEx(BYTE* destination, BYTE* source, unsigned int size, HANDLE hProcess)
{
	DWORD oldprotect;
	VirtualProtectEx(hProcess, destination, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, destination, source, size, nullptr);
	VirtualProtectEx(hProcess, destination, size, oldprotect, &oldprotect);
}
void mem::NopEx(BYTE* destination, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	PatchEx(destination, nopArray, size, hProcess);
	delete[] nopArray;
}

uintptr_t mem::FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;

	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}

	return addr;
}

void mem::Patch(BYTE* destination, BYTE* source, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(destination, source, size);
	VirtualProtect(destination, size, oldprotect, &oldprotect);
}
void mem::Nop(BYTE* destination, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(destination, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(destination, 0x90, size);
	VirtualProtect(destination, size, oldprotect, &oldprotect);
}

uintptr_t mem::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;

	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}

	return addr;
}
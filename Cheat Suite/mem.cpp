#include "pch.h"
#include "mem.h"

void mem::write(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD access_protection_val;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &access_protection_val);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, access_protection_val, &access_protection_val);
}

void mem::nop(BYTE* dst, unsigned int size)
{
	DWORD access_protection_val;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &access_protection_val);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, access_protection_val, &access_protection_val);
}
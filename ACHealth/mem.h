#pragma once

#include "stdafx.h"
#include <vector>
#include <Windows.h>

namespace mem
{
	void PatchEx(BYTE* destination, BYTE* source, unsigned int size, HANDLE hProcess);
	void NopEx(BYTE* destination, unsigned int size, HANDLE hProcess);
	uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

	void Patch(BYTE* destination, BYTE* source, unsigned int size);
	void Nop(BYTE* destination, unsigned int size);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
}
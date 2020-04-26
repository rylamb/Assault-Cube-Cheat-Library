#pragma once
#include "pch.h"
#include <Windows.h>

namespace mem
{
	void write(BYTE* dst, BYTE* src, unsigned int size);
	void nop(BYTE* dst, unsigned int size);
}
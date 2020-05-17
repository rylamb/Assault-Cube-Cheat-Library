#pragma once
#include <Windows.h>
#include "mem.h"

bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len);
BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len);

struct Hook
{
	bool bStatus{ false };

	BYTE* src{ nullptr };
	BYTE* dst{ nullptr };
	BYTE* PtrToGatewayFnPtr{ nullptr };
	uintptr_t len{ 0 };

	BYTE originalBytes[10]{ 0 };

	//Constructors
	Hook(BYTE* src, BYTE* dst, BYTE* PtrToGatewayFnPtr, uintptr_t len);
	Hook(const char* exportName, const char* modName, BYTE* dst, BYTE* PtrtoGatewayFnPtr, uintptr_t len);

	//Function Def's
	void Enable();
	void Disable();
	void Toggle();
};
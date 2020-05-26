#include "pch.h"
#include "hook.h"


/* Citation
https://guidedhacking.com/threads/how-to-get-started-with-opengl-hacks.11475/
*/

bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5) return false;	//Minumum overwritable bytes is 5

	DWORD curProtection;	//Stores copy of old protections so we can restore them later
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);	//Add write permissions to desired memory location

	uintptr_t relativeAddress = dst - src - 5;	//Calculating our relative address from the source to the destination

	*src = 0xE9;	//Change src byte (the first byte to be overwritten) to the 'jmp' instruction

	*(uintptr_t*)(src + 1) = relativeAddress; //write the relative address to the next byte (this is the address we're jumping to)

	VirtualProtect(src, len, curProtection, &curProtection);	//Reset protections to previous value
	return true;
}

//This func. is basically a wrapper around Detour32 -> sets up gateway 
BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5) return 0; //Minimum overwritable length is 5

	//Create gateway
	BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	//Write stolen bytes to gateway
	memcpy_s(gateway, len, src, len);

	//Get gateway to destination address
	uintptr_t gatewayRelativeAddr = src - gateway - 5;

	//Add jmp opcode to end of gateway
	*(gateway + len) = 0xE9;

	//Write address of the gateway to the jmp
	*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddr;

	//Perform detour
	Detour32(src, dst, len);

	return gateway;
}


Hook::Hook(BYTE* src, BYTE* dst, BYTE* PtrToGatewayFnPtr, uintptr_t len) //Basic Constructor
{
	this->src = src;
	this->dst = dst;
	this->len = len;
	this->PtrToGatewayFnPtr = PtrToGatewayFnPtr;
}
Hook::Hook(const char* exportName, const char* modName, BYTE* dst, BYTE* PtrToGatewayFnPtr, uintptr_t len) //Wrapper Constructor
{
	HMODULE hMod = GetModuleHandleA(modName);
	
	this->src = (BYTE*)GetProcAddress(hMod, exportName);
	this->dst = dst;
	this->len = len;
	this->PtrToGatewayFnPtr = PtrToGatewayFnPtr;
}

void Hook::Enable()
{
	memcpy(originalBytes, src, len);
	*(uintptr_t*)PtrToGatewayFnPtr = (uintptr_t)TrampHook32(src, dst, len);		//Get gateway returned by TrampHook
	bStatus = true;
}
void Hook::Disable()
{
	mem::write(src, originalBytes, len);
	bStatus = false;
}
void Hook::Toggle()
{
	if (bStatus) Enable();
	else Disable();
}
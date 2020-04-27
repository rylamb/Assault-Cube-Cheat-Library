// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "mem.h"

DWORD APIENTRY hackthread(LPVOID hModule)
{
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");
    uintptr_t* localPlayerAddress = (uintptr_t*)(moduleBase + 0x10F4F4);
    
    bool bAmmo = false, invincible = false;

    while (true)
    {

        if (GetAsyncKeyState(VK_END) & 1)
        {
            /* TODO: Restore NOP'ed functions on dll detach*/
            break;
        }

        if (GetAsyncKeyState(VK_F1) & 1)
        {
            if (invincible)
            {
                *(int*)(*localPlayerAddress + 0xF8) = 100;
                *(int*)(*localPlayerAddress + 0xFC) = 0;
            }

            invincible = !invincible;
        }

        if (invincible)
        {
            *(int*)(*localPlayerAddress + 0xF8) = 999;
            *(int*)(*localPlayerAddress + 0xFC) = 999;
        }
        
        if (GetAsyncKeyState(VK_F2) & 1)
        {
            bAmmo = !bAmmo;

            if (bAmmo)
            {
                //Give a grenade if empty
                int* grenade_count = (int*)(*localPlayerAddress + 0x158);
                if ((*grenade_count) < 1)
                {
                    (*grenade_count) = 1;
                }

                mem::nop((BYTE*)(moduleBase + 0x637E9), 2);     //NOP the ammo decrement function
                mem::nop((BYTE*)(moduleBase + 0x63378), 2);     //NOP the grenade decrement function
            }
            else
            {
                mem::write((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x0E", 2);        //Restore ammo decrement function
                mem::write((BYTE*)(moduleBase + 0x63378), (BYTE*)"\xFF\x08", 2);        //Restore grenade decrement function
            }
        }

        Sleep(5);
    }

    FreeLibraryAndExitThread((HMODULE)hModule, NULL);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, hackthread, hModule, 0, nullptr));
        break;
    default:
        break;
    }
    return TRUE;
}


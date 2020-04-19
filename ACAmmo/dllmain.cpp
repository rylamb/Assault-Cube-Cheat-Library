// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

void refill_ammo(DWORD* localPlayerAddress)
{
    int* pistol_reserve = (int*)(*localPlayerAddress + 0x114);
    int* carbine_reserve = (int*)(*localPlayerAddress + 0x118);
    int* shotgun_reserve = (int*)(*localPlayerAddress + 0x11C);
    int* smg_reserve = (int*)(*localPlayerAddress + 0x120);
    int* sniper_reserve = (int*)(*localPlayerAddress + 0x124);
    int* AR_reserve = (int*)(*localPlayerAddress + 0x128);
    int* akimbo_reserve = (int*)(*localPlayerAddress + 0x134);

    int* pistol_mag = (int*)(*localPlayerAddress + 0x13C);
    int* carbine_mag = (int*)(*localPlayerAddress + 0x140);
    int* shotgun_mag = (int*)(*localPlayerAddress + 0x144);
    int* smg_mag = (int*)(*localPlayerAddress + 0x148);
    int* sniper_mag = (int*)(*localPlayerAddress + 0x14C);
    int* AR_mag = (int*)(*localPlayerAddress + 0x150);
    int* akimbo_mag = (int*)(*localPlayerAddress + 0x15C);

    int* grenades = (int*)(*localPlayerAddress + 0x158);

    *pistol_reserve = 99;
    *carbine_reserve = 99;
    *shotgun_reserve = 99;
    *smg_reserve = 99;
    *sniper_reserve = 99;
    *AR_reserve = 99;
    *akimbo_reserve = 99;

    * pistol_mag = 99;
    *carbine_mag = 99;
    *shotgun_mag = 99;
    *smg_mag = 99;
    *sniper_mag = 99;
    *AR_mag = 99;
    *akimbo_mag = 99;

    * grenades = 99;
}

DWORD APIENTRY hackthread(LPVOID hModule)
{
    DWORD* localPlayerAddress = (DWORD*)(0x50F4F4);
    
    /* Inconsistencies with results. May not be good solution. */
    //DWORD* current_weapon = (DWORD*)(*localPlayerAddress + 0x374);
    //DWORD* pmag = (DWORD*)(*current_weapon + 0x14);
    //int* pmag_val = (int*)(*pmag + 0x0);
    //DWORD* pammo = (DWORD*)(*current_weapon + 0x10);
    //int* pammo_val = (int*)(*pammo + 0x0);

    //int* ammo = (int*)(*localPlayerAddress + 0x148);
    //int* grenades = (int*)(*localPlayerAddress + 0x158);


    bool bAmmo = true;

    while (true)
    {
        
        if (GetAsyncKeyState(VK_F1) & 1) bAmmo = !bAmmo;

        if (bAmmo)
        {
            refill_ammo(localPlayerAddress);
            //*ammo = 99;
            //*grenades = 99;
        }
            

        //if (GetAsyncKeyState(VK_F1) & 1) break;

        //Sleep(10);
    }

    //FreeLibraryAndExitThread((HMODULE)hModule, NULL);

    return NULL;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, hackthread, hModule, 0, nullptr);
        break;
    default:
        break;
    }
    return TRUE;
}


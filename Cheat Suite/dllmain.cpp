// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "mem.h"

DWORD APIENTRY hackthread(LPVOID hModule)
{
    uintptr_t module_base = (uintptr_t)GetModuleHandle(L"ac_client.exe");
    uintptr_t* local_player_addr = (uintptr_t*)(module_base + 0x10F4F4);
    
    bool bAmmo = false, bHealth = false, bRecoil = false;

    while (true)
    {

        if (GetAsyncKeyState(VK_END) & 1)
        {
            *(int*)(*local_player_addr + 0xF8) = 100;                               //Restore health to 100
            *(int*)(*local_player_addr + 0xFC) = 0;                                 //Restore armor to 0
            *(int*)(*local_player_addr + 0x158) -= 1;                               //Remove gobstopper grenade
            mem::write((BYTE*)(module_base + 0x637E9), (BYTE*)"\xFF\x0E", 2);       //Restore ammo decrement function
            mem::write((BYTE*)(module_base + 0x63378), (BYTE*)"\xFF\x08", 2);       //Restore grenade decrement function
            mem::write((BYTE*)(module_base + 0x62020), (BYTE*)"\x55\x8B\xEC", 3);   //Restore recoil function
            break;
        }

        if (GetAsyncKeyState(VK_F1) & 1)
        {
            if (bHealth)    //Restore default health/amor values
            {
                *(int*)(*local_player_addr + 0xF8) = 100;
                *(int*)(*local_player_addr + 0xFC) = 0;
            }
            bHealth = !bHealth;
        }

        if (bHealth)    //Replenish health/armor indefinitely
        {
            *(int*)(*local_player_addr + 0xF8) = 200;
            *(int*)(*local_player_addr + 0xFC) = 200;
        }
        
        if (GetAsyncKeyState(VK_F2) & 1)
        {
            bAmmo = !bAmmo;
            if (bAmmo)
            {
                *(int*)(*local_player_addr + 0x158) += 1;       //Give an gobstopper grenade. It's everlasting!
                mem::nop((BYTE*)(module_base + 0x637E9), 2);    //Disable the ammo decrement function
                mem::nop((BYTE*)(module_base + 0x63378), 2);    //Disable the grenade decrement function
            }
            else
            {
                *(int*)(*local_player_addr + 0x158) -= 1;                           //Remove gobstopper grenade
                mem::write((BYTE*)(module_base + 0x637E9), (BYTE*)"\xFF\x0E", 2);   //Enable ammo decrement function
                mem::write((BYTE*)(module_base + 0x63378), (BYTE*)"\xFF\x08", 2);   //Enable grenade decrement function
            }
        }

        if (GetAsyncKeyState(VK_F3) & 1)
        {
            bRecoil = !bRecoil;
            if (bRecoil)
            {
                mem::write((BYTE*)(module_base + 0x62020), (BYTE*)"\xC2\x08\x00", 3);       //Skip function by immediately returning 8 bytes
            }
            else
            {
                mem::write((BYTE*)(module_base + 0x62020), (BYTE*)"\x55\x8B\xEC", 3);       //Restore recoil function
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


// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "mem.h"
#include "proc.h"
#include "hook.h"
#include "glDraw.h"
#include "glText.h"
#include "wallHack.h"
#include <Windows.h>
#include <iostream>
#include "aimbot.h"

typedef BOOL(__stdcall* twglSwapBuffers)(HDC hDc);

twglSwapBuffers wglSwapBuffersGateway;
twglSwapBuffers owglSwapBuffers;

//Get module base and player ptr
uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");
uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10F4F4);

bool bAmmo = false, bInvincible = false, bRecoil = false, bFast = false, bAimbot = false, bESP = false, bEjected = false;

GL::Font glFont;
const int FONT_HEIGHT = 15;
const int FONT_WIDTH = 9;

const char* title = "AC_EZ_Mode";

const char* invincibleON = "Invincible [F1] [ON]";
const char* invincibleOFF = "Invincible [F1] [OFF]";

const char* ammoON = "Max Ammo [F2] [ON]";
const char* ammoOFF = "Max Ammo [F2] [OFF]";

const char* recoilON = "No Recoil [F3] [ON]";
const char* recoilOFF = "No Recoil [F3] [OFF]";

const char* speedON = "Speed Boost [F4] [ON]";
const char* speedOFF = "Speed Boost [F4] [OFF]";

const char* espON = "ESP [F5] [ON]";
const char* espOFF = "ESP [F5] [OFF]";

const char* aimbotON = "Aimbot [F6] [ON]";
const char* aimbotOFF = "Aimbot [F6] [OFF]";
const char* aimbotInstructions = "Hold Caps Lock";

void Draw()
{
    //Get current device and rendering context
    HDC currentHDC = wglGetCurrentDC();
    HGLRC currentHGLRC = wglGetCurrentContext();

    //Build if not already done or context has changed
    if (!glFont.bBuilt || currentHDC != glFont.hdc || currentHGLRC != glFont.hglrc)
    {
        glFont.Build(FONT_HEIGHT);
    }

    //Do the thing - setup, draw, and restore
    GL::SetupOrtho();

    //Draw outline of Menu
    //GL::DrawOutline(820, 205, 200, 110, 2.0f, rgb::green);
    GL::DrawOutline(10, 25, 200, 110, 2.0f, rgb::green);

    //Draw Title
    float title_textPointX = glFont.centerText(10, 200, strlen(title) * FONT_WIDTH);
    float title_textPointY = 25 - FONT_HEIGHT / 2;
    glFont.Print(title_textPointX, title_textPointY, rgb::green, "%s", title);

    //Declare starting positions for inner text elements
    float innerTextX = 15;
    float innerTextStartY = 45;

    //Draw Invincibility Status
    if (bInvincible) glFont.Print(innerTextX, innerTextStartY, rgb::green, "%s", invincibleON);
    else glFont.Print(innerTextX, innerTextStartY, rgb::red, "%s", invincibleOFF);

    //Draw Max Ammo Status
    if (bAmmo) glFont.Print(innerTextX, innerTextStartY + FONT_HEIGHT, rgb::green, "%s", ammoON);
    else glFont.Print(innerTextX, innerTextStartY + FONT_HEIGHT, rgb::red, "%s", ammoOFF);

    //Draw Recoil Status
    if (bRecoil) glFont.Print(innerTextX, innerTextStartY + (FONT_HEIGHT * 2), rgb::green, "%s", recoilON);
    else glFont.Print(innerTextX, innerTextStartY + (FONT_HEIGHT * 2), rgb::red, "%s", recoilOFF);

    //Draw Speed Status
    if (bFast) glFont.Print(innerTextX, innerTextStartY + (FONT_HEIGHT * 3), rgb::green, "%s", speedON);
    else glFont.Print(innerTextX, innerTextStartY + (FONT_HEIGHT * 3), rgb::red, "%s", speedOFF);

    //Draw ESP Status
    if (bESP) glFont.Print(innerTextX, innerTextStartY + (FONT_HEIGHT * 4), rgb::green, "%s", espON);
    else glFont.Print(innerTextX, innerTextStartY + (FONT_HEIGHT * 4), rgb::red, "%s", espOFF);

    //Draw Aimbot Status
    if (bAimbot) glFont.Print(innerTextX, innerTextStartY + (FONT_HEIGHT * 5), rgb::green, "%s", aimbotON);
    else glFont.Print(innerTextX, innerTextStartY + (FONT_HEIGHT * 5), rgb::red, "%s", aimbotOFF);

    //Draw Aimbot Additional Instructions
    float centerX = glFont.centerText(10, 200, strlen(aimbotInstructions) * FONT_WIDTH);
    if (bAimbot) glFont.Print(centerX, 152, rgb::green, "%s", aimbotInstructions);

    GL::RestoreGL();
}


//Hack moved into the hooked function
BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
    //std::cout << "Hooked" << std::endl;

    //Start of hack in old while loop
    if (localPlayerPtr)
    {
        // Eject DLL
        if (GetAsyncKeyState(VK_END) & 1)
        {
            *(int*)(*localPlayerPtr + 0xF8) = 100;                               //Restore health to 100
            *(int*)(*localPlayerPtr + 0xFC) = 0;                                 //Restore armor to 0
            //*(int*)(*localPlayerPtr + 0x158) -= 1;                               //Remove gobstopper grenade
            mem::write((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x0E", 2);       //Restore ammo decrement function
            mem::write((BYTE*)(moduleBase + 0x63378), (BYTE*)"\xFF\x08", 2);       //Restore grenade decrement function
            mem::write((BYTE*)(moduleBase + 0x62020), (BYTE*)"\x55\x8B\xEC", 3);   //Restore recoil function
            bEjected = true;                                                       //Raise Eject Flag
            return wglSwapBuffersGateway(hDc);                                     //Exit hook early
        }

        // Toggle Infinite Health and Armor
        if (GetAsyncKeyState(VK_F1) & 1)
        {
            if (bInvincible)
            {
                *(int*)(*localPlayerPtr + 0xF8) = 100;
                *(int*)(*localPlayerPtr + 0xFC) = 0;
            }

            bInvincible = !bInvincible;
        }

        if (bInvincible)    //Replenish health/armor indefinitely
        {
            *(int*)(*localPlayerPtr + 0xF8) = 999;
            *(int*)(*localPlayerPtr + 0xFC) = 999;
        }

        // Toggle Infinite Ammo and Grenades
        if (GetAsyncKeyState(VK_F2) & 1)
        {
            bAmmo = !bAmmo;
            if (bAmmo)
            {
                //*(int*)(*localPlayerPtr + 0x158) += 1;       //Give an gobstopper grenade. It's everlasting!
                mem::nop((BYTE*)(moduleBase + 0x637E9), 2);    //Disable the ammo decrement function
                //mem::nop((BYTE*)(moduleBase + 0x63378), 2);    //Disable the grenade decrement function
            }
            else
            {
                //*(int*)(*localPlayerPtr + 0x158) -= 1;                           //Remove gobstopper grenade
                mem::write((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x0E", 2);   //Enable ammo decrement function
                //mem::write((BYTE*)(moduleBase + 0x63378), (BYTE*)"\xFF\x08", 2);   //Enable grenade decrement function
            }
        }

        // Toggle Recoil
        if (GetAsyncKeyState(VK_F3) & 1)
        {
            bRecoil = !bRecoil;
            if (bRecoil)
            {
                mem::write((BYTE*)(moduleBase + 0x62020), (BYTE*)"\xC2\x08\x00", 3);       //Skip function by immediately returning 8 bytes
            }
            else
            {
                mem::write((BYTE*)(moduleBase + 0x62020), (BYTE*)"\x55\x8B\xEC", 3);       //Restore recoil function
            }
        }

        // Toggle Speed Increase
        if (GetAsyncKeyState(VK_F4) & 1)
        {
            bFast = !bFast;
        }

        if (bFast)
        {
            //Switch statement -> check value of Speed/Direction memory address and update as needed
            int* speed = (int*)(*localPlayerPtr + 0x80);
            switch (*speed)
            {
            case 1:     //Forward
            {
                *speed = 2;
                break;
            }
            case 255:   //Backwards
            {
                *speed = 254;
                break;
            }
            default:
                break;
            }
        }

        // Toggle ESP
        if (GetAsyncKeyState(VK_F5) & 1)
            bESP = !bESP;

        if (bESP)
            wallHackMain();

        // Toggle Aimbot
        if (GetAsyncKeyState(VK_F6) & 1)
        {
            bAimbot = !bAimbot;
        }

        if (GetAsyncKeyState(VK_CAPITAL) && bAimbot)
        {
            Aimbot::run();
        }
        else if (!GetAsyncKeyState(VK_CAPITAL) && bAimbot && !GetAsyncKeyState(VK_LBUTTON))  // Aimbot is running, so make sure we aren't shooting
        {
            *(int*)(*localPlayerPtr + 0x224) = 0;
        }
    }
    //End of old while loop hack
    Draw();

    return wglSwapBuffersGateway(hDc);
}

DWORD WINAPI HackThread(HMODULE hModule)
{
    Hook SwapBuffersHook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);
    
    //Enable Hook
    SwapBuffersHook.Enable();

    while (!bEjected) {
        //empty while loop to hold execution here until the player chooses to exit DLL
        continue;
    }

    //Disable Hook
    SwapBuffersHook.Disable();
    Sleep(10);
    
    //Exits the created thread so we can inject again
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


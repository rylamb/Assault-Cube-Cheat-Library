#include "stdafx.h"
#include "mem.h"
#include "proc.h"
#include "hook.h"
#include "glDraw.h"
#include "glText.h"
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

typedef BOOL(__stdcall* twglSwapBuffers)(HDC hDc);

twglSwapBuffers wglSwapBuffersGateway;
twglSwapBuffers owglSwapBuffers;

//Get module base
uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

bool bFast = false;

GL::Font glFont;
const int FONT_HEIGHT = 15;
const int FONT_WIDTH = 9;

const char* example = "ESP Box";
const char* example2 = "I'm inside fam";

void Draw()
{
    //Get current device context
    HDC currentHDC = wglGetCurrentDC();

    //Build if not already done or context has changed
    if (!glFont.bBuilt || currentHDC != glFont.hdc)
    {
        glFont.Build(FONT_HEIGHT);
    }

    //Do the thing - setup, draw, and restore
    GL::SetupOrtho();
    GL::DrawOutline(300, 300, 200, 200, 2.0f, rgb::red);
    float textPointX = glFont.centerText(300, 200, strlen(example) * FONT_WIDTH);
    float textPointY = 300 - FONT_HEIGHT / 2;
    glFont.Print(textPointX, textPointY, rgb::green, "%s", example);
    vec3 insideTextPoint = glFont.centerText(300, 300 + 100, 200, 200, strlen(example2) * FONT_WIDTH, FONT_HEIGHT);
    glFont.Print(insideTextPoint.x, insideTextPoint.y, rgb::green, "%s", example2);
    GL::RestoreGL();
}

//Hack moved into the hooked function
BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
    std::cout << "Hooked" << std::endl;

    //Start of hack in old while loop

    if (GetAsyncKeyState(VK_NUMPAD1) & 1)
    {
        bFast = !bFast;
    }

    //Continuous write/freeze
    uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10f4f4);

    if (localPlayerPtr)
    {
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
    }

    //End of old while loop hack

    Draw();

    return wglSwapBuffersGateway(hDc);
}

DWORD WINAPI HackThread(HMODULE hModule)
{
    //Create console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "OG for a fee, stay sippin' fam\n";

    //
    //Hook SwapBuffersHook((BYTE*)GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers"), (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);
    Hook SwapBuffersHook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);

    SwapBuffersHook.Enable();
    //Sleep(10000);
    //SwapBuffersHook.Disable();
    //

    //cleanup & eject
    //fclose(f);
    FreeConsole();
    //FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
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


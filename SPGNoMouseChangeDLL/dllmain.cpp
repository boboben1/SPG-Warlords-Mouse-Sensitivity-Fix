// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "PolyHook/PolyHook.hpp"
#include <memory>
#include <winuser.h>
#include <winuser.h>
#include <sstream>

#pragma comment(lib, "user32.lib")  


decltype(&SystemParametersInfoW) oSystemParametersInfoW = 0;

BOOL WINAPI hkSystemParametersInfoW(
	_In_    UINT  uiAction,
	_In_    UINT  uiParam,
	_Inout_ PVOID pvParam,
	_In_    UINT  fWinIni
)
{
	if (uiAction != 0x71 && uiAction != 0x20 && uiAction != 0x4)
		return oSystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);
	return TRUE;
}

wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

void DoHooks()
{
	PLH::Detour* systemParametersInfoWHook = new PLH::Detour();
	systemParametersInfoWHook->SetupHook((BYTE*)&SystemParametersInfoW, (BYTE*)&hkSystemParametersInfoW);
	if (!systemParametersInfoWHook->Hook())
		MessageBoxW(0, convertCharArrayToLPCWSTR(systemParametersInfoWHook->GetLastError().GetString().c_str()), L"ERROR", 0);

	oSystemParametersInfoW = systemParametersInfoWHook->GetOriginal<decltype(&SystemParametersInfoW)>();
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DoHooks, 0, 0, 0);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


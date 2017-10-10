// SPGNoMouseChange.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "loader.h"


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DWORD dwPid;
	std::string path = GetDirectoryFile("SPGNoMouseChangeDLL.dll");
	
	ShellExecute(0, 0, "steam://run/419480", 0, 0, 0);



	do
	{
		dwPid = GetProcessByName("GameLauncher.exe");

		Sleep(100);
	} while (!dwPid);

	InjectDLL(dwPid, path.c_str());

	do
	{
		dwPid = GetProcessByName("StarpointGeminiWarlords.exe");

		Sleep(10);
	} while (!dwPid && GetProcessByName("GameLauncher.exe"));
	
	if (dwPid)
		InjectDLL(dwPid, path.c_str());

    return 0;
}


#ifndef LOADER_H
#define LOADER_H

#include <windows.h>
#include <string>
#include <tlhelp32.h>

DWORD GetProcessByName( LPTSTR lpExeName )
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if ( hSnapShot == INVALID_HANDLE_VALUE )
	{
		#ifdef _DEBUG
			printf( "[!] Failed to call CreateToolhelp32Snapshot, GetLastError( ) = %d\n", GetLastError( ) );
		#endif
			
		return false;
	}

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof( PROCESSENTRY32 );

	for (BOOL	success = Process32First( hSnapShot, &pe );
				success == TRUE;
				success = Process32Next( hSnapShot, &pe ))
		{
			if ( strcmp( lpExeName, pe.szExeFile ) == 0 )
			{
				CloseHandle( hSnapShot );
				return pe.th32ProcessID;
			}
		}

	CloseHandle( hSnapShot );
	return NULL;
}


BOOL InjectDLL( DWORD dwPid, std::string dllFile )
{
	HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, true, dwPid );

	if ( !hProcess )
	{
		printf( "[!] Failed to call OpenProcess, GetLastError( ) = %d\n", GetLastError( ) );
		return FALSE;
	}

	LPVOID lpRemoteAddress = VirtualAllocEx( hProcess, NULL, dllFile.length( ), MEM_COMMIT, PAGE_READWRITE );
	
	if( !lpRemoteAddress )
	{
		printf( "[!] Failed to call VirtualAllocEx Handle: %I64X, GetLastError( ) = %d\n", reinterpret_cast<uintptr_t>( hProcess ), GetLastError( ) );
		return FALSE;
	}
			
	SIZE_T bytes;
		
	if( !WriteProcessMemory( hProcess, lpRemoteAddress, dllFile.c_str( ), dllFile.length( ), &bytes ) )
	{
		printf( "[!] Failed to call WriteProcessMemory( ), GetLastError( ) = %d\n", GetLastError( ) );	
		return FALSE;
	}

	FARPROC lpLoadLibrary = GetProcAddress( GetModuleHandle( "KERNEL32.DLL" ), "LoadLibraryA" );

	if ( !lpLoadLibrary )
	{
		printf( "[!] Failed to get address of LoadLibrary\n" );	
		return FALSE;
	}

	HANDLE hThread =
		CreateRemoteThread( hProcess, NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>( lpLoadLibrary ), lpRemoteAddress, NULL, NULL );

	if( !hThread )
	{
		printf( "[!] Failed to call CreateRemoteThread, GetLastError( ) = %d\n", GetLastError( ) );
		return FALSE;
	}

	WaitForSingleObject( hThread, INFINITE );

	if( !VirtualFreeEx( hProcess, lpRemoteAddress, 0, MEM_RELEASE ) )
	{
		printf( "[!] Failed to call VirtualFreeEx, GetLastError( ) = %d\n", GetLastError( ) );
		return FALSE;
	}

	CloseHandle( hThread );

	return TRUE;
};

std::string GetDirectoryFile( const std::string& file )
{
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
        
	return std::string( buffer ).substr( 0, pos ) + "\\" + file;
}

#endif
// dummy1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <winioctl.h>
#include <shlobj.h>
#include <stdlib.h>
#include <iostream>
#include <winternl.h>
#include <fileapi.h>
#include <winbase.h>
#include <namedpipeapi.h>
#include <securitybaseapi.h>
#include <cstdio>
#include <iostream>
#include <synchapi.h>
#include "..\filedisk-proxy\FileDiskShared.h"
#include <tchar.h>

WCHAR* ConvertCharToUnicode(char* input, int nChars)
{
	WCHAR* wcharstring; //LPCWSTR
	// required size
	int size = MultiByteToWideChar(CP_ACP, 0, input, -1, NULL, 0);
	// allocate it
	wcharstring = new WCHAR[nChars];
	MultiByteToWideChar(CP_ACP, 0, input, -1, (LPWSTR)wcharstring, size);
	// use it....
	return wcharstring;
}

int main()
{
    std::cout << "Hello World!\n";

	char shmlink[256];
	HANDLE shmHandle;
	HANDLE shmMappedBuffer;
	int devicenumber = 0;
	memset(shmlink, 0, 256);
	sprintf_s((char*)shmlink, sizeof(shmlink), "%s%u", USERMODEAPP_SHM_NAME, devicenumber);
	std::cout << "phase 0 completed = \r\n" << shmlink;
	std::cin.get();

	shmHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, (LPCSTR)&shmlink);
	if (!shmHandle || shmHandle == INVALID_HANDLE_VALUE)
	{
		printf("OpenFileMappingA(read) 1 fail! Error: %u\n", GetLastError());
		std::cin.get();
		return 0;
	}
	std::cin.get();

	shmMappedBuffer = (char*)MapViewOfFile(shmHandle, FILE_MAP_ALL_ACCESS, 0, 0, DEVICE_OBJECT_SHM_SIZE_BYTES);
	if (!shmMappedBuffer)
	{
		printf("MapViewOfFile(read) 2 fail! Error: %u\n", GetLastError());
		std::cin.get();
		return 0;
	}
	std::cin.get();

	printf("SHM working\r\n");
	UnmapViewOfFile(shmMappedBuffer);
	CloseHandle(shmHandle);

	HANDLE DriverRequestDataSet;
	HANDLE ProxyIdle;
	HANDLE RequestComplete;
	char link[256];
	memset(link, 0, 256);
	sprintf_s((char*)link, sizeof(link), "%s%u", USERMODEAPP_REQUESTDATAEVENT_NAME, devicenumber);
	WCHAR* wcharlink = ConvertCharToUnicode(link, 256);
	DriverRequestDataSet = OpenEvent(EVENT_ALL_ACCESS, FALSE, wcharlink);
	// delete it
	delete[] wcharlink;

	if (DriverRequestDataSet == NULL)
	{
		std::cout << "OpenEvent call failure\r\n";
		std::cin.get();
		return -1;
	}

	memset(link, 0, 256);
	sprintf_s((char*)link, sizeof(link), "%s%u", USERMODEAPP_PROXYIDLEEVENT_NAME, devicenumber);
	wcharlink = ConvertCharToUnicode(link, 256);
	ProxyIdle = OpenEvent(EVENT_ALL_ACCESS, FALSE, wcharlink);

	// delete it
	delete[] wcharlink;

	if (ProxyIdle == NULL)
	{
		std::cout << "OpenEvent call failure\r\n";
		return FALSE;
	}

	memset(link, 0, 256);
	sprintf_s((char*)link, sizeof(link), "%s%u", USERMODEAPP_REQUESTCOMPLETEEVENT_NAME, devicenumber);
	wcharlink = ConvertCharToUnicode(link, 256);
	RequestComplete = OpenEvent(EVENT_ALL_ACCESS, FALSE, wcharlink);

	// delete it
	delete[] wcharlink;

	if (RequestComplete == NULL)
	{
		std::cout << "OpenEvent call failure\r\n";
		return FALSE;
	}

	printf("Driver's Events working\r\n");
	std::cin.get();


}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

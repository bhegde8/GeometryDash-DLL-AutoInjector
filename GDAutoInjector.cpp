//  Original by mgostIH
// Auto injection by Pacnet

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>


HANDLE GetHandle(LPCSTR windowname)
{
	
	std::stringstream stream;
	stream << "\"C:\\Program Files (x86)\\Steam\\Steam.exe\""
		<< " " 
		<< "-applaunch 322170";
	system(stream.str().c_str());

	HWND hWnd = FindWindowA(0, windowname);

	std::cout << "Waiting for " << windowname << "..." << std::endl;
	while (!hWnd) {
		hWnd = FindWindowA(0, windowname);
		Sleep(2500);

	}
	system("CLS");
	DWORD pId;

	GetWindowThreadProcessId(hWnd, &pId);

	HANDLE hProc = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD, FALSE, pId);
	int hprocerror = GetLastError();
	if (!hProc) {
		std::cerr << "Cannot open process. Error No. " << hprocerror << " happened.\n Try running this program in administrator mode." << std::endl;
		std::cin.get();
		return 0;
	}
	else {
		return hProc;
	}
}

DWORD AllocString(HANDLE hwnd, const char* ToAlloc, size_t strsize) {
	PVOID addr = VirtualAllocEx(hwnd, NULL, strsize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	
	WriteProcessMemory(hwnd, addr, ToAlloc, strsize, NULL);
	return (DWORD)addr;

}

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

int main(int argc, char** argv) {
	SetConsoleTitleA("Geometry Dash Hack AutoInjector");
	DWORD LLa = (DWORD)LoadLibraryA;             //Get LoadLibraryA address
	std::string DllPath;

	
	char ownPth[MAX_PATH];

	HMODULE hModule = GetModuleHandle(NULL);
	GetModuleFileNameA(hModule, ownPth, (sizeof(ownPth)));
	

	std::string str = std::string(ownPth);
	replace(str, "GDAutoInjector.exe", "dll.txt");

	std::ifstream infile(str);
	std::string sLine;
	if (infile.good())
	{
		
		std::getline(infile, sLine);
		std::cout << sLine << std::endl;
	}

	infile.close();

	replace(str, "dll.txt", sLine);

	DllPath = str;
	HANDLE hwnd = GetHandle("Geometry Dash");

	

	Sleep(3000);
	char Payload[13] = { 0xB8, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xD0, 0xC3 };

	/*
	MOV EAX,00000000    ; LoadLibraryA Address here later
	PUSH 00000000       ; Allocated DLL path Address here later
	CALL EAX            ;
	RET                 ;
	*/

	*(DWORD*)(Payload + 1) = LLa;                                   //Modify the Payload by adding LoadLibraryA address
	*(DWORD*)(Payload + 6) = AllocString(hwnd, DllPath.c_str(), DllPath.size());  //Modify the Payload by adding the allocated string address

	DWORD PayloadAddr = AllocString(hwnd, Payload, sizeof(Payload));
	CreateRemoteThread(hwnd, 0, 0, (LPTHREAD_START_ROUTINE)PayloadAddr, 0, 0, 0);

	std::cout << "Dll Injected" << std::endl;
	Sleep(1000);
	exit(0);

}


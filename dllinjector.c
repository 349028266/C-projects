#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

// -------------------------DLL INJECTOR ------------------------

int main(void){
    //printf("\033[1;32m(+) Starting DLL Injector\033[0m\n");

    HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    //printf("\033[1;32m(+) Created process snapshot\033[0m\n");
    if (Snapshot == INVALID_HANDLE_VALUE) {
        //printf("\033[0;31m(-) Failed to create snapshot %lu\n\033[0m", GetLastError());
        return 1;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    //printf("\033[1;32m(+) Entry created with size %lu\n\033[0m", pe32.dwSize);

    if (!Process32First(Snapshot, &pe32)) {
        //printf("\033[0;31m(-) Failed to get first process %lu\n\033[0m", GetLastError());
        CloseHandle(Snapshot);
        return 1;
    }

    do {
        if (strcmp(pe32.szExeFile, "notepad.exe") == 0) {
            //printf("\033[1;32m (+) Found target process: %s\033[0m\n", pe32.szExeFile);


            //Opening target process (permissions, child inheritance, PID)
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

            if (hProcess == NULL) {
                //printf("\033[0;31m(-) Failed to open target process\n\033[0m");
                CloseHandle(Snapshot);
                return 1;
            }

            //printf("\033[1;32m(+) Opened target process\033[0m\n");

            //write path to allocated memory
            char path[] = "PATH TO DLL";

            //printf("\033[1;32m(+) Allocating memory in target process\033[0m\n");

            LPVOID memory = VirtualAllocEx(hProcess, NULL, sizeof(path)+1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            if (memory == NULL) {
                //printf("\033[0;31m(-) Failed to allocate memory in target process %lu\n\033[0m", GetLastError());
                CloseHandle(hProcess);
                return 1;
            }

            WriteProcessMemory(hProcess, memory, &path, sizeof(path)+1, NULL);

            //printf("\033[1;32m(+) Wrote DLL path to target process memory: %s\033[0m\n", path);
            //find the LoadlibraryA function to pass it as the threadproc() for CreateRemoteThread

            //printf("\033[1;32m(+) Getting address of LoadLibraryA\033[0m\n");
            HMODULE hModule = GetModuleHandleA("kernel32.dll");
            FARPROC pLoadLibraryA = GetProcAddress(hModule, "LoadLibraryA");
            //printf("\033[1;32m(+) Got address of LoadLibraryA: %p\033[0m\n", pLoadLibraryA);


            HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryA, memory, 0, NULL);
            if (hThread == NULL) {
                //printf("\033[0;31m(-) Failed to create remote thread %lu\n\033[0m", GetLastError());
                CloseHandle(hProcess);
                CloseHandle(Snapshot);
                return 1;
            }
            //printf("\033[1;32m(+) Created remote thread\033[0m\n");

            //printf("\033[1;32m(+) Setting remote thread priority to highest\n\033[0m\n");
            SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);

            //printf("\033[1;32m(+) Waiting for remote thread to finish\n\033[0m");
            WaitForSingleObject(hThread, INFINITE);

            VirtualFreeEx(hProcess, memory, 0, MEM_RELEASE);
            CloseHandle(hThread);
            CloseHandle(hProcess);
            CloseHandle(Snapshot);
            return 0;

        }
    } while (Process32Next(Snapshot, &pe32));

    CloseHandle(Snapshot);
    return 0;

}

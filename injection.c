#include <windows.h>
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


BOOLEAN WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {

        case DLL_PROCESS_ATTACH:

            WSADATA wsadata;
            int iResult;
            iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
 
            if (iResult != 0) {
                MessageBox(NULL, "WSAStartup failed!", "Error", MB_OK | MB_ICONERROR);
                return FALSE;
            }

            struct addrinfo *result = NULL,*ptr = NULL,hints;
            ZeroMemory(&hints, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            #define DEFAULT_PORT "443"

            iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
            if (iResult != 0) {
                MessageBox(NULL, "getaddrinfo failed!", "Error", MB_OK | MB_ICONERROR);
                WSACleanup();
                return FALSE;
            }

            SOCKET ConnectSocket = INVALID_SOCKET;
            ptr = result;
            ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
            if (ConnectSocket == INVALID_SOCKET) {
                MessageBox(NULL, "Error at socket()", "Error", MB_OK | MB_ICONERROR);
                WSACleanup();
                return FALSE;
            }
            iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                MessageBox(NULL, "Error at connect()", "Error", MB_OK | MB_ICONERROR);
                freeaddrinfo(result);
                closesocket(ConnectSocket);
                WSACleanup();
                return FALSE;
            }
            


            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
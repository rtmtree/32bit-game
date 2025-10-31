#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hInstance; (void)hPrevInstance; (void)lpCmdLine; (void)nCmdShow;
    MessageBoxA(NULL, "Hello from minimal Win32 EXE", "Minimal", MB_OK | MB_ICONINFORMATION);
    return 0;
}



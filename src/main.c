#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>

typedef struct BackBuffer {
    HDC     memoryDC;
    HBITMAP bitmap;
    HBITMAP oldBitmap;
    int     width;
    int     height;
} BackBuffer;

static BackBuffer g_backBuffer = {0};
static int g_running = 1;

static LARGE_INTEGER g_perfFreq;

static void
DestroyBackBuffer(HWND hwnd) {
    if (g_backBuffer.memoryDC) {
        if (g_backBuffer.oldBitmap) {
            SelectObject(g_backBuffer.memoryDC, g_backBuffer.oldBitmap);
            g_backBuffer.oldBitmap = NULL;
        }
        if (g_backBuffer.bitmap) {
            DeleteObject(g_backBuffer.bitmap);
            g_backBuffer.bitmap = NULL;
        }
        DeleteDC(g_backBuffer.memoryDC);
        g_backBuffer.memoryDC = NULL;
    }
    g_backBuffer.width = 0;
    g_backBuffer.height = 0;
}

static void
CreateBackBuffer(HWND hwnd, int width, int height) {
    DestroyBackBuffer(hwnd);
    HDC windowDC = GetDC(hwnd);
    g_backBuffer.memoryDC = CreateCompatibleDC(windowDC);
    g_backBuffer.bitmap = CreateCompatibleBitmap(windowDC, width, height);
    g_backBuffer.oldBitmap = (HBITMAP)SelectObject(g_backBuffer.memoryDC, g_backBuffer.bitmap);
    g_backBuffer.width = width;
    g_backBuffer.height = height;
    ReleaseDC(hwnd, windowDC);
}

static void
Present(HWND hwnd) {
    HDC windowDC = GetDC(hwnd);
    BitBlt(windowDC, 0, 0, g_backBuffer.width, g_backBuffer.height, g_backBuffer.memoryDC, 0, 0, SRCCOPY);
    ReleaseDC(hwnd, windowDC);
}

static LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY: {
            g_running = 0;
            PostQuitMessage(0);
        } break;
        case WM_SIZE: {
            RECT rc;
            GetClientRect(hwnd, &rc);
            int w = rc.right - rc.left;
            int h = rc.bottom - rc.top;
            if (w < 1) w = 1;
            if (h < 1) h = 1;
            CreateBackBuffer(hwnd, w, h);
        } break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance; (void)lpCmdLine;

    QueryPerformanceFrequency(&g_perfFreq);

    WNDCLASSA wc = {0};
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "Win32GameWindowClass";
    RegisterClassA(&wc);

    DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
    RECT desired = {0, 0, 800, 450};
    AdjustWindowRect(&desired, style, FALSE);

    HWND hwnd = CreateWindowExA(
        0,
        wc.lpszClassName,
        "Win32 Game",
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        desired.right - desired.left,
        desired.bottom - desired.top,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    RECT rc;
    GetClientRect(hwnd, &rc);
    CreateBackBuffer(hwnd, rc.right - rc.left, rc.bottom - rc.top);

    // Simple game state
    float playerX = 100.0f;
    float playerY = 100.0f;
    float playerW = 40.0f;
    float playerH = 40.0f;
    float speed = 200.0f; // pixels per second

    LARGE_INTEGER lastTime;
    QueryPerformanceCounter(&lastTime);

    while (g_running) {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_running = 0;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        double dt = (double)(now.QuadPart - lastTime.QuadPart) / (double)g_perfFreq.QuadPart;
        lastTime = now;

        SHORT left  = GetAsyncKeyState(VK_LEFT);
        SHORT right = GetAsyncKeyState(VK_RIGHT);
        SHORT up    = GetAsyncKeyState(VK_UP);
        SHORT down  = GetAsyncKeyState(VK_DOWN);

        if (left & 0x8000)  playerX -= (float)(speed * dt);
        if (right & 0x8000) playerX += (float)(speed * dt);
        if (up & 0x8000)    playerY -= (float)(speed * dt);
        if (down & 0x8000)  playerY += (float)(speed * dt);

        if (playerX < 0) playerX = 0;
        if (playerY < 0) playerY = 0;
        if (playerX + playerW > g_backBuffer.width)  playerX = (float)(g_backBuffer.width - playerW);
        if (playerY + playerH > g_backBuffer.height) playerY = (float)(g_backBuffer.height - playerH);

        HBRUSH bg = CreateSolidBrush(RGB(18, 18, 18));
        RECT full = {0, 0, g_backBuffer.width, g_backBuffer.height};
        FillRect(g_backBuffer.memoryDC, &full, bg);
        DeleteObject(bg);

        HBRUSH playerBrush = CreateSolidBrush(RGB(240, 240, 240));
        RECT pr = {(int)playerX, (int)playerY, (int)(playerX + playerW), (int)(playerY + playerH)};
        FillRect(g_backBuffer.memoryDC, &pr, playerBrush);
        DeleteObject(playerBrush);

        Present(hwnd);

        Sleep(1);
    }

    DestroyBackBuffer(hwnd);
    return 0;
}



// app.c
#ifndef UNICODE
#define UNICODE
#endif 
#include <windows.h>
#include <stdio.h>
#include "../include/server.h"
#include "../include/app.h"

// Global variable to store the button state
BOOL isButtonOn = FALSE;

// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SimpleWindowClass";
    wc.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wc);

    // Creating window
    HWND hwnd = CreateWindowEx(
        0,
        L"SimpleWindowClass",     // Class name registered
        L"Super Application",     // Title of window
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX, // Window style (without maximizing and resizing)
        CW_USEDEFAULT,            // Window X position
        CW_USEDEFAULT,            // Window Y position
        400,                      // Window width
        200,                      // Window height
        0,                        // Father's HWND (none in this case)
        0,                        // Menu handle (none in this case)
        hInstance,                // Application instance handle
        0                         // Creation parameter (none in this case)
    );

    // <BTN - ON>
    HWND buttonOn = CreateWindow(
        L"BUTTON",                // Control class
        L"Start server",          // Button text
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Button style
        100,                      // X position of the button
        50,                       // Button Y position
        100,                      // Button width
        30,                       // Button height
        hwnd,                     // Parent window handle
        (HMENU)1,                 // Button identifier
        hInstance,                // Application instance handle
        0                         // Creation parameter (none in this case)
    );

    // <BTN - OFF>
    HWND buttonOff = CreateWindow(
        L"BUTTON",                // Control class
        L"Stop",                  // Button text
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Button style
        210,                      // X position of the button
        50,                       // Button Y position
        100,                      // Button width
        30,                       // Button height
        hwnd,                     // Parent window handle
        (HMENU)2,                 // Button identifier
        hInstance,                // Application instance handle
        0                         // Creation parameter (none in this case)
    );

    // Show window
    ShowWindow(hwnd, nCmdShow);

    // Message loop
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    StopServer();

    return 0;
}

// Window procedure function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        // Check which button was pressed
        switch (LOWORD(wParam)) {
        case 1: // On button
            isButtonOn = TRUE;
            StopServer();  
            
            HANDLE serverThreadHandle = CreateThread(NULL, 0, ServerThread, NULL, 0, NULL);
            
            if (serverThreadHandle == NULL) {
                MessageBox(NULL, L"Error creating server thread", L"Error", MB_OK | MB_ICONERROR);
                return 1;
            }

            InvalidateRect(hwnd, NULL, TRUE); // Request window repainting
            break;
        case 2: // Off button
            isButtonOn = FALSE;
            InvalidateRect(hwnd, NULL, TRUE); // Request window repainting
            StopServer(); // Stop server
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Define the color of text based in the button state
        SetTextColor(hdc, isButtonOn ? RGB(0, 128, 0) : RGB(255, 0, 0));

        // Write running and stopped
        RECT textRect = { 150, 90, 250, 120 };
        wchar_t* text = isButtonOn ? L"Running" : L"Stopped";
        DrawText(hdc, text, wcslen(text), &textRect, DT_CENTER);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_SIZE: {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        // Adjust the position and the size of buttons
        MoveWindow(GetDlgItem(hwnd, 1), 100, 50, 100, 30, TRUE);   // On
        MoveWindow(GetDlgItem(hwnd, 2), 210, 50, 100, 30, TRUE);   // Off

        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#ifndef UNICODE
#define UNICODE
#endif 
#include <windows.h>
#include <stdio.h>
#include "../include/server.h"

// Variável global para armazenar o estado do botão
BOOL isButtonOn = FALSE;

// Protótipo da função de janela
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

DWORD WINAPI ServerThread(LPVOID lpParam);

// Função principal
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Registrando a classe da janela
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SimpleWindowClass";
    wc.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wc);

    // Criando a janela
    HWND hwnd = CreateWindowEx(
        0,
        L"SimpleWindowClass",     // Class name registered
        L"Super Application",     // Title of window
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX, // Window style (without maximizing and resizing)
        CW_USEDEFAULT,            // Posição X da janela
        CW_USEDEFAULT,            // Posição Y da janela
        400,                      // Largura da janela
        200,                      // Altura da janela
        0,                        // HWND do pai (nenhum neste caso)
        0,                        // Handle do menu (nenhum neste caso)
        hInstance,                // Handle da instância da aplicação
        0                         // Parâmetro de criação (nenhum neste caso)
    );

    // Criando o botão Ligar
    HWND buttonOn = CreateWindow(
        L"BUTTON",                // Classe do controle
        L"Ligar",                 // Texto do botão
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Estilo do botão
        100,                      // Posição X do botão
        50,                       // Posição Y do botão
        100,                      // Largura do botão
        30,                       // Altura do botão
        hwnd,                     // Handle da janela pai
        (HMENU)1,                 // Identificador do botão
        hInstance,                // Handle da instância da aplicação
        0                         // Parâmetro de criação (nenhum neste caso)
    );

    // Criando o botão Desligar
    HWND buttonOff = CreateWindow(
        L"BUTTON",                // Classe do controle
        L"Desligar",              // Texto do botão
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Estilo do botão
        210,                      // Posição X do botão
        50,                       // Posição Y do botão
        100,                      // Largura do botão
        30,                       // Altura do botão
        hwnd,                     // Handle da janela pai
        (HMENU)2,                 // Identificador do botão
        hInstance,                // Handle da instância da aplicação
        0                         // Parâmetro de criação (nenhum neste caso)
    );

    // Exibindo a janela
    ShowWindow(hwnd, nCmdShow);

    // Loop de mensagens
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    StopServer();

    return 0;
}

// Função de procedimento da janela
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        // Verificar qual botão foi pressionado
        switch (LOWORD(wParam)) {
        case 1: // Botão Ligar
            isButtonOn = TRUE;
            StopServer();  
            
            HANDLE serverThreadHandle = CreateThread(NULL, 0, ServerThread, NULL, 0, NULL);
            
            if (serverThreadHandle == NULL) {
                MessageBox(NULL, L"Error creating server thread", L"Error", MB_OK | MB_ICONERROR);
                return 1;
            }

            InvalidateRect(hwnd, NULL, TRUE); // Request window repainting
            break;
        case 2: // Botão Desligar
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
        MoveWindow(GetDlgItem(hwnd, 2), 210, 50, 100, 30, TRUE);  // Off

        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

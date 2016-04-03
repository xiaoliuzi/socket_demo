#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFLEN 1024
#define PORT 9102

#define MAX_LINE 100
#define PORT 8000

// page 72
ssize_t      /* Read "n" bytes from a descriptor. */
readn( int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) < 0) {
            if ( errno == EINTR)
                nread = 0;     /* and call read() again */
            else
                return (-1);
        
        } else if (nread == 0)
            break;            /* EOF */
        
        nleft -= nread;
        ptr += nread;
    }

    return (n - nleft);      /* return >= 0 */
}

//page 73
ssize_t                       /* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;


    ptr = vptr;
    nleft = n;
    while( nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;   /*adn call write() again */
            else
                return (-1);    /* error */
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return (n);
}








int main(int argc, char* argv[])
{
	SOCKET soc;
	SOCKADDR_IN serveraddr;
	SOCKADDR_IN clientaddr;
	unsigned char buf[BUFLEN];
	
	WSADATA wsa;
	WSAStartup(MAKEWORD(1,1), &wsa);
	
	if((soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0) {
		printf("Create socket fail!\n");
		return -1;
	}
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	
	printf("Try to connect...\n");
	if (connect(soc, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) != 0) {
		printf("connect fail!\n");
		return -1;
	}
	printf("Connected\n");
	while(1) {
		scanf("%s", buf);
		if (send(soc, buf, strlen(buf)+1, 0) < = 0) {
			printf("Send error!\n");
		}
	}
	
	WSACleanup();
	
	return 0;
}

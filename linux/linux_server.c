#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LINE 100
#define PORT 8000


int main(void)
{
	struct sockaddr_in sin;
	struct sockaddr_in cin;
	int l_fd;
	int c_fd;
	socklen_t len;
	char buf[MAX_LINE];
	char addr_p[INET_ADDRSTRLEN];
	int port = PORT;
	int n;

	int socket_fd[MAX_LINE];
	int count = 0;

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
        
	l_fd = socket(PF_INET, SOCK_STREAM, 0);
	bind(l_fd, (struct sockaddr*)&sin, sizeof(sin));
	listen(l_fd, 1024);
	printf("waiting ...\n");

	int flags = fcntl(c_fd, F_GETFL, 0);
    fcntl(c_fd, F_SETFL, flags | O_NONBLOCK);

	while(1) {
		c_fd = accept(l_fd, (struct sockaddr*)&cin, &len);
		printf("errno = %x\n", errno);
		printf("EAGAIN = %x\n", EAGAIN);
		printf("EWOULDBLOCK = %x\n", EWOULDBLOCK);
		
		if (c_fd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
			printf("no client connect\n");

			for ( int i=0; i< count+1&& i<MAX_LINE; i++) {
				socket_fd[i] = -2;
				socket_fd[i] = c_fd;
				count++;
				n=read(socket_fd[i], buf, MAX_LINE);
				n=write(socket_fd[i], buf, n);
				printf("write read n is :%d\n", n );	
			}	
		


		inet_ntop(AF_INET, &cin.sin_addr, addr_p, sizeof(addr_p));
		printf("client IP is %s, port is %d\n", addr_p, ntohs(sin.sin_port));
 		printf("content is :%s\n",buf );	
		
	} 
	
	if (close(l_fd) == -1) {
		printf("fail to close\n");
	}
	return 0;
} 

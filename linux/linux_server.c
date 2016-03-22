#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_LINE 100
#define PORT 8000


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
	struct sockaddr_in sin;
	struct sockaddr_in cin;
	int l_fd;
	int c_fd;
	socklen_t len;
	char buf[MAX_LINE];
	char addr_p[INET_ADDRSTRLEN];
	int port = PORT;
	int n;
	static int i;
	int socket_fd[MAX_LINE];
	int count = 0;

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);

	l_fd = socket(AF_INET, SOCK_STREAM, 0);
	bind(l_fd, (struct sockaddr*)&sin, sizeof(sin));
	listen(l_fd, 1024);
	printf("waiting ...\n");

	set_socket_blocking_enable(c_fd, false);
	c_fd = accept(l_fd, (struct sockaddr*)&cin, &len);
	printf("cfd is :%d\n", c_fd);
	printf("errno is :%d\n", errno);
	printf("EAGAIN is :%d\n", EAGAIN);
	printf("EWOULDBLOCK is :%d\n", EWOULDBLOCK);
	printf("EINTR is :%d\n", EINTR);
#if 0

	while(1) {
		set_socket_blocking_enable(c_fd, false);
		c_fd = accept(l_fd, (struct sockaddr*)&cin, &len);
	
		printf("c_fd is :%d\n", c_fd);

		if (c_fd == EINTR ) {
			printf("No.%d connected\n", count);
			if(count < 100)
				socket_fd[count++] = c_fd;
		}
		for (i=0; i< count+1 && i< 3; i++) {
			n=read(socket_fd[i], buf, MAX_LINE);
			printf("Server recv :%s\n", buf );	
			buf[strlen(buf)+1] = '\0';
			n=write(socket_fd[i], buf, strlen(buf)+1);
		}	
		
		inet_ntop(AF_INET, &cin.sin_addr, addr_p, sizeof(addr_p));
		printf("client IP is %s, port is %d\n", addr_p, ntohs(sin.sin_port));
		
	}
#endif 


	
	if (close(l_fd) == -1) {
		printf("fail to close\n");
	}
	return 0;
} 

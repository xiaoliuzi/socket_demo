#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <winsock2.h>


#define MAX_LINE 100
#define PORT 8000

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


int no_block_readn(int n)
{
	int result=0;
	if ( n < 0 ) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			printf("no content\n");
			result = 1;
		}
		else {
			for(int i=0; i<1000; i++) {
				for(int j=0; j<1000; j++) {
					;	
				}
			}	
		}
	}
	
	return result;

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
	int i;
	int socketfd[MAX_LINE];
	int count = 0;
	int len_content;

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);

	l_fd = socket(AF_INET, SOCK_STREAM, 0);
	bind(l_fd, (struct sockaddr*)&sin, sizeof(sin));
	listen(l_fd, 1024);
	printf("waiting ...\n");

	for(i=0; i<MAX_LINE; i++)
		set_socket_blocking_enable(socketfd[i], false);
	i = 0;
	count = 0;
	while(1) {
		set_socket_blocking_enable(l_fd, false);
		set_socket_blocking_enable(c_fd, false);
		c_fd = accept(l_fd, (struct sockaddr*)&cin, &len);
		if(c_fd > 0) {
			if(count < 10) {
				printf("No.%d connected\n", count);
				socketfd[count++] = c_fd;
			}
			else {
				printf("too much client beyond 10\n");
			}
		}
		else {
//if(c_fd == -1&& (errno == EAGAIN || errno == EWOULDBLOCK) ) {
			for (i=0; i< count && i< 100; i++) {
				set_socket_blocking_enable(socketfd[i], false);
//				printf("i is :%d\n", i);
//				printf("count is :%d\n", count);
//				printf("socket[%d] is :%d\n", i, socket_fd[i]);
//				fcntl(socket_fd[i], F_SETFL, flags|O_NONBLOCK);//prevent read() write() block.
				//n=readn(socket_fd[i], &len_content, sizeof(len_content));
				n=read(socketfd[i], buf, MAX_LINE);
				printf("socketfd[%d] is :%d\n", i, socketfd[i]);
//				if(	no_block_readn(n) == 1)
//					break;
				if (n < 0) {
					printf("readn length n is :%d\n", n);
					continue;
				}
				printf("Server recv :%s\n", buf );	
//
//				printf("read length is :%d\n", len_content);
			//	n=readn(socket_fd[i], buf, len_content);
			//	if (n < 0)
			//		printf("readn content n is :%d\n", n);
//					continue;
//				printf("read :%d characters \n", n);
				//printf("Server recv :%s\n", buf );	

				buf[strlen(buf)+1] = '\0';
			//	len_content = strlen(buf)+1;
				//n=writen(socket_fd[i], &len_content, sizeof(len_content));
				n=write(socketfd[i], buf, strlen(buf)+1);
				if (n < 0) {
					printf(" writen length n is :%d\n", n);
					continue;
				}
		//		n=write(socket_fd[i], buf, len_content);
		//		if (n < 0)
		//			printf(" writen content n is :%d\n", n);
//					continue;
//				printf("write :%d characters \n", n);
			}	
		}
		
		
	}
//#endif 
	
	if (close(l_fd) == -1) {
		printf("fail to close\n");
	}
	return 0;
} 

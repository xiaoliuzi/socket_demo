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


ssize_t my_read(int fd, void *buffer, size_t length)
{
	ssize_t done = length;
	char buf[MAX_LINE];
	while (done > 0) {
		done = read(fd, buf, length);
		
		if (done != length) {
			if (errno == EINTR) {
				done = length;

			}
			else {
				perror("fail to read11111111");
				return -1;
			}
		}	
		else
			break;

	}
	return done;
}

ssize_t my_write(int fd, void *buffer, size_t length)
{
	ssize_t done = length;
	char buf[MAX_LINE];
	while(done > 0){
		done = write(fd, buf , length);
		if (done != length){
			if (errno == EINTR)
				done = length;
			else {
				perror("fail to write");
				return -1;
			}
		}
		else
			break;
	}
	return done;
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

	int socket_fd[MAX_LINE];
	int count = 0;

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
        
	if( (l_fd = socket(AF_INET, SOCK_STREAM, 0))== -1 ) {
		printf("fail to create socket\n");
	}
	if (bind(l_fd, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		printf("fail to bind\n");
	} 
	if ( listen(l_fd, 1024) ) {
		printf("fail to listen\n");
	}
	printf("waiting ...\n");

	int flags = fcntl(c_fd, F_GETFL, 0);
    fcntl(c_fd, F_SETFL, flags | O_NONBLOCK);

	while(1) {
			
		c_fd = accept(l_fd, (struct sockaddr*)&cin, &len);
		if (c_fd  == -1 && errno == EAGAIN) {
			printf("No clinet connections yes\n");
			continue;
		}
		if (c_fd  == -1) {
			printf("fail to accept\n");
		}
		else {
			for ( int i=0; i< count+1&& i<MAX_LINE; i ++) {
				socket_fd[i] = -2;
				socket_fd[i] = c_fd;
				count ++;
				//n=my_read(socket_fd[i], buf, MAX_LINE);
				n=read(socket_fd[i], buf, MAX_LINE);
				if (n == -1) {
					printf("fail to read222\n");
				}
				else if (n > 0) {
					if ( (n=write(c_fd, buf, n)) == -1) {
						printf("From server to client send error!\n");
					}
				}
			}
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

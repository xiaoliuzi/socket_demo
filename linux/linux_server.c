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

// page 72
      /* Read "n" bytes from a descriptor. */

// If not read n length characters , readn() will be blocked.
ssize_t readn( int fd, void *vptr, size_t n)
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
                      /* Write "n" bytes to a descriptor. */
ssize_t writen(int fd, const void *vptr, size_t n)
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
	static int i;
	int socket_fd[MAX_LINE];
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

	set_socket_blocking_enable(l_fd, false);
	int flags = fcntl(l_fd, F_GETFL, 0);
	fcntl(l_fd, F_SETFL, flags|O_NONBLOCK);

//	c_fd = accept(l_fd, (struct sockaddr*)&cin, &len);
#if 0
	printf("cfd is :%d\n", c_fd);
	printf("errno is :%d\n", errno);
	printf("EAGAIN is :%d\n", EAGAIN);
	printf("EWOULDBLOCK is :%d\n", EWOULDBLOCK);
	printf("EINTR is :%d\n", EINTR);
#endif
#if 0
	printf("cfd is :%d\n", c_fd);
	printf("errno is :%d\n", errno);
	printf("EAGAIN is :%d\n", EAGAIN);
	printf("EWOULDBLOCK is :%d\n", EWOULDBLOCK);
	printf("EINTR is :%d\n", EINTR);
#endif
int liu_c = 0;
//	while(1) {
{
		set_socket_blocking_enable(l_fd, true);
		//set_socket_blocking_enable(l_fd, false);
		//set_socket_blocking_enable(c_fd, false);
		set_socket_blocking_enable(c_fd, true);
		c_fd = accept(l_fd, (struct sockaddr*)&cin, &len);
//		printf("cfd is :%d\n", c_fd);
		if(c_fd != -1) {
			if(count < 10) {
				printf("No.%d connected\n", count);
				socket_fd[count++] = c_fd;
			}
		}
//		else if(c_fd == -1&& (errno == EAGAIN || errno == EWOULDBLOCK) ) {
{			for (i=0; i< count && i< 100; i++) {
				printf("i is :%d\n", i);
				printf("count is :%d\n", count);
//				printf("socket[%d] is :%d\n", i, socket_fd[i]);
//				fcntl(socket_fd[i], F_SETFL, flags|O_NONBLOCK);//prevent read() write() block.
				n=readn(socket_fd[i], &len_content, sizeof(len_content));
//				if(	no_block_readn(n) == 1)
//					break;
				if (n < 0)
					//continue;
					printf("readn length n is :%d\n", n);
//
//				printf("read length is :%d\n", len_content);
				n=readn(socket_fd[i], buf, len_content);
				if (n < 0)
					printf("readn content n is :%d\n", n);
//					continue;
//				printf("read :%d characters \n", n);
				printf("Server recv :%s\n", buf );	

				buf[strlen(buf)+1] = '\0';
				len_content = strlen(buf)+1;
				n=writen(socket_fd[i], &len_content, sizeof(len_content));
				if (n < 0)
					printf(" writen length n is :%d\n", n);
		//			continue;
				n=write(socket_fd[i], buf, len_content);
				if (n < 0)
					printf(" writen content n is :%d\n", n);
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

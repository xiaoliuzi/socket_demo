#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

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

	char ch;
	char str[MAX_LINE] = {"\0"};
	int i = 0;

	struct sockaddr_in sin;
	char buf[MAX_LINE] = {"\0"};
	int s_fd;
	int port = PORT;
	int n;
	int j = 0;
	int data_len;


	if (argc < 1) {
		printf("Not enough arguments\n");
	}
	
//	port = atoi(argv[2]);
	port = 8000;
	printf("port is :%d\n", port);
//	printf("ip is :%s\n", argv[1]);
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	//inet_pton(AF_INET, argv[1], &sin.sin_addr);
	inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);
	sin.sin_port = htons(port);
	
	if ( (s_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
		printf("fail to create socket\n");
	}	
	if ( connect(s_fd, (struct sockaddr_in *)&sin, sizeof(sin)) == -1) {
		printf("fail to connect\n");
	}
	printf("Input your string:\n");

	while( (ch=getchar()) != EOF ) {
		if (i<MAX_LINE && ch != '\n' ) {
			str[i++] = ch;
			continue;
		}
		str[i] = '\n';
		str[i+1] = '\0';

		data_len = strlen(str)+1;
		
		n = writen(s_fd, &data_len, sizeof(data_len));
		n = writen(s_fd, str, strlen(str)+1);

		n = readn(s_fd, &data_len, sizeof(data_len));
		n=readn(s_fd, buf, strlen(str)+1);
		printf("%s", buf);

		i = 0;	
	}

	if( close(s_fd) == -1 ) {
		printf("fail to close s_fd\n");
	}

	return 0;
}

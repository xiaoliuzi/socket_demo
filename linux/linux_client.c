#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>


#define MAX_LINE 100
#define PORT 8000


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
	if (argc < 1) {
		printf("Not enough arguments\n");
	}
	
	port = atoi(argv[2]);
	printf("port is :%d\n", port);
	printf("ip is :%s\n", argv[1]);
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &sin.sin_addr);
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
		n = write(s_fd, str, strlen(str)+1);
		n=read(s_fd, buf, strlen(str)+1);
		printf("%s", buf);

		i = 0;	
	}

	if( close(s_fd) == -1 ) {
		printf("fail to close s_fd\n");
	}

	return 0;
}

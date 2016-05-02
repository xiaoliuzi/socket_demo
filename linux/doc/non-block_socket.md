accept 非阻塞

C网络编程：Server处理多个Client(多进程并发Server，non-blocking与select结合)

	参看基于TCP/UDP的socket代码，同一时间Server只能处理一个Client请求：
		在使用当前连接的socket和client进行交互的时候，不能够accept新的连接请求。为了使Server能够处理多个Client请求，常见的方法：

		多进程方法(每个子进程单独处理一个Client连接)
	在每个accept成功之后，使用fork创建一个子进程专门处理该client的connection，父进程(server)本身可以继续accept其他新的client的连接请求。

	具体代码如下：

```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define DEFAULT_PORT 1984  //默认端口
#define BUFFER_SIZE 1024   // buffer size

void sigCatcher(int n) {
	// printf("a child process dies\n");
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int clientProcess(int new_sock);

int main(int argc, char *argv[])
{
	unsigned short int port;

	// get port, use default if not set
	if (argc == 2) {
		port = atoi(argv[1]);
	} else if (argc < 2) {
		port = DEFAULT_PORT;
	} else {
		fprintf(stderr, "USAGE: %s [port]\n", argv[0]);
		return 1;
	}

	// create socket
	int sock;
	if ((sock=socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket failed, ");
		return 1;
	}
	printf("socket done \n");

	// create socket address and initialize
	struct sockaddr_in bind_addr;
	memset(&bind_adr, 0, sizeof(bind_addr));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.s_addr = htonl(INADDR_ANY); //设置接受任意地址发来的请求
	bind_addr.sin_port = htons(port);			// 将host byte order转换为network byte order(big-end)

	//bind (bind socket to the create socket address)
	if ( bind(sock, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) == -1 ) {
		perror("bind failed, ");
		return 1;
	}
	printf("bind done \n");

	// listen
	if ( listen(sock, 5) == -1) {
		perror("listen failed.");
		return 1;
	}
	printf("listen done\n");

	// handler to clear zombie process
	signal(SIGCHLD, sigCatcher);

	//loop and respond to client
	int new_sock;
	int pid;
	while(1) {
		// wait for a connection, then accept it
		if( (new_sock = accept(sock, NULL, NULL)) == -1 ) {
			perror("accept failed.\n");
			return 1;
		}
		printf("accept done\n");

		pid = for();
		if (pid < 0) {
			perror("fork failed\n");
			return 1;
		} else if (pid == 0) {
			// This is child process
			close(sock); // 子进程中不需要server中的监听套接字
			clientProcess(new_sock);	//	使用新的new_sock和client进行交互
			close(new_sock);		//关闭client的连接
			exit(EXIT_SUCCESS);		//子进程退出
		} else {
			// This is parent process
			close(new_sock);	//	由于new_sock已经交给子进程处理，这里可以关闭了。
		}
	}
	
	return 0;
}


int clientProcess(int new_sock)
{
	int recv_size;
	char buffer[BUFFER_SIZE];

	memset(buffer, 0, BUFFER_SIZE);
	if ( (recv_size = recv(new_sock, buffer, sizeof(buffer), 0)) == -1 ) {
		perror("recv failed\n");
		return 1;
	}
	printf("%s\n", buffer);

	char *response = "This is the response";
	if ( send(new_sock, response, strlen(response)+1, 0) == -1) {
		perror("send failed");
		return 1;
	}

	return 0;
}
```



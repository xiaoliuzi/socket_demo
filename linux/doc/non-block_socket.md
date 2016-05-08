accept 非阻塞

C网络编程：Server处理多个Client(多进程并发Server，non-blocking与select结合)

	一般的，常见的基于TCP/UDP的socket代码，同一时间Server只能处理一个Client请求：
		在使用当前连接的socket和client进行交互的时候，不能够accept新的连接请求。为了使Server能够处理多个Client请求，常见的方法：

#### 多进程方法(每个子进程单独处理一个Client连接)
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

其中signal(SIGCHLD, sigCatcher)代码为了处理僵尸进程(zombie process)的问题：

	当server进程运行时间较长，且产生越来越多的子进程，当这些子进程运行结束都会成为僵尸进程, 占据系统的process table。
	解决方法：在父进程(server process)中显式地处理子进程结束之后发出的SIGCHLD信号: 调用wait/waitpid 清理子进程的zombie信息。
	测试：运行server程序，然后同时运行2个client(telnet localhost 1984)， 可看到该server能够很好地处理两个client.
	多进程方法的优点：
	每个独立进程处理一个独立的client，对server进程来说，只需要accept新的连接，对每个子进程来说，只需要处理自己的client即可。
	多进程方法的缺点：
	子进程的创建需要独立的父进程资源副本，开销较大，对高并发的请求不太适合;(假设，一个进程需要2M的内存空间资源，那么如果有1024个进程，那么需要占用2G的内存空间)
	并且，一个进程仅处理一个client不能有效发挥作用。另外，有些情况下，还需要进程间进行通信以协调各进度要完成的任务。


#### 使用Select实现non-blocking socket(single process soncurrent server)
	blocking socket VS non-blocking socket
	默认情况下，socket是blocking的，即函数accept(),recv/recvfrom,send/sendto,connect等，需要等待函数执行结束之后才能够返回(此时，操作系统切换到其他进程执行)。accept()需要等待到有client连接请求并接受成功之后才能返回，recv/recvfrom需要读取完client发送的数据之后才能够返回。

可设置socket为non-blocking模式，即调用函数立即返回，而不是必须等待满足一定条件才返回。参见http://www.scottklement.com/rpg/socktut/nonblocking.html

	non-blocking: by default, socket are blocking - this means that they stop the function from returning until all data has been transfered.
	With multiple connection which may or may not be transmitting data to a server, this would not be very good as connections may have to wait to transmit their data.

	设置socket为非阻塞模式non-blocking
	使用socket()创建的socket(file descriptor), 默认是阻塞的(blocking); 使用fcntl()(file control)可设置创建的socket为非阻塞的non-blocking.

```C
#include <unistd.h>
#include <fcntl.h>

sock = socket(PF_INET, SOCK_STREAM, 0);

int flags = fcntl(sock, F_GETFL, 0);
fcntl(sock, F_SETFL, flags | O_NONBLOCK);

```

这样，使用原本的blocking的各种函数，可以立即获得返回结果。通过判断返回的error了解状态:

> * accept():
	
在non-blocking模式下，如果返回值为-1,且error == EAGAIN或error == EWOULDBLOCK表示no new connections request, 表示没有新的连接请求;
> * recv()/recvfrom():
在non-blocking模式下，如果返回值为-1,且error == EAGAIN表示没有可接收的数据或还在接收数据，尚未完成数据接收。
> * send()/sendto():
在non-blocking模式下，如果返回值为-1, 且error == EAGAIN或者error == EWOULDBLOCK表示，没有可发送的数据，或者数据发送正在进行，尚未完成数据的发送。
> * read/write():
在non-blocking模式下，如果返回值为-1, 且error == EAGAIN表示没有可读写的数据，或者数据正在进行可读写操作，尚未完成。
> * connect():
在non-blocking模式下，如果返回-1, 且error == EINPROGRESS表示正在连接。
使用如上方法，可以创建一个non-blocking的server的程序，类似如下代码:

```C
int main(int argc, char *argv[])
{
	int sock;
	if ( (sock = socket(PF_INET, SOCK_STREAM, 0)) == -1 ) {
		perror("socket failed");
		return 1;

	}

	//set socket to be non-blocking
	int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

	// create socket address to bind
	struct sockaddr_in bind_addr
	...

	// bind
	bing(...);
	...

	
	// listen
	listen(...);
	...
	
	//loop
	int new_sock;

	while(1) {
		new_sock = accept(sock, NULL, NULL);
		if (new_sock == -1 && error == EAGAIN) {
			fprintf(stderr, "no client connections yet\n");
			continue;
		} else if (new_sock == -1) {
			perror("accept failed");
			return 1;
		}

		// read and write
		...


	} // end of while(1)

	...
}
```
纯non-blocking程序缺点:
	如果运行如上程序会发现调用accept可以理解返回，但这样会耗费大量的cpu time，实际中并不会这样使用。实际中将non-blocking和select结合使用。
	
#### non-blocking和select结合使用
select通过轮询, 

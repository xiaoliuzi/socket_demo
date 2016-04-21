套接字接口(socket interface)是一组函数，也是操作系统提供给应用程序的接口。在Unix系统中，套接字和Unix I/O函数结合起来，用来创建网络应用程序。这里最典型的就是客户端--服务器模型。


因特网客户端和服务器通过在**连接**上发送的接收字节流来通信。从连接一对进程的意义上而言，连接是**点对点**的。从数据可以同时双向流动的角度来说，它是**全双工**的。并且从(除了一些灾难性的引起的失败以外，如:农民伯伯切断了电缆。)由源进程发出的字节流最终被目的进程以它发出的顺序收到它的角度来说，它也是**可靠的**。

一个**套接字**是连接的一个端点。每个套接字都有相应的**套接字地址**，是由一个因特网地址(IP地址)和一个16位的整数**端口**组成的，用"地址:端口"来表示。当客户端发起一个连接请求时，客户端套接字地址中的端口是由内核自动分配的，称为**临时端口**(ephemeral port)。然而，服务器套接字地址中的端口通常是和某个**知名**的端口，是和这个服务相对应的。例如，Web服务器通常使用端口80,而电子邮件服务器使用端口25.在Unix机器上，文件/etc/services包含一张这台机器提供的服务以及它们的知名端口号的综合列表。

一个连接是由它两端的套接字地址唯一确定的。这对套接字地址叫做**套接字对**(socket pair)，由下列元组来表示：

	(cliaddr:cliport, servaddr:servport)

其中cliaddr是客户端的IP地址，cliport是客户端的端口，servaddr是服务器的IP地址，而servport是服务器的端口。例如，有如下连接套接字对：

	(128.2.194.242:51213, 208.216.181.15:80)

在这个示例中，Web客户端的套接字地址是：

	128.2.194.242:51213

其中,端口号51213是内核分配的临时的端口号。

Web服务器的套接字地址是：

	208.216.181.15:80

其中，端口号80是和Web服务器的通用的端口号。给定了客户端和服务器套接字的地址和端口号。客户端和服务器之间的连接就由下列套接字对唯一确定了:
	
	(128.2.194.242:51213, 208.216.181.15:80)


套接字的地址结构
从Unix内核的角度来看，一个套接字就是通信的一个端点。从Unix程序的角度来看，套接字就是一个有相应描述符的打开文件。
因特网的套接字地址存放在如下结构中，如下图所示的类型为sockaddr_in的16字节结构中。对于因特网应用，sin_family成员是AF_INET,sin_port成员是一个16位的端口号，而sin_addr成员就是一个32位的IP地址。IP地址和端口号总是以网络字节顺序(大端法)存放的。

------------------------sockaddr:socketbits.h (included by socket.h), sockaddr_in:netinet/in.h

/* Generic socket address structure (for connect, bind, adn accept) */
struct sockaddr {
	unsigned short sa_family;	/* Protocol family */
	char		   sa_data[14]; /* Address data */
};

/* Internet-style socket address structure */
struct sockaddr_in {
	unsigned short sin_family;	/* Address family (always AF_INET) */
	unsigned short sin_port;	/* Port number in network byte order */
	struct in_addr sin_addr;	/* IP address in network byte order */
	unsigned char sin_zero[8];	/* Pad to sizeof(struct sockaddr) */
};

--------------------sockaddr:socketbits.h(included by socket.h), sockaddr_in:netinet/in.h


##### _in后缀意味着什么？
_in后缀是互联网(internet)的缩写，而不是输入(input)的缩写。
connect、bing和accept函数要求一个指向与协议相关的套接字地址结构的指针。套接字接口的设计者面临的问题是，如何定义这些函数，使之能接受各种类型的套接字地址结构。现在，我们可以使用通用的void*指针，那时在C中并不存在这种类型的指针。解决办法是定义套接字函数要求一个指向通用sockaddr结构的指针，然后要求应用程序将与协议特定的结构的指针强制转换成这个通用结构。为了简化代码示例，我们跟随
Stevens的指导，定义下面的类型：
	typedef struct sockaddr SA;
然后无论何时需要将sockaddr_in结构强制转换成通用sockaddr结构，我们都使用这个类型。


accept函数
服务器通过调用accept函数来等待来自客户端的连接请求：
#include <sys/socket.h>
int accept(int listenfd, struct sockaddr * addr, int *addrlen);
					返回：若成功则为非负连接描述符，若出错则为-1。

accpet函数等待来自客户端的连接请求到达侦听描述符listenfd，然后在addr中填写客户端的套接字地址，并返回一个**已连接描述符**(connected descriptor),这个描述符可被用来利用Unix I/O函数与客户端通信。

监听描述符和已连接描述符之间的区别使很多人感到迷惑。

监听描述符是作为客户端链接请求的一个端点。典型地，它被创建一次，并存在于服务器的整个生命周期。

已连接描述符是客户端和服务器之间已经建立起来了的连接的一个端点。服务器每次接受连接请求时都会创建一次，它只存在于服务器为一个客户端服务的过程中。

在在csapp第2版page630中，图11-18描绘了监听描述符和已连接描述符的角色。
第一步。服务器调用accept，等待连接请求到达监听描述符，具体的我们设定为描述符3.(回忆一下，描述符0～2是预留给了标准文件的)
		1.服务器阻塞在accpet,等待监听描述符listenfd上的连接请求。
第二步。客户端调用connect函数，发送一个连接请求到listenfd。
		2.客户端通过调用和阻塞在connect,创建连接请求。
第三步。accept函数打开了一个新的已连接描述符connfd（我们假设是描述符4),在clientfd和connfd之间建立连接，并且随后返回connfd给应用程序。客户端也从connect返回，在这一点以后，客户端和服务器就可以分别通过读和写clientfd和connfd来回传送数据了。
		3.服务器从accept返回connfd。客户端从connect返回。现在在clientfd和connfd之间已经建立起了链接。

上面说的这样的服务器，即是简单的echo服务器，一次只能处理一个客户端。这种类型的服务器一次一个地在客户间迭代，成为迭代服务器(iterative server)

而，同时可以处理多个客户端的socke请求的服务器，我们称之为并发服务器(soncurrent server).


为何要有监听描述符和已连接描述符之间的区别？
因为，区分这两者是很有用的，因为它使得我们可以建立并发服务器，它能够同时处理多个客户端连接。例如，每次一个连接请求到达监听描述符时，我们可以派生(fork)一个新的进程，它通过已连接描述符与客户端通信。



----------------------------
参考资料:
可能部分内容有修改。

《深入理解计算机系统》 Page 625, 623, 629。

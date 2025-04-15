#include "../HttpServer/server.h"
#include <arpa/inet.h>
#include <stdio.h>

int initListenFD(unsigned short port)
{
	//1、创建用于监听的套接字
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd == -1) {
		perror("socket");
		return -1;
	}
	//2、设置端口复用（选）
	int opt = 1;
	int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (ret == -1) {
		perror("setsocket");
		return -1;
	}
	//3、绑定端口
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);
	if (ret == -1) {
		perror("bind");
		return -1;
	}
	//4、设置监听
	ret = listen(lfd, 128);
	if (ret == -1) {
		perror("listen");
		return -1;
	}
	//5、返回fd
	return lfd;
}

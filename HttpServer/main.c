#include "server.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	if (argc < 3) {
		printf("input error!!! ./a.out port path\n");
		return -1;
	}
	unsigned short port = atoi(argv[1]);	//获取端口哦
	chdir(argv[2]);							//切换服务器工作路径

	//启动服务器程序
	int lfd = initListenFD(port);
	epollRun(lfd);
	
	return 0;
}
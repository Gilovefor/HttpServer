#include "client.h"
#include <stdio.h>
#include <sys/epoll.h>
#include <fcntl.h>;

int acceptClient(int lfd, int efpd)
{
	//1����������
	int cfd = accept(lfd, NULL, NULL);
	if (cfd == -1) {
		perror("accept");
		return -1;
	}

	//2�����÷�����
	int flag = fcntl(lfd, F_GETFL);
	if (flag == -1) {
		perror("fcntl");
		close(cfd);
		return -1;
	}
	flag |= O_NONBLOCK;
	if (fcntl(cfd, F_SETFL, flag) == -1) {
		perror("fcntl");
		close(cfd);
		return -1;
	}

	//3����cfd��ӵ�epoll����
	struct epoll_event ev;
	ev.data.fd = cfd;
	ev.events = EPOLLIN | EPOLLET;		//����Ϊ����ģʽ��Ĭ��ˮƽģʽ��	
										//epollЧ�����ģʽ�����ط�����ģʽ
	int ret = epoll_ctl(efpd, EPOLL_CTL_ADD, cfd, &ev);
	if (ret == -1) {
		perror("epoll_ctl");
		close(cfd);
		return -1;
	}
	return 0;
}

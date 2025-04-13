#include "../HttpServer/server.h"
#include <sys/epoll.h>
#include "../HttpServer/client.h"
#include "../HttpServer/recvHttpRequest.h"
#include <stdio.h>

int epollRun(int lfd) {
	//1������epollʵ��������epoll������ĸ��ڵ㣩
	int epfd = epoll_create(1);
	if (epfd == -1) {
		perror("epoll_create");
		return -1;
	}
	//2��lfd��ӵ�epoll����
	struct epoll_event ev;
	ev.data.fd = lfd;
	ev.events = EPOLLIN;
	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	if (ret == -1) {
		perror("epoll_ctl");
		return -1;
	}
	//3���������
	struct epoll_event evs[1024];
	int size = sizeof(evs) / sizeof(struct epoll_event);

	while (1) {
		int num = epoll_wait(epfd, evs, size, -1);
		for (int i = 0; i < num; i++) {
			if (num == -1) {
				perror("epoll_wait");
				return -1;
			}
			int fd = evs[i].data.fd;
			if (fd == lfd) {
				//����Ǽ������ļ�������������������
				acceptClient(lfd, epfd);

			}
			else {
				//����Ϊͨ�ŵ��ļ���������������ܶԶ˵����ݣ�http��ʽ��
				recvHttpRequest(fd, epfd);
			}
		}
	}
	return 0;
}















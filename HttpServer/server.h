#pragma once

//��ʼ���ļ�������
int initListenFD(unsigned short port);

//����epoll
int epollRun(int lfd);


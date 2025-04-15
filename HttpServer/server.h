#pragma once

//初始化文件描述符
int initListenFD(unsigned short port);

//启动epoll
int epollRun(int lfd);


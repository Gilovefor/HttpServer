#pragma once

/*
 * HTTP GET �����ʽ��
 * --------------------------
 * [������] ��ʽ��GET /path?query HTTP/1.1\r\n
 * [����ͷ] ÿ�и�ʽ��HeaderName: Value\r\n
 * ͷ�������һ������ \r\n
 *
 * ʾ����
 * GET /search?q=hello HTTP/1.1\r\n
 * Host: example.com\r\n
 * User-Agent: curl/7.68.0\r\n
 * \r\n
 */


int parseRequestLine(const char* line, int cfd);	//����������

int recvHttpRequest(int cfd, int epfd);				//������������Ϣ
	


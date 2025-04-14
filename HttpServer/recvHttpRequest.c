#include "../HttpServer/recvHttpRequest.h"
#include "../HttpServer/send.h"
#include "../HttpServer/filetype.h"
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
	
int parseRequestLine(const char* line, int cfd)
{
	char method[12];
	char path[1024];

	printf("Request line: %s\n", line);

	sscanf(line, "%[^ ] %[^ ]", method, path);

	//���pathĩβ���ܴ��ڵ� \r ���� \n 
	char* p = strchr(path, '\r');
	if (p) 
		*p = '\0';
	p = strchr(path, '\n');
	if (p)
		*p = '\0';

	printf("Original path: %s\n", path);

	// ��������ӽ����߼�
	char decoded_path[1024];
	decodeMsg(decoded_path, path);  // ���� URL �����·��
	printf("Decoded path: %s\n", decoded_path);

	if (strcasecmp(method, "get") != 0) {
		return -1;	
	}

	//����ͻ�������ľ�̬��Դ��Ŀ¼/�ļ���
	const char* file = NULL;
	if (strcmp(decoded_path, "/") == 0) {
		file = "./";
	}
	else {
		file = decoded_path + 1;	
	}

	printf("Parsed file path: %s\n", file);

	//��ȡ�ļ�����
	struct stat st;
	int ret = stat(file, &st);
	if (ret == -1) {
		// �ļ������ڣ����� 404 ҳ��
		struct stat st404;
		if (stat("404.html", &st404) == -1) {
			// �� 404.html ��û�У�ֱ�ӷ��ͼ���Ϣ
			const char* notFoundMsg = "<h1>404 Not Found</h1>";
			sendHeadMsg(cfd, 404, "NOT FOUND", "text/html", strlen(notFoundMsg));
			send(cfd, notFoundMsg, strlen(notFoundMsg), 0);
		}
		else {
			sendHeadMsg(cfd, 404, "NOT FOUND", getFileType(".html"), st404.st_size);
			sendFile("404.html", cfd);
		}
		return 0;
	}

	// �ж��Ƿ���Ŀ¼
	if (S_ISDIR(st.st_mode)) {
		// ����Ŀ¼����
		sendDir(file, cfd);  
	}
	else {
		// ��ͨ�ļ����ȷ���ͷ�����ٷ����ļ�����
		sendHeadMsg(cfd, 200, "OK", getFileType(file), st.st_size);
		sendFile(file, cfd);
	}

	return 0;
}

int recvHttpRequest(int cfd, int epfd)
{
	printf("start to receive data......\n");
	int len = 0, total = 0;
	char tmp[1024] = { 0 };
	char buf[4096] = { 0 };
	while ((len = recv(cfd, tmp, sizeof tmp, 0)) > 0) {
		if (total + len >= sizeof(buf) - 1) {
			printf("Request too long, beyond buffer......\n");
			break;
		}

		memcpy(buf + total, tmp, len);
		total += len;
		buf[total] = '\0';

		// ����Ƿ���������� HTTP ����ͷ���� \r\n\r\n ��β��
		if (strstr(buf, "\r\n\r\n")) {
			break;
		}
	}

	if (len < 0) {
		perror("recv error");
	}
	else if (len == 0) {
		printf("client closed connection.\n");
	}
	else {
		// �� buf ����ȡ����һ�У����� parseRequestLine
		char* lineEnd = strstr(buf, "\r\n");
		if (lineEnd) {
			*lineEnd = '\0';  // �ضϵ�һ��
			parseRequestLine(buf, cfd);
		}
		else {
			printf("Invalid HTTP request: missing CRLF.\n");
		}
	}

	return 0;
}

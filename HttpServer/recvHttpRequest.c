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

	//清除path末尾可能存在的 \r 或者 \n 
	char* p = strchr(path, '\r');
	if (p) 
		*p = '\0';
	p = strchr(path, '\n');
	if (p)
		*p = '\0';

	printf("Original path: %s\n", path);

	// 在这里添加解码逻辑
	char decoded_path[1024];
	decodeMsg(decoded_path, path);  // 解码 URL 编码的路径
	printf("Decoded path: %s\n", decoded_path);

	if (strcasecmp(method, "get") != 0) {
		return -1;	
	}

	//处理客户端请求的静态资源（目录/文件）
	const char* file = NULL;
	if (strcmp(decoded_path, "/") == 0) {
		file = "./";
	}
	else {
		file = decoded_path + 1;	
	}

	printf("Parsed file path: %s\n", file);

	//获取文件属性
	struct stat st;
	int ret = stat(file, &st);
	if (ret == -1) {
		// 文件不存在，返回 404 页面
		struct stat st404;
		if (stat("404.html", &st404) == -1) {
			// 连 404.html 都没有，直接发送简单消息
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

	// 判断是否是目录
	if (S_ISDIR(st.st_mode)) {
		// 发送目录内容
		sendDir(file, cfd);  
	}
	else {
		// 普通文件，先发送头部，再发送文件内容
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

		// 检查是否读到完整的 HTTP 请求头（以 \r\n\r\n 结尾）
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
		// 从 buf 中提取出第一行，传给 parseRequestLine
		char* lineEnd = strstr(buf, "\r\n");
		if (lineEnd) {
			*lineEnd = '\0';  // 截断第一行
			parseRequestLine(buf, cfd);
		}
		else {
			printf("Invalid HTTP request: missing CRLF.\n");
		}
	}

	return 0;
}

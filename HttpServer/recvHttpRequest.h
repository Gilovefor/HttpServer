#pragma once

/*
 * HTTP GET 请求格式：
 * --------------------------
 * [请求行] 格式：GET /path?query HTTP/1.1\r\n
 * [请求头] 每行格式：HeaderName: Value\r\n
 * 头结束后跟一个空行 \r\n
 *
 * 示例：
 * GET /search?q=hello HTTP/1.1\r\n
 * Host: example.com\r\n
 * User-Agent: curl/7.68.0\r\n
 * \r\n
 */


int parseRequestLine(const char* line, int cfd);	//解析请求行

int recvHttpRequest(int cfd, int epfd);				//接收请求行消息
	


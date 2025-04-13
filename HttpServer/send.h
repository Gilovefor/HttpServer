#pragma once

/*
 * ======================================================
 * HTTP 响应头格式
 * ======================================================
 *
 * 状态行: HTTP-Version Status-Code Reason-Phrase\r\n
 * 示例: HTTP/1.1 200 OK\r\n
 *
 * 必选头:
 * - Content-Length: 字节数\r\n  (除非是分块传输)
 * - Content-Type: MIME类型\r\n
 *
 * 可选头:
 * - Server: 服务器信息\r\n
 * - Date: 响应时间 (RFC 1123 格式)\r\n
 * - Connection: keep-alive/close\r\n
 *
 * 结束标志: \r\n\r\n
 *
 * 示例响应:
 * --------------------------------------
 * HTTP/1.1 404 Not Found\r\n
 * Content-Type: text/plain\r\n
 * Content-Length: 9\r\n
 * \r\n
 * Not Found
 */

int sendFile(const char* filename, int cfd);	//发送文件

//发送响应头（状态行+响应头）
//文件描述符+状态码+状态描述+content_type+contene_length
int sendHeadMsg(int cfd, int status, const char* descr, const char* type, int length);

int sendDir(const char* dirName, int cfd);
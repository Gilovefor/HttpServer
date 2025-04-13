#pragma once

/*
 * ======================================================
 * HTTP ��Ӧͷ��ʽ
 * ======================================================
 *
 * ״̬��: HTTP-Version Status-Code Reason-Phrase\r\n
 * ʾ��: HTTP/1.1 200 OK\r\n
 *
 * ��ѡͷ:
 * - Content-Length: �ֽ���\r\n  (�����Ƿֿ鴫��)
 * - Content-Type: MIME����\r\n
 *
 * ��ѡͷ:
 * - Server: ��������Ϣ\r\n
 * - Date: ��Ӧʱ�� (RFC 1123 ��ʽ)\r\n
 * - Connection: keep-alive/close\r\n
 *
 * ������־: \r\n\r\n
 *
 * ʾ����Ӧ:
 * --------------------------------------
 * HTTP/1.1 404 Not Found\r\n
 * Content-Type: text/plain\r\n
 * Content-Length: 9\r\n
 * \r\n
 * Not Found
 */

int sendFile(const char* filename, int cfd);	//�����ļ�

//������Ӧͷ��״̬��+��Ӧͷ��
//�ļ�������+״̬��+״̬����+content_type+contene_length
int sendHeadMsg(int cfd, int status, const char* descr, const char* type, int length);

int sendDir(const char* dirName, int cfd);
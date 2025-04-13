#include "send.h"
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

int sendFile(const char* filename, int cfd)
{
	//1�����ļ�
	int fd = open(filename, O_RDONLY);
	assert(fd > 0);

#if 0		//ʹ����ͨ��send����
	while (1) {
		char buf[1024];		//ÿ�δ��ļ��ж�ȡ1k�����ݣ��ڷ������˲��õ�����ʽ����Э�飬������ÿ�ζ���1k�������ȡ��ֱ��������
		int len = read(fd, buf, sizeof buf);
		if (len > 0) {
			send(cfd, buf, len, 0);
			usleep(10);		//��Ϣ10ms���ÿͻ��˴�����Ϣ
		}
		else if (len == 0) {
			break;		//��Ϣ�����������ѭ��
		}
		else {
			perror("read");
			break;
		}
	}
#elif 1		//ʹ��sendfile+lseek
	off_t offset = 0;
	off_t size = lseek(fd, 0,SEEK_END);	//��ȡ�ļ���С	
	if (size == -1) {
		perror("lseek(SEEK_END) failed");
		return -1;
	}
	
	if (lseek(fd, 0, SEEK_SET)==-1) {	//����ָ�뵽�ļ�ͷ
		perror("lseek(SEEK_SET) failed");
		return -1;
	}

	while (offset < size)
	{
		int ret = sendfile(cfd, fd, &offset, size - offset);
		printf("ret value: %d\n", ret);
		if (ret == -1 && errno == EAGAIN)
		{
			printf("no data......\n");
		}
	}

#elif 0		//ʹ�� struct stat
	// 1. ��ȡ�ļ���С�����ƶ�ָ�룩
	struct stat file_stat;
	if (fstat(fd, &file_stat) == -1) {
		perror("fstat failed");
		return -1;
	}
	off_t file_size = file_stat.st_size;

	// 2.ʼ�������ļ�ָ��λ�ã������������
	if (lseek(fd, 0, SEEK_SET) == -1) {		
		perror("lseek failed");
		return -1;
	}

	// 3. �����ļ�
	ssize_t sent = sendfile(cfd, fd, NULL, file_size);
	if (sent == -1) {
		perror("sendfile failed");
		return -1;
	}

#endif 
	close(cfd);
	return 0;
}

int sendHeadMsg(int cfd, int status, const char* descr, const char* type, int length)
{
	char buf[4096] = { 0 };

	// ״̬��
	sprintf(buf, "HTTP/1.1 %d %s\r\n", status, descr);

	// ��ȷƴд����Ӧͷ
	sprintf(buf + strlen(buf), "Content-Type: %s\r\n", type);

	// ֻ�е� length >= 0 ʱ�ŷ��� Content-Length
	if (length >= 0) {
		sprintf(buf + strlen(buf), "Content-Length: %d\r\n", length);
	}

	strcat(buf, "Connection: close\r\n");
		
	// ���У���ʾͷ������
	strcat(buf, "\r\n");

	// ����ͷ��
	send(cfd, buf, strlen(buf), 0);
	return 0;
}




/*
<html>
	<head>
		<title>test</title>
	</head>
	<body>
		<table>
			<tr>
				<td></td>
				<td></td>
			</tr>
			<tr>
				<td></td>
				<td></td>
			</tr>
		</table>
	</body>
</html>
*/
int sendDir(const char* dirName, int cfd)
{
	char buf[8192] = { 0 };  // �㹻��ػ���Ŀ¼����
	char temp[1024] = { 0 };

	sprintf(buf, "<html><head><title>%s</title></head><body><table>", dirName);

	struct dirent** namelist;
	int num = scandir(dirName, &namelist, NULL, alphasort);
	if (num < 0) {
		perror("scandir");
		return -1;
	}

	for (int i = 0; i < num; ++i)
	{
		char* name = namelist[i]->d_name;
		struct stat st;
		char subPath[1024] = { 0 };
		sprintf(subPath, "%s/%s", dirName, name);
		if (stat(subPath, &st) == -1) {
			perror("stat subPath");
			free(namelist[i]);
			continue;
		}

		if (S_ISDIR(st.st_mode)) {
			sprintf(temp,
				"<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td></tr>",
				name, name, (long)st.st_size);
		}
		else {
			sprintf(temp,
				"<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>",
				name, name, (long)st.st_size);
		}

		size_t len = strlen(buf);
		size_t remain = sizeof(buf) - len - 1;
		if (remain > 0) {
			strncat(buf, temp, remain);
		}

		free(namelist[i]);
	}

	sprintf(temp, "</table></body></html>");
	strcat(buf, temp);
	free(namelist);

	printf("send file right now......��\n%s\n", buf);
	// ������Ӧͷ�����ȱ�����ʵ��ҳ�泤�ȣ�
	sendHeadMsg(cfd, 200, "OK", "text/html", strlen(buf));

	// ���� HTML ����
	send(cfd, buf, strlen(buf), 0);

	return 0;
}

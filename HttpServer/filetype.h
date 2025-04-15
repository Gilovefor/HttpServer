#pragma once

const char* getFileType(const char* name);	//获取content_type

int hexToDec(char c);		//将字符转换为整形数

void decodeMsg(char* to, char* from);	//解码---- to 存储解码之后的数据，传出参数， from 存储被解码的数据，传入参数
// SJIS2GBK.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <iostream>
#include <atlconv.h>
using namespace std;

#define HEAD_STRING "$TEXT_LIST__"
typedef unsigned char byte;
typedef unsigned int uint;

long get_file_size(FILE* f) {
	long size_of_file;
	fseek(f, 0, SEEK_END);
	size_of_file = ftell(f);
	fseek(f, 0, SEEK_SET);
	return size_of_file;
}
#define _read_uint(pt,uv) \
		uv = *(uint*)pt; \
		pt += sizeof(uint) 
/*
char* _read_str(byte** pt)
{
	char* ptx;
	ptx = (char*)*pt;
	*pt += strlen(ptx) + 1;

	return ptx;
}*/
int CovtUTF8toGBK(const char* JISStr, char* GBBuf, int nSize)
{

	static wchar_t wbuf[2048];
	int nLen;

	nLen = strlen(JISStr) + 1;
	if (wbuf == NULL) return 0;
	nLen = MultiByteToWideChar(CP_UTF8, 0, JISStr,
		nLen, wbuf, nLen);
	if (nLen > 0)
		nLen = WideCharToMultiByte(CP_UTF8, 0, LPCWCH(JISStr),
			nLen, GBBuf, nSize, NULL, 0);
	return nLen;
}
/*
int CovtGbkToShiftJIS(const char* JISStr, char* GBBuf, int nSize)
{
	static wchar_t wbuf[2048];
	int nLen;

	nLen = strlen(JISStr) + 1;
	if (wbuf == NULL) return 0;
	nLen = MultiByteToWideChar(936, 0, JISStr,
		nLen, wbuf, nLen);
	if (nLen > 0)
		nLen = WideCharToMultiByte(932, 0, wbuf,
			nLen, GBBuf, nSize, NULL, 0);
	return nLen;
}*/
/*
void dec_char(byte* c)
{
	byte* pt;
	uint strcount;
	uint strid;
	char new_line_byte = 0xA;
	static char conv_char[2048];

	uint curid;
	char* cursc;
	FILE* f;

	pt = c;

	pt += sizeof(HEAD_STRING) - 1;

	_read_uint(pt, strcount);
	f = fopen("TEXT_JP.txt", "w+");

	printf("strcount:%u\n", strcount);

	for (strid = 0; strid < strcount; strid++)
	{
		_read_uint(pt, curid);
		cursc = _read_str(&pt);

		CovtShiftJISToGB(cursc, conv_char, sizeof(conv_char));
		fwrite(conv_char, strlen(conv_char), 1, f);
		fwrite(&new_line_byte, 1, 1, f);

	}
	fclose(f);
	printf("success\n");
	//getchar();
}*/

#define BUFFER_LEN		1024*1024*20
#define ARRAY_LEN(X)	(sizeof(X)/sizeof(X[0]))

unsigned char buffer_in[BUFFER_LEN] = { 0 };
unsigned char buffer_out[BUFFER_LEN * 3] = { 0 };

int main(int ac, char** av)
{
	if ((ac < 2) || (ac > 3)) {
		printf("\t%s <input> [output]\n", av[0], av[0]);
		return 0;
	}
	char * unicode_file = av[1];
	char * utf8_file = av[2];
	if (ac == 2) {
		utf8_file = av[1];
	}


	FILE * in, *out;
	unsigned char BOM[2] = { 0 };
	unsigned char data[2] = { 0 };
	unsigned short unicode_data = 0;
	bool is_bigendian = false;

	in = fopen(unicode_file, "rb");
	if (NULL == in)
	{
		printf("cannot open files:%s\n", unicode_file);
		return false;
	}
	out = fopen(utf8_file, "wb");
	if (NULL == out)
	{
		printf("cannot open files:%s\n", utf8_file);
		return false;
	}

	fread(BOM, sizeof(BOM), 1, in);

	if (BOM[0] != 0xFE && BOM[0] != 0xFF && BOM[1] != 0xFE && BOM[1] != 0xFF)
	{
		printf("the file: %s is not unicode file!", unicode_file);
		return false;
	}

	if (BOM[0] == 0xFE && BOM[1] == 0xFF)
		is_bigendian = true;

	int len = 0, idx = 0, out_idx = 0;
	len = fread(buffer_in, sizeof(char), ARRAY_LEN(buffer_in), in);
	while (1)
	{
		while (idx < len)
		{
			memcpy(data, buffer_in + idx, ARRAY_LEN(data));
			idx += 2;
			if (!is_bigendian)
			{
				unicode_data = (data[1] << 8 & 0xFF00) | data[0];
			}
			else {
				unicode_data = (data[0] << 8 & 0xFF00) | data[1];
			}

			if (unicode_data < 0x80)
			{
				buffer_out[out_idx++] = (unsigned char)unicode_data;
			}
			else if (unicode_data < 0x800)
			{
				unsigned char data_out[2] = { 0 };
				data_out[1] = 0x80 | (unicode_data & 0x3f);
				data_out[0] = 0xC0 | (unicode_data >> 6);
				memcpy(&buffer_out[out_idx], data_out, ARRAY_LEN(data_out));
				out_idx += ARRAY_LEN(data_out);
			}
			else {
				unsigned char data_out[3] = { 0 };
				data_out[2] = 0x80 | (unicode_data & 0x3f);
				data_out[1] = 0x80 | ((unicode_data >> 6) & 0x3f);
				data_out[0] = 0xE0 | (unicode_data >> 12);
				memcpy(&buffer_out[out_idx], data_out, ARRAY_LEN(data_out));
				out_idx += ARRAY_LEN(data_out);
			}
		}
		fwrite(buffer_out, sizeof(char), out_idx, out);
		out_idx = 0;
		idx = 0;

		if (feof(in)) break;
		len = fread(buffer_in, sizeof(char), ARRAY_LEN(buffer_in), in);
	}

	fclose(in);
	fclose(out);

	return 1;

	/*
	//printf("%d\n",ac);
	if (ac == 1) {
		printf("\t%s <input> [output]\n", av[0], av[0]);
		return 0;
	}
	if ((ac < 2) || (ac > 3)) {
		printf("\t%s <input> [output]\n", av[0], av[0]);
		return 0;
	}

	char gets_text[4096];
	byte * data;
	long size_of_file;

	vector <char*> text_list;

	static char conv_char[2048];



	FILE* txt_f = fopen(av[1], "r");

	if (!txt_f)
	{
		printf("open file error\n");
		return -1;
	}
	while (fgets(gets_text, sizeof(gets_text), txt_f))
	{
		int len = strlen(gets_text);
		//gets_text[len - 1] = 0xA;
		char* push_text = new char[len + 1];

		strcpy(push_text, gets_text);
		text_list.push_back(push_text);
		//printf("%s\n",push_text);
	}

	char * outputFile = av[1];
	if (ac == 3) {
		outputFile = av[2];
	}



	FILE* f = fopen(outputFile, "w+");
	char new_line_byte = 0xA;


	for (size_t i = 0; i < text_list.size(); i++)
	{
		//fwrite(&i,4,1,f);
		CovtUTF8toGBK(text_list[i], conv_char, sizeof(conv_char));

		//fwrite(text_list[i],strlen(text_list[i]),1,f);
		fwrite(conv_char, strlen(conv_char), 1, f);

		//fwrite(&new_line_byte,1,1,f);
	}

	/*
	while(fgets(gets_text,sizeof(gets_text),txt_f))
	{
		int len = strlen(gets_text);
		gets_text[len-1] = 0;
		char* push_text = new char[len+1];

		cout<<*push_text<<endl;

		CovtShiftJISToGB(push_text,conv_char,sizeof(conv_char));
		fwrite(conv_char,strlen(conv_char),1,f);
		fwrite(&new_line_byte,1,1,f);
	}*/
	
	
	
	//fclose(f);

	//printf("success\n");



	return 1;
}

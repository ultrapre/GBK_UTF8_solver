// SJIS2GBK.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <iostream>
#include <atlconv.h>
#include <fstream>
#include <cstring>
#include <string>
#include <atlstr.h>
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <windows.h>
#include <cstdint>
#include <string>
#include <locale>
#include <codecvt>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

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

#ifdef _UNICODE
#define COUT wcout
#else
#define COUT cout
#endif

int main(int ac, char** av)
{
	if (ac == 1) {
		printf("\t%s <input> [output]\n", av[0], av[0]);
		return 0;
	}
	if ((ac < 2) || (ac > 3)) {
		printf("\t%s <input> [output]\n", av[0], av[0]);
		return 0;
	}

	char * outputFile = av[1];
	if (ac == 3) {
		outputFile = av[2];
	}

	FILE* f = _wfopen(L"utf16.txt", L"w,ccs=Unicode");

	const std::locale empty_locale = std::locale::empty();
	typedef std::codecvt_utf8<wchar_t> converter_type;  //std::codecvt_utf16
	const converter_type* converter = new converter_type;
	const std::locale utf8_locale = std::locale(empty_locale, converter);

	std::wifstream fin1(av[1]);  //input
	fin1.imbue(utf8_locale);
	std::wstring line1;

	while (!fin1.eof())
	{
		std::getline(fin1, line1);

		line1 = line1.append(L"\n");

		CString str_CString(line1.c_str());
		fwprintf(f, (LPCWSTR)str_CString);
	}
	fin1.close();

	return 1;
}

// 02x64���.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

//x64��������
long long fun(long long a, long long b, long long c, long long d, long long x)
{
	return a + b + c + d + x;

}

int main()
{
	//64λ����
	fun(0x1, 0x2, 0x3, 0x4, 0x5);

    return 0;
}


// 04x64��ϱ��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

//��������
extern "C" long add_fun(int a, int c);


int main()
{
	//���û�ຯ��
	int a =add_fun(5, 6);
    return 0;
}


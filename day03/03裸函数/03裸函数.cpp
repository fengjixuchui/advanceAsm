// 03�㺯��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

//�㺯�� �����������κ��Ż�
void _declspec(naked) fun1()
{
	_asm mov eax,99
	_asm ret
	
}

//��ͨ����
void fun2()
{
	//����ӿ���ջ֡
}


int main()
{
	//1.�㺯����������
	fun1();
	//2.��ͨ����
	fun2();
    return 0;
}


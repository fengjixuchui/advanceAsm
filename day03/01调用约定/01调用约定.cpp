// 01����Լ��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

//1.c���÷�ʽ :���ҵ�����ջ,�����ⲿƽ���ջ
int _cdecl fun_cdecl(int a,int b)
{
	return a + b;
}

//2.stdcall windowsAPI����Լ�� :���ҵ�����ջ,�����ڲ�ƽ���ջ : ret 8
int _stdcall fun_stdcall(int a, int b)
{
	return a + b;
}


//3.fastcall���ٵ���Լ�� :���ҵ�����ջ,�����ڲ�ƽ���ջ 
int _fastcall fun_fastcall(int a, int b,int c,int d )
{
	return a + b+c+d;
}

//4.this c++����Լ��    ���ҵ�����ջ,�����ڲ�ƽ���ջ
class OBJ {
public:
	int  fun_thiscall(int a, int b, int c, int d)
	{
		return a + b + c + d;
	}
	int m_number;
};


int main()
{
	//1.c���÷�ʽ
	//fun_cdecl(1, 2);

	//2.stdcall windowsAPI����Լ��
	//fun_stdcall(1, 2);

	//3.fastcall���ٵ���Լ��
	//fun_fastcall(1, 2,3,4);

	//4.this c++����Լ��
	OBJ obj;
	obj.fun_thiscall(1, 2, 3, 4);
    return 0;
}


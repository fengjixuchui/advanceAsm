// dbg.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <vector>
// ��������������ͷ�ļ��Ϳ��ļ�
#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "BeaEngine_4.1/Win32/headers/BeaEngine.h"
#pragma comment(lib,"BeaEngine_4.1\\Win32\\Win32\\Lib\\BeaEngine.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")

#define DBGPRINT(error)  \
		printf("�ļ���%s�к�����%s ��%d�У�����%s\n",\
			__FILE__,\
			__FUNCTION__,\
			__LINE__,\
			error);

DWORD OnException(DEBUG_EVENT& dbgEvent);
void  showDebugInformation(HANDLE hProc,
	HANDLE hThread,
	LPVOID pExceptionAddress)
{
	// 1. ����Ĵ�����Ϣ
	// 1.1 ͨ��GetThreadContext
	CONTEXT ct = { CONTEXT_FULL };
	if (!GetThreadContext(hThread, &ct)) {
		DBGPRINT("��ȡ�߳�������ʧ��");
	}
	printf("\tEAX:%08X ECX:%08X EDX:%08X EBX:%08X\n",
		ct.Eax, ct.Ecx, ct.Edx, ct.Ebx);
	// 2. ���ջ��Ϣ
	// 2.1 ��ȡջ����ַ
	// 2.2 ReadProcessMemroy��ȡ20�ֽڵ�����
	//     Ȼ�������ֽ���ʽ���
	DWORD buff[5];
	SIZE_T read = 0;
	if (!ReadProcessMemory(hProc, (LPVOID)ct.Esp, buff, 20, &read)) {
		DBGPRINT("��ȡ�����ڴ�ʧ��");
	}
	else {
		printf("\tջ���ݣ�\n");
		for (int i =0;i<5;++i)
		{
			printf("\t%08X|%08X\n",
				ct.Esp+i*4, 
				buff[i]);
		}
	}
	// 3. ����ڴ����ݵ���Ϣ

	// 4. ����������Ϣ
	// 4.1 �쳣��ַ���������Ҫ�ӱ����Խ����н��쳣��ַ�ϵ�
	//     �������ȡ���������С�
	// 4.2 ���÷�������棬�������뷭��ɻ��ָ�Ȼ�������
	LPBYTE opcode[200];
	if (!ReadProcessMemory(hProc, pExceptionAddress, opcode, 200, &read)) {
		DBGPRINT("��ȡ�ڴ�ʧ��\n");
	}
	else {
		DISASM disAsm = { 0 };
		disAsm.EIP = (UIntPtr)opcode;
		disAsm.VirtualAddr = (UInt64)pExceptionAddress;
		disAsm.Archi = 0;// x86���
		int nLen = 0;

		// nLen ���ص��Ƿ���������ָ��Ļ������ֽ���
		// ��������ʧ�ܣ��򷵻�-1
		int nSize = 16*6;
		while (nSize >= 0)
		{
			nLen = Disasm(&disAsm);
			if(nLen==-1)
				break;
			// ��������
			printf("\t0x%08X|%s\n",
				(DWORD)disAsm.VirtualAddr,
				disAsm.CompleteInstr);

			disAsm.EIP += nLen;
			disAsm.VirtualAddr += nLen;
			nSize -= nLen;
		}
	}


}

typedef	struct Breakpoint
{
	LPVOID address;
	DWORD  dwType; // �ϵ�����ͣ�����ϵ㣬Ӳ���ϵ�
}Breakpoint;

std::vector<Breakpoint> g_bps;


int main()
{
	TCHAR path[MAX_PATH] = { L"../debug/helloworld.exe" };;

	// 1. �������ԻỰ
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
	BOOL bRet = 0;
	bRet = CreateProcess(path,
		NULL,
		NULL,
		NULL,
		FALSE,
		DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi);
	if (bRet == FALSE) {
		DBGPRINT("�޷���������");
	}

	// 2. ��������¼�
	DEBUG_EVENT dbgEvent;
	DWORD       code = 0;
	while (true)
	{
		// ��������Խ��̲����˵����¼��� �����ͻ�
		// ����Ӧ����Ϣ������ṹ������У�����
		// �����з��ء���������Խ���û�е����¼���
		// �����ᴦ������״̬��
		WaitForDebugEvent(&dbgEvent, -1);
		code = DBG_CONTINUE;
		switch (dbgEvent.dwDebugEventCode)
		{
			case EXCEPTION_DEBUG_EVENT:
			printf("�쳣�¼�\n");
			code = OnException(dbgEvent);
			break;
			case CREATE_PROCESS_DEBUG_EVENT: 
			printf("���̴����¼�\n");
			printf("\n���ػ�ַ��%08X,OEP:%08X\n",
				dbgEvent.u.CreateProcessInfo.lpBaseOfImage,
				dbgEvent.u.CreateProcessInfo.lpStartAddress);
			break; 
			case CREATE_THREAD_DEBUG_EVENT :
			printf("�̴߳����¼�\n");
			break;
			case EXIT_PROCESS_DEBUG_EVENT  : 
			printf("�����˳��¼�\n");
			goto _EXIT;

			case EXIT_THREAD_DEBUG_EVENT   :
			printf("�߳��˳��¼�\n"); 
			break;
			case LOAD_DLL_DEBUG_EVENT 	   : 
			printf("DLL�����¼�\n");
			printf("\t���ػ�ַ��%08X\n", 
				dbgEvent.u.LoadDll.lpBaseOfDll);
			break;
			case UNLOAD_DLL_DEBUG_EVENT    :
			printf("DLLж���¼�\n");
			break;
			case OUTPUT_DEBUG_STRING_EVENT : 
			printf("�����ַ�������¼�\n");
			break;
			case RIP_EVENT 				   :
			printf("RIP�¼����Ѿ���ʹ����\n"); 
			break;
		}
		// 2.1 ���������Ϣ
		// 2.2 �����û�����

		// 3. �ظ�������ϵͳ
		// �����Խ��̲��������¼�֮�󣬻ᱻϵͳ����
		// �ڵ������ظ�������ϵͳ֮�󣬱����Խ��̲�
		// �����У��ظ�DBG_CONTINUE�������У������
		// �ظ���DBG_CONTINUE����ô�����ԵĽ��̵��쳣
		// ������ƽ��޷������쳣��
		// ����ظ���DBG_EXCEPTION_HANDLED�� ���쳣
		// �ַ��У�����ǵ�һ���쳣�����쳣�ͱ�ת����
		// �û����쳣�������ȥ��������ǵڶ��Σ�����
		// �ͱ���������
		// һ������£������쳣�¼�֮�⣬���ظ�DBG_CONTINUE
		// ���쳣�¼��£�����������в�ͬ�Ļظ���ԭ���ǣ�
		// 1. ����쳣�Ǳ����Խ�����������ģ���ô����������
		//    �ظ�DBG_EXCEPTION_HANDLED����������Ϊ����
		//    �����Խ��̵��쳣������ƴ�����쳣��
		// 2. ����쳣�ǵ��������������(�¶ϵ�)����ô������
		//    ��Ҫ��ȥ���쳣֮��ظ�DBG_CONTINUE��
		ContinueDebugEvent(dbgEvent.dwProcessId,
			dbgEvent.dwThreadId,
			code);
	}


_EXIT:
	system("pause");
    return 0;
}

// �����쳣�¼��ĺ���
DWORD OnException(DEBUG_EVENT& dbgEvent)
{
	EXCEPTION_RECORD& er = dbgEvent.u.Exception.ExceptionRecord;

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS,
		FALSE,
		dbgEvent.dwProcessId);
	HANDLE hThrad = OpenThread(THREAD_ALL_ACCESS,
		FALSE,
		dbgEvent.dwThreadId);

	
	// 
	//typedef struct _EXCEPTION_DEBUG_INFO {
	//	// �쳣��¼
	//	EXCEPTION_RECORD ExceptionRecord;
	//	// �Ƿ��ǵ�һ���쳣�ַ�
	//	DWORD dwFirstChance;
	//} EXCEPTION_DEBUG_INFO, *LPEXCEPTION_DEBUG_INFO;
	// �����Խ��̲����ĵ�һ���쳣�¼������
	// �쳣�¼�����ϵͳ�ϵ�
	printf("\t�쳣���룺%08X\n", er.ExceptionCode);
	printf("\t�쳣��ַ��%08X\n", er.ExceptionAddress);
	// ���������Ϣ
	showDebugInformation(hProc, hThrad, er.ExceptionAddress);
	
	static BOOL isSystemBreakpoint = TRUE;
	if (isSystemBreakpoint) {
		printf("\t����ϵͳ�ϵ�\n");
		isSystemBreakpoint = FALSE;

		return DBG_CONTINUE;
	}

	// ����쳣�Ƿ��ǵ�������װ�Ķϵ�������
	BOOL flag = FALSE;
	for (auto&i : g_bps) {
		if (i.address == er.ExceptionAddress) {
			// �޸��ϵ�
			flag = TRUE;
		}
	}






	CloseHandle(hThrad);
	CloseHandle(hProc);
	if (flag == FALSE) {
		return DBG_EXCEPTION_NOT_HANDLED;
	}
	else {
		return DBG_CONTINUE;
	}
}


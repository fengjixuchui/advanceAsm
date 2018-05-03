// dbg.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "RegStruct.h"
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

BOOL g_isUserTf = TRUE;

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
	printf("\tEIP:%08X\n", ct.Eip);
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
		int nSize = 0;
		while (nSize < 5)
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
			++nSize;
		}
	}


}

// ��һ����������Ķϵ�
void setBreakpoint_tf(HANDLE hThread)
{
	// 1. ��ȡ�߳�������
	CONTEXT ct = { CONTEXT_CONTROL };
	if (!GetThreadContext(hThread, &ct)) {
		DBGPRINT("��ȡ�̻߳���ʧ��");
	}
	// 2. ����EFLAGS��TF��־λ
	PEFLAGS pEflags = (PEFLAGS)&ct.EFlags;
	pEflags->TF = 1;
	// 3. �����߳�������
	if (!SetThreadContext(hThread, &ct)) {
		DBGPRINT("��ȡ�̻߳���ʧ��");
	}
}

typedef	struct Breakpoint
{
	LPVOID address;
	DWORD  dwType; // �ϵ�����ͣ�����ϵ㣬Ӳ���ϵ�

	BYTE   oldData;//��int3�ϵ㸲�ǵ�ԭʼ����
}Breakpoint;

std::vector<Breakpoint> g_bps;



bool setBreakpoint_cc(HANDLE hProc,LPVOID pAddress,Breakpoint* bp)
{
	/*ԭ��
	 1. ��ָ����ַ��д��0xCC(int3ָ��Ļ�����)����
	    ����ִ��int3ָ���ʱ�򣬾ͻ�����쳣��������
		���ܽ��յ����쳣��Ҳ�����׳ƵĶ����ˣ�.
	*/
	Breakpoint bp = { 0 };
	bp->address = pAddress;
	bp->dwType = EXCEPTION_BREAKPOINT;
	// 1. ��������
	SIZE_T dwRead = 0;
	if (!ReadProcessMemory(hProc, pAddress, &bp->oldData, 1, &dwRead)) {
		DBGPRINT("��ȡ�����ڴ�ʧ��");
		return false;
	}
	// 2. д��CC
	if (!WriteProcessMemory(hProc, pAddress, "\xCC", 1, &dwRead)) {
		DBGPRINT("д������ڴ�ʧ��");
		return false;
	}
	return true;
}

void addBreakpoint(Breakpoint* bp) {
	g_bps.push_back(*bp);
}

bool rmBreakpoint_cc(HANDLE hProc, 
					HANDLE hThread, 
					LPVOID pAddress, BYTE oldData)
{
	// 1. ֱ�ӽ�ԭʼ����д���ȥ
	SIZE_T write = 0;
	if (!WriteProcessMemory(hProc, pAddress, &oldData, 1, &write)) {
		DBGPRINT("д���ڴ�ʧ��");
		return false;
	}
	// 2. ��Ϊint3�������쳣������֮��eip����һ��ָ��ĵ�ַ
	//    ��ˣ�����Ҫ��eip-1
	CONTEXT ct = { CONTEXT_CONTROL };
	if (!GetThreadContext(hThread, &ct)) {
		DBGPRINT("��ȡ�߳�������ʧ��");
		return false;
	}
	ct.Eip--;
	if (!SetThreadContext(hThread, &ct)) {
		DBGPRINT("�����߳�������ʧ��");
		return false;
	}
	return true;

}

void setAllBreakpoint(HANDLE hProc)
{
	for (auto&i : g_bps) {
		if (i.dwType == EXCEPTION_BREAKPOINT) {
			setBreakpoint_cc(hProc, i.address, &i);
		}
		else if (i.dwType == EXCEPTION_SINGLE_STEP) {
			//setBreakpoint_hard();
		}
	}
}

// void rmAllBreakpoint(HANDLE hProc)
// {
// 
// }


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

void userInput(HANDLE hPorc,HANDLE hTread)
{
	// �����û�����
	// 1. ��ʾ��Ϣ
	// 1.1 ��ʾ�Ĵ�����Ϣ
	// 1.2 ��ʾջ��Ϣ
	// 1.3 ��ʾָ����ַ�������Ϣ
	// 1.4 ��ʾָ����ַ�ϵ��ڴ�������Ϣ
	// 2. �������
	// 2.1 ��������
	// 2.2 �¶ϵ�
	// 2.2.1 ����ϵ�
	// 2.2.2 Ӳ���ϵ�
	// 2.2.3 �ڴ���ʶϵ�
	// 2.3 ֱ������
	char cmd[200];
	while (true)
	{
		printf(">");
		scanf_s("%s", cmd,sizeof(cmd));
		// ������������
		if (strcmp(cmd, "t") == 0) {
			// TF �ϵ�ԭ��
			// ��һ������Ҫ����ָ��ʱ��CPU
			// ����EFLAGS��TF��־λ�Ƿ�Ϊ1
			// �����1����CPU����������һ��Ӳ��
			// �ϵ��쳣��
			setBreakpoint_tf(hTread);
			g_isUserTf = TRUE;
			break;
		}
		else if (strcmp(cmd, "bp") == 0) {
			LPVOID dwAddr = 0;
			scanf_s("%x", &dwAddr);
			Breakpoint bp;
			if (!setBreakpoint_cc(hPorc, dwAddr,&bp)) {
				printf("���öϵ�ʧ��\n");
			}
			else {
				addBreakpoint(&bp);
			}
		}
		else if (strcmp(cmd, "g") == 0) {
			break;
		}
	}
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

	// �����еĶϵ㶼�ָ�
	setAllBreakpoint(hProc);

	/* 
	*typedef struct _EXCEPTION_DEBUG_INFO {
	*	// �쳣��¼
	*	EXCEPTION_RECORD ExceptionRecord;
	*	// �Ƿ��ǵ�һ���쳣�ַ�
	*	DWORD dwFirstChance;
	*} EXCEPTION_DEBUG_INFO, *LPEXCEPTION_DEBUG_INFO;
	*/
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
		userInput(hProc, hThrad);
		return DBG_CONTINUE;
	}

	// ����쳣�Ƿ��ǵ�������װ�Ķϵ�������
	BOOL flag = FALSE;

	for (auto&i : g_bps) {
		if (i.address == er.ExceptionAddress) {
			// �޸��ϵ�
			flag = TRUE;
			switch (i.dwType)
			{
			case EXCEPTION_BREAKPOINT:
			{
				// ȥ���쳣
				rmBreakpoint_cc(hProc,
					hThrad,
					i.address,
					i.oldData);
				setBreakpoint_tf(hThrad);
				g_isUserTf = FALSE;
			}
			break;
			}
		}
	}

	switch (er.ExceptionCode)
	{
	case EXCEPTION_BREAKPOINT:// ����ϵ�
	{

	}
	break;
	case EXCEPTION_ACCESS_VIOLATION://�ڴ�����쳣���ڴ���ʶϵ㣩
	break;
	
	// TF��Ӳ���ϵ��쳣
	// ͨ��DR6�Ĵ�����һ���ж�����쳣��
	// TF�����Ļ���DR0~DR3������
	case EXCEPTION_SINGLE_STEP: 
	{
		if (g_isUserTf == FALSE) {
			goto _EXIT;
		}
	}
	flag = TRUE;
	break;
	}

	userInput(hProc, hThrad);
_EXIT:
	CloseHandle(hThrad);
	CloseHandle(hProc);
	if (flag == FALSE) {
		return DBG_EXCEPTION_NOT_HANDLED;
	}
	else {
		return DBG_CONTINUE;
	}
}


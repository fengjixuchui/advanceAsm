// PE�ļ�Day002.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>

BYTE* g_pFileImageBase = 0;
PIMAGE_NT_HEADERS g_pNt = 0;
void ReadFileToMem(WCHAR* szFilePath)
{
	//���ļ���ȡ�ļ����
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FALSE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("�ļ���ʧ��\n");
		return;
	}
	//��ȡ�ļ���С
	DWORD dwFileSize = GetFileSize(hFile, NULL);

	// 	HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, dwFileSize, NULL);
	// 	char* lpBuf = (char*)MapViewOfFile(hMap, FILE_MAP_READ, NULL,  NULL,dwFileSize);
	// 	char* p = lpBuf++;
	g_pFileImageBase = new BYTE[dwFileSize]{};
	DWORD dwRead;
	//���ļ���ȡ���ڴ���
	bool bRet =
		ReadFile(hFile, g_pFileImageBase, dwFileSize, &dwRead, NULL);
	if (!bRet)
	{
		delete[] g_pFileImageBase;
	}
	//�رվ��
	CloseHandle(hFile);
}
bool IsPEFile()
{
	//ʹ��PIMAGE_DOS_HEADER��ռ64�ֽڣ�����ǰ64���ֽ�
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)g_pFileImageBase;
	//�ж�PE�ļ��ı�ʶ�Ƿ���ȷ����һ�����ԣ���ô���Ͳ���PE�ļ�
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE)//0x5A4D('MZ')
	{
		return false;
	}
	g_pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + g_pFileImageBase);
	if (g_pNt->Signature != IMAGE_NT_SIGNATURE)//0x00004550('PE')
	{
		return false;
	}
	return true;
}

void ShowNtImportance()
{
	printf("��ڵ�RVA��%08X", g_pNt->OptionalHeader.AddressOfEntryPoint);
	printf("�ļ�Ĭ�ϼ��أ�%08X", g_pNt->OptionalHeader.ImageBase);

	printf("�ļ����θ�����%d", g_pNt->FileHeader.NumberOfSections);
	//....
}

void ShowDirTable()
{
	//��ȡĿ¼�����
	int nCountOfDirTable = g_pNt->OptionalHeader.NumberOfRvaAndSizes;
	printf("����Ŀ¼�������%d", g_pNt->OptionalHeader.NumberOfRvaAndSizes);
	for (int i = 0; i < nCountOfDirTable; i++)
	{
		//���VirtualAddress��Ϊ0��˵���˱����
		if (g_pNt->OptionalHeader.DataDirectory[i].VirtualAddress)
		{
			//....
		}
	}
}

void ShowSectionTable()
{
	//��ȡ���θ���
	int nCountOfSection = g_pNt->FileHeader.NumberOfSections;
	//�õ��׸����ε�λ��
	PIMAGE_SECTION_HEADER pSec = IMAGE_FIRST_SECTION(g_pNt);
	//�����������֣��������ֿ��ܲ�����0Ϊ��β�ģ�
	char strName[9] = {};
	for (int i = 0; i < nCountOfSection; i++)
	{
		memcpy(strName, pSec->Name, 8);
		printf("��%d����������%s", i + 1, strName);
		printf("����RVA��%08X", pSec->VirtualAddress);
		//....
		//��һ�����ε�ַ
		pSec++;
	}
}

DWORD RVAtoFOA(DWORD dwRVA)
{
	//��RVA�����ĸ�������
	//�ҵ��������κ�
	//��ȥ�������ε���ʼλ�ã��������ļ��е���ʼλ��
	int nCountOfSection = g_pNt->FileHeader.NumberOfSections;
	PIMAGE_SECTION_HEADER pSec = IMAGE_FIRST_SECTION(g_pNt);

	DWORD dwSecAligment = g_pNt->OptionalHeader.SectionAlignment;
	for (int i = 0; i < nCountOfSection; i++)
	{
		//�����ڴ��е���ʵ��С
		DWORD dwRealVirSize = pSec->Misc.VirtualSize % dwSecAligment ?
			pSec->Misc.VirtualSize / dwSecAligment * dwSecAligment + dwSecAligment
			: pSec->Misc.VirtualSize;
		if (dwRVA >= pSec->VirtualAddress &&
			dwRVA < pSec->VirtualAddress + dwRealVirSize)
		{
			//FOA = RVA - �ڴ������ε���ʼλ�� + ���ļ������ε���ʼλ�� 
			return dwRVA - pSec->VirtualAddress + pSec->PointerToRawData;
		}
		//��һ�����ε�ַ
		pSec++;
	}
}
// DWORD FOAtoRVA(DWORD dwFOA)
// {
// 
// }

void TravseralExportTable()
{
	//�ҵ�������
	DWORD dwExportRVA = 
		g_pNt->OptionalHeader.DataDirectory[0].VirtualAddress;
	//��ȡ���ļ��е�λ��
	PIMAGE_EXPORT_DIRECTORY pExport = 
		(PIMAGE_EXPORT_DIRECTORY)(RVAtoFOA(dwExportRVA) + g_pFileImageBase);

	//ģ������
	char* pName = (char*)(RVAtoFOA(pExport->Name) + g_pFileImageBase);
	printf("%s\n", pName);
	//��ַ���еĸ���
	DWORD dwCountOfFuntions = pExport->NumberOfFunctions;
	//���Ʊ��еĸ���
	DWORD dwCountOfNames = pExport->NumberOfNames;

	//��ַ���ַ
	PDWORD pAddrOfFuntion = (PDWORD)(RVAtoFOA(pExport->AddressOfFunctions) + g_pFileImageBase);
	//���Ʊ��ַ
	PDWORD pAddrOfName = (PDWORD)(RVAtoFOA(pExport->AddressOfNames) + g_pFileImageBase);
	//��ű��ַ
	PWORD pAddrOfOrdial = (PWORD)(RVAtoFOA(pExport->AddressOfNameOrdinals) + g_pFileImageBase);
	//baseֵ
	DWORD dwBase = pExport->Base;
	//������ַ���е�Ԫ��
	for (int i = 0; i < dwCountOfFuntions;i++)
	{
		//��ַ���п��ܴ������õ�ֵ������Ϊ0��ֵ��
		if (pAddrOfFuntion[i] == 0)
		{
			continue;
		}
		//������ű����Ƿ���ֵ��ַ����±�ֵ��
		//���ж��Ƿ������Ƶ���
		bool bRet = false;
		for (int j = 0; j < dwCountOfNames;j++)
		{
			if (i == pAddrOfOrdial[j])
			{
				//ȡ�����Ʊ��е����Ƶ�ַRVA
				DWORD dwNameRVA = pAddrOfName[j];
				char* pFunName = (char*)(RVAtoFOA(dwNameRVA) + g_pFileImageBase);
				printf("%04d  %s  0x%08x\n", i + dwBase, pFunName, pAddrOfFuntion[i]);
				bRet = true;
				break;
			}
		}
		if (!bRet)
		{
			//��ű���û�У�˵��������ŵ�����
			printf("%04d  %08X\n", i + dwBase, pAddrOfFuntion[i]);
		}
				
	}

}
void TravseralImportTable()
{
	//�ҵ������
	DWORD dwImpotRVA = g_pNt->OptionalHeader.DataDirectory[1].VirtualAddress;
	//���ļ��е�λ��
	DWORD dwImportInFile = (DWORD)(RVAtoFOA(dwImpotRVA) + g_pFileImageBase);
	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)dwImportInFile;
	
	//����ÿһ�������
	while (pImport->Name)
	{
		//�������Ƶ�ַ
		PIMAGE_THUNK_DATA pFirsThunk =
			(PIMAGE_THUNK_DATA)(RVAtoFOA(pImport->FirstThunk) + g_pFileImageBase);
		//ģ����
		char* pName = (char*)(RVAtoFOA(pImport->Name) + g_pFileImageBase);
		printf("����ģ������%s\n", pName);
		while (pFirsThunk->u1.AddressOfData)
		{
			//�жϵ��뷽ʽ
			if (IMAGE_SNAP_BY_ORDINAL32(pFirsThunk->u1.AddressOfData))
			{
				//˵������ŵ���(��16λ�������)
				printf("\t\t%04X \n", pFirsThunk->u1.Ordinal & 0xFFFF);
			}
			else
			{
				//���Ƶ���
				PIMAGE_IMPORT_BY_NAME pImportName =
					(PIMAGE_IMPORT_BY_NAME)(RVAtoFOA(pFirsThunk->u1.AddressOfData) + g_pFileImageBase);
				printf("\t\t%04X %s \n", pImportName->Hint, pImportName->Name);
			}
			//
			pFirsThunk++;
		}
		pImport++;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//ReadFileToMem(L"TestDll.dll");
	//IsPEFile();
	//TravseralExportTable();
	//TravseralImportTable();
// 	typedef void(*PF)();
// 	HMODULE hMod = LoadLibrary(L"TestDll.dll");
// 	PF fA = (PF)GetProcAddress(hMod, "funA");
// 	PF fB = (PF)GetProcAddress(hMod, "funB");
// 
// 	fA();
// 	fB();
	return 0;
}


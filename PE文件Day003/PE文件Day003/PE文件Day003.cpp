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
	for (int i = 0; i < dwCountOfFuntions; i++)
	{
		//��ַ���п��ܴ������õ�ֵ������Ϊ0��ֵ��
		if (pAddrOfFuntion[i] == 0)
		{
			continue;
		}
		//������ű����Ƿ���ֵ��ַ����±�ֵ��
		//���ж��Ƿ������Ƶ���
		bool bRet = false;
		for (int j = 0; j < dwCountOfNames; j++)
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
void ShowResouceTable()
{
	//�ҵ���Դ��
	DWORD dwResRVA = 
		g_pNt->OptionalHeader.DataDirectory[2].VirtualAddress;
	DWORD dwResFOA = (DWORD)(RVAtoFOA(dwResRVA) + g_pFileImageBase);
	PIMAGE_RESOURCE_DIRECTORY pRes = (PIMAGE_RESOURCE_DIRECTORY)dwResFOA;

	//��һ�㣨���ࣩ
	//�������
	DWORD dwCountOfResType =
		pRes->NumberOfIdEntries + pRes->NumberOfNamedEntries;

	for (int i = 0; i < dwCountOfResType;i++)
	{
		PIMAGE_RESOURCE_DIRECTORY_ENTRY pResEntry = 
			(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pRes + 1);

		//�ж�������Դ���ַ�������ID
		if (pResEntry->NameIsString)
		{
			//������ַ�����NameOffset����ľ�������ַ�����RVA
			//�õ������ַ�����FOA
			DWORD dwNameFOA = (DWORD)(RVAtoFOA(pResEntry->NameOffset) + g_pFileImageBase);
			//NameOffset��ָ��Ľṹ����IMAGE_RESOURCE_DIR_STRING_U����
			//���ﱣ�����ַ����ĳ��Ⱥ���ʼλ��
			PIMAGE_RESOURCE_DIR_STRING_U pName = (PIMAGE_RESOURCE_DIR_STRING_U)dwNameFOA;
			//������ַ���������0��β�ģ�������Ҫ�����������ϡ�\0����β���ٴ�ӡ
			WCHAR *pResName = new WCHAR[pName->Length + 1]{};
			memcpy(pResName, pName, (pName->Length)*sizeof(WCHAR));
			//��Ϊ��WCHAR��������wprintf
			wprintf(L"%s\n", pResName);
			//�ͷ��ڴ�
			delete[] pResName;
		}
		else   //id
		{
			char* arryResType[] = { "", "���ָ�루Cursor��", "λͼ��Bitmap��", "ͼ�꣨Icon��", "�˵���Menu��"
				, "�Ի���Dialog��", "�ַ����б�String Table��", "����Ŀ¼��Font Directory��", "���壨Font��", "��ݼ���Accelerators��"
				, "�Ǹ�ʽ����Դ��Unformatted��", "��Ϣ�б�Message Table��", "���ָ���飨Croup Cursor��", "", "ͼ���飨Group Icon��", ""
				, "�汾��Ϣ��Version Information��" };
			if (pResEntry->Id < 17)
			{
				printf("%s\n", arryResType[pResEntry->Id]);
			}
			else
			{
				printf("%04X\n", pResEntry->Id);
			}

			//�ж��Ƿ�����һ��
			if (pResEntry->DataIsDirectory)
			{
				DWORD dwResSecond = (DWORD)pRes + pResEntry->OffsetToDirectory;
				PIMAGE_RESOURCE_DIRECTORY pResSecond = (PIMAGE_RESOURCE_DIRECTORY)dwResSecond;
				//�ڶ������
				DWORD dwCountOfSecond = 
					pResSecond->NumberOfIdEntries + pResSecond->NumberOfNamedEntries;
				//����ÿһ����Դ
				for (int iSecond = 0; iSecond < dwCountOfSecond;iSecond++)
				{
					PIMAGE_RESOURCE_DIRECTORY_ENTRY pResSecondEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResSecond + 1);

					//�ж�������Դ���ַ�������ID
					if (pResEntry->NameIsString)
					{
						//������ַ�����NameOffset����ľ�������ַ�����RVA
						//�õ������ַ�����FOA
						DWORD dwNameFOA = (DWORD)(RVAtoFOA(pResEntry->NameOffset) + g_pFileImageBase);
						//NameOffset��ָ��Ľṹ����IMAGE_RESOURCE_DIR_STRING_U����
						//���ﱣ�����ַ����ĳ��Ⱥ���ʼλ��
						PIMAGE_RESOURCE_DIR_STRING_U pName = (PIMAGE_RESOURCE_DIR_STRING_U)dwNameFOA;
						//������ַ���������0��β�ģ�������Ҫ�����������ϡ�\0����β���ٴ�ӡ
						WCHAR *pResName = new WCHAR[pName->Length + 1]{};
						memcpy(pResName, pName, (pName->Length)*sizeof(WCHAR));
						wprintf(L"%s\n", pResName);
						delete[] pResName;
					}
					else   //id
					{
						printf("%04X\n", pResEntry->Id);
					}
					//�ж���û����һ��
					//������
					if (pResSecondEntry->DataIsDirectory)
					{
						//���������ʼλ��
						DWORD dwResThrid = 
							(DWORD)pRes + pResSecondEntry->OffsetToDirectory;						
						PIMAGE_RESOURCE_DIRECTORY pResThrid = (PIMAGE_RESOURCE_DIRECTORY)dwResThrid;

						PIMAGE_RESOURCE_DIRECTORY_ENTRY pResThridEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResThrid + 1);
						//�����㣬�Ѿ������һ�㣬ʹ��PIMAGE_RESOURCE_DIRECTORY_ENTRY�е�
						//OffsetToData��Ա���õ�PIMAGE_RESOURCE_DATA_ENTRY�ṹ��λ��
						PIMAGE_RESOURCE_DATA_ENTRY pResData =
							(PIMAGE_RESOURCE_DATA_ENTRY)(pResThridEntry->OffsetToData + (DWORD)pRes);
						//��Դ��RVA��Size
						DWORD dwResDataRVA = pResData->OffsetToData;
						DWORD dwResDataSize = pResData->Size;
						//PIMAGE_RESOURCE_DATA_ENTRY�е�OffsetToData�Ǹ�RVA
						DWORD dwResDataFOA = (DWORD)(RVAtoFOA(dwResDataRVA) + g_pFileImageBase);
						//��Դ�Ķ���������
						//������ӡ��Դ�Ķ���������
						PBYTE pData = (PBYTE)dwResDataFOA;
						for (int iData = 0; iData < dwResDataSize; iData++)
						{
							if (iData % 16 == 0 && iData != 0)
							{
								printf("\n");
							}
							printf("%02X ", pData[iData]);
						}
					}
					//��һ����Դ
					pResSecondEntry++;
				}
			}			
		}
		//��һ����Դ
		pResEntry++;
	}
}
void ShowRelocTable()
{
	typedef struct _OFFSET_TYPE 
	{
		WORD offset : 12; //��ҳ��ƫ����
		WORD type : 4;    //�ض�λ���ͣ�3��
	}OFFSET_TYPE, *POFFSET_TYPE;
	//�ض�λ��RVA
	DWORD dwRelocRVA = g_pNt->OptionalHeader.DataDirectory[5].VirtualAddress;
	//�Ƿ�Ϊ��
	if (!dwRelocRVA)
	{
		printf("û���ض�λ��\n");
		return;
	}
	//�ض�λ�����ļ��еĵ�ַ
	PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)(RVAtoFOA(dwRelocRVA) + g_pFileImageBase);
	
	//ѭ���ض�λ��
	//���SizeOfBlockΪ0��˵��û����Ҫ�ض�λ��������
	while (pReloc->SizeOfBlock)
	{
		//��ǰ�ض�λҳRVA
		printf("%08X\n\n", pReloc->VirtualAddress);
		//��һҳһ���ж��ٸ��ض�λ�飨�����ٸ���Ҫ�ض�λ�����ݣ�
		DWORD dwCount = (pReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
		//ָ���ض�λ���ַ
		POFFSET_TYPE pOffset = (POFFSET_TYPE)(pReloc + 1);
		//����ÿһ���ض�λ��
		for (int i = 0; i < dwCount;i++)
		{
			//����һҳ�е�λ�õ�ַRVA
			DWORD dwRelocDataRVA = pReloc->VirtualAddress + pOffset->offset;
			//ת��FOA
			DWORD dwRelocDataFOA = (DWORD)(RVAtoFOA(dwRelocDataRVA) + g_pFileImageBase);
			//ʵ����Ҫ�ض�λ�����ݵ�ַ�Ǹ�VA
			DWORD dwRealDataVA = *(DWORD*)dwRelocDataFOA;
			//ת��RVA���õ�FOA
			DWORD dwRealDataRVA = dwRealDataVA - g_pNt->OptionalHeader.ImageBase;			
			DWORD dwRealDataFOA = (DWORD)(RVAtoFOA(dwRealDataRVA) + g_pFileImageBase);
			//��Ҫ�ض�λ�ľ������ݣ��ֽ�����ȷ����
			DWORD dwData = *(DWORD*)dwRealDataFOA;

			printf("��Ҫ�ض�λ�ĵ�%d������ RVA��%08X  VA:%08X  DATA:%08X\n", 
				i + 1, dwRelocDataRVA, dwRealDataVA,dwData);
			//��һ���ض�λ����λ��
			pOffset++;
		}
		
		//��һҳ
		pReloc = 
			(PIMAGE_BASE_RELOCATION)(pReloc->SizeOfBlock + (DWORD)pReloc);
	}

}


int _tmain(int argc, _TCHAR* argv[])
{
	ReadFileToMem(L"RVAtoFOA.exe");
	IsPEFile();
// 	TravseralExportTable();
// 	TravseralImportTable();
// 	
//  	typedef void(*PF)();
//  	HMODULE hMod = LoadLibrary(L"TestDll.dll");
//  	PF fA = (PF)GetProcAddress(hMod, "funA");
//  	PF fB = (PF)GetProcAddress(hMod, (char*)0x10);
//  
//  	fA();
//  	fB();
	//ShowResouceTable();
	ShowRelocTable();
	return 0;
}


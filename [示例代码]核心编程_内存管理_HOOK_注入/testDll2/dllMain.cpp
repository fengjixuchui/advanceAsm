#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
)
{
	MessageBox(0, L"Dll������", L"DLL�ڲ��ĵ���", 0);
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		
	}
	return TRUE;
}
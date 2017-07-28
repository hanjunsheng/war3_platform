#define _DLL_EXPORT
#include "InlineHook.h"
#include "HookNetwork.h"
#include <WINDOWS.H>
#include <iostream>
using namespace std;

//网络API HOOK 类
CInlineHook* g_InlineHook = NULL;

FILE *fp;

//dll入口
BOOL WINAPI   DllMain(HINSTANCE  hModule,DWORD  dwReason,LPVOID  lpvReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		//进程附加		
		fopen_s(&fp,"..\\..\\test.txt", "w+");
		fprintf(fp,"Load Dll Successful\n");
		//fwrite("111", 3, 1, fp);
		g_InlineHook = new CInlineHook();		
		
		break;
	case DLL_PROCESS_DETACH:
		//进程卸载
		fclose(fp);
		g_InlineHook?delete g_InlineHook:NULL;
		break;
	}
	
	return TRUE;
}

int WINAPI Add(int a,int b)
{
	return a+b;
}
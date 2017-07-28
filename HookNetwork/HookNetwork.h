#pragma once
#ifdef _DLL_EXPORT
#define DLL_FUNCTION	extern "C" __declspec(dllexport)
#else
#define DLL_FUNCTION	extern "C" __declspec(dllimport)
#endif

DLL_FUNCTION int WINAPI Add(int a,int b);
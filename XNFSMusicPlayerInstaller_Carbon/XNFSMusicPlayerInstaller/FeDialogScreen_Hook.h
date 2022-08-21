// FeDialogScreen hook for Carbon & ProStreet

#include "FeDialogScreen_Carbon.h"
#include "stdafx.h"
#include "stdio.h"
#include <stdlib.h>

void(__cdecl* FEDialogScreen_ShowDialog)(const char* pMsg, const char* pButton1Msg, const char* pButton2Msg, const char* pButton3Msg) = (void(__cdecl*)(const char*, const char*, const char*, const char*))FEDIALOGSCREEN_SHOWDIALOG_ADDRESS;
void*(__thiscall* cFEng_FindPackage)(void* cFeng, const char* pPackageName) = (void*(__thiscall*)(void*, const char*))CFENG_FINDPACKAGE_ADDRESS;
void*(__thiscall* FEPackage_FindObjectByHash)(void* FEPackage, unsigned int obj_hash) = (void*(__thiscall*)(void*, unsigned int))FEPACKAGE_FINDOBJECTBYHASH_ADDRESS;
int(*FE_String_Printf)(void* FEObject, const char* fmt, ...) = (int(*)(void*, const char*, ...))FE_STRING_PRINTF_ADDRESS;
void(__cdecl* FE_String_SetString)(void* FEObject, wchar_t* string) = (void(__cdecl*)(void*, wchar_t*))FE_STRING_SETSTRING_ADDRESS;
unsigned int(__cdecl* FEDialogScreen_GetButtonHash)(int buttonIdx) = (unsigned int(__cdecl*)(int))GETBUTTONHASH_ADDRESS;
unsigned int(__cdecl* bStringHash)(char* string) = (unsigned int(__cdecl*)(char*))0x00471050;


char FEPrintf_Buffer[1024];
wchar_t FEPrintf_Wide_Buffer[1024];

int __declspec(naked) cFEng_FindPackageWithControl()
{
	_asm
	{
		mov ecx, ds: [CFENG_MINSTANCE]
		mov     eax, [ecx]
		add     eax, 0xD8
		jz      loc_516BAE
		mov     eax, [eax + 0xC]
		test    eax, eax
		jz      loc_516BAE

	loc_516BA0 :
		mov     ecx, [eax + 0x20]
		test    ecx, ecx
		jnz     loc_516BB3
		mov     eax, [eax + 8]
		test    eax, eax
		jnz     loc_516BA0

	loc_516BAE :
		xor eax, eax
		ret

	loc_516BB3 :
		ret
	}
}

char* cFEng_FindPackageWithControl_Name()
{
	int pkg = cFEng_FindPackageWithControl();
	if (pkg)
		return *(char**)(pkg + 0xC);
	return NULL;
}

int CurrentDialog_FEPrintf(unsigned int obj_hash, const char* fmt, ...)
{
	va_list ArgList;
	int Result = 0;
	int* FEDialogScreenInstance = *(int**)FEDIALOGSCREEN_SINSTANCE;

	__crt_va_start(ArgList, fmt);
	Result = vsprintf(FEPrintf_Buffer, fmt, ArgList);
	__crt_va_end(ArgList);

	mbstowcs(FEPrintf_Wide_Buffer, FEPrintf_Buffer, 1024);

	FE_String_SetString(FEPackage_FindObjectByHash(cFEng_FindPackage((void*)CFENG_MINSTANCE, cFEng_FindPackageWithControl_Name()), obj_hash), FEPrintf_Wide_Buffer);
	return Result;
}

int CurrentDialog_FEWPrintf(unsigned int obj_hash, wchar_t* fmt, ...)
{
	va_list ArgList;
	int Result = 0;
	int* FEDialogScreenInstance = *(int**)FEDIALOGSCREEN_SINSTANCE;

	__crt_va_start(ArgList, fmt);
	Result = vswprintf(FEPrintf_Wide_Buffer, fmt, ArgList);
	__crt_va_end(ArgList);

	FE_String_SetString(FEPackage_FindObjectByHash(cFEng_FindPackage((void*)CFENG_MINSTANCE, cFEng_FindPackageWithControl_Name()), obj_hash), FEPrintf_Wide_Buffer);
	return Result;
}
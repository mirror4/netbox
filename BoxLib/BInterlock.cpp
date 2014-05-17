#include "StdAfx.h"

__declspec(naked) ULONG __stdcall InterlockedAddRef(long* pdwRef)
{
	_asm
	{
		mov ecx, [esp+4]
		mov eax, 1
		lock xadd [ecx],eax
		cmp eax, 0
		jz InternalAddRefRet
		inc eax
		ret 4
	InternalAddRefRet:
		mov [ecx], eax
		ret 4
	}
}

__declspec(naked) ULONG __stdcall InterlockedRelease(long* pdwRef)
{
	_asm
	{
		mov ecx, [esp+4]
		mov eax,0FFFFFFFFh
		lock xadd [ecx],eax
		dec eax
		ret 4
	}
}

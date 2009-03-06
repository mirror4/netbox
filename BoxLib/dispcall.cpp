#include "StdAfx.h"

/////////////////////////////////////////////////////////////////////////////
// Platform specific defines

#if defined(MIPSIV) || defined(MIPSIV_FP)
    // stack arguments are packed in 8-byte slots
	#define _STACK_INT      __int64
	#define _STACK_LONG     __int64
#else
	#define _STACK_INT      int
	#define _STACK_LONG     long
#endif // MIPSIV || MIPSIV_FP
	#define _STACK_FLOAT    float
	#define _STACK_DOUBLE   double
#if defined(MIPSIV) || defined(MIPSIV_FP)
    // stack arguments are packed in 8-byte slots
	#define _STACK_PTR      __int64
#else
	#define _STACK_PTR      void*
#endif // MIPSIV || MIPSIV_FP
#if defined(_WIN32_WCE_CEPC) || defined(_WIN32_WCE_EMULATION) || defined(_M_IX86) || defined(x86)
	#define _SCRATCH_SIZE   16
	#define _STACK_OFFSET   0
	#define _STACK_MIN      0
#elif defined(_SHX_) || defined(_SH3_) || defined(_SH4_)
	#define _ALIGN_DOUBLES  4	
	#define _SCRATCH_SIZE   0
	#define _STACK_OFFSET   0
	#define _STACK_MIN      32      // 4 32-bit registers
	#define _RETVAL_FIRST
#elif defined(_ARM_) || defined(_THUMB_) || defined(ARMV4) || defined(ARMV4T) || defined(ARMV4I)
	#define _ALIGN_DOUBLES  4
	#define _SCRATCH_SIZE   0
	#define _STACK_OFFSET   0
	#define _STACK_MIN      32      // 4 32-bit registers
#elif defined(_MIPS_) || defined(_MIPS16_) || defined(MIPSII) || defined(MIPSII_FP) || defined(MIPSIV) || defined(MIPSIV_FP)
	#define _ALIGN_DOUBLES  8
	#define _SCRATCH_SIZE   0
	#define _STACK_OFFSET   0
	#define _STACK_MIN      32      // 4 32-bit registers
#elif defined(_PPC_)
	#define _ALIGN_DOUBLES  8
	#define _SHADOW_DOUBLES 13
	#define _SCRATCH_SIZE   (_SHADOW_DOUBLES*sizeof(double))
	#define _STACK_OFFSET   0
	#define _STACK_MIN      (64+32) // 8 32-bit registers, 32 bytes param space
	#define _RETVAL_FIRST
#elif defined(_WIN32_WCE_CEF)
	#include <cefrtl.h>
	#define _ALIGN_STACK	s_cache.nAlignStack		
	#define _ALIGN_DOUBLES	s_cache.nAlignDoubles	
	#define _SHADOW_DOUBLES	s_cache.nShadowDoubles	
	#define _SCRATCH_SIZE	s_cache.nScratchSize		
	#define _STACK_OFFSET	s_cache.nStackOffset		
	#define _STACK_MIN		s_cache.nStackMin		
	#define _RETVAL_FIRST 	s_cache.bRetValFirst		
	#define _AtlceDispatchCall helper__AtlceDispatchCall
#else
	#pragma message("Warning: no _AtlceDispatchCall information set for this CPU.")
#endif

#define VT_MFCVALUE         0xFFF       // special value for DISPID_VALUE
#define VT_MFCBYREF         0x40        // indicates VT_BYREF type
#define VT_MFCMARKER        0xFF        // delimits named parameters (INTERNAL USE)

static const UINT _afxByValue[] =
{
	0,							// VT_EMPTY
	0,							// VT_NULL
	sizeof(_STACK_INT),			// VT_I2
	sizeof(_STACK_LONG),		// VT_I4
#if defined(MIPSIV) || defined(MIPSIV_FP)
	sizeof(_STACK_DOUBLE),		// VT_R4
#else
	sizeof(_STACK_FLOAT),		// VT_R4
#endif
	sizeof(_STACK_DOUBLE),		// VT_R8
	sizeof(CY),					// VT_CY
	sizeof(DATE),				// VT_DATE
	sizeof(BSTR),				// VT_BSTR
	sizeof(LPDISPATCH),			// VT_DISPATCH
	sizeof(_STACK_LONG),		// VT_ERROR
	sizeof(_STACK_LONG),		// VT_BOOL
	sizeof(VARIANT),			// VT_VARIANT
	sizeof(LPUNKNOWN),			// VT_UNKNOWN
	sizeof(DECIMAL),			// VT_DECIMAL
	0,
	sizeof(_STACK_INT),			// VT_I1
	sizeof(_STACK_INT),			// VT_UI1
	sizeof(_STACK_INT),			// VT_UI2
	sizeof(_STACK_LONG)			// VT_UI4
};

__declspec(naked) HRESULT __stdcall _dispCall(DWORD pfn, void* pArgs, UINT nSizeArgs)
{
#if defined(_WIN32_WCE_CEPC) || defined(_M_IX86)
	_asm
	{
		pop     edx         // edx = return address
		pop     eax         // eax = pfn
		pop     ecx         // ecx = pArgs
		mov     esp,ecx     // esp = pArgs
		push	edx         // put the return address at top of stack
		jmp		eax			// jump to the target function
	}
#elif defined(x86) && !defined(_WIN32_WCE_EMULATION) // CE/PC only
	_asm
	{
		pop     edx         // edx = return address
		pop     eax         // eax = pfn
		pop     ecx         // ecx = pArgs
		mov     esp,ecx     // esp = pArgs
		push	edx         // put the return address at top of stack
		jmp		eax			// jump to the target function
	}
#elif defined(_WIN32_WCE_EMULATION)  // Emulation only
	_asm
	{
		pop     edx         // edx = return address
		pop     eax         // eax = pfn
		pop     ecx         // ecx = pArgs
		add     ecx,[esp]   // ecx += nSizeArgs (=scratch area)
		mov     [ecx],edx   // scratch[0] = return address
		sub     ecx,[esp]   // ecx = pArgs (again)
		mov     esp,ecx     // esp = pArgs (usually already correct)
		call    eax         // call member function
		ret                 // esp[0] should = scratch[0] = return address
	}
#elif defined(_MIPS_) && (defined(_MIPS16_) || defined(MIPS16))
	__emit(0x65bd);		// move   sp,a1
	__emit(0x6744);		// move   v0,a0
	__emit(0x9400);		// lw     a0,0(sp)
	__emit(0x9501);		// lw     a1,4(sp)
	__emit(0x9602);		// lw     a2,8(sp)
	__emit(0xea00);		// jr     v0
	__emit(0x9703);		// lw     a3,0xC(sp)
#elif (defined(_SH4_) || defined(SH4) || defined(MIPSII_FP) || defined(MIPSIV_FP))
	// We need to deal with the function arguments, packaged on the stack, in a special way. 
	// So AtlceDispatchCall will not be used for these processors. Rather, a special function will be called
	// instead of AtlceDispatchCall. The reason for special handling is because of the way
	// floating point values are passed on the stack. At present, none of the other architectures
	// bother about floating point values at all. As a result, for these processors, we need to generate
	// a thunk on the fly, encode and execute it dynamically. At present, for other
	// architectures, we are statically creating a thunk that will work well for int arguments.
	// Eventually other architectures will have to follow these processors's example.
#elif defined(MIPSIV)
	__asm("addiu     %sp,%a1,0x0");      // sp = pArgs
	__asm("addiu     %t6,%a0,0x0");      // t6 = pfn (save it)
	__asm("ld        %a0,0x0(%sp)");     // a0 = param0
	__asm("ld        %a1,0x8(%sp)");     // a1 = param1
	__asm("ld        %a2,0x10(%sp)");    // a2 = param2
	__asm("ld        %a3,0x18(%sp)");    // a3 = param3
	__asm("j         %t6");              // ip = pfn (jump to target function)
#elif defined(_MIPS_) || defined(MIPSII)
	__asm("addiu     %sp,%a1,0x0");      // sp = pArgs
	__asm("addiu     %t6,%a0,0x0");      // t6 = pfn (save it)
	__asm("lw        %a0,0x0(%sp)");     // a0 = param0
	__asm("lw        %a1,0x4(%sp)");     // a1 = param1
	__asm("lw        %a2,0x8(%sp)");     // a2 = param2
	__asm("lw        %a3,0xc(%sp)");     // a3 = param3
	__asm("j         %t6");              // ip = pfn (jump to target function)
#elif (defined(_SH3_) || defined(SH3))
	__asm("mov   R5, sp");  			  // sp = pStack
	__asm("mov   R4, R1");                // R1 = pfn (save it)
	__asm("mov.l @sp, R4");               // R4 = arg1 ('this' pointer)
	__asm("mov   #4, R0");                // R5 = arg2
	__asm("mov.l @(R0,sp), R5");
	__asm("mov   #8, R0");                // R6 = arg3
	__asm("mov.l @(R0,sp), R6");
	__asm("mov  #12, R0");                // R7 = arg4
	// __asm("mov.l @(R0,sp), R7"); moved to delay slot
	__asm("jmp  @R1\n"                    // jump to target function
		  "mov.l @(R0,sp), R7");
#elif defined(_ARM_) && defined(ARMV4T)
	// Set up the stack pointer. Note that the first four dwords of arguments
	// are saved on the callee's stack so 16 is added to the pointer to
	// the arguments.
	__emit(0x468d);     // mov  sp, r1
	__emit(0xb004);     // add  sp, #16
	// Save the function pointer in r12 (the only available scratch register)
	__emit(0x4684);     // mov  r12, r0
	// Copy four words of arguments into r0-r3
	__emit(0x6808);     // ldr  r0, [r1]
	__emit(0x688a);     // ldr  r2, [r1, #8]
	__emit(0x68cb);     // ldr  r3, [r1, #12]
	__emit(0x6849);     // ldr  r1, [r1, #4]
	// Jump to the function
	__emit(0x4760);     // mov  pc, r12
#elif defined(_ARM_) && defined(ARMV4I)
	// Set up the stack pointer. Note that the first four dwords of arguments
	// are saved on the callee's stack so 16 is added to the pointer to
	// the arguments.
	__emit(0xe281d010);     // add  sp, r1, #16
	// Save the function pointer in r12 (the only available scratch register)
	__emit(0xe1a0c000);     // mov  r12, r0
	// Copy four words of arguments into r0-r3
	__emit(0xe5910000);     // ldr  r0, [r1]
	__emit(0xe5912008);     // ldr  r2, [r1, #8]
	__emit(0xe591300c);     // ldr  r3, [r1, #12]
	__emit(0xe5911004);     // ldr  r1, [r1, #4]
	// Jump to the function
	__emit(0xe12fff1c);     // bx   r12
#elif defined(_ARM_) && defined(ARMV4)
	// Set up the stack pointer. Note that the first four dwords of arguments
	// are saved on the callee's stack so 16 is added to the pointer to
	// the arguments.
	__emit(0xe281d010);     // add  sp, r1, #16
	// Save the function pointer in r12 (the only available scratch register)
	__emit(0xe1a0c000);     // mov  r12, r0
	// Copy four words of arguments into r0-r3
	__emit(0xe5910000);     // ldr  r0, [r1]
	__emit(0xe5912008);     // ldr  r2, [r1, #8]
	__emit(0xe591300c);     // ldr  r3, [r1, #12]
	__emit(0xe5911004);     // ldr  r1, [r1, #4]
	// Jump to the function
	__emit(0xe1a0f00c);     // mov  pc, r12
#elif defined(_PPC_)
	__emit(0x7C812378); // mr          r1,r4
	__emit(0x7C6903A6); // mtctr       r3
	__emit(0x80610000); // lwz         r3,0(r1)
	__emit(0x80810004); // lwz         r4,4(r1)
	__emit(0x80A10008); // lwz         r5,8(r1)
	__emit(0x80C1000C); // lwz         r6,0Ch(r1)
	__emit(0x80E10010); // lwz         r7,10h(r1)
	__emit(0x81010014); // lwz         r8,14h(r1)
	__emit(0x81210018); // lwz         r9,18h(r1)
	__emit(0x8141001C); // lwz         r10,1Ch(r1)
	__emit(0x3821FFF8); // addi        r1,r1,FFF8h 
	__emit(0x4E800420); // bctr
#else
	#error No _AtlceDispatchCall implementation found for this CPU.
#endif // end of WinCE CPUs
}

////////////////////////////////////////////////////////////////////////////////////////////


UINT _dispGetStackSize(ELEMDESC *lprgelemdescParam, UINT nParams, BOOL bResult)
{
	// sizeof 'this' pointer
	UINT nCount = sizeof(void*);
#ifdef _ALIGN_STACK
	nCount = (nCount + (_ALIGN_STACK-1)) & ~(_ALIGN_STACK-1);
#endif

	if(bResult)nCount += sizeof(_STACK_PTR);

	// count arguments
	for(int i = 0; i < (int)nParams; i++)
	{
		VARTYPE vt = lprgelemdescParam[i].tdesc.vt;
		// align if necessary
		// get and add appropriate byte count

		if(vt & VT_BYREF )
			nCount += sizeof(_STACK_PTR);
		else
		{
#ifdef _ALIGN_DOUBLES
			// align doubles on 8 byte for some platforms
			if (vt == VT_R8)
				nCount = (nCount + _ALIGN_DOUBLES-1) & ~(_ALIGN_DOUBLES-1);
#endif

			nCount += _afxByValue[vt & ~VT_BYREF];
		}

#ifdef _ALIGN_STACK
		nCount = (nCount + (_ALIGN_STACK-1)) & ~(_ALIGN_STACK-1);
#endif
	}
#if defined(_ALIGN_DOUBLES) && defined(_SHADOW_DOUBLES)
	// align doubles on 8 byte for some platforms
	nCount = (nCount + _ALIGN_DOUBLES-1) & ~(_ALIGN_DOUBLES-1);
#endif

	if (nCount < _STACK_MIN)
		nCount = _STACK_MIN;

	return nCount + _SCRATCH_SIZE;
}

/////////////////////////////////////////////////////////////////////////////////////////////

extern BSTR _s_bstrBlank;

// push arguments on stack appropriate for C++ call (compiler dependent)
HRESULT _dispPushStackArgs(PVOID pvInstance, BYTE* pStack, UINT nSizeArgs, UINT cArgs, VARIANT** ppArgs)
{
#ifdef _SHADOW_DOUBLES
	double* pDoubleShadowMax = (double*)(pStack + nSizeArgs);
	double* pDoubleShadow = pDoubleShadowMax - _SCRATCH_SIZE;
#endif

	// push the 'this' pointer
#ifdef _ALIGN_STACK
	ASSERT(((DWORD)pStack & (_ALIGN_STACK-1)) == 0);
#endif
	*(_STACK_PTR*)pStack = (_STACK_PTR)pvInstance;
	pStack += sizeof(_STACK_PTR);
#ifdef _ALIGN_STACK
	ASSERT(((DWORD)pStack & (_ALIGN_STACK-1)) == 0);
#endif

	UINT i;
	for( i = 0; i < cArgs; i++)
	{
		VARIANT* pArg = ppArgs[i];
		VARTYPE vt = pArg->vt;

		if(vt & VT_BYREF)
		{
			*(_STACK_PTR*)pStack = (_STACK_PTR)pArg->piVal;
			pStack += sizeof(_STACK_PTR);
		}else switch (vt)
		{
		case VT_I1:
			*(_STACK_INT*)pStack = (char)pArg->bVal;
			pStack += sizeof(_STACK_INT);   // 'short' is passed as 'int'
			break;
		case VT_UI1:
			*(_STACK_INT*)pStack = pArg->bVal;
			pStack += sizeof(_STACK_INT);   // 'short' is passed as 'int'
			break;
		case VT_I2:
		case VT_UI2:
			*(_STACK_INT*)pStack = pArg->uiVal;
			pStack += sizeof(_STACK_INT);   // 'short' is passed as 'int'
			break;
		case VT_I4:
		case VT_INT:
		case VT_UI4:
		case VT_UINT:
			*(_STACK_LONG*)pStack = pArg->lVal;
			pStack += sizeof(_STACK_LONG);
			break;
		case VT_R4:
			*(_STACK_FLOAT*)pStack = (_STACK_FLOAT)pArg->fltVal;
#if defined(MIPSIV) || defined(MIPSIV_FP)
			pStack += sizeof(_STACK_DOUBLE);
#else
			pStack += sizeof(_STACK_FLOAT);
#endif
#ifdef _SHADOW_DOUBLES
			if (pDoubleShadow < pDoubleShadowMax)
				*pDoubleShadow++ = (double)pArg->fltVal;
#endif
			break;
		case VT_R8:
#ifdef _ALIGN_DOUBLES
			// align doubles on 8 byte for some platforms
			pStack = (BYTE*)(((DWORD)pStack + _ALIGN_DOUBLES-1) &
				~(_ALIGN_DOUBLES-1));
#endif
			*(_STACK_DOUBLE*)pStack = (_STACK_DOUBLE)pArg->dblVal;
			pStack += sizeof(_STACK_DOUBLE);
#ifdef _SHADOW_DOUBLES
			if (pDoubleShadow < pDoubleShadowMax)
				*pDoubleShadow++ = pArg->dblVal;
#endif
			break;
		case VT_DATE:
#ifdef _ALIGN_DOUBLES
			// align doubles on 8 byte for some platforms
			pStack = (BYTE*)(((DWORD)pStack + _ALIGN_DOUBLES-1) &
				~(_ALIGN_DOUBLES-1));
#endif
			*(_STACK_DOUBLE*)pStack = (_STACK_DOUBLE)pArg->date;
			pStack += sizeof(_STACK_DOUBLE);
#ifdef _SHADOW_DOUBLES
			if (pDoubleShadow < pDoubleShadowMax)
				*pDoubleShadow++ = pArg->date;
#endif
			break;
		case VT_CY:
			*(CY*)pStack = pArg->cyVal;
			pStack += sizeof(CY);
			break;
		case VT_BSTR:
			if(pArg->bstrVal)
				*(_STACK_PTR*)pStack = (_STACK_PTR)pArg->bstrVal;
			else *(_STACK_PTR*)pStack = (_STACK_PTR)_s_bstrBlank;
			pStack += sizeof(_STACK_PTR);
			break;
		case VT_ERROR:
			*(_STACK_LONG*)pStack = (_STACK_LONG)pArg->scode;
			pStack += sizeof(_STACK_LONG);
			break;
		case VT_BOOL:
			*(_STACK_LONG*)pStack = (_STACK_LONG)pArg->boolVal;
			pStack += sizeof(_STACK_LONG);
			break;
		case VT_VARIANT:
			*(VARIANT*)pStack = *pArg->pvarVal;
			pStack += sizeof(VARIANT);
			break;
		case VT_DISPATCH:
		case VT_UNKNOWN:
			*(_STACK_PTR*)pStack = (_STACK_PTR)pArg->punkVal;
			pStack += sizeof(_STACK_PTR);
			break;
		case VT_DECIMAL:
			*(DECIMAL*)pStack = pArg->decVal;
			pStack += sizeof(DECIMAL);
			break;

		default:
			return E_INVALIDARG;
		}

#ifdef _ALIGN_STACK
		// align stack as appropriate for next parameter
		pStack = (BYTE*)(((DWORD)pStack + (_ALIGN_STACK-1)) &
			~(_ALIGN_STACK-1));
		ASSERT(((DWORD)pStack & (_ALIGN_STACK-1)) == 0);
#endif
	}

	return S_OK;    // success!
}


//---------------------------------------------------------------------------
#include "stk_stasm.h"
//---------------------------------------------------------------------------
// Stasm is only for GCC compatible inline asembler
#if defined( __GNUC__)  | defined(__CLANG__)
// AMD/Intel 32 or 64 bits
#if defined(__x86_64__) | defined(__i386__)
// ^^ We assume x87 is present since 486/Pentium era by default
#define __X87__
//---------------------------------------------------------------------------
// __STASM_DECLARE_SHARE_STACK Template
//---------------------------------------------------------------------------
#define __STASM_DECLARE_SHARE_STACK(mode_name,data_type)                                            \
    uint8_t    __stasm_##mode_name##_max asm("__stasm_"#mode_name"_max") =__STASM_MAX_PARARELL;                \
    data_type  __stasm_##mode_name##_stack[__STASM_MAX_PARARELL+1][__STASM_STACK_SIZE] asm("__stasm_"#mode_name"_stack");		\
    data_type *__stasm_##mode_name asm("__stasm_"#mode_name) = (data_type*)(__stasm_##mode_name##_stack[__stasm_##mode_name##_max]);
//---------------------------------------------------------------------------
__STASM_DECLARE_SHARE_STACK(x86,uint32_t)
__STASM_DECLARE_SHARE_STACK(eax,uint32_t)
__STASM_DECLARE_SHARE_STACK(ebx,uint32_t)
__STASM_DECLARE_SHARE_STACK(ecx,uint32_t)
__STASM_DECLARE_SHARE_STACK(edx,uint32_t)
__STASM_DECLARE_SHARE_STACK(esi,uint32_t)
__STASM_DECLARE_SHARE_STACK(edi,uint32_t)
//---------------------------------------------------------------------------
#ifdef __x86_64__
    __STASM_DECLARE_SHARE_STACK(x86,uint64_t)
    __STASM_DECLARE_SHARE_STACK(rax,uint64_t)
    __STASM_DECLARE_SHARE_STACK(rbx,uint64_t)
    __STASM_DECLARE_SHARE_STACK(rcx,uint64_t)
    __STASM_DECLARE_SHARE_STACK(rdx,uint64_t)
    __STASM_DECLARE_SHARE_STACK(x64r89,uint64_t)
    __STASM_DECLARE_SHARE_STACK(x64rxx,uint64_t)
    // TODO: Make sure __SSE__ to be defined if __x86_64__ is present
    // To declare necessary placeholders for FPU/XMM registers push/pop states
#endif
//---------------------------------------------------------------------------
#ifdef __X87__
    __STASM_DECLARE_SHARE_STACK(x87,double)
    // In addition we declare FPU state save placeholder
    __STASM_STRUCT_FNSAVE __stasm_fnsaved_stack[__STASM_MAX_PARARELL+1] asm("__stasm_fnsaved_stack");
#endif
//---------------------------------------------------------------------------
#ifdef __MMX__
    __STASM_DECLARE_SHARE_STACK(mmx,__m64)
    // We DON'T need to declare FPU state since MMX/FPU shares same thing ^^
    #if !defined(__X87__)
    __STASM_STRUCT_FNSAVE __stasm_fnsaved_stack[__STASM_MAX_PARARELL+1] asm("__stasm_fnsaved_stack");
    #endif
#endif
//---------------------------------------------------------------------------
#ifdef __SSE__
    __STASM_DECLARE_SHARE_STACK(sse1,__m128)
    // In addition we declare FPU state save placeholder
    // ASM: fxsaved
    __STASM_STRUCT_FXSAVE __stasm_fxsaved_stack[__STASM_MAX_PARARELL+1] asm("__stasm_fxsaved_stack");
#endif
//---------------------------------------------------------------------------
#ifdef __SSE2__
    __STASM_DECLARE_SHARE_STACK(sse2,__m128d)
    #if !defined(__SSE__)
    // If SSE was not declared, we have to make FPU xstate enlarged placeholder as well
    // ASM: fxsaved
    __STASM_STRUCT_FXSAVE __stasm_fxsaved_stack[__STASM_MAX_PARARELL+1] asm("__stasm_fxsaved_stack");
    #endif
#endif
//---------------------------------------------------------------------------
#ifdef __SSE3__
    __STASM_DECLARE_SHARE_STACK(sse3,__m128i)
    #if !defined(__SSE__) and !defined(__SSE2__)
    // If SSE was not declared, we have to make FPU xstate enlarged placeholder as well
    // ASM: fxsaved
    __STASM_STRUCT_FXSAVE __stasm_fxsaved_stack[__STASM_MAX_PARARELL+1] asm("__stasm_fxsaved_stack");
#endif
//---------------------------------------------------------------------------
#endif
#if defined(__SSE4A__)\
 or defined(__SSE41__)\
 or defined(__SSE42__)
    __STASM_DECLARE_SHARE_STACK(sse4,__m128i)
    #if !defined(__SSE__) and !defined(__SSE2__) and !defined(__SSE3__)
    // If SSE was not declared, we have to make FPU xstate enlarged placeholder as well
    // ASM: fxsaved
    __STASM_STRUCT_FXSAVE __stasm_fxsaved_stack[__STASM_MAX_PARARELL+1] asm("__stasm_fxsaved_stack");
    #endif
#endif
//---------------------------------------------------------------------------
#ifdef __AVX__
    __STASM_DECLARE_SHARE_STACK(avx,__m256)
    // For AVX we must declare even bigger placeholder for FPU state almost 8kB of data!
    // ASM: xsaved
    __STASM_STRUCT_XSAVE __stasm_axsaved_stack[__STASM_MAX_PARARELL+1] asm("__stasm_axsaved_stack*");
#endif
//---------------------------------------------------------------------------
size_t __stasm_stack[__STASM_STACK_SIZE];
//---------------------------------------------------------------------------
#endif
#endif

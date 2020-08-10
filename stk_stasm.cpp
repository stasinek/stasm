//---------------------------------------------------------------------------
#include "stk_stasm.h"
//---------------------------------------------------------------------------
// stasm is only for GCC compatible inline asembler
#if defined( __GNUC__)  | defined(__CLANG__)
// AMD/Intel 32 or 64 bits
#if defined(__x86_64__) | defined(__i386__)
//---------------------------------------------------------------------------
// __STASM_DECLARE_SHARE_STACK Template
//---------------------------------------------------------------------------
#define __STASM_DECLARE_SHARE_STACK(mode_name,data_type)                                            \
    uint8_t    __s_##mode_name##_max asm("__s_"#mode_name"_max") =__STASM_MAX_PARARELL;                \
    data_type  __m_##mode_name[__STASM_MAX_PARARELL+1][__STASM_STACK_SIZE] asm("__m_"#mode_name);		\
    data_type *__s_##mode_name asm("__s_"#mode_name) =	(data_type*)(__m_##mode_name[__s_##mode_name##_max]);
//---------------------------------------------------------------------------
__STASM_DECLARE_SHARE_STACK(x86,uint32_t)
__STASM_DECLARE_SHARE_STACK(eax,uint32_t)
__STASM_DECLARE_SHARE_STACK(ebx,uint32_t)
__STASM_DECLARE_SHARE_STACK(ecx,uint32_t)
__STASM_DECLARE_SHARE_STACK(edx,uint32_t)
__STASM_DECLARE_SHARE_STACK(esi,uint32_t)
__STASM_DECLARE_SHARE_STACK(edi,uint32_t)
//---------------------------------------------------------------------------
__STASM_DECLARE_SHARE_STACK(x87,double)
__stasm_struct_FNSAVE __m_fnsaved[__STASM_MAX_PARARELL+1] asm("__m_fnsaved");
//---------------------------------------------------------------------------
#ifdef __MMX__
__STASM_DECLARE_SHARE_STACK(mmx,__m64)
#endif
//---------------------------------------------------------------------------
#ifdef __x86_64__
//---------------------------------------------------------------------------
__STASM_DECLARE_SHARE_STACK(x86,uint64_t)
__STASM_DECLARE_SHARE_STACK(rax,uint64_t)
__STASM_DECLARE_SHARE_STACK(rbx,uint64_t)
__STASM_DECLARE_SHARE_STACK(rcx,uint64_t)
__STASM_DECLARE_SHARE_STACK(rdx,uint64_t)
__STASM_DECLARE_SHARE_STACK(x64r89,uint64_t)
__STASM_DECLARE_SHARE_STACK(x64rxx,uint64_t)
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
#ifdef __SSE__
__STASM_DECLARE_SHARE_STACK(sse1,__m128)
__stasm_struct_FXSAVE __m_fxsaved[__STASM_MAX_PARARELL+1] asm("__m_fxsaved");
#endif
//---------------------------------------------------------------------------
#ifdef __SSE2__
__STASM_DECLARE_SHARE_STACK(sse2,__m128d)
#if !defined(__SSE__)
__stasm_struct_FXSAVE __m_fxsaved[__STASM_MAX_PARARELL+1] asm("__m_fxsaved");
#endif
#endif
//---------------------------------------------------------------------------
#ifdef __SSE3__
__STASM_DECLARE_SHARE_STACK(sse3,__m128i)
#if !defined(__SSE__) and !defined(__SSE2__)
__stasm_struct_FXSAVE m_fxsaved[__STASM_MAX_PARARELL+1] asm("__m_fxsaved");
#endif
//---------------------------------------------------------------------------
#endif
#if defined(__SSE4A__)\
 or defined(__SSE41__)\
 or defined(__SSE42__)
__STASM_DECLARE_SHARE_STACK(sse4,__m128i)
#if !defined(__SSE__) and !defined(__SSE2__) and !defined(__SSE3__)
__stasm_struct_FXSAVE __m_fxsaved[__STASM_MAX_PARARELL+1] asm("__m_fxsaved");
#endif
#endif
//---------------------------------------------------------------------------
#ifdef __AVX__
__STASM_DECLARE_SHARE_STACK(avx,__m256)
__stasm_struct_XSAVE __m_axsaved[__STASM_MAX_PARARELL+1] asm("__m_axsaved*");
#endif
//---------------------------------------------------------------------------
size_t stasm_stack[__STASM_STACK_SIZE];
//---------------------------------------------------------------------------
#endif
#endif

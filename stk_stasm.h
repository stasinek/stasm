//---------------------------------------------------------------------------
#ifndef __stk_stasm_h
#define __stk_stasm_h
//---------------------------------------------------------------------------
#include "./../stk_main.h"
//---------------------------------------------------------------------------
//If you don't want stasm warnings uncomment this define:
//#define   __STASM_DO_NOT_WARN__
//Define your CPU cache if it's non convencional.
#ifndef     __STASM_CPU_CACHE_LINE_SIZE
        #define __STASM_CPU_CACHE_LINE_SIZE 64
#endif
//---------------------------------------------------------------------------
//Define your favorite syntax and size of CPU cache if it's non convencional.
#ifndef     __STASM_INLINE_SYNTAX
        #define __STASM_INLINE_SYNTAX   "\n.intel_syntax noprefix\n"
#endif
//---------------------------------------------------------------------------
// Be careful specialy with recurence and potential deadlock(barier) using threads!
// I think nothing bad will happen if there would not be more than 12 calls & function in where first called copy is waiting for 13 copy to end...
// Otherwise 33 would wait for first 32 to relese semaphore and 32 would be waiting for 33 up to infinity..
// When called functions uses __stasm are independent to each other there will be "only" barier for another one.
// Then nothing bad would happen, and can be even milions of them in pararell but only 32 would start execution at the same time.
// Sometimes this limit could be even usefull i think? Your fork bomb would not choke OS? :)
#ifndef     __STASM_MAX_PARARELL
        #define __STASM_MAX_PARARELL 64
#endif
//---------------------------------------------------------------------------
// TODO: __STASM_STACK_SIZE AUTO determined to be exacly number of MMX,SSE,SSE2,SSE3,SSE4,AVX registers count on processor
// It means how many registers mine data exchange structure (between GCC and GAS) will fit. Ex. 8 on x86 means 16xEAX's = 64B
// Or 8xMMX = 64B to be exact one cache line.
#ifndef     __STASM_STACK_SIZE
#if defined(__x86_64__)
     #define __STASM_STACK_SIZE  8 //  8x = 8 bytes 64 bit = 64 bytes a cache line on most x86-64 CPU's
#else 
     #define __STASM_STACK_SIZE 16 // 16x = 4 bytes 32 bit = 64 bytes a cache line on most CPUs
#endif
#endif
//---------------------------------------------------------------------------
// Put some warnings to compiler, depending what compiler you use currently. If you use multiple ones, you would be able to detect wherever STASM was/should be called
// If not, warnings reminds you to review code and hide __stasm() blocks conditionaly to replace with something else.
#if defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__MSVC__)  || defined(__POCC__)

    #if !defined(asm) && !defined(asm_warned)
        #define asm_warned
        #pragma message("This compiler doesn't support GCC asm(\"\":::) inline asembler, directive ::asm(variable_name) will expand to nothing")
    #endif
    #if !defined(__attribute__) && !defined(attribute_warned)
        #define attribute_warned
        #pragma message("This compiler doesnt support GCC __attribute__() macro, you must use different data packing directives")
    #endif
    // It's possible to direct convert stasm into inline assembler builtin MS, Borland, Pelees C Compiler Collection
    // Because why to even use STASM then?
    // So let's do this!
    #define __stasm(var) __asm { var }
    #define   stasm(var) __asm { var }
    // TODO: the only problem then, for direct conversion are codewords passed to stasm macro: "code,save,x86,mmx" so on so forth.. 
    // But don't worry too much and let all other asm's("") to be converted into nothing
    #ifndef __attribute__
        #define __attribute__(var)
    #endif
    #ifndef asm
        #define asm(var)
    #endif
    #ifndef __asm__
        #define __asm__(var)
    #endif
    #ifndef volatile
        #define volatile(var)
    #endif
    #ifndef __volatile__
        #define __volatile__(var)
    #endif
    //---------------------------------------------------------------------------
    // This is what inspired me to make stasm at first place:
    // BTW: VARGS for older C99 compiler are defined here, could be usefull in WATCOM, GCC2(HaikuOS) or Borland compiler.
    // If you use modern MSVC, LLVM, GCC compiler this part is not for you.
    #if !defined(__BORLANDC__) & !defined(__WATCOMC__)
        #define VA_ARGS_EVAL(_1, _2, _3, _4, _5, N, ...) N
        #define VA_ARGS(...) VA_ARGS_EVAL(__VA_ARGS__, 5, 4, 3, 2, 1)

        #define IF_ELVIS_EVAL_HELPER(count, ...) IF_ELVIS_##count(__VA_ARGS__)
        #define IF_ELVIS_EVAL(count, ...) IF_ELVIS_EVAL_HELPER(count, __VA_ARGS__)
        #define IF_ELVIS(...) IF_ELVIS_EVAL(VA_ARGS(__VA_ARGS__), __VA_ARGS__)
        #define IF_ELVIS_1(__a) if (__a!=NULL))
        #define IF_ELVIS_2(__a,__b) if (__a!=NULL) if (__a.__b!=NULL)
        #define IF_ELVIS_3(__a,__b,__c) if ((a!=NULL ? a.b!= NULL : false) ? a.b.c!=NULL : false)
        #define IF_ELVIS_4(__a,__b,__c,__d) if (((a!=NULL ? a.b!= NULL : false) ? a.b.c!=NULL : false) ? a.b.c.d!=NULL : false)
        #define IF_ELVIS_5(__a,__b,__c,__d,__e) if ((((a!=NULL ? a.b!= NULL : false) ? a.b.c!=NULL : false) ? a.b.c.d!=NULL : false) ? a.b.c.d.e!=NULL : false)
    #endif
    //---------------------------------------------------------------------------
    /* Borland C++ compiler */
    #if defined(__BORLANDC__)
    #endif
    //---------------------------------------------------------------------------
    /* OpenWatcom C++ compiler */
    #if defined(__WATCOMC__)
    #endif
    //---------------------------------------------------------------------------
    /* Microsoft C/C++-compatible compiler */
    #if defined(__MSVC__)
    #endif
#elif defined (__GNUC__) || defined(__MINGW__) || defined(__CLANG__) || defined(__llvm__)
// **************************************************************************
//
// HERE IS WHERE IT ALL BEGINS; FOR GCC, CLANG
//
// **************************************************************************

extern int __stdcall _asm(const char *, ...);

//#pragma GCC system_header
//#pragma GCC diagnostic ignored "-Wno-variadic-macros"
//#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic push
#if defined(__CLANG__) || defined(__llvm__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif
#if defined(__x86_64__)
          /* GCC-compatible compiler, targeting x86/x86-64 */
#endif
#if defined(__ARM_NEON__)
          /* GCC-compatible compiler, targeting ARM with NEON */
          #include <arm_neon.h>
#endif
#if defined(__IWMMXT__)
          /* GCC-compatible compiler, targeting ARM with WMMX */
          #include <mmintrin.h>
#endif
#if defined(__xlC__) && (defined(__VEC__) || defined(__ALTIVEC__))
          /* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
          #include <altivec.h>
#endif
#if defined(__SPE__)
          /* GCC-compatible compiler, targeting PowerPC with SPE */
          #include <spe.h>
#endif
// Depending on GCC/GAS command line switches, fix GCC/GAS BUG's of copy pasting asm("") blocks in different syntax than chosen by command line
#ifdef  __INLINE_INTEL__
        #define __GCC_INLINE_SYNTAX             "\n.intel_syntax noprefix\n"
#else
        #define __GCC_INLINE_SYNTAX             "\n.att_syntax\n"
#endif
//---------------------------------------------------------------------------
// Typical typedefs for SAVE(PUSH/POP) structures and includes for FPU & SIMD extensions
// FPU x87 template for typedef
//---------------------------------------------------------------------------
#pragma pack(push, 1)
#define __STASM_TEMPLATE_FNSAVE_X87                                             \
uint16_t FCW; /* Control Word */                                                \
uint16_t reserved1;                                                             \
uint16_t FSW; /* Status Word */                                                 \
uint16_t reserved2;                                                             \
uint16_t FTW; /* TAG Word */                                                    \
uint16_t reserved3;                                                             \
union {                                                                         \
/*64 BIT */                                                                     \
        struct {                                                                \
        uint64_t RIP; /* Instruction Pointer */                                 \
        uint64_t RDP; /* Data Pointer */                                        \
        };                                                                      \
/*32 BIT */                                                                     \
        struct {                                                                \
        uint32_t FIP; /* IP Offset */                                           \
        uint16_t FCS; /* IP Selector */                                         \
        uint16_t FOP; /* Last Opcode */                                         \
        uint32_t FDP; /* Operand Offset */                                      \
        uint16_t FDS; /* Operand Selector */                                    \
        };                                                                      \
};
//---------------------------------------------------------------------------
// FPU x87 STK_IMPEXP
//---------------------------------------------------------------------------
struct __STASM_STRUCT_STX_MMX {
    uint64_t VAL;
    uint16_t EXP;
};
extern STK_IMPEXP struct __STASM_STRUCT_FNSAVE {
__STASM_TEMPLATE_FNSAVE_X87
struct __STASM_STRUCT_STX_MMX
                 ST0,ST1,ST2,ST3,ST4,ST5,ST6,ST7;
} __attribute__((packed,aligned (16))) __stasm_fnsave[__STASM_MAX_PARARELL+1];
//---------------------------------------------------------------------------
// SSE typedef template
//---------------------------------------------------------------------------
struct __STASM_STRUCT_XMM {
    union {
        uint8_t  B[16]; /* perfect for ASCII */
        uint16_t W[8]; /* UTF16 */
        uint32_t I[4]; float F[4]; /* 3D Vector */
        uint64_t Q[2]; /* 2 MMX */
        double   D[2];
    };
};
#define __STASM_TEMPLATE_FXSAVE_SSE                                             \
uint16_t         FCW; /* Control Word */                                        \
uint16_t         FSW; /* Status Word */                                         \
uint16_t         FTW; /* TAG Word */                                            \
uint16_t         FOP; /* Last Opcode */                                         \
union {                                                                         \
        struct { /*64 BIT */                                                    \
        uint64_t RIP; /* Instruction Pointer */                                 \
        uint64_t RDP; /* Data Pointer */                                        \
        };                                                                      \
        struct { /*32 BIT */                                                    \
        uint32_t FIP; /* IP Offset */                                           \
        uint16_t FCS; /* IP Selector */                                         \
        uint16_t reserved1;                                                     \
        uint32_t FDP; /* Operand Offset */                                      \
        uint16_t FDS; /* Operand Selector */                                    \
        uint16_t reserved2;                                                     \
        };                                                                      \
};                                                                              \
uint32_t         MXCR; /* SSE status */                                         \
uint32_t         MXCR_MASK;                                                     \
struct __STASM_STRUCT_STX_MMX                                                   \
                 ST0,ST1,ST2,ST3,ST4,ST5,ST6,ST7;                               \
struct __STASM_STRUCT_XMM                                                       \
                 XMM0,XMM1,XMM2,XMM3,XMM4,XMM5,XMM6,XMM7,                       \
                 XMM8,XMM9,XMM10,XMM11,XMM12,XMM13,XMM14,XMM15;                 \
/* FILL UNUSED 4kB RESERVED */                                                  \
uint32_t         reserved3[(464 - 416) / sizeof(uint32_t)];                     \
/* FILL UNUSED 4kB */                                                           \
uint32_t         user[(512 - 464) / sizeof(uint32_t)];
//---------------------------------------------------------------------------
// SSE STK_IMPEXP
//---------------------------------------------------------------------------
extern STK_IMPEXP struct __STASM_STRUCT_FXSAVE {
__STASM_TEMPLATE_FXSAVE_SSE
} __attribute__((packed,aligned (16)))  __stasm_fxsave[__STASM_MAX_PARARELL+1];
//---------------------------------------------------------------------------
// AVX STK_IMPEXP
//---------------------------------------------------------------------------
extern STK_IMPEXP struct __STASM_STRUCT_XSAVE {
__STASM_TEMPLATE_FXSAVE_SSE
union {
 uint64_t PAD[64]; /* align 64 */
 struct {
        uint64_t XSTATE_BV;
        uint64_t XSTATE_COMP_BV;
        uint64_t RESERVED[6];
        } __attribute__((packed)) HDR;
};
struct __YMM {
union {
        uint8_t  BYTE[16]; /* perfect for ASCII */
        uint16_t W[8]; /* UTF16 */
        uint32_t I[4]; float F[4]; /* 3D Vector */
        uint64_t Q[2]; /* 2 MMX */
        double   D[2];
};
} YMM0,YMM1,YMM2,YMM3,YMM4,YMM5,YMM6,YMM7,YMM8,YMM9,YMM10,YMM11,YMM12,YMM13,YMM14,YMM15;
struct __ZMML {
                uint8_t BYTE[32]; // 256 bit
        } __attribute__((packed)) ZMML[16];
struct __ZMMH {
                uint8_t BYTE[32]; // 256 bit
        } __attribute__((packed)) ZMMH[16];
//uint8_t reserved5[8192 - 512 - 64 - 32*32 - 32*32];
} __attribute__ ((packed, aligned (64))) __stasm_xsave[__STASM_MAX_PARARELL+1];
//---------------------------------------------------------------------------
// UNIVERSAL stack for common pointer type
// Should be big enought that one of each fits separate cache lines for it's own CPU core!
//---------------------------------------------------------------------------
extern STK_IMPEXP size_t __stasm_stack[__STASM_STACK_SIZE] __attribute__ ((aligned (sizeof(size_t))));
//---------------------------------------------------------------------------
#pragma pack(pop)
//---------------------------------------------------------------------------
#define __STASM_SHARE_STACK_CLEAR(mode_name, data_type)                                                 \
        for (uint32_t __stasm_##mode_name##_thread = 0;__stasm_##mode_name##_thread <=__STASM_MAX_PARARELL;__stasm_##mode_name##_t++)     \
        for (uint32_t __stasm_##mode_name##_i = 0;__stasm_##mode_name##_i < __STASM_STACK_SIZE;  __stasm_##mode_name##_c++)     \
             __stasm_##mode_name##_stack[__stasm_##mode_name##_thread][__stasm_##mode_name##_i] = 0;
//---------------------------------------------------------------------------
#define __STASM_SHARE_STACK_DEFINE(mode_name, data_type)                                                \
        extern uint8_t        __stasm_##mode_name##_max asm("__stasm_"#mode_name"_max");                    \
        extern data_type     *__stasm_##mode_name asm("__stasm_"#mode_name);                                \
        extern data_type     (__stasm_##mode_name##_stack)[__STASM_MAX_PARARELL+1][__STASM_STACK_SIZE] asm("__stasm_"#mode_name"_stack") __attribute__ ((aligned (sizeof(data_type))));
//---------------------------------------------------------------------------
// For __stdcall, __cdecl, __fastcall for naked C functions: the "code" trigger word, must be defined before "save"!
// Because code also must be placed after "load" block not before it, and it's part of "code" in hidden way for programmer.
// If you want to avoid time consuming process push, pops to be called for all registers use save with care.
//---------------------------------------------------------------------------
#define __stasm_s_head(prologue,mode,...) prologue __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_head(prologue,mode,...) __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_head(prologue,mode,...) __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_head(prologue,mode,...) __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_head(prologue,mode,...) __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_head(prologue,mode,...) __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_head(prologue,mode,...) __stasm_r_##mode(__VA_ARGS__)

// NEXT: Obviously stack POP's must be declared in reverse order than prologue PUSHes, therefore epilog it's in X block,
// i/o/r blocks are reseved for GAS clobbers, there is no code there, only variables. 

#define __stasm_s_back(epilog,mode,...) __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_back(epilog,mode,...) __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_back(epilog,mode,...) __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_back(epilog,mode,...) __stasm_x_##mode(__VA_ARGS__) epilog
#define __stasm_o_back(epilog,mode,...) __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_back(epilog,mode,...) __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_back(epilog,mode,...) __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
// Shared memory piece is defined here only because of macro limitation, without them there would be "," added by preprocessor after variables,
// So I put some text never used as variable in real, just to solve preprocessor limitations.
// It could be used for common, static defined variable for all asm's() blocks(profiling, debug?), there is place for static code and vars in there ;)
// Enable EFLAGS modification to be passed from GAS to GCC ("cc" in block r) ALWAYS!
// It's for safe programming rather super-duper-speed. FLAGS to be always passed to influence C++ code from ASM block.
//---------------------------------------------------------------------------
#define __stasm_s_code(...)
#define __stasm_t_code(...) #__VA_ARGS__
#define __stasm_l_code(...)
#define __stasm_x_code(...)
#define __stasm_o_code(...) "=g" (__stasm_stack)
#define __stasm_i_code(...)  "g" (__stasm_stack)
#define __stasm_r_code(...) "cc"
//---------------------------------------------------------------------------
// Macros for var_i special word, predefined as read variable (tell GCC it's to be read so could be even register name)
//---------------------------------------------------------------------------
#define __stasm_s_var_i(as,varname,mode,...) __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_var_i(as,varname,mode,...) __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_var_i(as,varname,mode,...) __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_var_i(as,varname,mode,...) __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_var_i(as,varname,mode,...) __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_var_i(as,varname,mode,...) [varname] as (varname), __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_var_i(as,varname,mode,...) __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
// Macros for var_o special word, predefined as output variable (tell GCC it will be written)
//---------------------------------------------------------------------------
#define __stasm_s_var_o(as,varname,mode,...) __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_var_o(as,varname,mode,...) __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_var_o(as,varname,mode,...) __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_var_o(as,varname,mode,...) __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_var_o(as,varname,mode,...) [varname] as (varname), __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_var_o(as,varname,mode,...) __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_var_o(as,varname,mode,...) __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
// Specify macros for var_c special word, const value(must be #defined with same name in C):
//---------------------------------------------------------------------------
#define __stasm_s_var_c(constname,mode,...) __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_var_c(constname,mode,...) __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_var_c(constname,mode,...) __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_var_c(constname,mode,...) __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_var_c(constname,mode,...) __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_var_c(constname,mode,...) [constname] "n" (constname), __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_var_c(constname,mode,...) __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
// Specify any read-write variable(lvalue) in single definition:
//---------------------------------------------------------------------------
#define __stasm_s_var(as,varname,mode,...) __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_var(as,varname,mode,...) __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_var(as,varname,mode,...) __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_var(as,varname,mode,...) __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_var(as,varname,mode,...) [varname] as (varname), __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_var(as,varname,mode,...) "0" (varname), __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_var(as,varname,mode,...) __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
// Place for other simmilar macros "special word" ex. var_m, var_p etc.
// I will make i hope with name simmilar to GAS/GCC inline asembler manual for variable types
//---------------------------------------------------------------------------
// "tell" could be implemented for any processor x86, ARM, PowerPC so you could write eax,ebx,ecx without save stack or use special data structures for exchange it's values
// You could use word "tell" and register name after for all unknown to stasm register names
// This could be usefull for unknown platform(PowerPC, MIPS etc.) could be only way __stasm to access GCC clobbers
// Its the safer way that guarantee thread savety, recurence safety, maybe i shoud change default option to something like "static" "exchange"?
//---------------------------------------------------------------------------
#define __stasm_s_tell(reg,mode,...) __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_tell(reg,mode,...) __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_tell(reg,mode,...) __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_tell(reg,mode,...) __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_tell(reg,mode,...) __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_tell(reg,mode,...) __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_tell(reg,mode,...) #reg, __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
// Save registers ex, on stack, save fpu state, save mmx state and pop it out at end
// Invoke push command automatically
//---------------------------------------------------------------------------
#define __stasm_s_save(what,mode,...) __stasm_save_##what(__VA_ARGS__) "\n" __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_save(what,mode,...) __stasm_t_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
// Auto invoke pop but, keep in mind that it must be in reverse order than push
//---------------------------------------------------------------------------
#define __stasm_l_save(what,mode,...) __stasm_l_##mode(__VA_ARGS__) "\n" __stasm_load_##what(__VA_ARGS__)
#define __stasm_x_save(what,mode,...) __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_save(what,mode,...) __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_save(what,mode,...) __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_save(what,mode,...) __stasm_r_##mode(__VA_ARGS__)

//---------------------------------------------------------------------------

#ifdef __ARM__
// Actually not fully implemented, should i put r0-r7 registers on the stack? ;)
// or even more?
// Because of stupid GCC inline assembly in WinAPR, Atmel Studio i had never ever wanted to learn ARM
#define __stasm_save_arm "push ???\n"
#define __stasm_load_arm "push ???\n"

__STASM_SHARE_STACK_DEFINE(arm,uint32_t)
#define __TINIT_ARM(mode,...)                                                   \
        __stasm_arm_max = __STASM_MAX_PARARELL;                                     \
        __stasm_arm = __stasm_arm_stack[__stasm_arm_max];                                         \
        __TINITSTACK(arm)                                                       \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_arm(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_arm(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_arm(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_arm(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_arm(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_arm(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_arm(mode,...)                                                 \
                "r0","r1","r2","r3","r4","r5","r6","r7",                        \
                __stasm_r_##mode(__VA_ARGS__)

#endif
//---------------------------------------------------------------------------
// Adapters for macros and real asembly code
//---------------------------------------------------------------------------
#if defined(__x86_64__) | defined(__i386__)
#define __stasm_save_eax "push eax\n"
#define __stasm_save_ebx "push ebx\n"
#define __stasm_save_ecx "push ecx\n"
#define __stasm_save_edx "push edx\n"
#define __stasm_save_esi "push esi\n"
#define __stasm_save_edi "push edi\n"
#define __stasm_save_x86 __stasm_save_eax\
                         __stasm_save_ebx\
                         __stasm_save_ecx\
                         __stasm_save_edx\
                         __stasm_save_esi\
                         __stasm_save_edi
#define __stasm_save_x87 "fsave _m_fsaved\n"
#define __stasm_save_rax "push rax\n"
#define __stasm_save_rbx "push rbx\n"
#define __stasm_save_rcx "push rcx\n"
#define __stasm_save_rdx "push rdx\n"
#define __stasm_save_rsi "push rsi\n"
#define __stasm_save_rdi "push rdi\n"
#define __stasm_save_x64r8 "push r8\n\n"
#define __stasm_save_x64r9 "push r9\n"
#define __stasm_save_x64rxx "push r10\n push r11\n push r12\n push r13\n push r14\n push r15\n"
#define __stasm_save_x64 __stasm_save_rax\
                         __stasm_save_rbx\
                         __stasm_save_rcx\
                         __stasm_save_rdx\
                         __stasm_save_rsi\
                         __stasm_save_rdi\
                         __stasm_save_x64r8 __stasm_save_x64r9 __stasm_save_x64rxx
#define __stasm_save_mmx "fxsave _m_fxsaved\n"
#define __stasm_save_sse "fxsave _m_fxsaved\n"
#define __stasm_save_avx "xsave  _m_axsaved\n"

// **********//***********

#define __stasm_load_eax "pop eax\n"
#define __stasm_load_ebx "pop ebx\n"
#define __stasm_load_ecx "pop ecx\n"
#define __stasm_load_edx "pop edx\n"
#define __stasm_load_esi "pop esi\n"
#define __stasm_load_edi "pop edi\n"
#define __stasm_load_x86 __stasm_save_edi\
                         __stasm_save_esi\
                         __stasm_save_edx\
                         __stasm_save_ecx\
                         __stasm_save_ebx\
                         __stasm_save_eax
#define __stasm_load_x87 "frstor _m_fsaved\n"
#define __stasm_load_rax "pop rax\n"
#define __stasm_load_rbx "pop rbx\n"
#define __stasm_load_rcx "pop rcx\n"
#define __stasm_load_rdx "pop rdx\n"
#define __stasm_load_rsi "pop rsi\n"
#define __stasm_load_rdi "pop rdi\n"
#define __stasm_load_x64r8 "pop r8\n"
#define __stasm_load_x64r9 "pop r9\n"
#define __stasm_load_x64rxx "pop r15\n push r14\n push r13\n push r12\n push r11\n push r10\n"
#define __stasm_load_x64 __stasm_save_x64rxx __stasm_save_x64r9 __stasm_save_x64r8\
                         __stasm_save_rdi\
                         __stasm_save_rsi\
                         __stasm_save_rdx\
                         __stasm_save_rcx\
                         __stasm_save_rbx\
                         __stasm_save_rax
#define __stasm_load_mmx "fxrstor _m_fxsaved\n"
#define __stasm_load_sse "fxrstor _m_fxsaved\n"
#define __stasm_load_avx "xrstor  _m_axsaved\n"
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(x86,uint32_t)
#define __TINIT_x86(mode,...)                                                   \
        __stasm_x86_max = __STASM_MAX_PARARELL;                                     \
        __stasm_x86 = __stasm_x86_stack[__stasm_x86_max];                                         \
        __STASM_SHARE_STACK_CLEAR(x86,uint32_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_x86(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_x86(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_x86(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_x86(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_x86(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_x86(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_x86(mode,...)                                                 \
                "eax", "ebx", "ecx", "edx", "esi", "edi",                       \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(eax,uint32_t)
#define __TINIT_eax(mode,...)                                                   \
        __stasm_eax_max = __STASM_MAX_PARARELL;                                     \
        __stasm_eax = __stasm_eax_stack[__stasm_eax_max];                                           \
        __STASM_SHARE_STACK_CLEAR(eax,uint32_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_eax(mode,...)                                                 \
                 __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_eax(mode,...)                                                 \
                 __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_eax(mode,...)                                                 \
                 __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_eax(mode,...)                                                 \
                 __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_eax(mode,...)                                                 \
                 __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_eax(mode,...)                                                 \
                 __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_eax(mode,...)                                                 \
                "eax",                                                          \
                 __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(ebx,uint32_t)
#define __TINIT_ebx(mode,...)                                                   \
        __stasm_ebx_max = __STASM_MAX_PARARELL;                                     \
        __stasm_ebx = __stasm_ebx_stack[__stasm_ebx_max];                                         \
        __STASM_SHARE_STACK_CLEAR(ebx,uint32_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_ebx(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_ebx(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_ebx(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_ebx(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_ebx(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_ebx(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_ebx(mode,...)                                                 \
                "ebx",                                                          \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(ecx,uint32_t)
#define __TINIT_ecx(mode,...)                                                   \
        __stasm_ecx_max = __STASM_MAX_PARARELL;                                     \
        __stasm_ecx = __stasm_ecx_stack[__stasm_ecx_max];                                         \
        __STASM_SHARE_STACK_CLEAR(ecx,uint32_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_ecx(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_ecx(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_ecx(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_ecx(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_ecx(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_ecx(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_ecx(mode,...)                                                 \
                "ecx",                                                          \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(edx,uint32_t)
#define __TINIT_edx(mode,...)                                                   \
        __stasm_edx_max = __STASM_MAX_PARARELL;                                     \
        __stasm_edx = __stasm_edx_stack[__stasm_edx_max];                                         \
        __STASM_SHARE_STACK_CLEAR(edx,uint32_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_edx(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_edx(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_edx(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_edx(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_edx(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_edx(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_edx(mode,...)                                                 \
                "edx",                                                          \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(esi,uint32_t)
//---------------------------------------------------------------------------
#define __TINIT_esi(mode,...)                                                   \
        __stasm_esi_max = __STASM_MAX_PARARELL;                                     \
        __stasm_esi = __stasm_esi_stack[__stasm_esi_max];                                           \
        __STASM_SHARE_STACK_CLEAR(esi,uint32_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_esi(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_esi(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_esi(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_esi(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_esi(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_esi(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_esi(mode,...)                                                 \
                "esi",                                                          \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(edi,uint32_t)
//---------------------------------------------------------------------------
#define __TINIT_edi(mode,...)                                                   \
        __stasm_edi_max = __STASM_MAX_PARARELL;                                     \
        __stasm_edi = __stasm_edi_stack[__stasm_edi_max];                                         \
        __STASM_SHARE_STACK_CLEAR(edi,uint32_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_edi(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_edi(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_edi(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_edi(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_edi(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_edi(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_edi(mode,...)                                                 \
                "edi",                                                          \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(x87,double)
//---------------------------------------------------------------------------
#define __TINIT_x87(mode,...)                                                   \
        __stasm_x87_max = __STASM_MAX_PARARELL;                                     \
        __stasm_x87 = __stasm_x87_stack[__stasm_x87_max];                                         \
        __STASM_SHARE_STACK_CLEAR(x87,double)                                   \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_x87(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_x87(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_x87(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_x87(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_x87(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_x87(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_x87(mode,...)                                                 \
                "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)",  \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
// END OF SUBSTANCIAL x86 + x87
//---------------------------------------------------------------------------
#if defined(__x86_64__)
#if !defined(__STASM_DO_NOT_WARN__) & !defined(x64_warned)
    #define x64_warned
    #warning        "It seems that your CPU has got 64bit, make sure to add compiler option "-m64" to use 64bit mode"
#endif
__STASM_SHARE_STACK_DEFINE(rax,uint64_t)
#define __TINIT_rax(mode,...)                                                   \
        __stasm_rax_max = __STASM_MAX_PARARELL;                                     \
        __stasm_rax = __stasm_rax_stack[__stasm_rax_max];                                           \
        __STASM_SHARE_STACK_CLEAR(rax,uint64_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_rax(mode,...)                                                 \
                 __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_rax(mode,...)                                                 \
                 __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_rax(mode,...)                                                 \
                 __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_rax(mode,...)                                                 \
                 __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_rax(mode,...)                                                 \
                 __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_rax(mode,...)                                                 \
                 __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_rax(mode,...)                                                 \
                "rax",                                                          \
                 __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(rbx,uint64_t)
#define __TINIT_rbx(mode,...)                                                   \
        __stasm_rbx_max = __STASM_MAX_PARARELL;                                     \
        __stasm_rbx = __stasm_rbx_stack[__stasm_rbx_max];                                         \
        __STASM_SHARE_STACK_CLEAR(rbx,uint64_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_rbx(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_rbx(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_rbx(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_rbx(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_rbx(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_rbx(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_rbx(mode,...)                                                 \
                "rbx",                                                          \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(rcx,uint64_t)
#define __TINIT_rcx(mode,...)                                                   \
        __stasm_rcx_max = __STASM_MAX_PARARELL;                                     \
        __stasm_rcx = __stasm_rcx_stack[__stasm_rcx_max];                                         \
        __STASM_SHARE_STACK_CLEAR(rcx,uint64_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_rcx(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_rcx(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_rcx(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_rcx(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_rcx(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_rcx(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_rcx(mode,...)                                                 \
                "rcx",                                                          \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(rdx,uint64_t)
#define __TINIT_rdx(mode,...)                                                   \
        __stasm_rdx_max = __STASM_MAX_PARARELL;                                     \
        __stasm_rdx = __stasm_rdx_stack[__stasm_rdx_max];                                         \
        __STASM_SHARE_STACK_CLEAR(rdx,uint64_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_rdx(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_rdx(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_rdx(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_rdx(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_rdx(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_rdx(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_rdx(mode,...)                                                 \
                "rdx",                                                          \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(rsi,uint64_t)
//---------------------------------------------------------------------------
#define __TINIT_rsi(mode,...)                                                   \
        __stasm_rsi_max = __STASM_MAX_PARARELL;                                     \
        __stasm_rsi = __stasm_rsi_stack[__stasm_rsi_max];                                           \
        __STASM_SHARE_STACK_CLEAR(rsi,uint64_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_rsi(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_rsi(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_rsi(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_rsi(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_rsi(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_rsi(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_rsi(mode,...)                                                 \
                "rsi",                                                          \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(rdi,uint64_t)
//---------------------------------------------------------------------------
#define __TINIT_rdi(mode,...)                                                   \
        __stasm_rdi_max = __STASM_MAX_PARARELL;                                     \
        __stasm_rdi = __stasm_rdi_stack[__stasm_rdi_max];                                         \
        __STASM_SHARE_STACK_CLEAR(rdi,uint64_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_rdi(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_rdi(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_rdi(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_rdi(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_rdi(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_rdi(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_rdi(mode,...)                                                 \
                "rdi",                                                          \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(x64,uint64_t)
//---------------------------------------------------------------------------
#define __TINIT_x64(mode,...)                                                   \
        __stasm_x64 = __STASM_MAX_PARARELL;                                         \
        __stasm_x64 = __stasm_x64_stack[__stasm_x64_max];                                         \
        __STASM_SHARE_STACK_CLEAR(x64,uint64_t)                                 \
        __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_s_x64(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_x64(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_x64(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_x64(mode,...)                                                 \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_x64(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_x64(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_x64(mode,...)                                                 \
                "rax", "rbx", "rcx", "rdx", "rsi", "rdi",                       \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(x64r08,uint64_t)
//---------------------------------------------------------------------------
#define __TINIT_x64r08(mode,...)                                                 \
        __stasm_x64r08_max = __STASM_MAX_PARARELL;                                   \
        __stasm_x64r08 = __stasm_x64r08_stack[__stasm_x64r08_max];                                   \
        __STASM_SHARE_STACK_CLEAR(x64r08,uint64_t)                               \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_x64r08(mode,...)                                              `\
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_x64r08(mode,...)                                              `\
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_x64r08(mode,...)                                              `\
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_x64r08(mode,...)                                              `\
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_x64r08(mode,...)                                              `\
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_x64r08(mode,...)                                              `\
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_x64r08(mode,...)                                              `\
                "r8lb","r8hb","r8w","r8d","r8",                                `\
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(x64r09,uint64_t)
//---------------------------------------------------------------------------
#define __TINIT_x64r09(mode,...)                                                 \
        __stasm_x64r09_max = __STASM_MAX_PARARELL;                                   \
        __stasm_x64r09 = __stasm_x64r09_stack[__stasm_x64r09_max];                                   \
        __STASM_SHARE_STACK_CLEAR(x64r09,uint64_t)                               \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_x64r09(mode,...)                                               \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_x64r09(mode,...)                                               \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_x64r09(mode,...)                                               \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_x64r09(mode,...)                                               \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_x64r09(mode,...)                                               \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_x64r09(mode,...)                                               \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_x64r09(mode,...)                                               \
                "r9lb","r9hb","r9w","r9d","r9",                                 \
                __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(x64rxx,uint64_t)
//---------------------------------------------------------------------------
#define __TINIT_x64rxx(mode,...)                                                \
        __stasm_x64rxx_max = __STASM_MAX_PARARELL;                                  \
        __stasm_x64rxx = __stasm_x64rx_stack[__stasm_x64xx_max];                                    \
        __STASM_SHARE_STACK_CLEAR(x64rxx,uint64_t)                              \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_x64rxx(mode,...)                                              \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_x64rxx(mode,...)                                              \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_x64rxx(mode,...)                                              \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_x64rxx(mode,...)                                              \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_x64rxx(mode,...)                                              \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_x64rxx(mode,...)                                              \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_x64rxx(mode,...)                                              \
                "r10lb","r10hb","r10w","r10d","r10",                            \
                "r11lb","r11hb","r11w","r11d","r11",                            \
                "r12lb","r12hb","r12w","r12d","r12",                            \
                "r13lb","r13hb","r13w","r13d","r13",                            \
                "r14lb","r14hb","r14w","r14d","r14",                            \
                "r15lb","r15hb","r15w","r15d","r15",                            \
                __stasm_r_##mode(__VA_ARGS__)
#endif
//---------------------------------------------------------------------------
#ifdef __MMX__
#if !defined(__STASM_DO_NOT_WARN__) & !defined(mmx_warned)
    #define mmx_warned
    #warning        "It seems that your CPU has got MMX, make sure to add compiler option "-mmmx" then you can use __stasm::mmx"
#endif
#include  <mmintrin.h>
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(mmx,__m64)
//---------------------------------------------------------------------------
#define __TINIT_mmx(mode,...)                                                   \
        __stasm_mmx_max = __STASM_MAX_PARARELL;                                     \
        __stasm_mmx = __stasm_mmx_stack[__stasm_mmx_max];                                         \
        __STASM_SHARE_STACK_CLEAR(mmx,__m64)                                    \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_mmx(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_mmx(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_mmx(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_mmx(mode,...)                                                 \
                "emms\n"                                                        \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_mmx(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_mmx(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_mmx(mode,...)                                                 \
                "mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7",                \
                __stasm_r_##mode(__VA_ARGS__)
#endif
//---------------------------------------------------------------------------
#ifdef __SSE__
#if !defined(__STASM_DO_NOT_WARN__) & !defined(sse_warned)
    #define sse_warned
#warning        "It seems that your CPU has got SSE, make sure to add compiler option  "-msse" then you can use __stasm::sse"
#endif
#include <xmmintrin.h>
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(sse,__m128)
//---------------------------------------------------------------------------
#define __TINIT_sse(mode,...)                                                   \
        __stasm_sse_max = __STASM_MAX_PARARELL;                                     \
        __stasm_sse = __stasm_sse_stack[__stasm_sse_max];                                           \
        __STASM_SHARE_STACK_CLEAR(sse,__m128)                                   \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_sse(mode,...)                                                 \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_sse(mode,...)                                                 \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_sse(mode,...)                                                 \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_sse(mode,...)                                                 \
                "emms\n"                                                        \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_sse(mode,...)                                                 \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_sse(mode,...)                                                 \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_sse(mode,...)                                                 \
                "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7",                                \
                __stasm_r_##mode(__VA_ARGS__)
#endif
//---------------------------------------------------------------------------
#if defined(__SSE2__)
#if !defined(__STASM_DO_NOT_WARN__) & !defined(sse2_warned)
    #define sse2_warned
    #warning    "It seems that your CPU has got SSE2, make sure to add compiler option "-msse2" then you can use __stasm::sse2"
#endif
#include <emmintrin.h>
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(sse2,__m128d)
//---------------------------------------------------------------------------
#define __TINIT_sse2(mode,...)                                                  \
        __stasm_sse2_max = __STASM_MAX_PARARELL;                                    \
        __stasm_sse2 = __stasm_sse2_stack[__stasm_sse2_max];                                      \
        __STASM_SHARE_STACK_CLEAR(sse2,__m128d)                                 \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_sse2(mode,...)                                                \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_sse2(mode,...)                                                \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_sse2(mode,...)                                                \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_sse2(mode,...)                                                \
                "emms\n"                                                        \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_sse2(mode,...)\
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_sse2(mode,...)                                                \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_sse2(mode,...)                                                \
                "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7",        \
                __stasm_r_##mode(__VA_ARGS__)
#endif

#if defined(__SSE3__) || defined(__SSEE3__)
#if !defined(__STASM_DO_NOT_WARN__) & !defined(sse3_warned)
    #define sse3_warned
    #warning    "It seems that your CPU has got SSE3, make sure to add compiler option "-msse3" then you can use __stasm::sse3"
#endif
#include <pmmintrin.h>
#ifdef __SSEE3__
#include <tmmintrin.h>
#endif
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(sse3,__m128i)
//---------------------------------------------------------------------------
#define __TINIT_sse3(mode,...)                                                  \
        __stasm_sse3_max = __STASM_MAX_PARARELL;                                    \
        __stasm_sse3 = __stasm_sse3_stack[__stasm_sse3_max];                                      \
        __STASM_SHARE_STACK_CLEAR(sse3,__m128i)                                 \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_sse3(mode,...)                                                \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_sse3(mode,...)                                                \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_sse3(mode,...)                                                \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_sse3(mode,...)                                                \
                "emms\n"                                                        \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_sse3(mode,...)                                                \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_sse3(mode,...)                                                \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_sse3(mode,...)                                                \
                "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7",        \
                __stasm_r_##mode(__VA_ARGS__)
#endif

#if defined(__SSE4A__) || defined(__SSE41__) || defined(__SSE42__)
#ifdef __SSE4A__
#include <ammintrin.h>
#endif
#ifdef __SSE41__
#include <smmintrin.h>
#endif
#ifdef __SSE42__
#include <nmmintrin.h>
#endif
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(sse4,__m128i)
//---------------------------------------------------------------------------
#define __TINIT_sse4(mode,...)                                                      \
        __stasm_sse4_max = __STASM_MAX_PARARELL;                                        \
        __stasm_sse4 = __stasm_sse4_stack[__stasm_sse4_max];                                          \
        __STASM_SHARE_STACK_CLEAR(sse4,__m128i)                                     \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_sse4(mode,...)                                                    \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_sse4(mode,...)                                                    \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_sse4(mode,...)                                                    \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_sse4(mode,...)                                                    \
                "emms\n"                                                            \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_sse4(mode,...)                                                    \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_sse4(mode,...)                                                    \
                __stasm_i_##mode(__VA_ARGS__)
#if !defined(__x86_64__)
#define __stasm_r_sse4(mode,...)                                                    \
                "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7",            \
                __stasm_r_##mode(__VA_ARGS__)
#else
#define __stasm_r_sse4(mode,...)                                                    \
                "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7",            \
                "xmm8","xmm9","xmm10","xmm11","xmm12","xmm13","xmm14","xmm15",      \
                __stasm_r_##mode(__VA_ARGS__)
#endif
#endif
//---------------------------------------------------------------------------
#ifdef __AVX__
#if !defined(__STASM_DO_NOT_WARN__) & !defined(avx_warned)
    #define avx_warned
    #warning        "It seems that your CPU has got AVX, make sure to add compiler option "-mavx" then you can use __stasm::avx"
#endif
#include <immintrin.h>
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(avx,__m256)
//---------------------------------------------------------------------------
#define __TINIT_avx(mode,...)                                                       \
        __stasm_avx_max = __STASM_MAX_PARARELL;                                         \
        __stasm_avx = __stasm_avx_stack[__stasm_avx_max];                                               \
        __STASM_SHARE_STACK_CLEAR(avx,__m256)                                       \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_avx(mode,...)                                                     \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_avx(mode,...)                                                     \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_avx(mode,...)                                                     \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_avx(mode,...)                                                     \
                "emms\n"                                                            \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_avx(mode,...)                                                     \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_avx(mode,...)                                                     \
                __stasm_i_##mode(__VA_ARGS__)
#if !defined(__x86_64__)
#define __stasm_r_avx(...)                                                          \
                "ymm0","ymm1","ymm2","ymm3","ymm4","ymm5","ymm6","ymm7",            \
                __stasm_r_##mode(__VA_ARGS__)
#else
#define __stasm_r_avx(...)                                                          \
                "ymm0","ymm1","ymm2","ymm3","ymm4","ymm5","ymm6","ymm7",            \
                "ymm8","ymm9","ymm10","ymm11","ymm12","ymm13","ymm14","ymm15",      \
                __stasm_r_##mode(__VA_ARGS__)
#endif
#endif
//---------------------------------------------------------------------------
#ifdef __AES__
#if !defined(__STASM_DO_NOT_WARN__) & !defined(aes_warned)
    #define aes_warned
    #warning        "It seems that your CPU has got AES, make sure to add compiler option "-maes" then you can use __stasm::aes"
#endif
#include <wmmintrin.h>
//---------------------------------------------------------------------------
__STASM_SHARE_STACK_DEFINE(aes,__m256)
//---------------------------------------------------------------------------
#define __TINIT_aes(mode,...)                       \
        __stasm_aes_max = __STASM_MAX_PARARELL;         \
        __stasm_aes = __stasm_aes_stack[__stasm_aes_max];            \
        __STASM_SHARE_STACK_CLEAR(aes,__m256)       \
        __TINIT_##mode(__VA_ARGS__)
#define __stasm_s_aes(mode,...)                     \
                __stasm_s_##mode(__VA_ARGS__)
#define __stasm_t_aes(mode,...)                     \
                __stasm_t_##mode(__VA_ARGS__)
#define __stasm_l_aes(mode,...)                     \
                __stasm_l_##mode(__VA_ARGS__)
#define __stasm_x_aes(mode,...)                     \
                __stasm_x_##mode(__VA_ARGS__)
#define __stasm_o_aes(mode,...)                     \
                __stasm_o_##mode(__VA_ARGS__)
#define __stasm_i_aes(mode,...)                     \
                __stasm_i_##mode(__VA_ARGS__)
#define __stasm_r_aes(...)                          \
                __stasm_r_##mode(__VA_ARGS__)
#endif
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
#define __TINIT(mode,...) __TINIT_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __TLOCK(mode)                                           \
        while (__stasm_##mode##_max==0) stk::time::wait_us(1);      \
        __stasm_##mode =(__stasm_##mode##_stack[__stasm_##mode##_max--]);
//---------------------------------------------------------------------------
#define __TUNLOCK(mode)                                         \
        __stasm_##mode =(__stasm_##mode##_stack[++__stasm_##mode##_max]);
//---------------------------------------------------------------------------
#define __stasm(mode,...)                           \
        __asm__ __volatile__(                       \
        __STASM_INLINE_SYNTAX                       \
         __stasm_s_##mode(__VA_ARGS__) "\n"         \
         __stasm_t_##mode(__VA_ARGS__) "\n"         \
         __stasm_l_##mode(__VA_ARGS__) "\n"         \
         __stasm_x_##mode(__VA_ARGS__) "\n"         \
        __GCC_INLINE_SYNTAX                         \
 :      __stasm_o_##mode(__VA_ARGS__)               \
 :      __stasm_i_##mode(__VA_ARGS__)               \
 :      __stasm_r_##mode(__VA_ARGS__)               \
        );
//---------------------------------------------------------------------------
#define __stasmex(mode,...)                         \
        __asm__ __volatile__(__STASM_INLINE_SYNTAX  \
        __stasm_s_##mode(__VA_ARGS__) "\n"          \
        __stasm_t_##mode(__VA_ARGS__) "\n"          \
        __stasm_l_##mode(__VA_ARGS__) "\n"          \
        __stasm_x_##mode(__VA_ARGS__) "\n"          \
        __GCC_INLINE_SYNTAX
//---------------------------------------------------------------------------
#define __stasmex_i(var)                            \
        : [var]  "r" (&var)
//---------------------------------------------------------------------------
#define __stasmex_o(var)                            \
        : [var] "=r" (&var)
//---------------------------------------------------------------------------
#define __stasmex_v(...)                            \
        : __VA_ARGS__
//---------------------------------------------------------------------------
#define __stasmex_code(...)                         \
        : __VA_ARGS__
//---------------------------------------------------------------------------
#define __stasmex_auto(mode,...)                    \
        __stasm_r_##mode(__VA_ARGS__)
//---------------------------------------------------------------------------
#define __stasm_ex_end                              \
        );
//---------------------------------------------------------------------------
#pragma GCC diagnostic pop
#endif
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------


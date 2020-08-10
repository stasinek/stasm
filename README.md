# __stasm(...)
### The inline assembly wrapper for GCC and Clang C/C++ compiler. Made for human beings.

Hi, my name is Stanley. First of all, this are my first steps using GitHub(and Git also) and also first steps in C++ since many years. Plus english is not my primary language. So i can't guarantee even the readme to be usefull. 

Actually this easy-peasy macro tool is daily working with MinGW 4.9 and Clang3.7 (windows and linux gcc) can work with any compiler that have simmilar inline asembly syntax as GCC. Just get it import .c file to your project makefile and include .h to any unit you want use asembly then use the macros __stasm() instead asm(""). It's working for targeting all x386, x86-64 processors with all of those MMX,3Dnow, SSE, AVX extensions. Could work with ARM (this was made but wasn't tested). Could work also with any other processor and it need to be adapted exclusivly one by one. This would be done in future.

I need mention this is still alpha version of "wrapper" for asm volatile("":::); i wanted to have. Wanted to have using just C preprocessor, without need for any special meta compilers or something like that. Just pure C preprocessor capable for __VA_ARGS__, glue ## and #. Macros has ability to make inline assembly, a lot easier and intuitive for any C programmer that wanted to learn asembly without painfull GCC/GAS AT&T style. Could be usefull for testing and debuging the code. Then after testing you can copy-paste a whole functional block into prepared extern "C" function, obscuficate blocks with specific calling conventions and let it compile by any external asembler such as GAS, FASM, YASM, NASM. With stasm you can copy, copy-back the code without many effort. Stasm inline assembly could be as easy as in Visual Studio or Borland __asm {} with Intel specific syntax:

~~~{.cpp}
__stasm(
       	mov eax,0x00000001;
        xor ecx,ecx;
        cpuid;
        mov ecx,0x00000001;
        shl ecx,27;
        and ecx,edx;
        xor eax,eax;
        xor edx,edx;

       test ecx,ecx;
         jz tsc_exit;
        mov eax,1;
        cpuid;
        jmp tsc_intel_measure;
        tsc_amd_measure:\n
        sfence;
        rdtsc;

        tsc_intel_measure:\n
        sfence;
        rdtsc;

       tsc_exit:\n
        mov edi,s_x86;
        mov [edi+0],eax;
        mov [edi+4],edx;
)
~~~ 
If you want to debug(make traps for specific line) instead C styled line separator ';' (actually comment separator in GAS) you must put code for next line '\n':

~~~{.cpp}
__stasm(
	... \n
	mov [edi+0],eax \n 
)
~~~

With __stasm You can copy-paste code from and to Visual Studio, Borland asm {} block. Stasm can also simplify work with processor multimedia extensions registers and simplify using of local variables that is painfull in GCC inline asm. Stasm can simplify it by using special words written just before actual code. The options are:

~~~{.asm}
save,var,label,x86, eax, ebx, ecx, edx, esi, edi, x87, x64, x64r89, x64rxx, mmx, sse, sse2, sse3, sse4, avx, aes
~~~

You could write one of them or all of them altogether (sepparated with comma) just before your asm code begins remembering only to add "code" word before actual asembly. That's it - magic would happen automaticaly. If you write, a single "mmx" for example, macro will automatically generate warn message for you to add -mmx option into GCC commmand line and will automatically include headers needed for C to exchange data types speciffic to MMX. Will also add special clobbers into asm() clobber list. Will create additional static variables needed for painless data sharing - migration from Microsoft C compiler to GCC. For example "x86" means all x86 registers that userspace program can reach would be added to clobbers, x64r89 means r8+r9, x64rxx means all from r10 to r15. But you can write just one of them. Intuitive as defining local variables in C functions. Opposite to GCC inline where those are defined at end.

Because those simple words written just as i have listed are affecting so called "clobbers" GCC could know what registers you want to use so will not put local variables onto them. If you don't want to affect GCC compilation process that potentially slows down the C code part. You can tell __stasm to save them for you on the stac in way simmilar for calling a C function. Hidden from your attencion. It can be done by adding magical word "save" before those register names i had mentioned as an additional sub option: "save,register_name" instead "register_name". It would make "push register" "pop register" pair in your code or even fnsave,fsave,xsave pair before and after your code block. (lot of kB without need to learn how much data is needed - i had done it for you!) This stack way would not break GCC optimisation for local variables, and you can forget to push them and pop from stack just in the same way as in MSVC and Borland C asm {}. 

If you want fnsave,fxsave,xsave state of FPU, SSE, AVX register state made by C code you just have to write: save,x87,save,sse,save,avx and even dont bother memory assignment or variable to hold it. 

##But that's not whole magic: Variables. 

As you may know GCC inline asembly code can't reach local variables directly by it's name. It's not simple as in Microsoft _asm{} block. It's harder mostly because asm(""); block is almost blindly passed by GCC to GAS .s as string just before the end of C compilation process. Rather than bidirectional analizer GCC is simply copy-paster. In addition because of linker nameing conventions (underscore added to variable names), namespaces, classes it's even more complicated. That's why even special GCC extended inline options for getting access to static variables is vary painfull. Of course with stasm i had resolved this problem it a little :) 

There is magical spell for variables: "var". If you put this before a variable name (separated by comma from "var") Instead of using: unnamed variable "%1" in asembly code, you could write intuitive "mov you_named_in_C, eax; 

### But there is limitation:
Extended syntax using "var's" and "save" is currently not working for two or more variables.
This need to be fixed if it's can be fixed? In another attempt i will try create additional macros named:
~~~
use(...), save(...), var(...), label(...)
~~~
For now options had to be separated by comma 'var,"+r",var_name,save,eax,code', with additional macros can form something like this:
~~~
c_goto_label:
scanf("Please write a number bigger than 0: %d",c_var_name);
stasm(save(eax,ebx,ecx),var(+r,c_var_name),label(c_goto_label),code(
	mov eax,c_var_name; 
	jnz c_goto_label;
))
~~~
##GAS limitations and differences to asm{}

As you can see there is additional "+r" word before sub option "var". You need to inform how GAS should thread name you provided. Details about it can be found in GAS extended syntax manual. https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
You need to remember at least "+r" because is adequate to most common global/local variable [EBP-some_unique_number] or [ESP-some_other_number]. The code written as: 'lea esi,c_variable' to variable defined as +r will always move variable address, to ESI not it's value whatever it's value or pointer. Oposite to Borland C and MSVC by with is detected automatically. To retrive variable you need to: mov eax,[esi]; 

Why is that? Names for local variables passed to inline asembly are replaced by [register-some values] in simmilar way as printf("mov %s,%s","[register1]","[register2]"); That's why also this Microsoft trick: mov eax,[ponter+index]; won't work because it will produce code that looks like this: [[register-index]+index]. Witch is wrong cant be undestanded by GAS. No AGU operations can't be done by those imported c_variable. It can be used only to import values from C code.

#Another bad information. Due the GCC bugs...
`
Again invalidated by bugzilla guys macro is not as useful and intuitive as could be.
[Ticket number: 65897](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65897)
[Ticket number: 65898](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65898)
`
Var has no chance working well with GCC without -masm=intel in compiler command line options. All because AT&T %
But it is working well in Clang inline asm() without passing any additional options to compiler, and it was tested to without problems... I was just like holy-grall for me when i tested it. Asembly just as it should be local variables visible to inline asembler. To get this working in GCC (share data between C code and asm) i have created global variables protected by locking mechanism(for multithreading safety) as temporary feature. It is currently only one used by me for now.

Meantime you should not use var's but use static share "stacks", created automaticly for you as you type x86,x87 options to stasm. Those variables would be named: s_x86, s_x87 created at minimum size of 1 cache line 64 bytes. Also because of that exchanged buffers can be shared by separated stasm block. The type of those variables seen from C code is automatically definied adequate to type of extension you will declare to use: registers 8,16,32 bytes wide. Literally arrays of variables in number of 8,16,32 of them. It's not too much RAM and adequate to common number: 8,16,32 of XMM registers - don't you think?;)

In actual form macro sucesfully helped me to switch from obsolete Borland C++ 6 inline asm {} into QT Creator + MinGW. Change usles asembly code into C code step by step. Move some code to external .s file compiled as it should be.. This is not ultimate answer just, a easy tool to make GNU GAS human friendly. 

#Static? What about multithreading?

Static and global variables are bad for many reasons even in single threaded program(cache miss). In multithreaded should be always protected by locks,unlock mechanism. Anyway i think there are some advantage over local variables for couple reasons. Common data generates less code/data size so can easly fit in cache. (in other words: cache hit probability will increases) Althought locking mechanism makes the oposite to what asm inline asembly is made for. But anyway locking semaphores(counting from MAX_PARARELL to 0) were created for multithread safety and it works! You could safety use __stasm inline in up to MAX_PARARELL=12 threads. The biggest disadventage you should know is that it can create synchronizing barier for you multithreaded code. And it is very risky.. specially in recrusion ;) Think about this. This is important!

Althoug global variables are for easy exchange data. Im working to avoid them. Bugzilla is not helping me do what i want to do just now, so i must use them... This is not good idea(makes asm opposite to program speedup) but it just works for GCC. Clang don't need them at all because local variables are working well.

Actually in GCC inline asembly it's copy-pasted by GCC onto the GAS compiler stage. GAS has nothing to do with inline asembly i don't blame it for bugs. But it's a part of GNU GCC and it's not for human beings. I hope someone will change this then stasm would work as charm without static globals.

~~~{.cpp}
 asm("i hate gas universalism fuck %gnu\n"
						"some other hatres words\n"
						: input : output or whatever i hate that : registers hate to be used : goto shitty labels);
~~~

With globals you could easly write(for weird example):

~~~{.asm}
	s_x86[0] = 13; // input my lovely number for test 
	__stasm(x86,code,
		mov esi, s_x86;
		mov eax,[esi];
		imul eax,3;
		mov edi, s_x86;
		add edi, 4;
		mov [edi],eax;
	);
	int another_c_variable = s_x86[1];
	assert(s_x86[1]==3*13);
~~~

You may say: Using common GCC inliner the generated could be only 1 liner: imul %1,3; GCC would create mov eax,[ebs-8]; or whatever.. it would be a lot faster and have much more sense. 
Yeah You right, You got me, but for me it was the only way for painless migration from something like that(althought it's stupit):

~~~{.cpp}
	int __stdcall somefunc(int variable){
		__asm {
		mov eax,variable;
		imul eax,3;
		mov variable,eax;
		}
		return variable;
	}
~~~

Into previously presented form, by just copy-paste blocks of asm code. You may say, I should copy them into ".s" files, "offline" asembly. Learn great GCC manual or one of many asms compilers: NASM,YASM,FASM.
Of course. I did at first attempt in QT Creator by modifying qmakefile:
	
~~~{.pro}
#NASM.output  = ${QMAKE_FILE_BASE}_asm.o
#NASM.commands = c:/nasm/nasm -f elf -g -F dwarf --prefix ${PWD}${QMAKE_FILE_NAME} -o ${PWD}/${QMAKE_FILE_OUT}
#NASM.input = ASM_SOURCES
#YASM.output  = ${QMAKE_FILE_BASE}_asm.o
#YASM.commands = c:/YASM/YASM -f elf -g dwarf2 -w -o ${QMAKE_FILE_OUT} ${PWD}${QMAKE_FILE_NAME}
#YASM.input = ASM_SOURCES
#FASM.output  = ${QMAKE_FILE_BASE}_asm.o
#FASM.commands = c:/fasm/fasm  ${PWD}${QMAKE_FILE_NAME} ${QMAKE_FILE_OUT}
#FASM.input = ASM_SOURCES
#QMAKE_EXTRA_COMPILERS += YASM
#ASM_SOURCES += ../../../x86_libraries/SSTSOFT/cpu/tsoft_cpuid_nasm.asm\
#ASM_SOURCES += ../../../x86_libraries/SSTSOFT/mem/tsoft_mem32_nasm.asm\
#ASM_SOURCES += ../../../x86_libraries/SSTSOFT/text/tsoft_cstr_nasm.asm\
#ASM_SOURCES += ../../../x86_libraries/SSTSOFT/time/tsoft_time_nasm.asm
~~~
	
I had done it because if you want to compile .s file by GAS you need add GCC option to make stage files. So external asembler is much more usefull. When i tried to utilize them then i had realized that whatever asembler is. I don't want to learn linker nameing convention at all, change all C functions into extern "C". I don't want to learn macro languages so nice as in FASM/Fresh IDE - C is one of them for christ sake! I don't want to change whole C code into asm whatever great ASM compiler wouuld be. I don't want to use __buildin, __instrict or whatever it's called. I want just use sometimes test an pure inline asembly fragments. Mostly because i love to code in C, even if it's stupud and feel how processor works by using asm! I love to code! It makes me think how processor works how it can be speeded up after all learn how to write better C code or algorithms. Plus in embedded world it's very usefull specially to adapt some asm code into C program. Probably i will soon adapt __stasm to AVR8 microcontrollers.

Before __stasm was made i had started to ask myself: Why create external files when i can use #ifdefs for x86, x86-64,ARM and compile single C file without so much files? Why i should learn linker nameing conventions, line by line modifying __calling conventions? Let C compiler do this for me magically. I wanted GNU to bring my asm{} simplicity and freedom! But instead of that GNU guys makes the oposite. I know why - this is much more universal, faster, better. But however it is I have decided that I will never, ever, learn GCC dumb inline assembly. No other human should. I Hate GCC since my first attempt in WinAVR. It was awful experience. Actually GCC style had destroyed Atmel Studio and Embarcadero RAD's

Hopefully Clang has experimental support for asm {} so.. there is a hope i wont be forced to it anymore. I know there are  "+r","r" faster better ways.. But it's not intuitive as it could be. BCC32 is very old, Pelles C is only C compiler.. Clang is not working well with Qt Creator, Windows SDK with MSVC Windows SDK is Windows only so.. __stasm was the only way to focus on different aspects of code than fighting against AT&T. Rather than hateres.

##	Here it is, as it is.

	Maybe you could make it better, so give me a sign.
	I don't know what im done wrong with vars(actually "dim" i wil change that in .cpp further). 
	It worked like a charm but suddenly i have make some mistake? Sadly it was before GIT repo created
	Help me if you can to bring it back.
	Cheers.

#License

Just as Linus Torvalds does i think GPL3 license is wrong, but GPL2 is problematic also, public could be best but i want to be forgeted as an original author of this concepr. The copyright license for __stasm is Berkley Standard Distribution clause 2 license:

~~~{.txt}
Copyright (c) 2015, Stanisław Stasiak
sstsoft.pl
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
~~~

Write me an github message if you liked it. ;)
Feel free to commit.
You should also check my incbin repo.. another trick (not mine concept) in this case GAS can be very is usefull :)

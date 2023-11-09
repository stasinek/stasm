#
# Qmake Qt5.5.1 file last modified 2018.5, Windows MSC2010,MinGW4.92,LLVM3.7 builds tester
#
DEFINES += BUILD_STK_LIBRARY
# -------------------------------------------------------------
# CONFIG: LIBRARY: STATIC, DLL OR EXE: TESTING.APP
# -------------------------------------------------------------
contains(DEFINES, BUILD_STASM_LIBRARY) {
message("TARGET stasm.DLL")
message($$DEFINES)
    TARGET = stasm
    TEMPLATE = lib
#    CONFIG += staticlib
    CONFIG += embed_manifest_dll
    CONFIG += dll
    CONFIG -= warn_on
    CONFIG += warn_off
}
else {
message("TARGET Example application stasm.exe")
    TARGET  = stasm_example
    TEMPLATE = app
    CONFIG += console
    CONFIG += embed_manifest_exe
    CONFIG -= warn_on
    CONFIG += warn_off
}
# -------------------------------------------------------------
CONFIG += precompiled_header
# -------------------------------------------------------------
# MESSAGE ABOUT TARGET
# -------------------------------------------------------------
message(QMAKESPEC:$$QMAKESPEC)
message(CONFIG:$$CONFIG)

contains(TEMPLATE, dll) {
    contains(CONFIG, dll) {
        message($$TARGET".dll")
    }
    contains(CONFIG, static) {
        message("lib"{$$TARGET}".lib")
    }
}
# -------------------------------------------------------------
# MORE CONFIG
# -------------------------------------------------------------
QT -= core gui
CONFIG -= qt
CONFIG += precompile_header
#CONFIG -= static
CONFIG += warn_on
CONFIG += exceptions
CONFIG += c++11
contains(QMAKE_COMPILER_DEFINES,_MSC_VER) {
# -------------------------------------------------------------
# Microsoft Visual C++
# -------------------------------------------------------------
message("Using MSVC Microsoft Compiler QMAKE_FLAGS")
QMAKE_CXXFLAGS -= /W1
QMAKE_CXXFLAGS -= /W2
QMAKE_CXXFLAGS -= /W3
QMAKE_CXXFLAGS -= /W4
QMAKE_CXXFLAGS_RELEASE += /w
QMAKE_CXXFLAGS_DEBUG += /W0
QMAKE_CXXFLAGS_RELEASE += /arch:SSE2
} else {
contains(QMAKE_COMPILER_DEFINES,__clang__) {
# -------------------------------------------------------------
# LLVM - Clang
# -------------------------------------------------------------
message("Using LLVM Clang Compiler QMAKE_FLAGS")

WARNINGS_LLVM += -Wextra
WARNINGS_LLVM += -Wunknown-pragmas -Wundef
WARNINGS_LLVM += -Wold-style-cast
WARNINGS_LLVM += -Wdisabled-optimization -Wstrict-overflow=4
WARNINGS_LLVM += -Weffc++ #-Wuseless-cast
WARNINGS_LLVM += -Winit-self -Wpointer-arith
WARNINGS_LLVM += -Wlogical-op
WARNINGS_LLVM += -Wunsafe-loop-optimizations -Wno-error=unsafe-loop-optimizations
WARNINGS_LLVM += -Wno-write-strings -Wno-multichar
WARNINGS_LLVM += -Wno-ignored-attributes

NO_WARNINGS_LLVM += -Wno-write-strings -Wno-multichar
NO_WARNINGS_LLVM += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-label
NO_WARNINGS_LLVM += -Wno-unused-but-set-variable
NO_WARNINGS_LLVM += -Wattributes -Winline
NO_WARNINGS_LLVM += -Wunknown-pragmas
NO_WARNINGS_LLVM += -Wno-ignored-attributes
NO_WARNINGS_LLVM += -Wattributes -Winline -Wshadow -Wall
#NO_WARNINGS_LLVM += -fverbose-asm
#NO_WARNINGS_LLVM += -fstrict-aliasing
#NO_WARNINGS_LLVM += -dD
#NO_WARNINGS_LLVM += -g
NO_WARNINGS_LLVM += -std=gnu++0x -pthread
NO_WARNINGS_LLVM += -Qunused-arguments -Wno-error=unused-command-line-argument-hard-error-in-future

FLAGS_LLVM += -fno-use-linker-plugin
FLAGS_LLVM += -malign-double
FLAGS_LLVM += -momit-leaf-frame-pointer
FLAGS_LLVM += -fwrapv
FLAGS_LLVM += -funroll-loops
FLAGS_LLVM += -m32 -mfpmath=sse -flto -O3
FLAGS_LLVM += -mpreferred-stack-boundary=8
FLAGS_LLVM += -mmmx -msse -msse2
FLAGS_LLVM -= -fno-keep-inline-dllexport
FLAGS_LLVM -= -finline-small-functions
#FLAGS_LLVM -= -pipe
#FLAGS_LLVM += -save-temps

# C++ FLAGS
QMAKE_CXXFLAGS += $$FLAGS_LLVM
QMAKE_CXXFLAGS_RELEASE += $$NO_WARNINGS_LLVM
QMAKE_CXXFLAGS_DEBUG += $$WARNINGS_LLVM
QMAKE_CXXFLAGS += clang++

# Pure C FLAGS
QMAKE_CFLAGS   += $$FLAGS_LLVM
QMAKE_CFLAGS_RELEASE += $$NO_WARNINGS_LLVM
QMAKE_CFLAGS_DEBUG += $$WARNINGS_LLVM
QMAKE_CFLAGS += clang

# Linker FLAGS
QMAKE_LFLAGS += -Qunused-arguments -Wno-error=unused-command-line-argument-hard-error-in-future
QMAKE_LFLAGS -= -mthreads
} else {
contains(QMAKE_COMPILER_DEFINES,__GNUC__) {
# -------------------------------------------------------------
# MINGW / GCC
# -------------------------------------------------------------
message("Using MinGW/GCC QMAKE_FLAGS")

WARNINGS_GNU += -Wextra
WARNINGS_GNU += -Wunknown-pragmas -Wundef
WARNINGS_GNU += -Wold-style-cast
WARNINGS_GNU += -Wdisabled-optimization -Wstrict-overflow=4
WARNINGS_GNU += -Weffc++ #-Wuseless-cast
WARNINGS_GNU += -Winit-self -Wpointer-arith
WARNINGS_GNU += -Wlogical-op
WARNINGS_GNU += -Wunsafe-loop-optimizations -Wno-error=unsafe-loop-optimizations
WARNINGS_GNU += -Wno-write-strings -Wno-multichar

NO_WARNINGS_GNU += -Wno-write-strings -Wno-multichar
NO_WARNINGS_GNU += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-label
NO_WARNINGS_GNU += -Wno-unused-but-set-variable
NO_WARNINGS_GNU += -Wattributes -Winline
NO_WARNINGS_GNU += -Wunknown-pragmas
NO_WARNINGS_GNU += -Wattributes -Winline -Wshadow -Wall
#NO_WARNINGS_GNU += -fverbose-asm
#NO_WARNINGS_GNU += -fstrict-aliasing
#NO_WARNINGS_GNU += -dD
#NO_WARNINGS_GNU += -g
NO_WARNINGS_GNU += -pthread

FLAGS_GNU += -fno-use-linker-plugin
FLAGS_GNU += -malign-double
FLAGS_GNU += -momit-leaf-frame-pointer
FLAGS_GNU += -fwrapv
FLAGS_GNU += -funroll-loops
FLAGS_GNU += -m32 -mfpmath=sse -flto -O3
FLAGS_GNU += -mpreferred-stack-boundary=8
FLAGS_GNU += -mmmx -msse -msse2
FLAGS_GNU += -save-temps

QMAKE_CXXFLAGS_DEBUG += $$FLAGS_GNU $$WARNINGS_GNU -std=gnu++0x
QMAKE_CXXFLAGS_RELEASE += $$FLAGS_GNU $$NO_WARNINGS_GNU -std=gnu++0x
QMAKE_CXXFLAGS_DEBUG -= -pipe
QMAKE_CXXFLAGS_RELEASE -= -pipe

QMAKE_CFLAGS_DEBUG += $$FLAGS_GNU $$WARNINGS_GNU
QMAKE_CFLAGS_RELEASE += $$FLAGS_GNU $$NO_WARNINGS_GNU
QMAKE_CFLAGS_DEBUG -= -pipe
QMAKE_CFLAGS_RELEASE -= -pipe
}}}
# -------------------------------------------------------------
#
# Common section SOURCES, HEADERS, OTHER FILES
#
# -------------------------------------------------------------
SOURCES += \
    example/variables.cpp\
    stk_stasm.cpp
# -------------------------------------------------------------
#
# Both GCC and LLVM
#
# -------------------------------------------------------------
contains(QMAKE_COMPILER_DEFINES,__clang__) {
message("LLVM GAS Assembler SOURCES")
SOURCES+=
#    gas.s
} else {
contains(QMAKE_COMPILER_DEFINES,__GNUC__) {
message("GNUC GAS Asembler SOURCES")
SOURCES+=
#    gas.s
} else {
contains(QMAKE_COMPILER_DEFINES,_MSC_VER) {
message("Microsoft Visual C++ MASM SOURCES")
SOURCES+=
#    gas.s
}}}
# -------------------------------------------------------------
#PRECOMPILED_HEADER += stk_MAIN.h
HEADERS += \
    stk_stasm.h
# -------------------------------------------------------------
# QT GUI
# -------------------------------------------------------------
contains(DEFINES, QT_GUI) {
message("QT5 GUI specific SOURCE")
CONFIG +=
SOURCES +=
HEADERS +=
FORMS +=
RESOURCES +=
}
# -------------------------------------------------------------
contains(DEFINES, _MSC_VER) {
message("LINKER Microsoft Visual C++ LIBS")
#    LIBS += vcomp.lib
    LIBS += crypt32.lib
    LIBS += kernel32.lib shell32.lib gdi32.lib
    LIBS += user32.lib uuid.lib
    LIBS += wsock32.lib ws2_32.lib
    LIBS += winmm.lib
    LIBS += ole32.lib oleaut32.lib
    LIBS -= gomp.lib
    contains(DEFINES, QT_GUI) {
        LIBS += QtCored.lib
    }
} else {
contains(QMAKE_COMPILER_DEFINES,__clang__) {
message("LLVM LIBS")
    LIBS += crypt32.lib
    LIBS += kernel32.lib shell32.lib gdi32.lib
    LIBS += user32.lib uuid.lib
    LIBS += wsock32.lib ws2_32.lib
    LIBS += winmm.lib
    LIBS += ole32.lib oleaut32.lib
    LIBS -= gomp.lib
    contains(DEFINES, QT_GUI) {
        LIBS += QtCored.lib
    }
} else {
contains(QMAKE_COMPILER_DEFINES,__GNUC__) {
message("GNUC LIBS")
    LIBS += -lkernel32
    LIBS += -lgdi32 -lcomctl32 -lshell32 -luser32 -luserenv
    LIBS += -lws2_32 -lwsock32
    LIBS += -lwinmm -limm32
    LIBS += -lole32 -loleaut32
    LIBS += -ladvapi32
    LIBS += -lcrypt32 -uiid
    LIBS += -lgomp
    contains(DEFINES, QT_GUI) {
        LIBS += QtCored.a
    }
}
}}
# -------------------------------------------------------------
win32: {
message("Windows RC Resource Files RC_FILE")
##    RC_FILE     =  stk_stasm.rc
#-manifest file.manifest
}
OTHER_FILES +=
DISTFILES += test.txt
# -------------------------------------------------------------
message("QMAKE FINISH")
# -------------------------------------------------------------
# External ASM .s compiler
# -------------------------------------------------------------
#QMAKE_CFLAGS_RELEASE += rtti_off stl_off exceptions_off
#QMAKE_CXXFLAGS += -save-temps
#QMAKE_CXXFLAGS += -fverbose-asm
#QMAKE_CXXFLAGS -= -pipe
#moc.depend_command = g++ -E -M ${QMAKE_FILE_NAME} | sed "s,^.*: ,,"
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
#ASM_SOURCES += 	../STK/cpu/stk_cpu_nasm.asm


#postlink.target = .buildfile
#postlink.commands = dlltool -z --output-def,exported.def -l $$postlink.target
#QMAKE_EXTRA_TARGETS += postlink
#QMAKE_POST_LINK += postlink

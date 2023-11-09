#include <iostream>
#include <stk_stasm.h>

using namespace std;

int main()
{
    __stasm_x86[0] = 12; // input data A
    __stasm_x86[1] = 13; // input data, static variable, directly available to __stasm, and protected by atomic lock, unlock!
    __stasm(x86,code,
        mov esi, __stasm_x86;
        mov eax,[esi+0];
        mov ebx,[esi+4];
        imul eax,ebx;
        mov edi, __stasm_x86;
        mov [edi+8],eax;
    );
    int another_c_variable = __stasm_x86[2];

    cout << "Hello World!" << endl;
    cout << __stasm_x86[0] << "*" << __stasm_x86[1] << "=" << __stasm_x86[2] << endl;
    assert(__stasm_x86[2]==12*13);
    cout << "Success!" << endl;
    return 0;
}


// ** Условие **
// Считать 2 числа, положить их на стек и вывести.



//  Содержимое стека  Адреса в памяти
//
//  .                .
//  .                .
//  .                .
//  +----------------+ 0x0000F040
//  |                |
//  +----------------+ 0x0000F044 <-- вершина стека (на неё указывает rsp)
//  |     данные     |
//  +----------------+ 0x0000F048
//  |     данные     |
//  +----------------+ 0x0000F04C
//  .                .
//  .                .
//  .                .
//  +----------------+ 0x0000FFF8
//  |     данные     |
//  +----------------+ 0x0000FFFC
//  |     данные     |
//  +----------------+ 0x00010000 <-- дно стека

    .intel_syntax noprefix
    .global main

main:
    // [ return   xxxxx ]
    //   ^rsp
    call    readi64
    push    rax
    // [ rax1, return   xxxxx ]
    //   ^rsp

    call    readi64
    push    rax
    // [ rax2, rax1, return   xxxxx ]
    //   ^rsp

    // Выведем то, что лежит под rsp (последнее введенное число)
    mov     rdi, [rsp]
    call    writei64
    mov     rdi, [rsp + 8]
    call    writei64

    pop     rdi
    call    writei64
    // [ rax1, return   xxxxx ]
    //   ^rsp

    pop     rdi
    call    writei64
    // [ return   xxxxx ]
    //   ^rsp

    call    finish

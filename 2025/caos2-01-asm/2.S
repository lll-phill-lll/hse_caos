    // Простое использование переменной
    .intel_syntax noprefix
    .global main
    .text
main:
    // [ base + index*scale + displacement ]
    // displacement - константа
    // base - регистр
    // index - регистр
    // scale - 1, 2, 4, 8

    mov     qword ptr [var + rip], 500
    xor     rax, rax

    ret

// В таком коде мы не получится изменить значение var. Так как var находится в секции .text,
// секцию .text менять нельзя. Чтобы в var можно было записать значение, нужно ее переместить 
// в секцию .data.
// .text

.bss
var:
    .skip    5 * 100

// .byte - 1 байт
// .word - 2 байта
// .int  - 4 байта
// .quad - 8 байт


// byte  ptr
// word  ptr
// dword ptr
// qword ptr

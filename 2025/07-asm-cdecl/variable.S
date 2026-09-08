    // Простое использование переменной
    .intel_syntax noprefix
    .global main
    .text
main:
    // [ base + index*scale + disp ]
    mov     qword ptr [var + rip], 500
    mov     rdi, [var + rip]
    call    writei64

    call    finish

// В таком коде мы не получится изменить значение var. Так как var находится в секции .text,
// секцию .text менять нельзя. Чтобы в var можно было записать значение, нужно ее переместить 
// в секцию .data.

.data
var:
    .quad    42

// .byte - 1 байт
// .word - 2 байта
// .int  - 4 байта
// .quad - 8 байт


// byte  ptr
// word  ptr
// dword ptr
// qword ptr

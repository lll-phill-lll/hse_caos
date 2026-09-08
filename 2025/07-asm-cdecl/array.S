    .intel_syntax noprefix
    .global main
main:
    // прочитали индекс в r12
    call    readi64         
    mov     r12, rax


    // Прочитали новое значение в r13
    call    readi64
    mov     r13, rax

    // BREG + IREG * SCALE + OFFSET
    // SCALE = 1, 2, 4, 8

    // абсолютный адрес массива
    // load effective address
    lea     r14, [rip + arr]
    // абсолютный адрес ячейки
    lea     r14, [r14 + 8 * r12]

    mov     rdi, [r14]

    call    writei64

    mov     [r14], r13

    jmp     main

    .bss
arr:
    // 200 8-байтных чисел
    .skip 200 * 8

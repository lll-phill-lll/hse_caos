    .intel_syntax noprefix
    .global main
    .extern multiply_add

main:
    push    rbp
    mov     rbp, rsp

    // multiply_add(A, B)
    mov     rdi, [rip + A]
    mov     rsi, [rip + B]
    // Вызываем функцию, результат будет в rax
    call    multiply_add
    // rax

    // printf("Number: %d\n", rax)
    lea     rdi, [rip + printf_format]
    mov     rsi, rax
    call    printf


    mov     rsp, rbp
    pop     rbp

    xor     rax, rax

    ret

printf_format:
    .string "Number: %d\n"

A:
    .int 111
B:
    .int 2000

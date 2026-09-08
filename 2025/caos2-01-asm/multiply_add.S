// ** Условие **
// Написать функцию, которая примет аргументами a и b, вычислит
// a * 2 + b и вернуть значение

    .intel_syntax noprefix
    .global multiply_add

multiply_add:

    push    rbp
    mov     rbp, rsp

    mov     rax, rdi

    sal     rax, 1
    add     rax, rsi

    mov     rsp, rbp
    // pop     rbp

    ret

// Все библиотечные функции не меняют значения регистров rsp, rbp, rbx, r12, r13, r14, r15.
    .intel_syntax noprefix
    .global main

main:
    // Результат в rax
    call    readi64
    // mov dst, src  dst = src
    mov     rdi, rax

    // Пишем из rdi
    call    writei64

    call    finish

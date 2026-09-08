// Все библиотечные функции не меняют значения регистров rsp, rbp, rbx, r12, r13, r14, r15.
    .intel_syntax noprefix
    .global main

// add     DST, SRC   /* DST += SRC */
// sub     DST, SRC   /* DST -= SRC */
// inc     DST        /* ++DST */
// dec     DST        /* --DST */
// neg     DST        /* DST = -DST */
// mov     DST, SRC   /* DST = SRC */
// imul    SRC        /* (eax,edx) = eax * SRC - знаковое */
// mul     SRC        /* (eax,edx) = eax * SRC - беззнаковое */
// and     DST, SRC   /* DST &= SRC */
// or      DST, SRC   /* DST |= SRC */
// xor     DST, SRC   /* DST ^= SRC */
// not     DST        /* DST = ~DST */
// cmp     DST, SRC   /* DST - SRC, результат не сохраняется, */
// test    DST, SRC   /* DST & SRC, результат не сохраняется  */
// adc     DST, SRC   /* DST += SRC + CF */
// sbb     DST, SRC   /* DST -= SRC - CF */

main:
    // Результат в rax
    call    readi64
    // первое число
    mov     r12, rax

    // Результат в rax
    call    readi64
    // второе число
    mov     r13, rax

    // add     DST, SRC   DST += SRC
    add     r12, r13

    mov     rdi, r12
    call    writei64

    call    finish











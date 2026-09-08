    .intel_syntax noprefix
    .global main

// add     DST, SRC   /* DST += SRC */
// sub     DST, SRC   /* DST -= SRC */
// inc     DST        /* ++DST */
// dec     DST        /* --DST */
// neg     DST        /* DST = -DST */
// mov     DST, SRC   /* DST = SRC */
// imul    SRC        /* (rax,rdx) = rax * SRC - знаковое */
// mul     SRC        /* (rax,rdx) = rax * SRC - беззнаковое */
// and     DST, SRC   /* DST &= SRC */
// or      DST, SRC   /* DST |= SRC */
// xor     DST, SRC   /* DST ^= SRC */
// not     DST        /* DST = ~DST */
// cmp     DST, SRC   /* DST - SRC, результат не сохраняется, */
// test    DST, SRC   /* DST & SRC, результат не сохраняется  */
// adc     DST, SRC   /* DST += SRC + CF */
// sbb     DST, SRC   /* DST -= SRC - CF */

// jz      label   /* переход, если равно (нуль), ZF == 1 */
// jnz     label   /* переход, если не равно (не нуль), ZF == 0 */
// jc      label   /* переход, если CF == 1 */
// jnc     label   /* переход, если CF == 0 */
// jo      label   /* переход, если OF == 1 */
// jno     label   /* переход, если OF == 0 */
// jg      label   /* переход, если больше для знаковых чисел ZF == 0 && SF == OF */
// jge     label   /* переход, если >= для знаковых чисел SF == OF */
// jl      label   /* переход, если < для знаковых чисел SF != OF */
// jle     label   /* переход, если <= для знаковых чисел ZF == 1 || SF != OF */
// ja      label   /* переход, если > для беззнаковых чисел ZF == 0 && CF == 0 */
// jae     label   /* переход, если >= (беззнаковый) CF == 0 */
// jb      label   /* переход, если < (беззнакоый) CF == 1 */
// jbe     label   /* переход, если <= (беззнаковый) ZF == 1 || CF == 1 */
// jmp

main:
    mov     r12, 12
    mov     r13, -12

    // add     DST, SRC   DST += SRC
    add     r12, r13

    mov     al, 127
    add     al, 1

// cmp
    mov     r12, 5
    mov     r13, 6
compare:
    cmp     r12, r13

    jz      zero
    jg      greater
    jl      less

zero:
    // some code here
    nop

greater:
    // some code here
    nop

less:
    // some code here
    nop

    jmp compare
    
    
    ret


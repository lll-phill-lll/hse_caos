// Все библиотечные функции не меняют значения регистров rsp, rbp, rbx, r12, r13, r14, r15.
    .intel_syntax noprefix
    .global main

// Two's complement
//      u   s
// 000 	0 	0
// 001 	1 	1
// 010 	2 	2
// 011 	3 	3
// 100 	4 	−4
// 101 	5 	−3
// 110 	6 	−2
// 111 	7 	−1

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

main:
    // Результат в rax
    call    readi64
    // первое число
    mov     r12, rax

    // Результат в rax
    call    readi64
    // второе число
    mov     r13, rax

my_sub:
    // consider cmp
    sub     r12, r13
    jz      end
    jl      print_less
    jmp     print_greater

print_greater:
    mov     rdi, 1
    call    writei64
    jmp     main

print_less:
    mov     rdi, -1
    call    writei64
    jmp     main

end:
    call    finish











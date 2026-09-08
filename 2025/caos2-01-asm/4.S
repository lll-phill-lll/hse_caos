    .intel_syntax noprefix
    .global main

.text
add_numbers:
    add     rax, rdi
    pop     r12
    jmp     r12

main:
    mov     rdi, 5
    mov     rax, 6

    lea     r12, [rip + after_jmp]
    push    r12
    jmp     add_numbers
after_jmp:



    nop


    ret

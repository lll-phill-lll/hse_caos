    .intel_syntax noprefix
    .global main

add_numbers:
    add     rdi, rax
    pop     r12
    jmp     r12

main:
    mov     rdi, 5
    mov     rax, 6

    lea     r12, [rip + after_jmp]
    push    r12
    jmp     add_numbers

after_jmp:
    call    writei64
    call    finish




    .intel_syntax noprefix
    .global main

add_numbers:
    add     rdi, rax
    ret

main:
    mov     rdi, 5
    mov     rax, 6
    call    add_numbers

    call    writei64
    call    finish

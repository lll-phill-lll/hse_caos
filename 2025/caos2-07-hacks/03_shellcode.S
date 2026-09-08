// write(1, "HACKED!\n", 8); exit(0)
//
//   gcc -nostdlib -static -o shellcode shellcode.S
//
//   objcopy -O binary -j .text shellcode shellcode.bin
//   xxd shellcode.bin
//
//   xxd -p shellcode.bin | sed 's/\(..\)/\\x\1/g' | tr -d '\n'
//
//   objdump -d shellcode
//   objdump -D -m i386:x86-64  -b binary -d shellcode.bin

    .intel_syntax noprefix
    .global _start

_start:
    mov     rax, 1              // write
    mov     rdi, 1              // stdout
    lea     rsi, [rip + msg]    // указатель на строку
    mov     rdx, 8              // длина
    syscall

    mov     rax, 60             // exit
    xor     rdi, rdi            // код 0
    syscall

msg:
    .ascii "HACKED!\n"

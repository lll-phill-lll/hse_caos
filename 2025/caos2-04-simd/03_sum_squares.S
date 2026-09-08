.intel_syntax noprefix

.global sumsq

sumsq:
    push    rbp
    mov     rbp, rsp

    // result
    xorpd   xmm0, xmm0
    mov     rcx, rdi
    shr     rcx, 1

forloop:
    test    rcx, rcx
    jz      tail

// xmm0 - 128bit
// ymm0 - 256k
    // 2 doubles
    movupd  xmm1, [rsi]
    mulpd   xmm1, xmm1
    addpd   xmm0, xmm1

    add     rsi, 16
    dec     rcx
    jmp     forloop

tail:
    // check if odd
    test    rdi, 1
    jz      reduce

// [double|......]
    movsd   xmm1, [rsi]
    mulsd   xmm1, xmm1
    addsd   xmm0, xmm1

reduce:
    //  horizontal sum
    movupd  xmm1, xmm0
    unpckhpd xmm1, xmm1
    addsd   xmm0, xmm1
    // haddpd

    mov     rsp, rbp
    pop     rbp
    ret


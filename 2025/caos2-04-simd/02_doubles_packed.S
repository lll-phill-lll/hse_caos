    .intel_syntax noprefix
    .global process

process:
    push    rbp
    mov     rbp, rsp

    // Загрузка данных x и y в xmm0 и xmm1
    // unaligned
    movupd  xmm0, [rip + x]                  // xmm0 = x[0], x[1]
    movupd  xmm1, [rip + y]                  // xmm1 = y[0], y[1]

    // Вертикальная сумма (addpd)
    // xmm0[0] = xmm0[0] + xmm1[0]
    // xmm0[1] = xmm0[1] + xmm1[1]
    addpd  xmm0, xmm1                   
    // Горизонтальная сумма (haddpd)
    // xmm0[0] = xmm0[0] + xmm0[1]
    // xmm0[1] = xmm1[0] + xmm1[1]
    // haddpd  xmm0, xmm1                      


    // movhpd, movlpd - нельзя с регистрами

    // movsd
    // xmm0 [double1|double2]
    movlpd  [rdi], xmm0         // *res1 = xmm0[0]
    movhpd  [rsi], xmm0         // *res2 = xmm0[1]

    mov     rsp, rbp
    pop     rbp
    ret

    .data
x:
    .double 1.1, 2.2
y:
    .double 3.3, 4.4


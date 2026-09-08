.data
msg_sq:     .asciz  "square(7) = "
msg_max:    .asciz  "max(15, 42) = "
msg_sos:    .asciz  "sum_of_squares(3, 4) = "
msg_clamp:  .asciz  "clamp(150, 0, 100) = "
nl:         .asciz  "\n"

.text
.globl main
main:
    # main вызывает другие функции -> сохраняем ra
    # в rars не принципиально, так как у нас тут все равно exit(0)
    # addi - sp = sp - 4 (выделяем 4 байта на стеке)
    addi    sp, sp, -4
    # sw - сохраняем ra на стеке
    sw      ra, 0(sp)

    # ===== square(7) =====

    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес msg_sq
    la      a0, msg_sq
    # ecall - напечатать
    ecall

    # li - a0 = 7 (аргумент для square)
    li      a0, 7
    # jal (jump and link) - вызвать square
    # ra = адрес следующей инструкции, PC = square
    # После возврата a0 = square(7) = 49
    # то же самое, что и call square
    call    square

    # li - a7 = 1 (PrintInt)
    li      a7, 1
    # ecall - напечатать результат (a0 = 49)
    ecall
    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес nl
    la      a0, nl
    # ecall - перевод строки
    ecall

    # ===== max(15, 42) =====

    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес msg_max
    la      a0, msg_max
    # ecall - напечатать
    ecall

    # li - a0 = 15 (первый аргумент)
    li      a0, 15
    # li - a1 = 42 (второй аргумент)
    li      a1, 42
    # jal - вызвать max(15, 42), результат в a0
    jal     ra, max

    # li - a7 = 1 (PrintInt)
    li      a7, 1
    # ecall - напечатать результат (a0 = 42)
    ecall
    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес nl
    la      a0, nl
    # ecall - перевод строки
    ecall

    # ===== sum_of_squares(3, 4) =====

    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес msg_sos
    la      a0, msg_sos
    # ecall - напечатать
    ecall

    # li - a0 = 3 (первый аргумент)
    li      a0, 3
    # li - a1 = 4 (второй аргумент)
    li      a1, 4
    # jal - вызвать sum_of_squares(3, 4)
    # Результат: 3^2 + 4^2 = 9 + 16 = 25
    jal     ra, sum_of_squares
    // ->>>>>>


    # li - a7 = 1 (PrintInt)
    li      a7, 1
    # ecall - напечатать 25
    ecall
    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес nl
    la      a0, nl
    # ecall - перевод строки
    ecall

    # Восстанавливаем ra и выходим
    # lw - ra = memory[sp] (восстанавливаем адрес возврата)
    lw      ra, 0(sp)
    # addi - sp = sp + 4 (освобождаем стек)
    addi    sp, sp, 4

    # li - a7 = 10 (Exit)
    li      a7, 10
    # ecall - завершить
    ecall


# int square(int x) -> x * x
# input: a0 = x
# output:  a0 = x * x
square:
    # mul (multiply) - a0 = a0 * a0 (расширение M)
    # полный результат умножения:
    # mul   t0, t1, t2   # low
    # mulh  t3, t1, t2   # high
    # slli 
    mul     a0, a0, a0
    # ret - возврат из функции
    # Псевдоинструкция: jalr x0, ra, 0
    ret


# int max(int a, int b) -> (a > b) ? a : b
# Аргументы: a0 = a, a1 = b
# Возврат:   a0 = max(a, b)
max:
    # bge (branch if greater or equal) - если a0 >= a1 (a >= b),
    # прыгаем на max_done: a уже в a0, ничего делать не надо
    bge     a0, a1, max_done
    # mv - a0 = a1 (a < b, поэтому возвращаем b)
    mv      a0, a1
max_done:
    # ret - возврат
    ret


# int sum_of_squares(int a, int b) -> a^2 + b^2
# Аргументы: a0 = a, a1 = b
# Возврат:   a0 = a^2 + b^2
sum_of_squares:
    # --- Пролог ---
    # addi - sp = sp - 12 (3 слова: ra, s0, s1)
    addi    sp, sp, -12
    # sw - сохраняем ra (будет перезаписан при jal square)
    sw      ra, 8(sp)
    # sw - сохраняем s0 (b)
    sw      s0, 4(sp)
    # sw - сохраняем s1 (a^2)
    sw      s1, 0(sp)

    # mv - s0 = a1 (сохраняем b в callee-saved регистре)
    mv      s0, a1

    # a0 уже = a (первый аргумент)
    # jal - вызываем square(a), результат в a0
    call    square
    # mv - s1 = a0 (сохраняем a^2 в callee-saved регистре)
    mv      s1, a0

    # mv - a0 = s0 (достаём b)
    mv      a0, s0
    # jal - вызываем square(b), результат в a0
    jal     ra, square

    # add - a0 = s1 + a0 (a^2 + b^2)
    add     a0, s1, a0

    # --- Эпилог ---
    # lw - восстанавливаем s1
    lw      s1, 0(sp)
    # lw - восстанавливаем s0
    lw      s0, 4(sp)
    # lw - восстанавливаем ra
    lw      ra, 8(sp)
    # addi - sp = sp + 12 (освобождаем стек)
    addi    sp, sp, 12
    # ret - возврат
    ret

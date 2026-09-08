.data
prompt_n:   .asciz  "Enter array size: "
prompt_el:  .asciz  "Enter element: "
res_msg:    .asciz  "Sum = "
newline:    .asciz  "\n"
# .space N - зарезервировать N нулевых байт
# 400 байт = 100 элементов по 4 байта (word)
array:      .space  400

.text
.globl main
main:
    # --- Пролог ---
    # Используем s0, s1, s2 - обязаны сохранить их старые значения
    # addi - sp = sp - 12 (3 слова * 4 байта)
    addi    sp, sp, -12
    # sw - сохраняем s0, s1, s2 на стеке
    sw      s0, 8(sp)
    sw      s1, 4(sp)
    sw      s2, 0(sp)

    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес prompt_n
    la      a0, prompt_n
    # ecall - напечатать
    ecall

    # li - a7 = 5 (ReadInt)
    li      a7, 5
    # ecall - прочитать N в a0
    ecall
    # mv - s0 = a0 (сохраняем N в callee-saved регистре)
    mv      s0, a0
    # la - s1 = адрес начала массива array
    la      s1, array

    # --- Input ---

    # mv - t0 = 0 (i = 0)
    #0 <= t0 < s0
    mv      t0, zero
input_loop:
    # bge - если t0 >= s0 (i >= N), прыгаем на sum_start
    bge     t0, s0, sum_start

    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес prompt_el
    la      a0, prompt_el
    # ecall - напечатать
    ecall

    # li - a7 = 5 (ReadInt)
    li      a7, 5
    # ecall - прочитать число в a0
    ecall

    // [xxxx][xxxx][xxxx]...
    // s1 
    // t0 = i
    // t1 = t0 * 4

    # slli (shift left logical immediate) - сдвиг влево на 2
    # t1 = t0 << 2 = t0 * 4 (байтовое смещение: каждый int = 4 байта)
    slli    t1, t0, 2
    # add - t2 = s1 + t1 (t2 = &array[i] = базовый адрес + смещение)
    add     t2, s1, t1
    # sw (store word) - записать 4 байта из a0 в память по адресу t2
    # memory[t2 + 0] = a0, т.е. array[i] = введённое число
    sw      a0, 0(t2)

    # addi - t0 = t0 + 1 (i++)
    addi    t0, t0, 1
    # j - безусловный переход на начало цикла
    j       input_loop

    # --- Цикл суммирования ---

sum_start:
    # mv - t0 = 0 (i = 0)
    mv      t0, zero
    # mv - s2 = 0 (sum = 0)
    # переполнение?
    mv      s2, zero

sum_loop:
    # bge - если t0 >= s0 (i >= N), прыгаем на print_result
    bge     t0, s0, print_result

    # slli - t1 = t0 * 4 (байтовое смещение)
    slli    t1, t0, 2
    # add - t2 = &array[i]
    add     t2, s1, t1
    # lw (load word) - загрузить 4 байта из памяти в регистр
    # t3 = memory[t2 + 0] = array[i]
    lw      t3, 0(t2)
    # add - s2 = s2 + t3 (sum += array[i])
    add     s2, s2, t3

    # addi - t0 = t0 + 1 (i++)
    addi    t0, t0, 1
    # j - на начало цикла
    j       sum_loop

    # --- Вывод результата ---

print_result:
    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес res_msg
    la      a0, res_msg
    # ecall - напечатать "Sum = "
    ecall

    # li - a7 = 1 (PrintInt)
    li      a7, 1
    # mv - a0 = s2 (сумма)
    mv      a0, s2
    # ecall - напечатать число
    ecall

    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес newline
    la      a0, newline
    # ecall - напечатать перевод строки
    ecall

    # --- Эпилог ---
    # lw - восстанавливаем s2, s1, s0
    lw      s2, 0(sp)
    lw      s1, 4(sp)
    lw      s0, 8(sp)
    # addi - sp = sp + 12 (чистим стек)
    addi    sp, sp, 12

    # li - a0 = 0 (код возврата)
    li      a0, 0
    # li - a7 = 17 (Exit2)
    li      a7, 17
    # ecall - завершить с кодом 0
    ecall

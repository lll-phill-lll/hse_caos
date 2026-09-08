.data
greeting:   .asciz  "Hello, RISC-V!\n"
prompt:     .asciz  "Enter a number: "
result_msg: .asciz  "Your number * 2 = "
newline:    .asciz  "\n"

.text
.globl main
main:
    # --- Пролог ---
    # Мы будем использовать s0 (callee-saved)
    # push нет 
    addi    sp, sp, -4
    # sw - сохраняем старое значение s0 на стеке
    # s - store
    #w - word
    sw      s0, 0(sp)

    # --- Greeting ---

    # syscall: 
    # номер вызова a7
    # аргументы a0 - a5
    # результат a0
    # https://github.com/TheThirdOne/rars/wiki/Environment-Calls

    # li (load immediate) - загрузить константу в регистр
    # a7 = номер системного вызова.
    # 4 = PrintString
    li      a7, 4

    # la (load address) - загрузить адрес метки в регистр
    # a0 = адрес строки greeting
    la      a0, greeting

    # ecall - выполнить системный вызов
    ecall

    # --- Enter num? ---

    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес строки prompt
    la      a0, prompt
    # ecall - печатаем строку prompt
    ecall

    # --- Read num ---

    # li - a7 = 5 (ReadInt - прочитать целое число)
    li      a7, 5
    # ecall - читаем число, результат окажется в a0
    ecall
    // a0 - 

    # --- Mult by 2 ---

    # slli (shift left logical immediate) - логический сдвиг влево
    # a0 = a0 << 1, что равно a0 * 2
    # если бит выходит за границы, то нет CF, из которого его можно забрать
    slli    a0, a0, 1

    # mv (move) - скопировать значение из одного регистра в другой
    # s0 = a0 (сохраняем результат в s-регистре, a0 потеряется при вызове)
    # Псевдоинструкция: раскрывается в addi s0, a0, 0
    mv      s0, a0

    # --- Print res ---

    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес строки result_msg
    la      a0, result_msg
    # ecall - печатаем "Your number * 2 = "
    ecall

    # li - a7 = 1 (PrintInt - напечатать целое число)
    li      a7, 1
    # mv - a0 = s0 (достаём сохранённый результат)
    mv      a0, s0
    # ecall - печатаем число
    ecall

    # li - a7 = 4 (PrintString)
    li      a7, 4
    # la - a0 = адрес строки newline
    la      a0, newline
    # ecall - печатаем перевод строки
    ecall

    # --- Эпилог ---
    # lw - восстанавливаем старое значение s0
    lw      s0, 0(sp)
    # addi - sp = sp + 4 (освобождаем стек)
    addi    sp, sp, 4

    # --- Завершение ---

    # mv - a0 = 0
    # псевдоинструкция addi a0, x0, 0
    # x0 / zero
    mv      a0, zero
    # li - a7 = 17 (Exit2 - завершить программу с кодом возврата из a0)
    li      a7, 17
    # ecall - завершаем с кодом 0
    ecall

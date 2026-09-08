# RISC-V: архитектура, ассемблер


## Запуск

https://github.com/thethirdone/rars

```bash
java -jar rars.jar
```

Если шрифты кривые: `java -Dawt.useSystemAAFontSettings=on -jar rars.jar`

Онлайн
- https://riscv-programming.org/ale/
- https://ripes.me/


## Главные отличия от x86

| | x86 | RISC-V |
|-|-|-|
| Инструкций | ~1500+ | ~47 (база) |
| Длина инструкции | 1-15 байт | Всегда 4 байта |
| Регистров | 16 | 32 |
| Работа с памятью | Можно прямо в add/cmp | Только через load/store |
| Флаги | EFLAGS | Нет |
| Формат | `add eax, ebx` (двухадресный) | `add rd, rs1, rs2` (трёхадресный) |
| Лицензия | Проприетарная | Открытая |


### Фиксированная длина инструкций

В x86 `nop` - 1 байт, а `mov rax, [0x12345678]` - 8 байт. Процессору надо парсить поток байтов, чтобы понять где кончается одна инструкция и начинается другая.

В RISC-V каждая инструкция ровно 4 байта. Можно увидеть в RARS во вкладке Execute - адреса идут с шагом 4.

Это упрощает декодер и конвейер. (На самом деле есть еще расширение с 2-юайтными инструкциями, но оно опциональное).

### Load/Store - вычисления только на регистрах

В x86 можно складывать прямо с памятью:

```asm
add     rax, [rbx + 8]
```

В RISC-V надо сначала загрузить в регистр
```asm
lw      t0, 8(t1)
add     t2, t2, t0
```
Инструкций получается больше, но по идее должны выигрывать за счет более простого парсера и конвеера.


### Нет регистра флагов

В x86 операции выставляют EFLAGS, а потом `jl`/`je`/`jg` читают флаги

```asm
cmp     eax, ebx
// add     ecx, 1 можно случайно затереть
jl      less
```

В RISC-V сравнение встроено в сам переход так как инструкции трехадресные
```asm
// branch less: if t0 < t1 -> jmp less
blt     t0, t1, less
// addi    t3, t3, 1 регистр флагов не портится (его нет)
```

Если нужно сохранить результат сравнения:
```asm
// t2 = (t0 < t1) ? 1 : 0
slt     t2, t0, t1
// t3 = (t2 == 0) ? 1 : 0
seqz    t3, t2
```

### Immediate - константы в инструкциях

В x86: `mov eax, 0xDEADBEEF` - константа тоже занимает байты инструкции.

В RISC-V инструкция 32 бита, часть уходит на opcode и регистры. На константу только 12 бит (-2048..2047)

Маленькие числа помещаются в эти 32 бита:

```asm
// addi t0, x0, 100
li      t0, 100
```

Для больших числен нужно больше инструкци
```asm
// lui t0, 2 
// addi t0, t0, 0x00000710
li      t0, 0x12345
```

### Псевдоинструкции

Для удобства ассемблер добавляет удобные обёртки:

```
mv   t0, t1        ->  addi t0, t1, 0         
nop                ->  addi x0, x0, 0         
ret                ->  jalr x0, ra, 0         
j    label         ->  jal  x0, label         
neg  t0, t1        ->  sub  t0, x0, t1        
not  t0, t1        ->  xori t0, t1, -1        
beqz t0, label     ->  beq  t0, x0, label     
bgt  t0, t1, label ->  blt  t1, t0, label     
```

x0 - константный 0

-

## Регистры

32 штуки по 32 бита. С частями регистров работать нельзя.

| Регистр | Имя | Значение | Кто сохраняет |
|---|--|--|----|
| x0 | zero | Всегда 0, запись игнорируется | - |
| x1 | ra | Адрес возврата | caller |
| x2 | sp | Указатель стека | callee |
| x3 | gp | Global pointer | - |
| x4 | tp | Thread pointer | - |
| x5-x7 | t0-t2 | Временные | caller |
| x8 | s0/fp | Saved / frame pointer | callee |
| x9 | s1 | Saved | callee |
| x10-x11 | a0-a1 | Аргументы + возврат | caller |
| x12-x17 | a2-a7 | Аргументы | caller |
| x18-x27 | s2-s11 | Saved | callee |
| x28-x31 | t3-t6 | Временные | caller |

**a-регистры** - аргументы функции (a0-a7), возвращаемое значение (a0, a1)

-

## Инструкции RV32I

Удобный список вот тут

https://projectf.io/posts/riscv-cheat-sheet/


-

## Syscall в RARS

Отличаются от реальных!

номер системноого выхова кладем в a7. Аргументы в a0, a1... Дальше вызываем `ecall` (местный syscall)

| a7 | Что делает | Аргументы -> Результат |
|-|---|------|
| 1 | PrintInt | a0 = число |
| 4 | PrintString | a0 = адрес строки |
| 5 | ReadInt | -> a0 |
| 8 | ReadString | a0 = буфер, a1 = длина |
| 10 | Exit | - |
| 11 | PrintChar | a0 = символ |
| 12 | ReadChar | -> a0 |
| 17 | Exit2 | a0 = код возврата |
| 34 | PrintIntHex | a0 = число |

-

## Calling convention

```
Аргументы:    a0-a7
Возврат:      a0 (a1)
Временные:    t0-t6  (можно менять,но вызов функции тоже может их изменить)
Сохраняемые:  s0-s11 (вызываемая функция должна вернуть в неизменном видще)


sp - stack pointer
ra - return address

```

Стек выравнивается по 16.

Пролог/эпилог:
```asm
my_func:
    addi    sp, sp, -16
    // если собираемся кого-то вызывать,то надо сохранить ra, чтобы не перетереть и вернуться в нужное место
    sw      ra, 12(sp)
    // если нужно,можно сохранить s регистры, которые хотим использовать
    sw      s0, 8(sp)
    ///--------------
    lw      s0, 8(sp)
    lw      ra, 12(sp)
    addi    sp, sp, 16
    ret
```

-


**Random:**
https://riscv.org/wp-content/uploads/2024/12/riscv-calling.pdf
https://github.com/TheThirdOne/rars

https://github.com/riscv/riscv-isa-manual?tab=readme-ov-file

https://riscv.github.io/riscv-isa-manual/snapshot/unprivileged/#_programmers_model_for_base_integer_isa

инструкции:
https://msyksphinz-self.github.io/riscv-isadoc/

https://projectf.io/posts/riscv-cheat-sheet/

оф дока https://riscv.github.io/riscv-isa-manual/snapshot/unprivileged/#rv32

https://luplab.gitlab.io/rvcodecjs/

https://youtu.be/1zLxxxLc0xI?si=rurXkpHkuJKCNB_5  Константин Владимиров Syntacore

кто использует
https://riscv.org/members/

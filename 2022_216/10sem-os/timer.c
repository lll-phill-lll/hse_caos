// документация https://wiki.osdev.org/Programmable_Interval_Timer

#include "keyboard.h"
#include "../cpu/isr.h"
#include "../console.h"
#include "port.h"
#include "../lib/mem.h"
#include "vga.h"

static unsigned time;

static void timer_handler(registers_t *r) {
    time++;
}

// эту функцию нужно еще добавить в timer.h
void sleep2(unsigned ms) {
    unsigned old_time = time;

    while (time < old_time + ms) {
        asm("hlt");
    }
}

unsigned get_time() {
    return time;
}

// частота таймера поделенная на 1000 - столько срабатываний
// нам нужно прорпускать, чтобы отмечать примерно секунду
unsigned refresh = 1193182 / 1000;


/*
Bits         Usage
6 and 7      Select channel :
                0 0 = Channel 0
                0 1 = Channel 1
                1 0 = Channel 2
                1 1 = Read-back command (8254 only)
4 and 5      Access mode :
                0 0 = Latch count value command
                0 1 = Access mode: lobyte only
                1 0 = Access mode: hibyte only
                1 1 = Access mode: lobyte/hibyte
1 to 3       Operating mode :
                0 0 0 = Mode 0 (interrupt on terminal count)
                0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                0 1 0 = Mode 2 (rate generator)
                0 1 1 = Mode 3 (square wave generator)
                1 0 0 = Mode 4 (software triggered strobe)
                1 0 1 = Mode 5 (hardware triggered strobe)
                1 1 0 = Mode 2 (rate generator, same as 010b)
                1 1 1 = Mode 3 (square wave generator, same as 011b)
0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
*/
struct timer_mode {
    unsigned binary_mode : 1;
    unsigned operating_mode : 3;
    unsigned access_mode : 2;
    unsigned channel : 2;
};

// не забудьте вызвать эту функцию в kernel.c
void init_timer() {

    // нас интересует 0 канал, и 3 режим, остальное не инетересует
    struct timer_mode mode = {0, 0b011, 0, 0};

    // надо структуру привести к unsigned char.
    // код ниже приводит к нарушению strict aliasing rule
    // хотя много где он и используется
    // см пример из: https://en.cppreference.com/w/c/string/byte/memcpy
    // подробнее про strict aliasing: https://habr.com/ru/company/otus/blog/443602/
    //
    // Лучше делать так: https://github.com/lll-phill-lll/hse_caos/tree/master/2022_216/08sem-c#bit-fields-hack
	port_byte_out(0x43, *(unsigned char*)&mode);

	port_byte_out(0x40, refresh & 0xFF);
	port_byte_out(0x40, refresh >> 8);

    register_interrupt_handler(IRQ0, timer_handler);
}

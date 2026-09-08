// Не любое число из int может быть представлено в float

#include <stdio.h>

// объединение имеет размер максимального поля
// если мы обращаемся к какой-то полю, то просто получим
// те байты, что лежат в объединении. В данном случае мы
// можем положить байты как float, а прочитать их как
// struct parts. Так как и float и parts имеют размер 32 бита.
// https://en.cppreference.com/w/cpp/language/bit_field
// !!! Warning!!! в C++ такие операции приведут к UB.
// Требуется, чтобы из обхединения мы читали только то поле,
// которое положили
typedef union {
    float f;
    // Implementation defined. См. https://github.com/lll-phill-lll/hse_caos/tree/master/2021_207/08sem-c#bit-fields-hack
    struct {
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
    } parts;
} float_uni;

int main() {
    int n = 1;
    for (int i = 0; i != 31; ++i) {
        int num = 1U << i;
        // степени 2 могут быть представлены в float
        // Но если к ним что-то прибавить и использовать мантиссу, то тут
        // уже возникают проблемы
        num += 5;
        // не всегда выполняется
        // но вот в double все int помещаются уже
        printf("num = %d,  %d\n", num, num == (int)(float)num);
    }
}

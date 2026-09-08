// Напишем код, которые печатает части float

#include <stdio.h>

// печатаем число в бинарном виде
// первый аргмент - само число, второй аргумент
// - сколько бит нужно печатать
void print_bin(unsigned n, unsigned size) {
    unsigned i;
    for (i = 1 << (size-1); i > 0; i = i >> 1) {
        (n & i) ? printf("1") : printf("0");
    }
    printf("\n");
}

// объединение имеет размер максимального поля
// если мы обращаемся к какой-то полю, то просто получим
// те байты, что лежат в объединении. В данном случае мы
// можем положить байты как float, а прочитать их как
// struct parts. Так как и float и parts имеют размер 32 бита.
// https://en.cppreference.com/w/cpp/language/bit_field
// !!! Warning!!! в C++ такие операции приведут к UB.
// Требуется, чтобы из объединения мы читали только то поле,
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

void print_float(float f) {
    float_uni f1 = { .f = f };

    printf("%f\n", f);

    printf("sign\t = \t");
    print_bin(f1.parts.sign, 1);

    printf("exponent = \t");
    print_bin(f1.parts.exponent, 8);

    printf("mantissa = \t");
    print_bin(f1.parts.mantissa, 23);
}

int main() {

    float f = -4.5;

    float_uni f1 = { .f = f };

    print_float(f);
}

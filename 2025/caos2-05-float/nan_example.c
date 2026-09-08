// попробуем получить особые числа в float

#include <math.h>
#include <stdio.h>

// печатаем число в бинарном виде
// первый аргмент - само число, второй аргумент
// - сколько бит нужно печатать
void print_bin(unsigned n, unsigned size) {
    unsigned i;
    for (i = 1 << (size - 1); i > 0; i = i >> 1) {
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
// Требуется, чтобы из обхединения мы читали только то поле,
// которое положили
typedef union {
    float f;
    // Implementation defined. См.
    // https://github.com/lll-phill-lll/hse_caos/tree/master/2021_207/08sem-c#bit-fields-hack
    struct {
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
    } parts;
} float_uni;

void print_float(float f) {
    float_uni f1 = {.f = f};

    printf("-------\n");
    printf("%.17f\n", f);

    printf("sign\t = \t");
    print_bin(f1.parts.sign, 1);

    printf("exponent = \t");
    print_bin(f1.parts.exponent, 8);

    printf("mantissa = \t");
    print_bin(f1.parts.mantissa, 23);

    printf("-------\n");
}

// устанавливаем указанный бит в 1
// bit_num - номер бита, where - где этот бит поменять
void set_bit(unsigned bit_num, unsigned* where) {
    // 1U так как просто 1 - это int и сдвиг влево int приводит к UB
    *where |= 1U << bit_num;
}

// | Экспонента  |  Мантисса   | Значение                          |
// | ----------- | ----------- | ------------                      |
// |  11111111   |   все нули  | бесконечность (+ или -)           |
// |  11111111   | не все нули | not a number (NaN)                |
// |  00000000   |   все нули  | ноль                              |
// |  00000000   | не все нули | денормализованный вид             |

int main() {
    float_uni f = {.f = 0};
    print_float(f.f);

    // запишем  все 1 в экспоненту
    // мантисса и знак сейчас 0
    // получим inf
    f.parts.exponent = 255;
    print_float(f.f);

    // если поменяем знак, то получим
    // -inf
    f.parts.sign = 1;
    print_float(f.f);

    // Если поменяем еще дополнительно
    // мантиссу, то получим уже NaN
    f.parts.sign = 0;
    f.parts.exponent = 0b11111110;
    f.parts.mantissa = 0b11111111111111111111111;
    print_float(f.f);

    f.parts.sign = 0;
    f.parts.exponent = 0b11111110;
    f.parts.mantissa = 0b11111111111111111111110;
    print_float(f.f);

    // f.parts.exponent = 0;
    // printf("%g\n", f.f);
    // print_float(f.f);

    // NaN всегда != NaN, даже если биты в точности
    // совпадают
    // Код ниже вернет false
    // printf("%d\n", f.f == f.f);
    
    // f.parts.exponent = 0;
    // f.parts.mantissa = 0b00000000000000000000001;
    // printf("%g\n", f.f);

    // f.parts.exponent = 254;
    // f.parts.mantissa = 0b11111111111111111111111;
    // printf("%f\n", f.f);
    // printf("%g\n", f.f);
}

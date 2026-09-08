// Покажем, что сравнения чисел в арифметике с плавающей точкой
// работают не так же, как и в целочисленной

#include <stdio.h>

// печатаем число в бинарном виде
// первый аргмент - само число, второй аргумент
// - сколько бит нужно печатать
void print_bin(unsigned n, unsigned size) {
    unsigned i;
    for (i = 1 << (size-1); i > 0; i = i >> 1) {
        (n & i) ? printf("1") : printf("0");
    }
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
    // Implementation defined. См. https://github.com/lll-phill-lll/hse_caos/tree/master/2021_207/08sem-c#bit-fields-hack
    struct {
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
    } parts;
} float_uni;

// Такой же трюк, как и для float, только
// тут под мантиссу нужно уже 52 бита
typedef union {
    double d;
    // Implementation defined. См. https://github.com/lll-phill-lll/hse_caos/tree/master/2021_207/08sem-c#bit-fields-hack
    struct {
        unsigned int mantissa1 : 32;
        unsigned int mantissa2 : 20;
        unsigned int exponent : 11;
        unsigned int sign : 1;
    } parts;
} double_uni;

void print_float(float f) {
    float_uni f1 = { .f = f };

    printf("float -------\n");
    printf("%f\n", f);

    printf("sign\t = \t");
    print_bin(f1.parts.sign, 1);

    printf("\n");
    printf("exponent = \t");

    print_bin(f1.parts.exponent, 8);
    printf("\n");

    printf("mantissa = \t");
    print_bin(f1.parts.mantissa, 23);

    printf("\n");
    printf("-------\n");
}


void print_double(double d) {
    double_uni d1 = { .d = d };

    printf("double -------\n");
    printf("%f\n", d);

    printf("sign\t = \t");
    print_bin(d1.parts.sign, 1);
    printf("\n");

    printf("exponent = \t");
    print_bin(d1.parts.exponent, 11);
    printf("\n");

    printf("mantissa = \t");
    print_bin(d1.parts.mantissa2, 20);
    print_bin(d1.parts.mantissa1, 32);
    printf("\n");

    printf("-------\n");
}

int main() {
    // можно порпобовать также 1.5. В таком случае уже разницы не будет так
    // как представление 0.5 конечное.
    float f = 1.2;
    double d = 1.2;

    // вывод будет false.
    // Так как при сравнении f приведется к double
    // и новые биты заполнятся нулями, в то время как
    // в d они не заполнены нулями
    printf("%d\n", f == d);

    print_float(f);
    print_double(d);
    print_double(f);

    // из-за того, что 0.1 и 0.2 в двоичном виде -
    // бесконечная дробь, то при их сложении произойдет
    // округление и результат не будет в точности равен 3.
    // чтобы сравнения работали ожидаемо, нужно учитывать
    // epsilon, например: 0.1 + 0.2 < 0.3 + eps && 0.1+0.2 > 0.3 - eps
    // где знечение epsilon зависит от задачи
    printf("%d\n", 0.1 + 0.2 == 0.3);

    // в предыдущем примере числа костовались в double.
    // во float это работает, но лучше на это не полагаться
    // printf("%d\n", (float)0.1 + (float)0.2 == (float)0.3);

    double sum = 0.1 + 0.2;
    print_double(sum);
    print_double(0.3);
}

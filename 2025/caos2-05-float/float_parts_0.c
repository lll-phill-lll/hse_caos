// Еще один способ, как можно получить
// части float числа

#include <stdio.h>
#include <string.h>

// печатаем число в бинарном виде
// первый аргмент - само число, второй аргумент
// - сколько бит нужно печатать
// printf("%b")
void print_bin(unsigned n, unsigned size) {
    unsigned i;
    for (i = 1 << (size - 1); i > 0; i = i >> 1) {
        (n & i) ? printf("1") : printf("0");
    }
    printf("\n");
}

// функция для получения экспоненты
unsigned get_exp(float num) {
    const int sign_bits = 1;
    const int mant_bits = 23;
    // такое преобразование позволяет переложить
    // биты из float в unsigned, чтобы мы могли спокойно
    // воспользоваться операцией сдвига
    // в exp будут лежать ровно те же биты в том же порядке,
    // что лежали в num.
    // если бы мы сделали exp = num, то у нас бы случилось
    // преобразование float к unsigned int и биты лежали бы уже
    // другие
    unsigned exp;
    memcpy(&exp, &num, sizeof(num));

    // код ниже приводит к нарушению strict aliasing rule
    // хотя много где он и используется
    // см пример из: https://en.cppreference.com/w/c/string/byte/memcpy
    // подробнее про strict aliasing:
    // https://habr.com/ru/company/otus/blog/443602/ unsigned exp =
    // *(unsigned*)&num;

    // Чтобы получить экспоненту нужно избавится от бита знака (сдвинуть на один
    // влево) и избавиться от мантиссы (сдвинуть на 23 вправо (и еще на один
    // вправо так как мы до этого вдигали влево))
    return exp << sign_bits >> (sign_bits + mant_bits);
}

// получим мантиссу
unsigned get_mant(float num) {
    const int sign_bits = 1;
    const int exp_bits = 8;

    unsigned mant;
    memcpy(&mant, &num, sizeof(num));

    // код ниже приводит к нарушению strict aliasing rule
    // хотя много где он и используется
    // см пример из: https://en.cppreference.com/w/c/string/byte/memcpy
    // подробнее про strict aliasing:
    // https://habr.com/ru/company/otus/blog/443602/ unsigned mant =
    // *(unsigned*)&num;

    int to_remove = sign_bits + exp_bits;
    // сдвинем на знак и экспонену влево, чтобы обнулить их
    // а затем обратно вправо
    return mant << (to_remove) >> to_remove;
}

// Получим знак числа
unsigned get_sign(float num) {
    const int sign_bits = 1;
    const int all_bits = 32;

    unsigned sign;
    // memcpy(&sign, &num, sizeof(num));

    // код ниже приводит к нарушению strict aliasing rule
    // хотя много где он и используется
    // см пример из: https://en.cppreference.com/w/c/string/byte/memcpy
    // подробнее про strict aliasing:
    // https://habr.com/ru/company/otus/blog/443602/ unsigned sign =
    // sign = *(unsigned*)&num;

    // сдвинем число на 31 вправо, чтобы остался только бит знака
    return sign >> (all_bits - sign_bits);
}

int main() {
    float f = -114.3;

    unsigned sign = get_sign(f);
    unsigned exp = get_exp(f);
    unsigned mant = get_mant(f);

    printf("%f\n", f);
    printf("sign\t = \t");
    print_bin(sign, 1);
    printf("exponent = \t");
    print_bin(exp, 8);
    printf("mantissa = \t");
    print_bin(mant, 23);
}

#include <stdio.h>
#include <string.h>

int main() {
    // scanf будет считывать пока не закончится ввод.
    // Если явно ему не сказать считать 8 символов: `scanf("%8s")`, то он
    // будет считывать даже когда input переполнится.
    // Так как input и password лежат на стеке поряд, то когда input переполнится
    // scanf начнет записывать уже в password.

    // Поэтому если мы подберем хитро input, например: `aaaaaaaa\0aaaaaaaa\0`,
    // то мы не зная настоящего пароля добьемся вывода `Access granted`.

    // Проверить: `echo -e 'aaaaaaaa\0aaaaaaaa\0' | ./a.out

    // В input положется `aaaaaaaa\0` - валидная null-terminated строка, дальше
    // input переполнится и password перезатрется, туда положется следующая часть ввода
    // `aaaaaaaa\0` - та же строка, что и в input.

    // Дальше strcmp побайтово сравнит строки и вернет 0 (признак равенства строк)
    char input[9];
    char password[9] = "password";

    // Чтобы избежать ошибки, нужно всегда указывать в scanf размер ввода:
    // scanf("%8s", input);
    scanf("%s", input);

    if (strcmp(input, password) == 0) {
        printf("Access granted\n");
    } else {
        printf("Access denied\n");
    }

}

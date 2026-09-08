#include <iostream>

// структура, где хранится текущий баланс
struct Account {
    int balance;
};

// печатает баланс
void print_account(Account account) {
    std::cout << account.balance << std::endl;
}

// добавляем денюжку
void add_money(Account* account, int amount) {
    int old_balance = account->balance;
    int new_balance = old_balance + amount;
    account->balance = new_balance;
}

// 1000 раз добавляем по одной монетке
void work_1000_days(Account* account) {
    for (int i = 0; i != 1000; ++i) {
        add_money(account, 1);
    }
}

int main() {
    Account account = { 0 };
    print_account(account);

    work_1000_days(&account);

    print_account(account);
}


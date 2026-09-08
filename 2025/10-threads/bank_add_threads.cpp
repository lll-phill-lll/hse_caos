#include <iostream>
#include <thread>
#include <atomic>

#define N 2

struct Account {
    std::atomic<int> balance;
};

void print_account(const Account& account) {
    std::cout << account.balance << std::endl;
}

// добавляем денюжку
void add_money(Account& account, int amount) {
    // int old_balance = account.balance;
    // int new_balance = old_balance + amount;
    // account.balance = new_balance;
    account.balance.fetch_and(amount);
}

void child_action(Account& account) {
    for (int i = 0; i < 100000; ++i) {
        add_money(account, 1);
    }
}

int main() {
    Account account = { 0 };
    print_account(account);

    std::thread tids[N];
    for (int i = 0; i != N; ++i) {
        // создаем потоки,
        tids[i] = std::thread(child_action, std::ref(account));
    }

    // ждем каждый поток
    for (int i = 0; i != N; ++i) {
        tids[i].join();
    }

    print_account(account);
}


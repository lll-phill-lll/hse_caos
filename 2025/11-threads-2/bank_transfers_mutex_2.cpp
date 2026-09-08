#include <iostream>
#include <thread>
#include <string>
#include <format>
#include <mutex>

#define INIT_BALANCE   2000
#define CHILD_BALANCE  1000
#define OPS 50

struct Account {
    std::string name;
    int balance;
    std::mutex mtx;

    Account(std::string n, int initial = 0)
        : name(std::move(n)), balance(initial) {}
};

void add_money(Account& account, int amount) {
    account.balance += amount;
}

void take_money(Account& account, int amount) {
    account.balance -= amount;
}

void print_accounts(Account& family,
                    Account& mall,
                    Account& gas,
                    Account& child)
{
    std::scoped_lock lock(family.mtx, mall.mtx, gas.mtx, child.mtx);

    std::cout << std::format(
        "+------------------+\n"
        "|       BANK       |\n"
        "+------------------+\n"
        "| {:<8}: {:>8} |\n"
        "| {:<8}: {:>8} |\n"
        "| {:<8}: {:>8} |\n"
        "| {:<8}: {:>8} |\n"
        "+------------------+\n",
        family.name, family.balance,
        mall.name,   mall.balance,
        gas.name,    gas.balance,
        child.name,  child.balance
    );
}

void transfer(Account& from, Account& to, int amount) {
    // mall -> family
    // family -> mall
    // std::lock_guard l1(from.mtx); // mall / fam
    // std::lock_guard l2(to.mtx);

    // Account* first  = &from < &to ? &from : &to;
    // Account* second = &from < &to ? &to   : &from;
    // std::lock_guard<std::mutex> lock1(first->mtx);
    // std::lock_guard<std::mutex> lock2(second->mtx);

    std::scoped_lock lock(from.mtx, to.mtx);
    int current = from.balance;

    if (current >= amount) {
        std::cout << std::format(
            "transferring {} from {} ({}) to {} ({})\n",
            amount,
            from.name, current,
            to.name,   to.balance
        );

        add_money(to, amount);
        take_money(from, amount);
    } else {
        std::cout << std::format(
            "cant transfer {} from {} ({}) to {} ({})\n",
            amount,
            from.name, current,
            to.name,   to.balance
        );
    }
}

void wife(Account& family, Account& mall) {
    for (int i = 0; i != OPS; ++i)
        transfer(family, mall, 100);
}

void husband(Account& family, Account& gas) {
    for (int i = 0; i != OPS; ++i)
        transfer(family, gas, 50);
}

void child_spending(Account& child, Account& mall) {
    for (int i = 0; i != OPS; ++i)
        transfer(child, mall, 20);
}

int main() {
    Account family("FAMILY", INIT_BALANCE);
    Account mall("MALL");
    Account gas("GAS");
    Account child("CHILD", CHILD_BALANCE);

    print_accounts(family, mall, gas, child);

    std::thread wife_thread(wife, std::ref(family), std::ref(mall));
    std::thread husband_thread(husband, std::ref(mall), std::ref(family));
    std::thread child_thread(child_spending, std::ref(child), std::ref(mall));

    wife_thread.join();
    husband_thread.join();
    child_thread.join();

    print_accounts(family, mall, gas, child);
}

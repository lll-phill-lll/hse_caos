#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <format>
#include <mutex>

#define INIT_BALANCE 2000
#define OPS 50

struct Account {
    std::string name;
    std::atomic<int> balance;

    Account(std::string n, int initial = 0)
        : name(std::move(n)), balance(initial) {}
};

std::mutex bank_mutex;

void add_money(Account& account, int amount) {
    account.balance.fetch_add(amount);
}

void take_money(Account& account, int amount) {
    account.balance.fetch_sub(amount);
}

void print_accounts(const Account& family,
                    const Account& mall,
                    const Account& gas)
{
    bank_mutex.lock();

    std::cout << std::format(
        "+---------------+\n"
        "|     BANK      |\n"
        "+---------------+\n"
        "| {:<5}: {:>6} |\n"
        "| {:<5}: {:>6} |\n"
        "| {:<5}: {:>6} |\n"
        "+---------------+\n",
        family.name, family.balance.load(),
        mall.name,   mall.balance.load(),
        gas.name,    gas.balance.load()
    );

    bank_mutex.unlock();
}

// gas
// mall
// family
// child
//
// family -> mall
// child -> gas
void transfer(Account& from, Account& to, int amount)
{
    // -------
    std::lock_guard lock(bank_mutex);
    
    int current = from.balance.load();

    if (current >= amount) {
        std::cout << std::format(
            "transferring {} from {} ({}) to {} ({})\n",
            amount,
            from.name, current,
            to.name,   to.balance.load()
        );

        add_money(to, amount);
        take_money(from, amount);
        return;
    } else {
        std::cout << std::format(
            "cant transfer {} from {} ({}) to {} ({})\n",
            amount,
            from.name, current,
            to.name,   to.balance.load()
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

int main() {
    Account family("FAM", INIT_BALANCE);
    Account mall("MALL");
    Account gas("GAS");

    print_accounts(family, mall, gas);

    std::thread wife_thread(wife, std::ref(family), std::ref(mall));
    std::thread husband_thread(husband, std::ref(family), std::ref(gas));

    wife_thread.join();
    husband_thread.join();

    print_accounts(family, mall, gas);
}


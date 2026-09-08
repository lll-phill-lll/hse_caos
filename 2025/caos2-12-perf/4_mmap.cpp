#include <algorithm>
#include <charconv>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

struct TStats {
    double      Min   = 1e9;
    double      Max   = -1e9;
    double      Sum   = 0.0;
    uint64_t    Count = 0;

    void update(double v) {
        if (v < Min) Min = v;
        if (v > Max) Max = v;
        Sum += v;
        Count++;
    }
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);

    struct stat st;
    fstat(fd, &st);

    void *raw = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    const char *p   = static_cast<const char *>(raw);
    const char *end = p + st.st_size;

    std::unordered_map<std::string, TStats> stats;

    while (p < end) {
        const char *nl  = static_cast<const char *>(std::memchr(p, '\n', end - p));
        if (!nl) nl = end;
        const char *sep = static_cast<const char *>(std::memchr(p, ';',  nl - p));

        std::string name(p, sep - p);

        double v;
        auto [ptr, ec] = std::from_chars(sep + 1, nl, v);

        stats[name].update(v);

        p = nl + 1;
    }

    munmap(raw, st.st_size);

    std::vector<const std::pair<const std::string, TStats> *> ordered;
    ordered.reserve(stats.size());
    for (const auto &kv : stats) ordered.push_back(&kv);
    std::sort(ordered.begin(), ordered.end(),
        [](auto a, auto b) { return a->first < b->first; });

    std::cout << "{";
    bool first = true;
    for (const auto *kv : ordered) {
        if (!first) std::cout << ", ";
        first = false;
        std::cout << kv->first << "="
                  << std::fixed << std::setprecision(1)
                  << kv->second.Min << "/" << (kv->second.Sum / kv->second.Count) << "/" << kv->second.Max;
    }
    std::cout << "}\n";

    return 0;
}

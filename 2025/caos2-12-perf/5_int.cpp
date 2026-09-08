#include <algorithm>
#include <cstdint>
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
    int32_t  Min_x10 = INT32_MAX;
    int32_t  Max_x10 = INT32_MIN;
    int64_t  Sum_x10 = 0;
    uint64_t Count   = 0;

    void update(int32_t v_x10) {
        if (v_x10 < Min_x10) Min_x10 = v_x10;
        if (v_x10 > Max_x10) Max_x10 = v_x10;
        Sum_x10 += v_x10;
        Count++;
    }
};

int32_t parse_int(const char *p, const char *end) {
    bool neg = (*p == '-');
    if (neg) ++p;

    int32_t v = 0;
    while (p < end && *p != '.') {
        v = v * 10 + (*p - '0');
        ++p;
    }
    ++p;
    v = v * 10 + (*p - '0');

    return neg ? -v : v;
}

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

        int32_t v_x10 = parse_int(sep + 1, nl);

        stats[name].update(v_x10);

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
        const auto &s = kv->second;
        const double mn  = s.Min_x10 / 10.0;
        const double mx  = s.Max_x10 / 10.0;
        const double avg = static_cast<double>(s.Sum_x10) / (10.0 * s.Count);
        std::cout << kv->first << "="
                  << std::fixed << std::setprecision(1)
                  << mn << "/" << avg << "/" << mx;
    }
    std::cout << "}\n";

    return 0;
}

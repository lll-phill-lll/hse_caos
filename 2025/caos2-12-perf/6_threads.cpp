#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <sys/mman.h>
#include <sys/stat.h>
#include <thread>
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

    void merge(const TStats &o) {
        if (o.Min_x10 < Min_x10) Min_x10 = o.Min_x10;
        if (o.Max_x10 > Max_x10) Max_x10 = o.Max_x10;
        Sum_x10 += o.Sum_x10;
        Count   += o.Count;
    }
};

struct TStringHash {
    using is_transparent = void;
    using hash_type      = std::hash<std::string_view>;

    size_t operator()(std::string_view sv) const noexcept { return hash_type{}(sv); }
    size_t operator()(const std::string &s) const noexcept { return hash_type{}(s); }
};

using TMap = std::unordered_map<std::string, TStats, TStringHash, std::equal_to<>>;

int32_t parse_int(const char *&p) {
    bool neg = (*p == '-');
    if (neg) ++p;

    int32_t v = 0;
    while (*p != '.') {
        v = v * 10 + (*p - '0');
        ++p;
    }
    ++p;
    v = v * 10 + (*p - '0');
    ++p;

    return neg ? -v : v;
}

void process_chunk(const char *p, const char *end, TMap &stats) {
    stats.reserve(1024);
    while (p < end) {
        const char *sep = static_cast<const char *>(std::memchr(p, ';', end - p));
        if (!sep) break;

        std::string_view name(p, sep - p);

        const char *q = sep + 1;
        int32_t v_x10 = parse_int(q);

        // heterogeneous lookup: ищем по string_view, без копии std::string.
        auto it = stats.find(name);
        if (it == stats.end()) {
            it = stats.emplace(std::string(name), TStats{}).first;
        }
        it->second.update(v_x10);

        p = q + 1;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }

    int fd = ::open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    const size_t fsize = static_cast<size_t>(st.st_size);

    void *raw = ::mmap(nullptr, fsize, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    const char *data = static_cast<const char *>(raw);

    int nthreads = (argc > 2)
                       ? std::atoi(argv[2])
                       : static_cast<int>(std::thread::hardware_concurrency());
    if (nthreads <= 0) nthreads = 1;

    std::vector<size_t> bounds(nthreads + 1);
    bounds[0]        = 0;
    bounds[nthreads] = fsize;
    for (int i = 1; i < nthreads; ++i) {
        size_t guess = (fsize / nthreads) * i;
        while (guess < fsize && data[guess] != '\n') ++guess;
        if (guess < fsize) ++guess;
        bounds[i] = guess;
    }

    std::vector<TMap> partial(nthreads);
    std::vector<std::thread> workers;
    workers.reserve(nthreads);

    for (int i = 0; i < nthreads; ++i) {
        workers.emplace_back([&, i] {
            process_chunk(data + bounds[i], data + bounds[i + 1], partial[i]);
        });
    }
    for (auto &t : workers) t.join();

    TMap total = std::move(partial[0]);
    for (int i = 1; i < nthreads; ++i) {
        for (auto &kv : partial[i]) {
            auto it = total.find(kv.first);
            if (it == total.end()) {
                total.emplace(std::move(kv.first), kv.second);
            } else {
                it->second.merge(kv.second);
            }
        }
    }

    munmap(raw, fsize);

    std::vector<const std::pair<const std::string, TStats> *> ordered;
    ordered.reserve(total.size());
    for (const auto &kv : total) ordered.push_back(&kv);
    std::sort(ordered.begin(), ordered.end(),
        [](auto a, auto b) { return a->first < b->first; });

    std::cout << "{";
    bool first = true;
    std::cout << std::fixed << std::setprecision(1);
    for (const auto *kv : ordered) {
        if (!first) std::cout << ", ";
        first = false;
        const auto &s = kv->second;
        const double mn  = s.Min_x10 / 10.0;
        const double mx  = s.Max_x10 / 10.0;
        const double avg = static_cast<double>(s.Sum_x10) / (10.0 * s.Count);
        std::cout << kv->first << "=" << mn << "/" << avg << "/" << mx;
    }
    std::cout << "}\n";

    return 0;
}

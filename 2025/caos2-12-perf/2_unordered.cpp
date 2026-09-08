#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>

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

    std::ifstream in(argv[1]);

    std::unordered_map<std::string, TStats> stats;
    std::string line;

    while (std::getline(in, line)) {
        auto sep = line.find(';');

        std::string name = line.substr(0, sep);
        double v = std::stod(line.substr(sep + 1));

        stats[name].update(v);
    }

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

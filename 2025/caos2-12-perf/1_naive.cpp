#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>

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

    std::map<std::string, TStats> stats;
    std::string line;

    while (std::getline(in, line)) {
        auto sep = line.find(';');

        std::string name = line.substr(0, sep);
        double v = std::stod(line.substr(sep + 1));

        stats[name].update(v);
    }

    std::cout << "{";
    bool first = true;
    for (const auto &[name, s] : stats) {
        if (!first) std::cout << ", ";
        first = false;
        std::cout << name << "="
                  << std::fixed << std::setprecision(1)
                  << s.Min << "/" << (s.Sum / s.Count) << "/" << s.Max;
    }
    std::cout << "}\n";

    return 0;
}

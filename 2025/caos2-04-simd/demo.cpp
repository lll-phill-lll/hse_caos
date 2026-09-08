#include <immintrin.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string>

void tolower_scalar(unsigned char* s, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = s[i];
        if (c >= 'A' && c <= 'Z') s[i] = c + 32;
    }
}

__attribute__((target("avx512bw,avx512vl")))
void tolower_avx512(unsigned char* s, size_t n) {
    size_t i = 0;

    const __m512i A_1 = _mm512_set1_epi8('A' - 1);
    const __m512i Z_1 = _mm512_set1_epi8('Z' + 1);
    const __m512i add = _mm512_set1_epi8('a' - 'A');


    for (; i + 64 <= n; i += 64) {
        __m512i x = _mm512_loadu_si512(reinterpret_cast<const __m512i *>(s + i));

        // x > 'A'-1  ---> x >= 'A'
        __mmask64 geA = _mm512_cmpgt_epu8_mask(x, A_1);
        // 'Z'+1 > x  ---> x <= 'Z'
        __mmask64 leZ = _mm512_cmpgt_epu8_mask(Z_1, x);

        __mmask64 m = geA & leZ;

        __m512i y = _mm512_mask_add_epi8(x, m, x, add);

        _mm512_storeu_si512(reinterpret_cast<__m512i *>(s + i), y);
    }

    // tail
    for (; i < n; ++i) {
        unsigned char c = s[i];
        if (c >= 'A' && c <= 'Z') s[i] = c + 32;
    }
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "usage:\n"
                  << argv[0] << " scalar|avx512 input output\n";
        return 1;
    }

    std::string impl = argv[1];

    std::ifstream in(argv[2], std::ios::binary | std::ios::ate);
    if (!in) { std::cerr << "failed to open input\n"; return 1; }

    size_t size = in.tellg();
    in.seekg(0);

    std::vector<unsigned char> data(size);
    in.read((char*)data.data(), (std::streamsize)size);
    if (!in) { std::cerr << "failed to read input\n"; return 1; }
    in.close();

    auto t0 = std::chrono::high_resolution_clock::now();

    if (impl == "avx512") {
        tolower_avx512(data.data(), size);
    } else if (impl == "scalar") {
        tolower_scalar(data.data(), size);
    } else {
        std::cerr << "unknown impl: " << impl << " (use scalar|avx512)\n";
        return 2;
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    std::ofstream out(argv[3], std::ios::binary);
    if (!out) { std::cerr << "failed to open output\n"; return 1; }
    out.write((char*)data.data(), (std::streamsize)size);
    if (!out) { std::cerr << "failed to write output\n"; return 1; }

    std::chrono::duration<double> dt = t1 - t0;
    double gb = size / (1024.0 * 1024.0 * 1024.0);
    std::cout << "processed " << gb << " GB in " << dt.count()
              << " sec, " << (gb / dt.count()) << " GB/s\n";
}

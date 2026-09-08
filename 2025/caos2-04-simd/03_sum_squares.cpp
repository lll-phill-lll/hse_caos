#include <random>
#include <cstddef>
#include <iostream>
#include <vector>
#include <emmintrin.h>
#include <iomanip>

std::vector<double> generate_random_array(std::size_t n) {
    std::vector<double> v(n);

    std::mt19937_64 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (std::size_t i = 0; i < n; ++i) {
        v[i] = dist(rng);
    }

    return v;
}

extern "C"
double sumsq(std::size_t n, const double* vec);

double sumsq_scalar(std::size_t n, const double* vec) {
    double s = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        s += vec[i] * vec[i];
    }
    return s;
}

double sumsq_sse2(std::size_t n, const double* vec) {
    // xmm0 = [0,0]
    __m128d acc = _mm_setzero_pd(); 

    const double* p = vec;
    std::size_t pairs = n >> 1;

    for (std::size_t i = 0; i < pairs; ++i) {
        // movupd
        __m128d x = _mm_loadu_pd(p);
        // mulpd
        x = _mm_mul_pd(x, x);
        // addpd
        acc = _mm_add_pd(acc, x);
        p += 2;
    }

    // tail
    double tail = 0.0;
    if (n & 1) {
        double t = *p;
        tail = t * t;
    }

    // movupd xmm1, xmm0
    __m128d tmp = acc;
    // unpckhpd
    tmp = _mm_unpackhi_pd(tmp, tmp);
    // addsd
    acc = _mm_add_sd(acc, tmp);

    double result = _mm_cvtsd_f64(acc) + tail;
    return result;
}

int main() {
    std::vector<double> v = generate_random_array(100000);// 

    std::cout << std::setprecision(17);

    std::cout << "sse2:\t" << sumsq_sse2(v.size(), v.data()) << std::endl;
    std::cout << "asm:\t" << sumsq(v.size(), v.data()) << std::endl;
    std::cout << "scalar:\t" << sumsq_scalar(v.size(), v.data()) << std::endl;
}


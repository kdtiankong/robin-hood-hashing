#pragma once

#include <limits>
#include <cstdint>

/// Extremely fast, RNG that passes the BigCrush test.
/// @see http://xoroshiro.di.unimi.it/xoroshiro128plus.c
class XorShift128Plus {
public:
    typedef uint64_t result_type;

    // random state created at https://www.random.org/cgi-bin/randbyte?nbytes=8&format=h
    XorShift128Plus(uint64_t initialState = UINT64_C(0x853c49e6748fea9b)) {
        seed(initialState);
    }

    void seed(uint64_t state) {
        mState[0] = splitmix64(state);
        mState[1] = splitmix64(state);
    }

    inline uint64_t operator()() {
        const uint64_t s0 = mState[0];
        uint64_t s1 = mState[1];
        const uint64_t result = s0 + s1;

        s1 ^= s0;
        mState[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
        mState[1] = rotl(s1, 36); // c

        return result;
    }

    /// Random float value between 0 and 1.
    inline double rand01() {
        // 1.0 / (2^64 - 1)
        return operator()() * 5.4210108624275221703311375920553e-20;
    }

    /// Random float value between min and max.
    inline double operator()(double min_val, double max_val) {
        return (max_val - min_val) * rand01() + min_val;
    }

    // Generates using the given range. This has a modulo bias.
    // see https://github.com/imneme/pcg-c-basic/blob/master/pcg_basic.c#L79
    inline uint64_t operator()(uint64_t bound) {
        const auto threshold = (0 - bound) % bound;

        for (;;) {
            const auto r = operator()();
            if (r >= threshold) {
                return r % bound;
            }
        }
    }

    inline static uint64_t max() {
        return -1;
    }

    inline static uint64_t min() {
        return 0;
    }

private:
    static inline uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    // see http://xoroshiro.di.unimi.it/splitmix64.c
    static inline uint64_t splitmix64(uint64_t& x) {
        uint64_t z = (x += UINT64_C(0x9E3779B97F4A7C15));
        z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
        z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
        return z ^ (z >> 31);
    }

    uint64_t mState[2];
};

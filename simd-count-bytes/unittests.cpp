#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "all.h"

class UnitTests {

    static const size_t size = 1024 * 8;
    uint8_t array[size];
    uint8_t byte;

    struct Failed {};

public:
    bool run() {

        byte = 42;
     
        try {
            case1();
            case2();
            case3();

            puts("All OK");
            return true;
        } catch (Failed&) {
            return false;
        }
    }

private:

    void case1() {
        memset(array, 0, size);
        test();
    }

    void case2() {
        memset(array, byte, size);
        test();
    }

    void case3() {
        srand(0);
        memset(array, 0, size);
        for (size_t i=0; i < size; i++) {
            if (rand() % 4 == 0)
                array[i] = byte;
        }

        test();
    }

    void test() {
        const uint64_t reference = scalar_count_bytes((const uint8_t*)array, size, byte);
#ifdef HAVE_SSE
        test("SSE", sse_count_byte, reference);
        test("SSE (popcnt)", sse_count_byte_popcount, reference);
#endif
#ifdef HAVE_AVX2
#endif
#ifdef HAVE_AVX512BW
        test("AVX512", avx512bw_count_bytes, reference);
#endif
    }

    template <typename FUNCTION>
    void test(const char* name, FUNCTION fun, uint64_t reference) {
        const uint64_t result = fun(array, size, byte);
        if (result != reference) {
            printf("%s failed: reference = %lu result = %lu\n", name, reference, result);
            throw Failed{};
        }
    }

};

int main() {

    UnitTests tests;
    tests.run();
}

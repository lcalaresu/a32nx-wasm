#pragma once

#include <stdint.h>

/*
* ============== *
* Bit Operations *
* ============== *
*/


class bitOper {
private:
public:
    static bool getbit(UINT64 num, const int bit_loc) {
        /*
        return(((num & (1 << bit_loc)) << 63 - bit_loc) >> 63);
        */
        return (((num & ((uint64_t)1 << bit_loc)) == 0) ? 0 : 1);
    }
    static uint64_t setbit(uint64_t num, const int bit_loc) {
        return (num | ((uint64_t)1 << bit_loc));
    }
    static uint64_t togglebit(uint64_t num, const int bit_loc) {
        return (num ^ ((uint64_t)1 << bit_loc));
    }
    static uint64_t clearbit(uint64_t num, const int bit_loc) {
        return (num & ~((uint64_t)1 << bit_loc));
    }
    static uint64_t clearallbits(uint64_t num) {
        return (num & 0);
    }
    static uint64_t setallbits(uint64_t num) {
        return (num | UINT64_MAX);
    }
};

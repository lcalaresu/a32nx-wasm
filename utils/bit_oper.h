#pragma once

/*
* ============== *
* Bit Operations *
* ============== *
*/
class bitOper {
private:
public:
    bool getbit(unsigned long int num, int bit_loc) {
        /*
        return(((num & (1 << bit_loc)) << 63 - bit_loc) >> 63);
        */
        return ((num & (1 << bit_loc) == 0)? 0 : 1);
    }
    unsigned int setbit(unsigned long int num, int bit_loc) {
        return (num | (1 << bit_loc));
    }
    unsigned int togglebit(unsigned long int num, int bit_loc) {
        return (num ^ (1 << bit_loc));
    }
    unsigned int clearbit(unsigned long int num, int bit_loc) {
        return (num & ~(1 << bit_loc));
    }
};

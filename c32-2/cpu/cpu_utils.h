


#include <iostream>
#include <vector>
#include <cstdint>
#include "data.h"
#pragma once
using namespace std;

uint32_t rand32(uint32_t rand_state){
    rand_state ^= rand_state << 13;
    rand_state ^= rand_state >> 17;
    rand_state ^= rand_state << 5;
    return rand_state;
}

inline __attribute__((always_inline))
void stall_30() {
    asm volatile (
        ".rept 30\n\t"
        "nop\n\t"
        ".endr\n\t"
        ::: "memory"
    );
}


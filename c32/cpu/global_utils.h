#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>
#include <chrono> // for time
#pragma once

#if EX_BIT_MAPING == 0

inline
uint32_t get_bit_section(uint32_t value, uint32_t start_bit, uint32_t end_bit) {
    return (value >> start_bit) & ((1u << (end_bit - start_bit + 1)) - 1);
}
inline
uint32_t inject_bit_section(uint32_t original, uint32_t new_value, uint32_t start_bit, uint32_t end_bit) {
    uint32_t mask = (1u << (end_bit - start_bit + 1)) - 1;
    return (original & ~(mask << start_bit)) | ((new_value & mask) << start_bit);
}
#else 
#include <immintrin.h>
inline
uint32_t get_bit_section(uint32_t value, uint32_t start_bit, uint32_t end_bit) {
    uint32_t mask = ((1u << (end_bit - start_bit + 1)) - 1) << start_bit;
    return _pext_u32(value, mask);  // 1 instruction
}
inline
uint32_t inject_bit_section(uint32_t original, uint32_t new_value, uint32_t start_bit, uint32_t end_bit) {
    uint32_t mask = ((1u << (end_bit - start_bit + 1)) - 1) << start_bit;
    return _pdep_u32(new_value, mask) | (original & ~mask);  // 3 instructions
}
#endif

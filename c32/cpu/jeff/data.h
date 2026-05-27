#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <stdfloat> 
#include <cmath>
#include <immintrin.h>
#include <Utility>
#include <nlohmann/json.hpp>
using namespace std;



union Reg{
        uint32_t data;
        int32_t sdata;
        float fdata;

        // ============decode phase maps===========
        struct {
            uint32_t p :24;
            uint32_t opcode : 8;
        }opcode;
        // splitting maps
        struct { // 8_8_8 splittimg map
            uint32_t p1 : 8;
            uint32_t p2 : 8;
            uint32_t p3 : 8;
            uint32_t opcode : 8;
        } bits_8_8_8;
        struct { // 8_16 splitting map
            uint32_t p1 : 8;
            uint32_t p2 : 16;
            uint32_t opcode : 8;
        } bits_8_16;
        struct { // 16_8 splitting map
            uint32_t p1 : 16;
            uint32_t p2 : 8;
            uint32_t opcode : 8;
        } bits_16_8;
        // =========register maps ============
        struct {
            uint32_t data : 16;
            uint32_t flags : 8;
            uint32_t meta_data : 8;
        }reg;
};

enum sf_map {
    active = 0,
    interruptable = 1,
    do_jmp = 2,
    offset0 = 3,
    offset1 = 4,
    offset2 = 5,
    clk = 6,
    curent_loop = 7,
    sc_offset = 8,
    counter = 9,
    counter_target = 10,
    var0 = 11,
    var1 = 12,
    var2 = 13,
    alu_flags = 14,
    force_intrp_ptr = 15,
    force_sc_ptr = 16,
    event = 17,
    pf_in = 18,
    pf_out = 19,
    lpwt = 20,
    // stats sf's
    core_id = 21,
    reg_size = 22,
    stack_size = 23,
    cache_size = 24,
    core_count = 25,
    call_stack_depth = 26
};

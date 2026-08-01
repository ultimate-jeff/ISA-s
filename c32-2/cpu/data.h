
#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono> 
#pragma once
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

enum sf_map{
    active = 0,
    interuptable = 1,
    do_jmp = 2,
    offset0 = 3,
    offset1 = 4,
    offset2 = 5,
    clk = 6,
    current_loop = 7,
    sc_offset = 8,
    var0 = 9,
    var1 = 10,
    var2 = 11,
    alu_flags = 12,
    event = 13,
    intrp_ptr = 14,
    counter = 15,
    counter_target = 16,
    cs_intrp = 17,
    call_stack_ptr = 18,
    port_core_id = 19,
    ra = 20,
    event_target = 21,
    wating_on = 22,
    // 23-30 reserved
    core_id = 31,
    reg_size = 32,
    stack_size = 33,
    cache_size = 34,
    csd = 35,          // call stack depth
    device_type = 36,  // 0=cpu,1=gpu,2=ram,3=ssd,4=network_card,5=audio_card,6=io_manager,7=graphics_controller
};
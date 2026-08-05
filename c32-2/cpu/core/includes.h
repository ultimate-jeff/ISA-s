
#include <iostream>
#include <chrono> 
#include <vector>
#include <cstdint>
#include <array>
#include <bit>
#include <cmath>
#include <functional>
#include <any>

using std::uint32_t;
using std::uint16_t;
using std::uint8_t;
using std::array;
using std::cout;
using std::endl;
using std::function;
using std::isinf;
using std::fmod;
using std::abs;
using std::min;
using std::max;


#pragma once

#include "chip_config.h"




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

struct alignas(64) Memory_unit {
    array<Reg, REG_SIZE> regs;
    array<Reg, STACK_SIZE> stack;
    array<Reg, SF_SIZE> sf;
    array<Reg, CACHE_SIZE>* cache = nullptr;
    array<Reg, PORT_SIZE>* ports = nullptr;
    array<Reg, CALL_STACK_DEPTH> call_stack;
};

const Reg nullreg = {0};
const Reg error_reg = {0b00000000000000100000000000000000}; // error flag set

enum sf_map {
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

class Core;

class Device{
public:
    Memory_unit mem;
    bool brantched = false;
    array<Device*, AVAILABLE_CIDS> cores = {nullptr};
    virtual ~Device() = default;
    virtual void exacute(){
        cout << "unimplumented exacute / clock method of device at " << this << endl;
    };
    virtual void release(){
        cout << " release method not implumented in device type class at " << this << endl;
    };
    virtual void attach(){
        cout << "attach method is un implumented in device type class at " << this << endl;
    };

public:
    inline __attribute__((always_inline))
    void _call(uint16_t addr){
        append_call_stack(get_sf(sf_map::clk));
        set_sf(sf_map::clk,addr);
    }
    inline __attribute__((always_inline))
    void _intrp(uint16_t addr){
        if(get_sf(sf_map::interuptable).reg.data){
            set_sf(sf_map::intrp_ptr, addr);
        }
    }

public: // utils
    inline __attribute__((always_inline))
    void append_call_stack(Reg data){
        inc_cs_ptr();
        set_call_stack(
            get_sf(sf_map::call_stack_ptr).reg.data,
            data
        );
    }
    inline __attribute__((always_inline))
    void dec_cs_ptr(){
        set_sf(
            sf_map::call_stack_ptr,
            get_sf(sf_map::call_stack_ptr).reg.data - 1
        );
    }
    inline __attribute__((always_inline))
    void inc_cs_ptr(){
        set_sf(
            sf_map::call_stack_ptr,
            get_sf(sf_map::call_stack_ptr).reg.data + 1
        );
    }
    inline __attribute__((always_inline))
    Reg get_top_call_stack(){
        return get_call_stack(get_sf(sf_map::call_stack_ptr).reg.data);
    }

public: // getters
    inline __attribute__((always_inline))
    Reg get_reg(uint16_t addr){
        return mem.regs[addr & REG_MASK];
    }
    inline __attribute__((always_inline))
    Reg get_stack(uint16_t addr){
        return mem.stack[addr & STACK_MASK];
    }
    inline __attribute__((always_inline))
    Reg get_sf(uint16_t addr){
        return mem.sf[addr & SF_MASK];
    }
    inline __attribute__((always_inline))
    Reg get_call_stack(uint16_t addr){
        return mem.call_stack[addr & CALL_STACK_MASK];
    }
    inline __attribute__((always_inline))
    Reg get_cache(uint16_t addr){
        Reg result;
        Reg lock_data = (*mem.cache)[0];
        if(lock_data.reg.meta_data == 0 && lock_data.reg.data == get_sf(sf_map::core_id).reg.data){
            result = (*mem.cache)[addr & CACHE_MASK];
        }else{
            result = error_reg; // set error flag
        }
        return result;
    }
    inline __attribute__((always_inline))
    Reg get_port(uint16_t addr){
        Reg result;
        Reg lock_data = (*mem.ports)[0];
        if(lock_data.reg.meta_data == 0 && lock_data.reg.data == get_sf(sf_map::core_id).reg.data){
            result = (*mem.ports)[addr & PORT_MASK];
        }else{
            result = error_reg; // set error flag
        }
        return result;
    }
    inline __attribute__((always_inline))
    Device *get_core(uint16_t addr){
        return cores[addr & CORE_MASK];
    }
    inline __attribute__((always_inline))
    Core *get_core(uint16_t addr,bool overload_distinguisher){
        std::any d = cores[addr & CORE_MASK];
        return std::any_cast<Core*>(d);
    }
public: // setters
    inline __attribute__((always_inline))
    void set_reg(uint16_t addr , Reg data){
        mem.regs[addr & REG_MASK] = data;
    }
    inline __attribute__((always_inline))
    void set_reg(uint16_t addr, uint32_t data){
        mem.regs[addr & REG_MASK].data = data;
    }
    inline __attribute__((always_inline))
    void set_stack(uint16_t addr , Reg data){
        mem.stack[addr & STACK_MASK] = data;
    }
    inline __attribute__((always_inline))
    void set_stack(uint16_t addr , uint32_t data){
        mem.stack[addr & STACK_MASK].data = data;
    }
    inline __attribute__((always_inline))
    void set_sf(uint16_t addr , Reg data){
        mem.sf[static_cast<uint16_t>(addr) & SF_MASK] = data;
    }
    inline __attribute__((always_inline))
    void set_sf(uint16_t addr , uint32_t data){
        mem.sf[static_cast<uint16_t>(addr) & SF_MASK].data = data;
    }
    inline __attribute__((always_inline))
    void set_call_stack(uint16_t addr , Reg data){
        mem.call_stack[addr & CALL_STACK_MASK] = data;
    }
    inline __attribute__((always_inline))
    void set_cache(uint16_t addr , Reg data){
        Reg lock_data = (*mem.cache)[0];
        if(lock_data.reg.meta_data == 0 || lock_data.reg.data == get_sf(sf_map::core_id).reg.data){
            (*mem.cache)[addr & CACHE_MASK] = data;
        }else{
            // error
            set_sf(sf_map::event,error_reg); // set error flag / ------------------------------------------------------ make shure you look at this to define an event table 
        }
    }
    inline __attribute__((always_inline))
    void set_port(uint16_t addr , Reg data){
        Reg lock_data = (*mem.ports)[0];
        if(lock_data.reg.meta_data == 0 || lock_data.reg.data == get_sf(sf_map::core_id).reg.data){
            (*mem.ports)[addr & PORT_MASK] = data;
        }else{
            // error
            set_sf(sf_map::event,error_reg); // set error flag / ------------------------------------------------------ make shure you look at this to define an event table 
        }
    }
    
public: 
    inline __attribute__((always_inline))
    bool intrp_avalable(){
        return get_sf(sf_map::interuptable).reg.data && get_sf(sf_map::intrp_ptr).reg.data != 0;
    }
    inline __attribute__((always_inline))
    void handle_intrps(){
        if(intrp_avalable()){
            set_sf(sf_map::intrp_ptr,0);
            _intrp(get_sf(sf_map::intrp_ptr).reg.data);
            return;
        }
        return;
    }

};


#include <iostream>
#include <vector>
#include <cstdint>
#include "cpu_utils.h"
#include "data.h"
#include "config.h"
#include <array>
#include <bit>
#include <functional>

using namespace std;

struct alignas(64) Memory_unit {
    array<Reg, REG_SIZE> regs;
    array<Reg, STACK_SIZE> stack;
    array<Reg, SF_SIZE> sf;
    array<Reg, CACHE_SIZE>* cache = nullptr;
    array<Reg, PORT_SIZE>* ports = nullptr;
    array<Reg, CALL_STACK_DEPTH> call_stack;
};
struct alignas(16) WF_flags{
    bool wf_event = false;
    bool wf_intrp = false;
    bool wf_sc_intrp = false;

    uint16_t event_target = 0;
};

class ALU{
    uint8_t ALU_flags = 1;
    public:
    ALU(){

    }
    private:
    inline __attribute__((always_inline))
    uint32_t rol(uint32_t value, int shift) {
        shift &= 31;
        return (value << shift) | (value >> (32 - shift));
    }
    inline __attribute__((always_inline))
    uint8_t gen_flags_float(float result , float a , float b){
        return (uint8_t)(
            (1u) | //true
            ((result == 0) << 1) | // zero
            (isinf(result) << 3) | // carry
            ((result < 0) << 4) | // sign
            ((__builtin_parity((uint32_t)result & UINT32_MAX)) << 5) | 
            (0u << 6 ) | // error -- 64
            (1u << 7) // is float -- 128
        );
    }
    inline __attribute__((always_inline))
    uint8_t gen_flags_16(uint32_t result , uint16_t ra , uint16_t rb){
        return (uint8_t)(
            (1u) | //true  -- 1
            ((result == 0) << 1) | // zero -- 2
            (((ra & rb) > 0) << 2) | // carry -- 4
            ((result > UINT16_MAX) << 3) | // overflow -- 8
            (((result >> 15 ) & 1u) << 4) | // sigh -- 16
            ((__builtin_parity((result & UINT16_MAX))) << 5) | // parity -- 32
            (0u << 6 ) | // error -- 64
            (0u << 7) // is float -- 128
        );
    }
    inline __attribute__((always_inline))
    void pack_reg(Reg *result , Reg a , Reg b){
        result->reg.meta_data = a.reg.meta_data;
        result->reg.flags = gen_flags_16(result->reg.data,a.reg.data,b.reg.data);
    }
    public:
    inline __attribute__((always_inline))
    Reg ADD(Reg a , Reg b){
        Reg result;
        result.reg.data = a.reg.data + b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg SUB(Reg a , Reg b){
        Reg result;
        result.reg.data = a.reg.data - b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg AND(Reg a , Reg b){
        Reg result;
        result.reg.data = a.reg.data & b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg NAND(Reg a , Reg b){
        Reg result;
        result.reg.data = ~(a.reg.data & b.reg.data);
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg OR(Reg a , Reg b){
        Reg result;
        result.reg.data = a.reg.data | b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg XOR(Reg a , Reg b){
        Reg result;
        result.reg.data = a.reg.data ^ b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg NOT(Reg a){
        Reg result;
        result.reg.data = ~(a.reg.data);
        pack_reg(&result,a,nullreg);
        return result;
    }
    inline __attribute__((always_inline))
    Reg MULT(Reg a , Reg b){
        Reg result;
        result.reg.data = a.reg.data * b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg DIV(Reg a , Reg b){
        Reg result;
        result.reg.data = a.reg.data / b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg MOD(Reg a , Reg b){
        Reg result;
        result.reg.data = a.reg.data % b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg ABS(Reg a){
        Reg result;
        result.reg.data = abs((int16_t)a.reg.data);
        pack_reg(&result,a,nullreg);
        return result;
    }
    inline __attribute__((always_inline))
    Reg ADI(Reg a,uint16_t imm){
        Reg result;
        Reg b;
        b.reg.data = imm;
        result.reg.data = a.reg.data + b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg SDI(Reg a,uint16_t imm){
        Reg result;
        Reg b;
        b.reg.data = imm;
        result.reg.data = a.reg.data - b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg DDI(Reg a,uint16_t imm){
        Reg result;
        Reg b;
        b.reg.data = imm;
        result.reg.data = a.reg.data / b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg MDI(Reg a,uint16_t imm){
        Reg result;
        Reg b;
        b.reg.data = imm;
        result.reg.data = a.reg.data * b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg ROL_L(Reg a , Reg b){
        Reg result;
        result.reg.data = rol(a.reg.data,b.reg.data);
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg ROL_R(Reg a , Reg b){
        Reg result;
        result.reg.data = rol(a.reg.data,b.reg.data);
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg SHIFT_L(Reg a , Reg b){
        Reg result;
        result.reg.data = a.reg.data << b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg SHIFT_R(Reg a , Reg b){
        Reg result;
        result.reg.data = a.reg.data >> b.reg.data;
        pack_reg(&result,a,b);
        return result;
    }
    inline __attribute__((always_inline))
    Reg SHIFTF_L(Reg a , Reg b){
        Reg result;
        result.data = a.data << b.data;
        return result;
    }
    inline __attribute__((always_inline))
    Reg SHIFTF_R(Reg a , Reg b){
        Reg result;
        result.data = a.data >> b.data;
        return result;
    }
    inline __attribute__((always_inline))
    Reg fADD(Reg a , Reg b){
        Reg result;
        result.fdata = a.fdata + b.fdata;
        ALU_flags = gen_flags_float(result.fdata,a.fdata,b.fdata);
        return result;
    }
    inline __attribute__((always_inline))
    Reg fSUB(Reg a , Reg b){
        Reg result;
        result.fdata = a.fdata - b.fdata;
        ALU_flags = gen_flags_float(result.fdata,a.fdata,b.fdata);
        return result;
    }
    inline __attribute__((always_inline))
    Reg fADD(Reg a , Reg b){
        Reg result;
        result.fdata = a.fdata + b.fdata;
        ALU_flags = gen_flags_float(result.fdata,a.fdata,b.fdata);
        return result;
    }
    inline __attribute__((always_inline))
    Reg fMULT(Reg a , Reg b){
        Reg result;
        result.fdata = a.fdata * b.fdata;
        ALU_flags = gen_flags_float(result.fdata,a.fdata,b.fdata);
        return result;
    }
    inline __attribute__((always_inline))
    Reg fDIV(Reg a , Reg b){
        Reg result;
        result.fdata = a.fdata / b.fdata;
        ALU_flags = gen_flags_float(result.fdata,a.fdata,b.fdata);
        return result;
    }
    inline __attribute__((always_inline))
    Reg fMOD(Reg a , Reg b){
        Reg result;
        result.fdata = fmod(a.fdata,b.fdata);
        ALU_flags = gen_flags_float(result.fdata,a.fdata,b.fdata);
        return result;
    }
    
    inline __attribute__((always_inline))
    bool CMP(Reg a , uint8_t flags , uint8_t invert_mask){
        uint8_t a_flags = a.reg.flags ^ invert_mask;
        bool do_jmp = ((a_flags & flags) == flags);
        if(DEBUG >= 3){
            cout << "CMP: a_flags = " << (int)a_flags << ", flags = " << (int)flags << ", invert_mask = " << (int)invert_mask << ", sf_do_jmp = " << do_jmp << endl;
        }
        return do_jmp;
    }
    inline __attribute__((always_inline))
    bool CMP_OR(Reg a , uint8_t flags , uint8_t invert_mask){
        uint8_t a_flags = a.reg.flags ^ invert_mask;
        bool do_jmp = ((a_flags & flags) != 0);
        if(DEBUG >= 3){
            cout << "CMP_OR: a_flags = " << (int)a_flags << ", flags = " << (int)flags << ", invert_mask = " << (int)invert_mask << ", sf_do_jmp = " << do_jmp << endl;
        }
        return do_jmp;
    }
    inline __attribute__((always_inline))
    bool CMP_AND(Reg a , uint8_t flags , uint8_t invert_mask){
        uint8_t a_flags = a.reg.flags ^ invert_mask;
        bool do_jmp = (a_flags == flags);
        if(DEBUG >= 3){
            cout << "CMP_AND: a_flags = " << (int)a_flags << ", flags = " << (int)flags << ", invert_mask = " << (int)invert_mask << ", sf_do_jmp = " << do_jmp << endl;
        }
        return do_jmp;
    }

    

};

class Core{
public:
    ALU alu;
    Memory_unit mem;
    array<Core*, CORE_COUNT> cores = {nullptr};
    WF_flags wf_flags;

private:
    static void null_exacute(){
        stall_30();
    }
    static void WF_exacute(){
        stall_30();
    }
public:
    std::function<void()> exacute_ptr = null_exacute;
    Core(array<Reg, CACHE_SIZE>* cache_ptr, array<Reg, PORT_SIZE>* ports_ptr, array<Core*, CORE_COUNT> cores_ptr){
        mem.cache = cache_ptr;
        mem.ports = ports_ptr;
        cores = cores_ptr;
    }
    inline __attribute__((always_inline))
    void exacute(){
        exacute_ptr();
    }

private: // instructions
    void HALT(Reg instruction){
        set_sf(sf_map::active,0);
        exacute_ptr = null_exacute;
    }
    void NOP(Reg instruction){
        stall_30();
    }
    void STALL(Reg instruction){
        for(int i = 0 ; i < instruction.bits_8_8_8.p1 ; i++){
            stall_30();
        }
    }
    void LR(Reg instruction){
        set_stack(
            instruction.bits_8_16.p1,
            instruction.bits_8_16.p2
        );
    }
    inline __attribute__((always_inline))
    void move(Reg instruction){
        set_stack(
            instruction.bits_8_8_8.p1,
            get_stack(instruction.bits_8_8_8.p2)
        );
    }
    void move_mem(Reg instruction){
        Reg addr_a = get_stack(instruction.bits_8_8_8.p1);
        Reg addr_b = get_stack(instruction.bits_8_8_8.p2);
        set_reg(
            addr_a.reg.data,
            get_reg(addr_b.reg.data)
        );
    }
    void move_cache(Reg instruction){
        Reg addr_a = get_stack(instruction.bits_8_8_8.p1);
        Reg addr_b = get_stack(instruction.bits_8_8_8.p2);
        set_cache(
            addr_a.reg.data,
            get_cache(addr_b.reg.data)
        );
    }
    void push_stack(Reg instruction){
        set_stack(
            instruction.bits_16_8.p2,
            get_reg(instruction.bits_16_8.p1)   
        );
    }
    void pull_stack(Reg instruction){
        set_reg(
            instruction.bits_8_16.p2,
            get_stack(instruction.bits_8_16.p1)
        );
    }
    void push_cache(Reg instruction){
        Reg addr_a = get_stack(instruction.bits_8_8_8.p1);
        Reg addr_b = get_stack(instruction.bits_8_8_8.p2);
        set_cache(
            addr_a.reg.data,
            get_reg(addr_b.reg.data)
        );
    }
    void pull_cache(Reg instruction){
        Reg addr_a = get_stack(instruction.bits_8_8_8.p1);
        Reg addr_b = get_stack(instruction.bits_8_8_8.p2);
        set_reg(
            addr_b.reg.data,
            get_cache(addr_a.reg.data)
        );
    }
    void cmove(Reg instruction){
        if(get_sf(sf_map::do_jmp).reg.data){
            move(instruction);
        }
    }
    void move_ptr(Reg instruction){
        Reg addr_a = get_stack(instruction.bits_8_8_8.p1);
        Reg addr_b = get_stack(instruction.bits_8_8_8.p2);
        set_stack(
            addr_b.reg.data,
            get_stack(addr_a.reg.data)
        );
    }
    void push_sf(Reg instruction){
        set_sf(
            instruction.bits_8_8_8.p2,
            get_stack(instruction.bits_8_8_8.p1)
        );
    }
    void pull_sf(Reg instruction){
        set_stack(
            instruction.bits_8_8_8.p2,
            get_sf(instruction.bits_8_8_8.p1)
        );
    }
    void set_sf(Reg instruction){
        set_sf(
            instruction.bits_8_16.p1,
            instruction.bits_8_16.p2
        );
    }
    void swarm(Reg instruction){
        Reg data = get_stack(instruction.bits_8_8_8.p1);
        for(uint16_t i = instruction.bits_8_8_8.p2 ; i < instruction.bits_8_8_8.p3 ; i++){
            set_cache(i,data);
        }
    }
    void LS(Reg instruction){
        set_stack(
            instruction.bits_8_16.p1,
            instruction.bits_8_16.p2
        );
    }
    void LC(Reg instruction){
        set_cache(
            instruction.bits_8_16.p1,
            Reg {instruction.bits_8_16.p2}
        );
    }
    void append_cs(Reg instruction){
        set_sf(sf_map::call_stack_ptr,get_sf(sf_map::call_stack_ptr).reg.data + 1);
        set_call_stack(
            get_sf(sf_map::call_stack_ptr).reg.data,
            get_stack(instruction.bits_8_16.p1)
        );
    }
    void pop_cs(Reg instruction){
        set_sf(sf_map::call_stack_ptr,get_sf(sf_map::call_stack_ptr).reg.data - 1);
    }
    void get_top_cs(Reg instruction){
        set_stack(
            instruction.bits_8_16.p1,
            get_call_stack(get_sf(sf_map::call_stack_ptr).reg.data)
        );
    }
    void ECSI(Reg instruction){
        set_call_stack(
            instruction.bits_8_8_8.p1,
            get_stack(instruction.bits_8_8_8.p2)
        );
    }
    void CMP(Reg instruction){
        set_sf(
            sf_map::do_jmp,
            alu.CMP(
                get_stack(instruction.bits_8_8_8.p1),
                instruction.bits_8_8_8.p2,
                instruction.bits_8_8_8.p3
            )
        );
    }
    void CMP_OR(Reg instruction){
        set_sf(
            sf_map::do_jmp,
            alu.CMP_OR(
                get_stack(instruction.bits_8_8_8.p1),
                instruction.bits_8_8_8.p2,
                instruction.bits_8_8_8.p3
            )
        );
    }
    void CMP_AND(Reg instruction){
        set_sf(
            sf_map::do_jmp,
            alu.CMP_AND(
                get_stack(instruction.bits_8_8_8.p1),
                instruction.bits_8_8_8.p2,
                instruction.bits_8_8_8.p3
            )
        );
    }
    void JMP(Reg instruction){
        if(get_sf(sf_map::do_jmp).reg.data){
            set_sf(sf_map::clk,instruction.bits_16_8.p1);
        }
    }
    void JMPU(Reg instruction){
        set_sf(sf_map::clk,instruction.bits_16_8.p1);
    }
    void JMP_ptr(Reg instruction){
        if(get_sf(sf_map::do_jmp).reg.data){
            set_sf(sf_map::clk,get_stack(instruction.bits_8_16.p1).reg.data);
        }
    }
    void JMPU_ptr(Reg instruction){
        set_sf(sf_map::clk,get_stack(instruction.bits_8_16.p1).reg.data);
    }
    void JMPF(Reg instruction){
        if(get_sf(sf_map::alu_flags).reg.flags == instruction.bits_16_8.p2){
            JMPU(instruction);
        }
    }
    void JMPF_ptr(Reg instruction){
        if(get_sf(sf_map::alu_flags).reg.flags == instruction.bits_8_8_8.p2){
            JMPU_ptr(instruction);
        }
    }
    void move_chunk(Reg instruction){
        uint16_t addr_a = instruction.bits_8_8_8.p1;
        uint16_t addr_b = instruction.bits_8_8_8.p2;
        uint16_t target = instruction.bits_8_8_8.p3;
        for(uint16_t i = addr_a ; i < addr_b ; i++){
            set_stack(
                (target + (i - addr_a)),
                get_stack(i)
            );
        }
    }
    void move_chunk_ptr(Reg instruction){
        uint16_t addr_a = get_stack(instruction.bits_8_8_8.p1).reg.data;
        uint16_t addr_b = get_stack(instruction.bits_8_8_8.p2).reg.data;
        uint16_t target = get_stack(instruction.bits_8_8_8.p3).reg.data;
        for(uint16_t i = addr_a ; i < addr_b ; i++){
            set_stack(
                (target + (i - addr_a)),
                get_stack(i)
            );
        }
    }
    void CCALL(Reg instruction){
        if(get_sf(sf_map::do_jmp).reg.data){
            _call(instruction.bits_16_8.p1);
        }
    }
    void CALL(Reg instruction){
        _call(instruction.bits_16_8.p1);
    }
    void CCALL_ptr(Reg instruction){
        if(get_sf(sf_map::do_jmp).reg.data){
            _call(get_stack(instruction.bits_8_16.p1).reg.data);
        }
    }
    void CALL_ptr(Reg instruction){
        _call(get_stack(instruction.bits_8_16.p1).reg.data);
    }
    void RET(Reg instruction){
        uint16_t return_addr = get_top_call_stack().reg.data;
        dec_cs_ptr();
        set_sf(sf_map::clk,return_addr);
    }
    void IRET(Reg instruction){
        RET(instruction);
    }
    void INTRP(Reg instruction){
        Core *target = get_core(instruction.bits_8_16.p1);
        if(target->get_sf(sf_map::interuptable).reg.data){
            target->_call(instruction.bits_8_16.p2);
        }
    }
    void INTRP_ptr(Reg instruction){
        Core *target = get_core(instruction.bits_8_16.p1);
        if(target->get_sf(sf_map::interuptable).reg.data){
            target->_call(get_stack(instruction.bits_8_16.p2).reg.data);
        }
    }
    void push_sf_core(Reg instruction){
        Core *target = get_core(instruction.bits_8_8_8.p1);
        target->set_sf(
            instruction.bits_8_8_8.p2,
            get_stack(instruction.bits_8_8_8.p3)
        );
    }
    void pull_sf_core(Reg instruction){
        Core *target = get_core(instruction.bits_8_8_8.p1);
        set_stack(
            instruction.bits_8_8_8.p3,
            target->get_sf(instruction.bits_8_8_8.p2)
        );
    }
    void loop_p(Reg instruction){
        set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data+1);
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU(instruction);
        }
    }
    void loop_m(Reg instruction){
        set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data-1);
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU(instruction);
        }
    }
    void loop_ptr_p(Reg instruction){
        set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data+1);
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU_ptr(instruction);
        }
    }
    void loop_ptr_m(Reg instruction){
        set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data-1);
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU_ptr(instruction);
        }
    }
    void WHILE(Reg instruction){
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU(instruction);
        }
    }
    void WHILE_ptr(Reg instruction){
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU_ptr(instruction);
        }
    }
    void one_count(Reg instruction){
        set_stack(
            instruction.bits_8_8_8.p2,
            __popcount(get_stack(instruction.bits_8_8_8.p1).data)
        );
    }
    void zero_count(Reg instruction){
        set_stack(
            instruction.bits_8_8_8.p2,
            32 - __popcount(get_stack(instruction.bits_8_8_8.p1).data)
        );
    }
    void ADD(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.ADD(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void SUB(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.SUB(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void AND(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.AND(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void NAND(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.NAND(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void OR(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.OR(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void XOR(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.XOR(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void FADD(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.fADD(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void FSUB(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.fSUB(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void MULT(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.MULT(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void DIV(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.DIV(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void MOD(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.MOD(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void FMULT(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.fMULT(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void FDIV(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.fDIV(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void FMOD(Reg instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.fMOD(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void ITF(Reg instruction){
        Reg result;
        result.fdata = (float)(int16_t)get_stack(instruction.bits_8_8_8.p2).reg.data;
        set_stack(instruction.bits_8_8_8.p3, result);
    }
    void FTI(Reg instruction){
        Reg result;
        result.reg.data = (uint16_t)get_stack(instruction.bits_8_8_8.p1).fdata;
        set_stack(instruction.bits_8_8_8.p2,result);
    }
    void MIN(Reg instruction){
        set_stack(
            instruction.bits_8_8_8.p3,
            min(
                get_stack(instruction.bits_8_8_8.p1).reg.data,
                get_stack(instruction.bits_8_8_8.p2).reg.data
            )  
        );
    }
    void MAX(Reg instruction){
        set_stack(
            instruction.bits_8_8_8.p3,
            max(
                get_stack(instruction.bits_8_8_8.p1).reg.data,
                get_stack(instruction.bits_8_8_8.p2).reg.data
            )  
        );
    }
    void ABS(Reg instruction){
        set_stack(
            instruction.bits_8_8_8.p2,
            alu.ABS(get_stack(instruction.bits_8_8_8.p2))
        );
    }
    void FABS(Reg instruction){
        set_stack(
            instruction.bits_8_8_8.p2,
            (float)(uint16_t)alu.ABS(get_stack(instruction.bits_8_8_8.p1)).reg.data
        );
    }
    void jmpb(Reg instruction){
        if(get_stack(instruction.bits_8_16.p1).reg.data){
            set_sf(sf_map::clk,instruction.bits_8_16.p2);
        }
    }
    void jmpb(Reg instruction){
        if(get_stack(instruction.bits_8_8_8.p1).reg.data){
            set_sf(sf_map::clk,get_stack(instruction.bits_8_8_8.p2));
        }
    }
    void WFE(Reg instruction){
        wf_flags.wf_event = true;
        wf_flags.event_target = instruction.bits_16_8.p1;
        exacute_ptr = WF_exacute;
    }
    void WFE_ptr(Reg instruction){
        wf_flags.wf_event = true;
        wf_flags.event_target = get_stack(instruction.bits_8_16.p1).reg.data;
        exacute_ptr = WF_exacute;
    }
    void WFI(Reg instruction){
        wf_flags.wf_intrp = true;
        exacute_ptr = WF_exacute;
    }
    void syscall(Reg instruction){
        _call(instruction.bits_8_16.p1+get_sf(sf_map::sc_offset).reg.data);
    }
    inline __attribute__((always_inline))
    void ld_byte(Reg instruction){
        Reg r = get_stack(instruction.bits_8_8_8.p1);
        uint8_t byte_val = instruction.bits_8_8_8.p3 & 0xFF;
        switch(instruction.bits_8_8_8.p2 % 4){
            case 0:  // lowest byte  (bits 0-7)
                r.data = (r.data & 0xFFFFFF00) | byte_val;
                break;
            case 1:  // second byte  (bits 8-15)
                r.data = (r.data & 0xFFFF00FF) | (byte_val << 8);
                break;
            case 2:  // third byte   (bits 16-23)
                r.data = (r.data & 0xFF00FFFF) | (byte_val << 16);
                break;
            case 3:  // highest byte (bits 24-31)
                r.data = (r.data & 0x00FFFFFF) | (byte_val << 24);
                break;
        }
        set_stack(instruction.bits_8_8_8.p1, r);
    }
    
    // i was filling in opcodes 

public:
    inline __attribute__((always_inline))
    void _call(uint16_t addr){
        append_call_stack(get_sf(sf_map::clk));
        set_sf(sf_map::clk,addr);
    }


private: // utils
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

private: // getters
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
    Core *get_core(uint16_t addr){
        return cores[addr & CORE_MASK];
    }
private: // setters
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
        mem.sf[addr & SF_MASK] = data;
    }
    inline __attribute__((always_inline))
    void set_sf(uint16_t addr , uint32_t data){
        mem.sf[addr & SF_MASK].data = data;
    }
    inline __attribute__((always_inline))
    void set_call_stack(uint16_t addr , Reg data){
        mem.call_stack[addr & CALL_STACK_MASK] = data;
    }
    inline __attribute__((always_inline))
    void set_cache(uint16_t addr , Reg data){
        Reg lock_data = (*mem.cache)[0];
        if(lock_data.reg.meta_data == 0 && lock_data.reg.data == get_sf(sf_map::core_id).reg.data){
            (*mem.cache)[addr & CACHE_MASK] = data;
        }else{
            // error
            set_sf(sf_map::event,error_reg); // set error flag / ------------------------------------------------------ make shure you look at this to define an event table 
        }
    }
    inline __attribute__((always_inline))
    void set_port(uint16_t addr , Reg data){
        Reg lock_data = (*mem.ports)[0];
        if(lock_data.reg.meta_data == 0 && lock_data.reg.data == get_sf(sf_map::core_id).reg.data){
            (*mem.ports)[addr & PORT_MASK] = data;
        }else{
            // error
            set_sf(sf_map::event,error_reg); // set error flag / ------------------------------------------------------ make shure you look at this to define an event table 
        }
    }

};
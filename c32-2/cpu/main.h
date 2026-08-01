

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

private: // instructions
    void HALT(Reg& instruction){
        set_sf(sf_map::active,0);
        exacute_ptr = null_exacute;
    }
    void NOP(Reg& instruction){
        stall_30();
    }
    void STALL(Reg& instruction){
        for(int i = 0 ; i < instruction.bits_8_8_8.p1 ; i++){
            stall_30();
        }
    }
    void LR(Reg& instruction){
        set_stack(
            instruction.bits_8_16.p1,
            instruction.bits_8_16.p2
        );
    }
    void move(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p1,
            get_stack(instruction.bits_8_8_8.p2)
        );
    }
    void move_mem(Reg& instruction){
        Reg addr_a = get_stack(instruction.bits_8_8_8.p1);
        Reg addr_b = get_stack(instruction.bits_8_8_8.p2);
        set_reg(
            addr_a.reg.data,
            get_reg(addr_b.reg.data)
        );
    }
    void move_cache(Reg& instruction){
        Reg addr_a = get_stack(instruction.bits_8_8_8.p1);
        Reg addr_b = get_stack(instruction.bits_8_8_8.p2);
        set_cache(
            addr_a.reg.data,
            get_cache(addr_b.reg.data)
        );
    }
    void push_stack(Reg& instruction){
        set_stack(
            instruction.bits_16_8.p2,
            get_reg(instruction.bits_16_8.p1)   
        );
    }
    void pull_stack(Reg& instruction){
        set_reg(
            instruction.bits_8_16.p2,
            get_stack(instruction.bits_8_16.p1)
        );
    }
    void push_cache(Reg& instruction){
        Reg addr_a = get_stack(instruction.bits_8_8_8.p1);
        Reg addr_b = get_stack(instruction.bits_8_8_8.p2);
        set_cache(
            addr_a.reg.data,
            get_reg(addr_b.reg.data)
        );
    }
    void pull_cache(Reg& instruction){
        Reg addr_a = get_stack(instruction.bits_8_8_8.p1);
        Reg addr_b = get_stack(instruction.bits_8_8_8.p2);
        set_reg(
            addr_b.reg.data,
            get_cache(addr_a.reg.data)
        );
    }
    void cmove(Reg& instruction){
        if(get_sf(sf_map::do_jmp).reg.data){
            move(instruction);
        }
    }
    void move_ptr(Reg& instruction){
        Reg addr_a = get_stack(instruction.bits_8_8_8.p1);
        Reg addr_b = get_stack(instruction.bits_8_8_8.p2);
        set_stack(
            addr_b.reg.data,
            get_stack(addr_a.reg.data)
        );
    }
    void push_sf(Reg& instruction){
        set_sf(
            instruction.bits_8_8_8.p2,
            get_stack(instruction.bits_8_8_8.p1)
        );
    }
    void pull_sf(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p2,
            get_sf(instruction.bits_8_8_8.p1)
        );
    }
    void set_sf(Reg& instruction){
        set_sf(
            instruction.bits_8_16.p1,
            instruction.bits_8_16.p2
        );
    }
    void swarm(Reg& instruction){
        Reg data = get_stack(instruction.bits_8_8_8.p1);
        for(uint16_t i = instruction.bits_8_8_8.p2 ; i < instruction.bits_8_8_8.p3 ; i++){
            set_cache(i,data);
        }
    }
    void LS(Reg& instruction){
        set_stack(
            instruction.bits_8_16.p1,
            instruction.bits_8_16.p2
        );
    }
    void LC(Reg& instruction){
        set_cache(
            instruction.bits_8_16.p1,
            Reg {instruction.bits_8_16.p2}
        );
    }
    void append_cs(Reg& instruction){
        set_sf(sf_map::call_stack_ptr,get_sf(sf_map::call_stack_ptr).reg.data + 1);
        set_call_stack(
            get_sf(sf_map::call_stack_ptr).reg.data,
            get_stack(instruction.bits_8_16.p1)
        );
    }
    void pop_cs(Reg& instruction){
        set_sf(sf_map::call_stack_ptr,get_sf(sf_map::call_stack_ptr).reg.data - 1);
    }
    void get_top_cs(Reg& instruction){
        set_stack(
            instruction.bits_8_16.p1,
            get_call_stack(get_sf(sf_map::call_stack_ptr).reg.data)
        );
    }
    void ECSI(Reg& instruction){
        set_call_stack(
            instruction.bits_8_8_8.p1,
            get_stack(instruction.bits_8_8_8.p2)
        );
    }
    void CMP(Reg& instruction){
        set_sf(
            sf_map::do_jmp,
            alu.CMP(
                get_stack(instruction.bits_8_8_8.p1),
                instruction.bits_8_8_8.p2,
                instruction.bits_8_8_8.p3
            )
        );
    }
    void CMP_OR(Reg& instruction){
        set_sf(
            sf_map::do_jmp,
            alu.CMP_OR(
                get_stack(instruction.bits_8_8_8.p1),
                instruction.bits_8_8_8.p2,
                instruction.bits_8_8_8.p3
            )
        );
    }
    void CMP_AND(Reg& instruction){
        set_sf(
            sf_map::do_jmp,
            alu.CMP_AND(
                get_stack(instruction.bits_8_8_8.p1),
                instruction.bits_8_8_8.p2,
                instruction.bits_8_8_8.p3
            )
        );
    }
    void JMP(Reg& instruction){
        if(get_sf(sf_map::do_jmp).reg.data){
            set_sf(sf_map::clk,instruction.bits_16_8.p1);
        }
    }
    void JMPU(Reg& instruction){
        set_sf(sf_map::clk,instruction.bits_16_8.p1);
    }
    void JMP_ptr(Reg& instruction){
        if(get_sf(sf_map::do_jmp).reg.data){
            set_sf(sf_map::clk,get_stack(instruction.bits_8_16.p1).reg.data);
        }
    }
    void JMPU_ptr(Reg& instruction){
        set_sf(sf_map::clk,get_stack(instruction.bits_8_16.p1).reg.data);
    }
    void JMPF(Reg& instruction){
        if(get_sf(sf_map::alu_flags).reg.flags == instruction.bits_16_8.p2){
            JMPU(instruction);
        }
    }
    void JMPF_ptr(Reg& instruction){
        if(get_sf(sf_map::alu_flags).reg.flags == instruction.bits_8_8_8.p2){
            JMPU_ptr(instruction);
        }
    }
    void move_chunk(Reg& instruction){
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
    void move_chunk_ptr(Reg& instruction){
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
    void CCALL(Reg& instruction){
        if(get_sf(sf_map::do_jmp).reg.data){
            _call(instruction.bits_16_8.p1);
        }
    }
    void CALL(Reg& instruction){
        _call(instruction.bits_16_8.p1);
    }
    void CCALL_ptr(Reg& instruction){
        if(get_sf(sf_map::do_jmp).reg.data){
            _call(get_stack(instruction.bits_8_16.p1).reg.data);
        }
    }
    void CALL_ptr(Reg& instruction){
        _call(get_stack(instruction.bits_8_16.p1).reg.data);
    }
    void RET(Reg& instruction){
        uint16_t return_addr = get_top_call_stack().reg.data;
        dec_cs_ptr();
        set_sf(sf_map::clk,return_addr);
    }
    void IRET(Reg& instruction){
        RET(instruction);
    }
    void INTRP(Reg& instruction){
        Core *target = get_core(instruction.bits_8_16.p1);
        if(target->get_sf(sf_map::interuptable).reg.data){
            target->_call(instruction.bits_8_16.p2);
        }
    }
    void INTRP_ptr(Reg& instruction){
        Core *target = get_core(instruction.bits_8_16.p1);
        if(target->get_sf(sf_map::interuptable).reg.data){
            target->_call(get_stack(instruction.bits_8_16.p2).reg.data);
        }
    }
    void push_sf_core(Reg& instruction){
        Core *target = get_core(instruction.bits_8_8_8.p1);
        target->set_sf(
            instruction.bits_8_8_8.p2,
            get_stack(instruction.bits_8_8_8.p3)
        );
    }
    void pull_sf_core(Reg& instruction){
        Core *target = get_core(instruction.bits_8_8_8.p1);
        set_stack(
            instruction.bits_8_8_8.p3,
            target->get_sf(instruction.bits_8_8_8.p2)
        );
    }
    void loop_p(Reg& instruction){
        set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data+1);
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU(instruction);
        }
    }
    void loop_m(Reg& instruction){
        set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data-1);
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU(instruction);
        }
    }
    void loop_ptr_p(Reg& instruction){
        set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data+1);
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU_ptr(instruction);
        }
    }
    void loop_ptr_m(Reg& instruction){
        set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data-1);
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU_ptr(instruction);
        }
    }
    void WHILE(Reg& instruction){
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU(instruction);
        }
    }
    void WHILE_ptr(Reg& instruction){
        if(get_sf(sf_map::counter).reg.data == get_sf(sf_map::counter_target).reg.data){
            JMPU_ptr(instruction);
        }
    }
    void one_count(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p2,
            __popcount(get_stack(instruction.bits_8_8_8.p1).data)
        );
    }
    void zero_count(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p2,
            32 - __popcount(get_stack(instruction.bits_8_8_8.p1).data)
        );
    }
    void ADD(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.ADD(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void SUB(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.SUB(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void AND(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.AND(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void NAND(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.NAND(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void OR(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.OR(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void XOR(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.XOR(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void FADD(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.fADD(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void FSUB(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.fSUB(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void MULT(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.MULT(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void DIV(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.DIV(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void MOD(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.MOD(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void FMULT(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.fMULT(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void FDIV(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.fDIV(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void FMOD(Reg& instruction){
        set_stack(instruction.bits_8_8_8.p3,
            alu.fMOD(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void ITF(Reg& instruction){
        Reg result;
        result.fdata = (float)(int16_t)get_stack(instruction.bits_8_8_8.p2).reg.data;
        set_stack(instruction.bits_8_8_8.p3, result);
    }
    void FTI(Reg& instruction){
        Reg result;
        result.reg.data = (uint16_t)get_stack(instruction.bits_8_8_8.p1).fdata;
        set_stack(instruction.bits_8_8_8.p2,result);
    }
    void MIN(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p3,
            min(
                get_stack(instruction.bits_8_8_8.p1).reg.data,
                get_stack(instruction.bits_8_8_8.p2).reg.data
            )  
        );
    }
    void MAX(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p3,
            max(
                get_stack(instruction.bits_8_8_8.p1).reg.data,
                get_stack(instruction.bits_8_8_8.p2).reg.data
            )  
        );
    }
    void ABS(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p2,
            alu.ABS(get_stack(instruction.bits_8_8_8.p2))
        );
    }
    void FABS(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p2,
            (float)(uint16_t)alu.ABS(get_stack(instruction.bits_8_8_8.p1)).reg.data
        );
    }
    void jmpb(Reg& instruction){
        if(get_stack(instruction.bits_8_16.p1).reg.data){
            set_sf(sf_map::clk,instruction.bits_8_16.p2);
        }
    }
    void jmpb_ptr(Reg& instruction){
        if(get_stack(instruction.bits_8_8_8.p1).reg.data){
            set_sf(sf_map::clk,get_stack(instruction.bits_8_8_8.p2));
        }
    }
    void WFE(Reg& instruction){
        wf_flags.wf_event = true;
        wf_flags.event_target = instruction.bits_16_8.p1;
        exacute_ptr = WF_exacute;
    }
    void WFE_ptr(Reg& instruction){
        wf_flags.wf_event = true;
        wf_flags.event_target = get_stack(instruction.bits_8_16.p1).reg.data;
        exacute_ptr = WF_exacute;
    }
    void WFI(Reg& instruction){
        wf_flags.wf_intrp = true;
        exacute_ptr = WF_exacute;
    }
    void syscall(Reg& instruction){
        _call(instruction.bits_8_16.p1+get_sf(sf_map::sc_offset).reg.data);
    }
    void ld_byte(Reg& instruction) {
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
    void shift_L(Reg& instruction) {
        set_stack(
            instruction.bits_8_8_8.p3,
            alu.SHIFT_L(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void shift_R(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p3,
            alu.SHIFT_R(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void shiftf_L(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p3,
            alu.SHIFTF_L(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void shiftf_R(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p3,
            alu.SHIFTF_R(
                get_stack(instruction.bits_8_8_8.p1),
                get_stack(instruction.bits_8_8_8.p2)
            )
        );
    }
    void shift_L_abs(Reg& instruction){
        Reg data;
        data.reg.data = instruction.bits_8_8_8.p2;
        set_stack(
            instruction.bits_8_8_8.p3,
            alu.SHIFT_L(
                get_stack(instruction.bits_8_8_8.p1),
                data
            )
        );
    }
    void shift_R_abs(Reg& instruction){
        Reg data;
        data.reg.data = instruction.bits_8_8_8.p2;
        set_stack(
            instruction.bits_8_8_8.p3,
            alu.SHIFT_R(
                get_stack(instruction.bits_8_8_8.p1),
                data
            )
        );
    }
    void shiftf_L_abs(Reg& instruction){
        Reg data;
        data.reg.data = instruction.bits_8_8_8.p2;
        set_stack(
            instruction.bits_8_8_8.p3,
            alu.SHIFTF_L(
                get_stack(instruction.bits_8_8_8.p1),
                data
            )
        );
    }
    void shiftf_R_abs(Reg& instruction){
        Reg data;
        data.reg.data = instruction.bits_8_8_8.p2;
        set_stack(
            instruction.bits_8_8_8.p3,
            alu.SHIFTF_R(
                get_stack(instruction.bits_8_8_8.p1),
                data
            )
        );
    }
    void ADI(Reg& instruction){
        alu.ADI(
            get_stack(instruction.bits_8_16.p1),
            instruction.bits_8_16.p2
        );
    }
    void SDI(Reg& instruction){
        alu.SDI(
            get_stack(instruction.bits_8_16.p1),
            instruction.bits_8_16.p2
        );
    }
    void MDI(Reg& instruction){
        alu.MDI(
            get_stack(instruction.bits_8_16.p1),
            instruction.bits_8_16.p2
        );
    }
    void DDI(Reg& instruction){
        alu.DDI(
            get_stack(instruction.bits_8_16.p1),
            instruction.bits_8_16.p2
        );
    }
    void dump_reg(Reg& instruction){
        cout << get_stack(instruction.bits_8_16.p1).data << endl;
    }
    void dump_data(Reg& instruction){
        cout << get_reg(instruction.bits_16_8.p1).data << endl;
    }
    void dump_cache(Reg& instruction){
        cout << get_cache(instruction.bits_8_16.p1).data << endl;
    }
    void dump_cs(Reg& instruction){
        cout << get_call_stack(instruction.bits_8_16.p1).data << endl;
    }
    void dump_sf(Reg& instruction){
        cout << get_sf(instruction.bits_8_16.p1).data << endl;
    }
    void SHUT_DOWN(Reg& instructions){
        cout << "off" << endl;
    }
    void dump_ports(Reg& instruction){
        cout << get_port(instruction.bits_8_16.p1).data << endl;
    }
    void CMP_ALU_flags(Reg& instruction){
        set_sf(
            sf_map::do_jmp,
            alu.CMP(
                get_sf(sf_map::alu_flags),
                instruction.bits_8_8_8.p2,
                instruction.bits_8_8_8.p3
            )
        );
    }
    void LD_PTR(Reg& instruction){
        Reg addr = get_stack(instruction.bits_8_8_8.p1);
        set_stack(
            instruction.bits_8_8_8.p2,
            get_stack(addr.reg.data)
        );
    }
    void push_cs(Reg& instruction){
        set_call_stack(
            instruction.bits_8_8_8.p2,
            get_stack(instruction.bits_8_8_8.p1)
        );
    }
    void pull_cs(Reg& instruction){
        set_stack(
            instruction.bits_8_8_8.p2,
            get_call_stack(instruction.bits_8_8_8.p1)
        );
    }
    void sc_intrp(Reg& instruction){
        Core *target = get_core(instruction.bits_8_8_8.p1);
        target->_intrp(
            instruction.bits_8_8_8.p2 + target->get_sf(sf_map::sc_offset).reg.data
        );
    }
    void wf_SC_intrp(Reg& instruction){
        exacute_ptr = WF_exacute;
        set_sf(sf_map::wating_on,true);
    }

    void GCL(Reg& instruction){
        Reg value = get_cache(0);
        set_stack(instruction.bits_8_16.p1,value);
    }
    void GPL(Reg& instruction){
        Reg value = get_port(0);
        set_stack(instruction.bits_8_16.p1,value);
    }
    void force_set_flags(Reg& instruction){
        Reg value = get_stack(instruction.bits_8_8_8.p1);
        value.reg.flags = instruction.bits_8_8_8.p2;
        set_stack(instruction.bits_8_8_8.p1,value);
    }
    void load_metadata(Reg& instruction){
        Reg value = get_stack(instruction.bits_8_8_8.p1);
        value.reg.meta_data = instruction.bits_8_8_8.p2;
        set_stack(instruction.bits_8_8_8.p1,value);
    }
    void roll_Labs(Reg& instruction){
        Reg imm;
        imm.reg.data = instruction.bits_8_8_8.p2;
        set_stack(
            instruction.bits_8_8_8.p3,
            alu.ROL_L(
                get_stack(instruction.bits_8_8_8.p1),
                imm
            )
        );
    }
    void roll_Rabs(Reg& instruction){
        Reg imm;
        imm.reg.data = instruction.bits_8_8_8.p2;
        set_stack(
            instruction.bits_8_8_8.p3,
            alu.ROL_R(
                get_stack(instruction.bits_8_8_8.p1),
                imm
            )
        );
    }
    void cmp_eval_bool(Reg& instruction){
        bool data = alu.CMP(
            get_stack(instruction.bits_8_8_8.p1),
            instruction.bits_8_8_8.p2,
            instruction.bits_8_8_8.p3
        );
        set_stack(instruction.bits_8_8_8.p1,data);
    }
    void RAND(Reg& instruction){
        uint32_t value = rand32(get_stack(instruction.bits_8_8_8.p2).reg.data);
        set_stack(instruction.bits_8_8_8.p1,value);
    }
    void swarm_stack(Reg& instruction){
        Reg data = get_stack(instruction.bits_8_8_8.p1);
        uint32_t start = instruction.bits_8_8_8.p2;
        uint32_t end = instruction.bits_8_8_8.p3;
        for(uint32_t i = start ; i < end ; i++){
            set_stack(i,data);
        }
    }
    void GCSI(Reg& instruction){
        Reg value = get_call_stack(instruction.bits_8_8_8.p1);
        set_stack(instruction.bits_8_8_8.p2,value);
    }
    void swarm_mem(Reg& instruction){
        Reg data = get_stack(instruction.bits_8_8_8.p1);
        uint32_t start = get_stack(instruction.bits_8_8_8.p2).reg.data;
        uint32_t end = get_stack(instruction.bits_8_8_8.p3).reg.data;
        for(uint32_t i = start ; i < end ; i++){
            set_reg(i,data);
        }
    }
    void LM(Reg& instruction){
        uint32_t addr = get_stack(instruction.bits_8_16.p1).reg.data;
        set_reg(addr,instruction.bits_8_16.p2);
    }
    void swarm_cache(Reg& instruction){
        Reg data = get_stack(instruction.bits_8_8_8.p1);
        uint32_t start = instruction.bits_8_8_8.p2;
        uint32_t end = instruction.bits_8_8_8.p3;
        for(uint32_t i = start ; i < end ; i++){
            set_cache(i,data);
        }
    }
    void swarm_ports(Reg& instruction){
        Reg data = get_stack(instruction.bits_8_8_8.p1);
        uint32_t start = instruction.bits_8_8_8.p2;
        uint32_t end = instruction.bits_8_8_8.p3;
        for(uint32_t i = start ; i < end ; i++){
            set_port(i,data);
        }
    }
    


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
    
private: 
    inline __attribute__((always_inline))
    bool intrp_avalable(){
        return get_sf(sf_map::interuptable).reg.data && get_sf(sf_map::intrp_ptr).reg.data != 0;
    }
    inline __attribute__((always_inline))
    void handle_intrps(){
        if(intrp_avalable()){
            set_sf(sf_map::intrp_ptr,0);
            _intrp(get_sf(sf_map::intrp_ptr).reg.data);
        }
    }

private:
    void null_exacute(){
        stall_30();
    }
    void WF_exacute(){
        stall_30();
    }
    void EXACUTE(){
        handle_intrps();
    }
    void EXACUTE_WFE(){
        Reg event = get_sf(sf_map::event);
        if(event.reg.data == get_sf(sf_map::event_target).reg.data){
            exacute_ptr = &Core::EXACUTE;
        }
        stall_30();
    }
    void EXACUTE_WFI(){
        Reg intrp_ptr = get_sf(sf_map::intrp_ptr);
        if(intrp_ptr.reg.data != 0){
            set_sf(sf_map::intrp_ptr,0);
            _intrp(intrp_ptr.reg.data);
            exacute_ptr = &Core::EXACUTE;
            return;
        }
        stall_30();
    }
    using Op_Func_type = void(Core::*)(Reg&);

    static constexpr Op_Func_type opcode_table[256] = {
        &Core::HALT,        // 0
        &Core::NOP,          // 1
        &Core::STALL,        // 2
        &Core::LR,           // 3
        &Core::move,         // 4
        &Core::move_mem,     // 5
        &Core::move_cache,   // 6
        &Core::push_stack,   // 7
        &Core::pull_stack,   // 8
        &Core::push_cache,   // 9
        &Core::pull_cache,   // 10
        &Core::cmove,        // 11
        &Core::move_ptr,     // 12
        &Core::push_sf,      // 13
        &Core::pull_sf,      // 14
        &Core::set_sf,       // 15
        &Core::swarm,        // 16
        &Core::LS,           // 17
        &Core::LC,           // 18
        &Core::append_cs,    // 19
        &Core::pop_cs,       // 20
        &Core::get_top_cs,   // 21
        &Core::ECSI,         // 22
        &Core::CMP,          // 23
        &Core::CMP_OR,       // 24
        &Core::CMP_AND,      // 25
        &Core::JMP,          // 26
        &Core::JMPU,         // 27
        &Core::JMP_ptr,      // 28
        &Core::JMPU_ptr,     // 29
        &Core::JMPF,         // 30
        &Core::JMPF_ptr,     // 31
        &Core::move_chunk,      // 32
        &Core::move_chunk_ptr,  // 33
        &Core::CCALL,        // 34
        &Core::CALL,         // 35
        &Core::CCALL_ptr,    // 36
        &Core::CALL_ptr,     // 37
        &Core::RET,          // 38
        &Core::IRET,         // 39
        &Core::INTRP,        // 40
        &Core::INTRP_ptr,    // 41
        &Core::push_sf_core, // 42
        &Core::pull_sf_core, // 43
        &Core::loop_p,       // 44
        &Core::loop_m,       // 45
        &Core::loop_ptr_p,   // 46  <-- your doc dupes 46 for loop_ptr-, see note above
        &Core::loop_ptr_m,   // 47  (was mislabeled 46 in doc; renumber loop_ptr- to 47)
        &Core::WHILE,        // 48
        &Core::WHILE_ptr,    // 49
        &Core::one_count,    // 50
        &Core::zero_count,   // 51
        &Core::ADD,          // 52
        &Core::SUB,          // 53
        &Core::AND,          // 54
        &Core::NAND,         // 55
        &Core::OR,           // 56
        &Core::XOR,          // 57
        &Core::FADD,         // 58
        &Core::FSUB,         // 59
        &Core::MULT,         // 60
        &Core::DIV,          // 61
        &Core::MOD,          // 62
        &Core::FMULT,        // 63
        &Core::FDIV,         // 64
        &Core::FMOD,         // 65
        &Core::ITF,          // 66
        &Core::FTI,          // 67
        &Core::MIN,          // 68
        &Core::MAX,          // 69
        &Core::ABS,          // 70
        &Core::FABS,         // 71
        &Core::jmpb,         // 72
        &Core::jmpb_ptr,      // 73
        &Core::WFE,          // 74
        &Core::WFE_ptr,      // 75
        &Core::WFI,          // 76
        &Core::syscall,      // 77
        &Core::ld_byte,      // 78
        &Core::shift_L,      // 79
        &Core::shift_R,      // 80
        &Core::shiftf_L,     // 81
        &Core::shiftf_R,     // 82
        &Core::shift_L_abs,  // 83
        &Core::shift_R_abs,  // 84
        &Core::shiftf_L_abs, // 85
        &Core::shiftf_R_abs, // 86
        &Core::ADI,          // 87
        &Core::SDI,          // 88
        &Core::MDI,          // 89
        &Core::DDI,          // 90
        &Core::dump_reg,     // 91
        &Core::dump_data,    // 92
        &Core::dump_cache,   // 93
        &Core::dump_cs,      // 94 dump_cs
        &Core::dump_sf,      // 95
        &Core::SHUT_DOWN,    // 96
        &Core::dump_ports,   // 97
        &Core::CMP_ALU_flags,// 98 CMP_ALU_flags 
        &Core::LD_PTR,       // 99 ld_ptr
        &Core::push_cs,      // 100 push_cs
        &Core::pull_cs,      // 101 pull_cs 
        &Core::sc_intrp,     // 102 sc_intrp -- not implemented yet
        &Core::wf_SC_intrp,  // 103 wf_SC_intrp -- not implemented yet
        &Core::push_port,    // 104 push_port -- not implemented yet
        &Core::pull_port,    // 105 pull_port -- not implemented yet
        nullptr,             // 106 atp -- not implemented yet
        nullptr,             // 107 pta -- not implemented yet
        nullptr,             // 108 lp -- not implemented yet
        nullptr,             // 109 NOT -- not implemented yet (ALU has NOT, Core wrapper missing)
        nullptr,             // 110 push_core -- not implemented yet
        nullptr,             // 111 pull_core -- not implemented yet
        nullptr,             // 112 stall_core -- not implemented yet
        nullptr,             // 113
        nullptr,             // 114
        nullptr,             // 115 core_wfintp
        nullptr,             // 116 core_wfevent
        nullptr,             // 117 disable_core
        nullptr,             // 118 enable_core
        &Core::dump_ports,   // 119 (dup of 97 in your doc)
        nullptr,             // 120 claim_cache
        nullptr,             // 121 relese_cache
        nullptr,             // 122 claim_ports
        nullptr,             // 123 relese_ports
        &Core::GCL,          // 124
        &Core::GPL,          // 125
        &Core::force_set_flags, // 126
        &Core::load_metadata,   // 127
        &Core::roll_Labs,    // 128 roll_L -- NOTE: fix the duplicate name first (see above)
        &Core::roll_Rabs,    // 129 roll_R_abs
        &Core::cmp_eval_bool,// 130
        &Core::RAND,         // 131
        &Core::swarm_mem,    // 132
        &Core::GCSI,         // 133
        nullptr,             // 134 swarm_mem (2nd variant, ptr version) -- name collision, needs distinct name
        &Core::LM,           // 135
        // 136-255: unused / reserved in your doc
    };

public:
    void (Core::*exacute_ptr)() = &Core::null_exacute;
    Core(array<Reg, CACHE_SIZE>* cache_ptr, array<Reg, PORT_SIZE>* ports_ptr, array<Core*, CORE_COUNT> cores_ptr){
        mem.cache = cache_ptr;
        mem.ports = ports_ptr;
        cores = cores_ptr;
    }
    inline __attribute__((always_inline))
    void exacute(){
        (this->*exacute_ptr)();
    }
};
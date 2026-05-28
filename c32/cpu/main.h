#include <iostream>
#include <vector>
#include <cstdint>
#include <stdfloat> 
#include <cmath>
#include <immintrin.h>
#include <Utility>
#include <nlohmann/json.hpp>
#include <atomic>

#include "config.h"
#include "jeff/global_utils.h"
#include "jeff/data.h"
#pragma once

using namespace std;
using json = nlohmann::json;

struct alignas(64) Memory_unit {
    array<Reg, REG_SIZE> regs;
    array<Reg, STACK_SIZE> stack;
    array<Reg, SF_SIZE> sf;
    array<Reg, CACHE_SIZE>* cache = nullptr;
    vector<Reg> call_stack;

    //atomic<bool> locked = false;
    bool locked = false;
};
struct alignas(16) Core_vars{
    bool wating_on_event = false;
    uint32_t event_wating_on = 0;
    bool wating_for_external_write = false;
    uint32_t last_external_write_to_stack_target = 0;
};

class ALU {
    static constexpr Reg nullreg = {0};
    public:
        uint32_t alu_flags = 0;

        inline Reg add(Reg a, Reg b){
            Reg result;
            result.data = a.data + b.data;
            alu_complete16(&result, a, b);
            return result;
        }
        inline Reg sub(Reg a, Reg b){
            Reg result;
            result.data = a.data - b.data;
            alu_complete16(&result, a, b);
            return result;
        }
        inline Reg mul(Reg a, Reg b){
            Reg result;
            result.data = a.data * b.data;
            alu_complete16(&result, a, b);
            return result;
        }
        inline Reg div(Reg a, Reg b){
            Reg result;
            result.data = a.data / b.data;
            alu_complete16(&result, a, b);
            return result;
        }
        inline Reg AND(Reg a, Reg b){
            Reg result;
            result.data = a.data & b.data;
            alu_complete16(&result, a, b);
            return result;
        }
        inline Reg NAND(Reg a, Reg b){
            Reg result;
            result.data = ~(a.data & b.data);
            alu_complete16(&result, a, b);
            return result;
        }
        inline Reg OR(Reg a, Reg b){
            Reg result;
            result.data = a.data | b.data;
            alu_complete16(&result, a, b);
            return result;
        }
        inline Reg XOR(Reg a, Reg b){
            Reg result;
            result.data = a.data ^ b.data;
            alu_complete16(&result, a, b);
            return result;
        }
        inline Reg add32(Reg a, Reg b){
            Reg result;
            result.data = a.data + b.data;
            alu_complete32(&result, a, b);
            return result;
        }
        inline Reg sub32(Reg a, Reg b){
            Reg result;
            result.data = a.data - b.data;
            alu_complete32(&result, a, b);
            return result;
        }
        inline Reg OR32(Reg a, Reg b){
            Reg result;
            result.data = a.data | b.data;
            alu_complete32(&result, a, b);
            return result;
        }
        inline Reg AND32(Reg a, Reg b){
            Reg result;
            result.data = a.data & b.data;
            alu_complete32(&result, a, b);
            return result;
        }
        inline Reg NAND32(Reg a, Reg b){
            Reg result;
            result.data = ~(a.data & b.data);
            alu_complete32(&result, a, b);
            return result;
        }
        inline Reg XOR32(Reg a, Reg b){
            Reg result;
            result.data = a.data ^ b.data;
            alu_complete32(&result, a, b);
            return result;
        }
        inline Reg shift(Reg a, Reg b){
            Reg result;
            result.data = a.data >> b.data;
            alu_complete16(&result, a, b);
            return result;
        }
        inline Reg shiftl(Reg a, Reg b){
            Reg result;
            result.data = a.data << b.data;
            alu_complete16(&result, a, b);
            return result;
        }
        inline Reg shift32(Reg a, Reg b){
            Reg result;
            result.data = a.data >> b.data;
            alu_complete32(&result, a, b);
            return result;
        }
        inline Reg shift32l(Reg a, Reg b){
            Reg result;
            result.data = a.data << b.data;
            alu_complete32(&result, a, b);
            return result;
        }
        inline Reg shift_abs(Reg a, uint32_t imm){
            Reg result;
            result.data = a.data >> imm;
            alu_complete16(&result, a, nullreg);
            return result;
        }
        inline Reg shiftl_abs(Reg a, uint32_t imm){
            Reg result;
            result.data = a.data << imm;
            alu_complete16(&result, a, nullreg);
            return result;
        }
        inline Reg shift32_abs(Reg a, uint32_t imm){
            Reg result;
            result.data = a.data >> imm;
            alu_complete32(&result, a, nullreg);
            return result;
        }
        inline Reg shift32l_abs(Reg a, uint32_t imm){
            Reg result;
            result.data = a.data << imm;
            alu_complete32(&result, a, nullreg);
            return result;
        }
        inline Reg adi(Reg a, uint32_t imm){
            Reg result;
            result.reg.data = a.data + imm;
            alu_complete16(&result, a, nullreg);
            return result;
        }
        inline Reg sbi(Reg a, uint32_t imm){
            Reg result;
            result.reg.data = a.data - imm;
            alu_complete16(&result, a, nullreg);
            return result;
        }
        inline Reg mdi(Reg a, uint32_t imm){
            Reg result;
            result.reg.data = a.data * imm;
            alu_complete16(&result, a, nullreg);
            return result;
        }
        inline Reg ddi(Reg a, uint32_t imm){
            Reg result;
            result.reg.data = a.data / imm;
            alu_complete16(&result, a, nullreg);
            return result;
        }
        inline Reg abs(Reg a){
            Reg result;
            result.reg.data = (a.sdata < 0) ? -a.sdata : a.sdata;
            alu_complete16(&result, a, nullreg);
            return result;
        }
        inline Reg mod(Reg a, Reg b){
            Reg result;
            result.reg.data = a.data % b.data;
            alu_complete16(&result, a, b);
            return result;
        }
        inline Reg FMOD(Reg a , Reg b){
            Reg result;
            result.reg.data = fmod(a.fdata , b.fdata);
            alu_complete_float(&result, a, b);
            return result;
        }
        inline Reg sqrt(Reg a){
            Reg result;
            result.reg.data = static_cast<uint32_t>(sqrtf(static_cast<float>(a.data)));
            alu_complete16(&result, a, nullreg);
            return result;
        }
        inline Reg fadd(Reg a, Reg b){
            Reg result;
            result.fdata = a.fdata + b.fdata;
            alu_complete_float(&result , a, b);
            return result;
        }
        inline Reg fsub(Reg a, Reg b){
            Reg result;
            result.fdata = a.fdata - b.fdata;
            alu_complete_float(&result , a, b);
            return result;
        }
        inline Reg fmul(Reg a, Reg b){
            Reg result;
            result.fdata = a.fdata * b.fdata;
            alu_complete_float(&result , a, b);
            return result;
        }
        inline Reg fdiv(Reg a, Reg b){
            Reg result;
            result.fdata = a.fdata / b.fdata;
            alu_complete_float(&result , a, b);
            return result;
        }
       
    public: // cmp's
        inline Reg cmp(Reg a, uint8_t flags , uint8_t invert_mask){
            a.reg.flags ^= invert_mask; // invert flags according to instruction
            Reg sf_do_jmp;
            sf_do_jmp.reg.data = ((a.reg.flags & flags) == flags); // mask out unneeded flags
            return sf_do_jmp;
        }
        inline Reg cmp_or(Reg a , uint8_t flags , uint8_t invert_mask){
            a.reg.flags ^= invert_mask; // invert flags according to instruction
            Reg sf_do_jmp;
            sf_do_jmp.reg.data = ((a.reg.flags & flags) != 0); // mask out unneeded flags
            return sf_do_jmp;
        }
        inline Reg cmp_and(Reg a , uint8_t flags , uint8_t invert_mask){
            a.reg.flags ^= invert_mask; // invert flags according to instruction
            Reg sf_do_jmp;
            sf_do_jmp.reg.data = (a.reg.flags == flags); // mask out unneeded flags
            return sf_do_jmp;
        }
        inline Reg cmp_reg (Reg a, uint8_t flags , uint8_t invert_mask){
            a.bits_16_8.p2 ^= invert_mask; // invert flags according to instruction
            Reg sf_do_jmp;
            sf_do_jmp.reg.data = ((a.bits_16_8.p2 & flags) == flags); // mask out unneeded flags
            return sf_do_jmp;
        }
        

    public:
    inline void alu_complete_float(Reg* result, Reg a, Reg b){
        alu_flags = gen_flags_float(result->fdata, a.fdata, b.fdata);
    }
    inline 
    void alu_complete16(Reg *result , Reg a, Reg b){
        result->reg.flags = gen_flags_16(result->reg.data, a.reg.data, b.reg.data);
        result->reg.meta_data = a.reg.meta_data;
        //return result;
    }
    inline 
    void alu_complete32(Reg *result , Reg a, Reg b){
        alu_flags = gen_flags_32(result->reg.data, a.reg.data, b.reg.data);
        //return result;
    }
    inline 
    uint8_t gen_flags_16(uint32_t result,uint32_t ra ,uint32_t rb){
        uint8_t flags = (
            (1u) | 
            ((result == 0) << 1) |
            ((result > 0xFFFF) << 2) |
            (((~(ra ^ rb) & (ra ^ result) & 0x8000) != 0) << 3)|
            ((result >> 15) << 4) | 
            ((~result & 1) << 5) |
            (0u << 6) | // 16bit flag
            (ra == rb) << 7
        );
        return flags;
    }
    inline
    uint8_t gen_flags_32(uint32_t result, uint32_t ra, uint32_t rb) {
        return (
            (1u)                                               |
            ((result == 0) << 1)|
            ((result < ra) << 2)|  // carry - if result wrapped around it will be smaller than input
            (((~(ra ^ rb) & (ra ^ result) & 0x80000000) != 0) << 3)|
            ((result >> 31) << 4)|
            ((~result & 1) << 5) |
            (1u << 6) |  // 32bit flag
            (ra == rb) << 7
        );
    }
    inline
    uint8_t gen_flags_float(float result, float ra, float rb) {
        return (
            (1u)                         |   // true
            ((result == 0.0f)       << 1)|   // zero
            (isinf(result)          << 2)|   // overflow = inf
            (isnan(result)          << 3)|   // invalid = nan
            ((result < 0.0f)        << 4)|   // sign
            ((result == ra)         << 7)    // equal
        );
    }

};


class Core {
    public:
        uint32_t last_clk;
        unique_ptr<Memory_unit> mem;// = make_unique<Memory_unit>();
        ALU alu;
        Core* cores;
        Core_vars core_vars; // core_vars stands for Core_vars


        void (Core::*execute)() = &Core::execute_normal;

        Core() : mem(nullptr) , cores(nullptr) {}
        Core( Core* cores_array , uint32_t core_id) : mem(make_unique<Memory_unit>()) , cores(cores_array) {
            //load_sf_from_json(core_config, mem.get());
            //cores = cores_array;
            execute = &Core::execute_normal;
            mem->call_stack.resize(CALL_STACK_SIZE);
            init_sf_stats(core_id);
        }
        void execute_normal(){
            Reg instruction = get_reg(get_sf(sf_map::clk).data);
            last_clk = get_sf(sf_map::clk).data;
            uint32_t off1 = get_sf(sf_map::offset0).reg.data;
            uint32_t off2 = get_sf(sf_map::offset1).reg.data;
            uint32_t off3 = get_sf(sf_map::offset2).reg.data;

            #if DEBUG == 1 or DEBUG == 2
                cout << "core " << get_sf(sf_map::core_id).reg.data << " executing instruction with opcode " << instruction.opcode.opcode << " at clk " << get_sf(sf_map::clk).reg.data << endl;
            #endif

            switch(instruction.opcode.opcode){
                case 0:hult(instruction);break;
                case 1:lr(instruction,off1);break;
                case 2:move(instruction,off1,off2);break;
                case 3:move_abs(instruction,off1,off2);break;
                case 4:push_stack(instruction,off1,off2);break;
                case 5:pull_stack(instruction,off1,off2);break;
                case 6:push_cache(instruction,off1,off2);break;
                case 7:pull_cache(instruction,off1,off2);break;
                case 8:lr_block(instruction,off1) ;break;
                case 9:move_block(instruction,off1,off2) ; break;
                case 10:push_bs(instruction,off1,off2); break;
                case 11:pull_bs(instruction,off1,off2); break;
                case 12:push_bc(instruction,off1,off2) ; break;
                case 13:pull_bc(instruction,off1,off2) ; break;
                case 14:add(instruction,off1,off2,off3) ; break;
                case 15:sub(instruction,off1,off2,off3) ; break;
                case 16:add32(instruction,off1,off2,off3) ; break;
                case 17:AND(instruction,off1,off2,off3) ; break;
                case 18:NAND(instruction,off1,off2,off3);break;
                case 19:OR(instruction,off1,off2,off3);break;
                case 20:XOR(instruction,off1,off2,off3);break;
                case 21:shift(instruction,off1,off2,off3);break;
                case 22:shift_abs(instruction,off1,off3);break;
                case 23:shift32(instruction,off1,off2,off3);break;
                case 24:shift32_abs(instruction,off1,off3);break;
                case 25:cmp(instruction,off1);break;
                case 26:jmpu(instruction) ; break;
                case 27:jmp(instruction) ;break;
                case 28:jmp_ptr(instruction,off1);break;
                case 29:ccall(instruction) ; break;
                case 30:call(instruction) ; break;
                case 31:ret(instruction) ; break;
                case 32:append(instruction,off1) ; break;
                case 33:pop(instruction,off1) ; break;
                case 34:push_cstack(instruction,off1) ; break;
                case 35:pull_cstack(instruction,off2) ; break;
                case 36:ADI(instruction,off1) ; break;
                case 37:SDI(instruction,off1) ; break;
                case 38:ld_ptr(instruction,off1,off2) ; break;
                case 39:intrp(instruction,off2) ; break;
                case 40:mult(instruction,off1,off2,off3) ; break;
                case 41:push_clk(instruction,off1) ; break;
                case 42:pull_clk(instruction,off1) ; break;
                case 43:push_sf(instruction,off1) ; break;
                case 44:pull_sf(instruction,off2) ; break;
                case 45:ld_block_offset(instruction,off1) ; break;
                case 46:ld_block_offset_stack(instruction,off1) ; break;
                case 47:ld_block_offset_cache(instruction,off1) ; break;
                case 48:set_sf(instruction) ; break;
                case 49:cmp_and(instruction,off1) ; break;
                case 50:cmp_or(instruction,off1) ; break;
                case 51:cmove_abs(instruction,off1,off2) ; break;
                case 52:cmove_ptr(instruction,off1,off2) ; break;
                case 53:addr_to_block(instruction,off2) ; break;
                case 54:block_to_addr(instruction,off2) ; break;
                case 55:ATB_ptr(instruction,off1,off2) ; break;
                case 56:BTA_ptr(instruction,off1,off2) ; break;
                case 57:syscall(instruction) ; break;
                case 58:push_sc_table(instruction) ; break;
                case 59:pull_sc_table(instruction) ; break;
                case 60:ABS(instruction,off1,off2) ; break;
                case 61:nop(instruction) ; break;
                case 62:stall(instruction,off1) ; break;
                case 63:jmp_ptr_u(instruction,off1) ; break;
                case 64:push_cache_ptr(instruction,off1,off2) ; break;
                case 65:pull_cash_ptr(instruction,off1,off2) ; break;
                case 66:push_stack_ptr(instruction,off1,off2) ; break;
                case 67:pull_stack_ptr(instruction,off1,off2) ; break;
                case 68:push_cstate(instruction,off2) ; break; 
                case 69:pull_cstate(instruction,off2); break;
                case 70:push_core(instruction,off2,off3); break;
                case 71:pull_core(instruction,off2,off3); break;
                case 72:iret(instruction) ; break;
                case 73:cstall(instruction,off2) ; break;
                case 74:RAND(instruction,off1) ; break;
                case 75:onecount(instruction,off1,off2); break;
                case 76:zerocount(instruction,off1,off2); break;
                case 77:MOD(instruction,off1,off2,off3) ; break;
                case 78:div(instruction,off1,off2,off3) ; break;
                case 79:loop_p(instruction) ; break;
                case 80:loop_m(instruction) ; break;
                case 81:loop_ptr_p(instruction,off1) ; break;
                case 82:loop_ptr_m(instruction,off1) ; break;
                case 83:WHILE(instruction) ; break;
                case 84:push_block_core(instruction,off2,off3) ; break;
                case 85:pull_block_core(instruction,off2,off3) ; break;
                case 86:sub32(instruction,off1,off2,off3) ; break;
                case 87:and32(instruction,off1,off2,off3) ; break;
                case 88:NAND32(instruction,off1,off2,off3) ; break;
                case 89:OR32(instruction,off1,off2,off3) ; break;
                case 90:XOR32(instruction,off1,off2,off3) ; break;
                case 91:ld_stack(instruction,off1) ; break;
                case 92:ld_cash(instruction,off1) ; break;
                case 93:ld_byte(instruction,off1) ; break;
                case 94:mult_sined(instruction,off1,off2,off3) ; break;
                case 95:syscall_ptr(instruction,off1) ; break;
                case 96:set_flags(instruction,off1) ; break;
                case 97:test(instruction,off1,off2) ; break;
                case 98:SWAP(instruction,off1,off2) ; break;
                case 99:mdi(instruction,off1) ; break;
                case 100:ddi(instruction,off1) ; break;
                case 101:ld_sf_ptr(instruction,off2) ; break;
                case 102:lock_cache(instruction) ; break;
                case 103:unlock_cache(instruction) ; break;
                case 104:lock_mem(instruction) ; break;
                case 105:unlock_mem(instruction) ; break;
                case 106:push_flags(instruction,off1) ; break;
                case 107:jmpf(instruction) ; break;
                case 108:jmpf_ptr(instruction,off1) ; break;
                case 109:fadd(instruction,off1,off2,off3) ; break;
                case 110:fsub(instruction,off1,off2,off3) ; break;
                case 111:fmult(instruction,off1,off2,off3) ; break;
                case 112:fdiv(instruction,off1,off2,off3) ; break;
                case 113:int_to_f(instruction,off1,off2) ; break;
                case 114:f_to_int(instruction,off1,off2) ; break;
                case 115:NULL_116(instruction) ; break;
                case 116:clear(instruction,off1) ; break;
                case 117:MIN(instruction,off1,off2,off3) ; break;
                case 118:MAX(instruction,off1,off2,off3) ; break;
                case 119:clear_block(instruction,off1) ; break;
                case 120:intrp_abs(instruction) ; break;
                case 121:wf_intrp(instruction) ; break;
                case 122:fabs(instruction,off1,off2) ; break;
                case 123:fsqrt(instruction,off1,off2) ; break;
                case 124:dump_reg(instruction,off1) ; break;
                case 125:dump_sf(instruction) ; break;
                case 126:dump_cache(instruction,off1) ; break;
                case 127:dump_stack(instruction,off1) ;  break;
                case 128:int_sqrt(instruction,off1,off2) ; break;
                case 129:FMOD(instruction,off1,off2,off2) ; break;
                case 130:WFE(instruction) ; break;
                case 131:WFE_ptr(instruction,off1) ; break;
                case 132:offset_to_block(instruction,off1) ; break;
                case 133:jmpb(instruction,off1) ; break;
                case 134:jmpb_ptr(instruction,off1) ; break;
                case 135:NULL_137(instruction) ; break;
                case 136:NULL_138(instruction) ; break;
                case 137:NULL_140(instruction) ; break;
                case 138:wfews(instruction) ; break;
                case 139:send_pf(instruction) ; break;
                case 140:set_pf_out(instruction) ; break;
                case 141:send_pf_ptr(instruction) ; break;
                case 142:cmp_reg(instruction,off1) ; break;
                case 143:ccpus(instruction) ; break;
                case 144:split_byte(instruction,off2,off3) ; break;
                case 145:shift_l(instruction,off1,off2,off3) ; break;
                case 146:shift_l_abs(instruction,off1,off3) ; break;
                case 147:shift32_l(instruction,off1,off2,off3) ; break;
                case 148:shift32_l_abs(instruction,off1,off3) ; break;
                case 149:shut_down(instruction) ; break;
                case 150:break;
                case 151:break;
                case 152:break;
                case 153:break;
                case 154:break;
                case 155:break;
                case 156:break;
                case 157:break;
                case 158:break;
                case 159:break;
                case 160:break;
                case 161:break;
                case 162:break;
                case 163:break;
                case 164:break;
                case 165:break;
                case 166:break;
                case 167:break;
                case 168:break;
                case 169:break;
                case 170:break;
                case 171:break;
                case 172:break;
                case 173:break;
                case 174:break;
                case 175:break;
                case 176:break;
                case 177:break;
                case 178:break;
                case 179:break;
                case 180:break;
                case 181:break;
                case 182:break;
                case 183:break;
                case 184:break;
                case 185:break;
                case 186:break;
                case 187:break;
                case 188:break;
                case 189:break;
                case 190:break;
                case 191:break;
                case 192:break;
                case 193:break;
                case 194:break;
                case 195:break;
                case 196:break;
                case 197:break;
                case 198:break;
                case 199:break;
                case 200:break;
                case 201:break;
                case 202:break;
                case 203:break;
                case 204:break;
                case 205:break;
                case 206:break;
                case 207:break;
                case 208:break;
                case 209:break;
                case 210:break;
                case 211:break;
                case 212:break;
                case 213:break;
                case 214:break;
                case 215:break;
                case 216:break;
                case 217:break;
                case 218:break;
                case 219:break;
                case 220:break;
                case 221:break;
                case 222:break;
                case 223:break;
                case 224:break;
                case 225:break;
                case 226:break;
                case 227:break;
                case 228:break;
                case 229:break;
                case 230:break;
                case 231:break;
                case 232:break;
                case 233:break;
                case 234:break;
                case 235:break;
                case 236:break;
                case 237:break;
                case 238:break;
                case 239:break;
                case 240:break;
                case 241:break;
                case 242:break;
                case 243:break;
                case 244:break;
                case 245:break;
                case 246:break;
                case 247:break;
                case 248:break;
                case 249:break;
                case 250:break;
                case 251:break;
                case 252:break;
                case 253:break;
                case 254:break;
                case 255:break;
                default:
                    break;
            }
            
            if(get_sf(sf_map::clk).data == last_clk){ // if the instruction did not change the clk, increment it
                set_sf(sf_map::clk, {last_clk + 1});
            }

        }
        void execute_wfe(){
            if(!core_vars.wating_on_event){ // core's execute function should not be normal if wating on an event
                execute = &Core::execute_normal;
                return;
            }
            if(core_vars.event_wating_on == get_sf(sf_map::event).data){
                execute = &Core::execute_normal;
            }
            return;
        }
        void execute_WFEWS(){
            if(!core_vars.wating_for_external_write){
                execute = &Core::execute_normal;
                return;
            }
            if(core_vars.last_external_write_to_stack_target == get_sf(sf_map::lpwt).reg.data){
                execute = &Core::execute_normal;
            }
            return;
        }
        void cstall(){
            for(int i = 0 ; i < 50 ; i++){
                    __asm__ volatile("nop");
                }
        }
        void mstall(uint32_t stall_amount){
            for( int i = 0 ; i < stall_amount ; i++){
                cstall();
            }
        }


    private: 
        void hult(Reg instruction){
            set_sf(sf_map::active, {0});
        }
        void lr(Reg instruction ,uint32_t off1){
            set_reg(instruction.bits_8_16.p1+off1, instruction.bits_8_16.p2);
        }
        void move(Reg instruction,uint32_t off1,uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2+off2, get_reg(instruction.bits_8_8_8.p1+off1));
        }
        void move_abs(Reg instruction,uint32_t off1,uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2+off2, get_reg(instruction.bits_8_8_8.p1+off1));
        }
        void push_stack(Reg instruction,uint32_t off1,uint32_t off2){
            set_stack(instruction.bits_8_8_8.p2+off2, get_reg(instruction.bits_8_8_8.p1+off1));
        }
        void pull_stack(Reg instruction,uint32_t off1,uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2+off2, get_stack(instruction.bits_8_8_8.p1+off1));
        }
        void push_cache(Reg instruction ,uint32_t off1,uint32_t off2){
            set_cash(instruction.bits_8_8_8.p2+off2, get_reg(instruction.bits_8_8_8.p1+off1));
        }
        void pull_cache(Reg instruction,uint32_t off1,uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2+off2, get_cash(instruction.bits_8_8_8.p1+off1));
        }
        void lr_block(Reg instruction,uint32_t off1){
            for(int i = 0 ; i < 3 ; i++){
                set_reg(instruction.bits_8_16.p1 + i + off1, instruction.bits_16_8.p2);
            }
        }
        void move_block(Reg instruction,uint32_t off1,uint32_t off2){
            for(int i = 0 ; i < 3 ; i++){
                set_reg(instruction.bits_8_8_8.p2 + i + off2, get_reg(instruction.bits_8_8_8.p1 + i + off1));
            }
        }
        void push_bs(Reg instruction, uint32_t off1,uint32_t off2){
            for(int i = 0 ; i < 3 ; i++){
                set_stack(instruction.bits_8_8_8.p2 + i + off2, get_reg(instruction.bits_8_8_8.p1 + i + off1));
            }
        }
        void pull_bs(Reg instruction,uint32_t off1,uint32_t off2){
            for(int i = 0 ; i < 3 ; i++){
                set_reg(instruction.bits_8_8_8.p2 + i + off2, get_stack(instruction.bits_8_8_8.p1 + i + off1));
            }
        }
        void push_bc(Reg instruction,uint32_t off1,uint32_t off2){
            for(int i = 0 ; i < 3 ; i++){
                set_cash(instruction.bits_8_8_8.p2 + i + off2, get_reg(instruction.bits_8_8_8.p1 + i + off1));
            }
        }
        void pull_bc(Reg instruction,uint32_t off1,uint32_t off2){
            for(int i = 0 ; i < 3 ; i++){
                set_reg(instruction.bits_8_8_8.p2 + i + off2, get_cash(instruction.bits_8_8_8.p1 + i + off1));
            }
        }
        void add(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.add(get_reg(instruction.bits_8_8_8.p1 + off1), get_reg(instruction.bits_8_8_8.p2 + off2));
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
        }
        void sub(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.sub(get_reg(instruction.bits_8_8_8.p1 + off1), get_reg(instruction.bits_8_8_8.p2 + off2));
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
        }
        void add32(Reg instruction , uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.add32(get_reg(instruction.bits_8_8_8.p1 + off1), get_reg(instruction.bits_8_8_8.p2 + off2));
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void AND(Reg instruction , uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.AND(get_reg(instruction.bits_8_8_8.p1 + off1), get_reg(instruction.bits_8_8_8.p2 + off2));
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
        }
        void NAND(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.NAND(get_reg(instruction.bits_8_8_8.p1 + off1), get_reg(instruction.bits_8_8_8.p2 + off2));
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
        }
        void OR(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.OR(get_reg(instruction.bits_8_8_8.p1 + off1), get_reg(instruction.bits_8_8_8.p2 + off2));
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
        }
        void XOR(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.XOR(get_reg(instruction.bits_8_8_8.p1 + off1), get_reg(instruction.bits_8_8_8.p2 + off2));
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
        }
        void shift(Reg instruction, uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.shift(get_reg(instruction.bits_8_8_8.p1 + off1), get_reg(instruction.bits_8_8_8.p2 + off2));
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
        }
        void shift_abs(Reg instruction , uint32_t off1, uint32_t off3){
            Reg result = alu.shift_abs(get_reg(instruction.bits_8_8_8.p1 + off1), instruction.bits_8_8_8.p2);
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
        }
        void shift32(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.shift32(get_reg(instruction.bits_8_8_8.p1 + off1), get_reg(instruction.bits_8_8_8.p2 + off2));
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
        }
        void shift32_abs(Reg instruction , uint32_t off1, uint32_t off3){
            Reg result = alu.shift32_abs(get_reg(instruction.bits_8_8_8.p1 + off1), instruction.bits_8_8_8.p2);
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
        }
        void cmp(Reg instruction , uint32_t off1){
            Reg result = alu.cmp(get_reg(instruction.bits_8_8_8.p1 + off1), instruction.bits_8_8_8.p2, instruction.bits_8_8_8.p3);
            set_sf(sf_map::do_jmp, result);
        }
        void jmpu(Reg instruction){
            set_sf(sf_map::clk, instruction.bits_16_8.p1);
        }
        void jmp(Reg instruction){
            if(get_sf(sf_map::do_jmp).reg.data){
                set_sf(sf_map::clk, instruction.bits_16_8.p1);
            }
        }
        void jmp_ptr(Reg instruction,uint32_t off1){
            if(get_sf(sf_map::do_jmp).reg.data){
                set_sf(sf_map::clk, get_reg(instruction.bits_8_16.p1 + off1).data);
            }
        }
        void ccall(Reg instruction){
            if(get_sf(sf_map::do_jmp).data){
                mem->call_stack.push_back({get_sf(sf_map::clk).reg.data}); // push return address
                set_sf(sf_map::clk, instruction.bits_16_8.p1);
            }
        }
        void call(Reg instruction){
            mem->call_stack.push_back({get_sf(sf_map::clk).reg.data}); // push return address
            set_sf(sf_map::clk, instruction.bits_16_8.p1);
        }
        void ret(Reg instruction){
            if(!mem->call_stack.empty()){
                Reg return_address = mem->call_stack.back();
                mem->call_stack.pop_back();
                set_sf(sf_map::clk, return_address.data);
            }
        }
        void append(Reg instruction ,uint32_t off1){
            mem->call_stack.push_back(get_reg(instruction.bits_8_16.p1 + off1));
        }
        void pop(Reg instruction , uint32_t off1){
            if(!mem->call_stack.empty()){
                Reg value = mem->call_stack.back();
                mem->call_stack.pop_back();
                set_reg(instruction.bits_8_16.p1 + off1, value);
            }
        }
        void push_cstack(Reg instruction, uint32_t off1){
            mem->call_stack[instruction.bits_8_8_8.p2] = get_reg(instruction.bits_8_8_8.p1 + off1);
        }
        void pull_cstack(Reg instruction , uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2 + off2, mem->call_stack[instruction.bits_8_8_8.p1]);
        }
        void ADI(Reg instruction,uint32_t off1){
            Reg result = alu.adi(get_reg(instruction.bits_8_16.p1 + off1), instruction.bits_8_16.p2);
            set_reg(instruction.bits_8_16.p1 + off1, result);
        }
        void SDI(Reg instruction,uint32_t off1){
            Reg result = alu.sbi(get_reg(instruction.bits_8_16.p1 + off1), instruction.bits_8_16.p2);
            set_reg(instruction.bits_8_16.p1 + off1, result);
        }
        void ld_ptr(Reg instruction,uint32_t off1,uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2 + off2, get_reg(instruction.bits_8_8_8.p1 + off1));
        }
        void intrp(Reg instruction,uint32_t off2){
            Core& target = cores[instruction.bits_8_8_8.p1];
            if(target.get_sf(sf_map::interruptable).reg.data){
                target.append_call_stack(target.get_sf(sf_map::clk).reg.data);
                target.set_sf(sf_map::clk,get_reg(instruction.bits_8_8_8.p2 + off2).reg.data);
                target.set_sf(sf_map::active,1);
            }
        }
        void mult(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.mul(get_reg(instruction.bits_8_8_8.p1 + off1), get_reg(instruction.bits_8_8_8.p2 + off2));
            set_reg(instruction.bits_8_8_8.p3 + off3, result);
        }
        void push_clk(Reg instruction,uint32_t off1){
            set_sf(sf_map::clk,get_reg(instruction.bits_8_8_8.p1 + off1));
        }
        void pull_clk(Reg instruction,uint32_t off1){
            set_reg(instruction.bits_8_8_8.p1 + off1,get_sf(sf_map::clk));
        }
        void push_sf(Reg instruction, uint32_t off1){
            set_sf(instruction.bits_8_8_8.p2,get_reg(instruction.bits_8_8_8.p1 + off1));
        }
        void pull_sf(Reg instruction,uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2 + off2,get_sf(instruction.bits_8_8_8.p1));
        }
        void ld_block_offset(Reg instruction,uint32_t off1){
            for(int i = 0; i < 2 ; i++){
                set_sf(sf_map::offset0+i,get_reg(instruction.bits_16_8.p1+i+off1));
            }
        }
        void ld_block_offset_stack(Reg instruction,uint32_t off1){
            for(int i = 0; i < 2 ; i++){
                set_sf(sf_map::offset0+i,get_stack(instruction.bits_16_8.p1+i+off1));
            }
        }
        void ld_block_offset_cache(Reg instruction,uint32_t off1){
            for(int i = 0; i < 2 ; i++){
                set_sf(sf_map::offset0+i,get_cash(instruction.bits_16_8.p1+i+off1));
            }
        }
        void set_sf(Reg instruction){
            set_sf(instruction.bits_8_16.p1,instruction.bits_8_16.p2);
        }
        void cmp_and(Reg instruction, uint32_t off1){
            set_sf(sf_map::do_jmp,alu.cmp_and(get_reg(instruction.bits_8_8_8.p1 + off1),instruction.bits_8_8_8.p2,instruction.bits_8_8_8.p3));
        }
        void cmp_or(Reg instruction,uint32_t off1){
            set_sf(sf_map::do_jmp,alu.cmp_or(get_reg(instruction.bits_8_8_8.p1 + off1),instruction.bits_8_8_8.p2,instruction.bits_8_8_8.p3));
        }
        void cmove_abs(Reg instruction,uint32_t off1,uint32_t off2){
            if(get_sf(sf_map::do_jmp).reg.data){
                move_abs(instruction,off1,off2);
            }
        }
        void cmove_ptr(Reg instruction,uint32_t off1,uint32_t off2){
            if(get_sf(sf_map::do_jmp).reg.data){
                move(instruction,off1,off2);
            }
        }
        void addr_to_block(Reg instruction, uint32_t off2){
            set_reg(instruction.bits_16_8.p2+off2,instruction.bits_16_8.p1 % 4);
        }
        void block_to_addr(Reg instruction,uint32_t off2){
            set_reg(instruction.bits_16_8.p2+off2,instruction.bits_16_8.p1*4);
        }
        void ATB_ptr(Reg instruction,uint32_t off1,uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2+off2,get_reg(instruction.bits_8_8_8.p1+off1).reg.data % 4);
        }
        void BTA_ptr(Reg instruction,uint32_t off1,uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2+off2,get_reg(instruction.bits_8_8_8.p1+off1).reg.data);
        }
        void syscall(Reg instruction){
            uint32_t jmp_addr = get_reg(instruction.bits_8_16.p1+get_sf(sf_map::sc_offset).reg.data).reg.data;
            set_sf(sf_map::clk,jmp_addr);
        }
        void push_sc_table(Reg instruction){
            set_reg(instruction.bits_8_8_8.p1+get_sf(sf_map::sc_offset).reg.data,get_reg(instruction.bits_8_8_8.p2));
        }
        void pull_sc_table(Reg instruction){
            set_reg(instruction.bits_8_8_8.p1,get_reg(instruction.bits_8_8_8.p1 + get_sf(sf_map::sc_offset).reg.data));
        }
        void ABS(Reg instruction, uint32_t off1, uint32_t off2){
            set_reg(instruction.bits_8_8_8.p1+off2,alu.abs(get_reg(instruction.bits_8_8_8.p2+off1)));
        }
        void nop(Reg instruction){
            for(int i = 0 ; i < 50 ; i++){
                __asm__ volatile("nop");
            }
        }
        void stall(Reg instruction, uint32_t off1){
            for(int i = 0 ; i < get_reg(instruction.bits_8_16.p1+off1).reg.data ; i++){
                for(int i = 0 ; i < 50 ; i++){
                    __asm__ volatile("nop");
                }
            }
        }
        void jmp_ptr_u(Reg instruction, uint32_t off1){
            set_sf(sf_map::clk, get_reg(instruction.bits_8_16.p1+off1).data);
        }
        void push_cache_ptr(Reg instruction,uint32_t off1,uint32_t off2){
            set_cash(get_reg(instruction.bits_8_8_8.p2+off2).reg.data,get_reg(instruction.bits_8_8_8.p1+off1));
        }
        void pull_cash_ptr(Reg instruction,uint32_t off1,uint32_t off2){
            set_reg(instruction.bits_8_8_8.p1+off1, get_cash(get_reg(instruction.bits_8_8_8.p2+off2).reg.data));
        }
        void push_stack_ptr(Reg instruction,uint32_t off1,uint32_t off2){
            set_stack(get_reg(instruction.bits_8_8_8.p2+off2).reg.data,get_reg(instruction.bits_8_8_8.p1+off1));
        }
        void pull_stack_ptr(Reg instruction,uint32_t off1,uint32_t off2){
            set_reg(instruction.bits_8_8_8.p1+off1, get_stack(get_reg(instruction.bits_8_8_8.p2+off2).reg.data));
        }
        void push_cstate(Reg instruction,uint32_t off2){
            Core& core = cores[instruction.bits_8_8_8.p1];
            core.set_sf(instruction.bits_8_8_8.p3,get_reg(instruction.bits_8_8_8.p2+off2));
        }
        void pull_cstate(Reg instruction,uint32_t off2){
            Core& core = cores[instruction.bits_8_8_8.p1];
            set_reg(instruction.bits_8_8_8.p2+off2,core.get_sf(instruction.bits_8_8_8.p3));
        }
        void push_core(Reg instruction,uint32_t off2,uint32_t off3){
            Core& core = cores[instruction.bits_8_8_8.p1];
            core.set_reg(instruction.bits_8_8_8.p3+off3,get_reg(instruction.bits_8_8_8.p2+off2));
        }
        void pull_core(Reg instruction,uint32_t off2,uint32_t off3){
            Core& core = cores[instruction.bits_8_8_8.p1];
            set_reg(instruction.bits_8_8_8.p3+off3,core.get_reg(instruction.bits_8_8_8.p2+off2));
        }
        void iret(Reg instruction){
            if(!mem->call_stack.empty()){
                Reg return_address = mem->call_stack.back();
                mem->call_stack.pop_back();
                set_sf(sf_map::clk, return_address.data);
            }
        }
        void cstall(Reg instruction, uint32_t off2){
            Core& core = cores[instruction.bits_8_8_8.p1];
            core.mstall(get_reg(instruction.bits_8_8_8.p2+off2).reg.data);
        }
        void RAND(Reg instruction, uint32_t off1){
            set_reg(instruction.bits_8_16.p1+off1,rand32(get_sf(sf_map::curent_loop).data));
        }
        void onecount(Reg instruction, uint32_t off1, uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2+off2,__builtin_popcount(get_reg(instruction.bits_8_8_8.p1+off1).reg.data));
        }
        void zerocount(Reg instruction, uint32_t off1, uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2+off2,32-__builtin_popcount(get_reg(instruction.bits_8_8_8.p1+off1).reg.data));
        }
        void MOD(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.mod(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
        }
        void div(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.div(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
        }
        void loop_p(Reg instruction){
            if(get_sf(sf_map::counter).reg.data != get_sf(sf_map::counter_target).reg.data){
                set_sf(sf_map::clk, instruction.bits_16_8.p1);
                set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data+1);
            }
        }
        void loop_ptr_p(Reg instruction, uint32_t off1){
            if(get_sf(sf_map::counter).reg.data != get_sf(sf_map::counter_target).reg.data){
                set_sf(sf_map::clk, get_reg(instruction.bits_8_16.p1+off1).reg.data);
                set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data+1);
            }
        }
        void loop_m(Reg instruction){
            if(get_sf(sf_map::counter).reg.data != get_sf(sf_map::counter_target).reg.data){
                set_sf(sf_map::clk, instruction.bits_16_8.p1);
                set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data-1);
            }
        }
        void loop_ptr_m(Reg instruction, uint32_t off1){
            if(get_sf(sf_map::counter).reg.data != get_sf(sf_map::counter_target).reg.data){
                set_sf(sf_map::clk, get_reg(instruction.bits_8_16.p1+off1).reg.data);
                set_sf(sf_map::counter,get_sf(sf_map::counter).reg.data-1);
            }
        }
        void WHILE(Reg instruction){
            if(get_sf(sf_map::counter).reg.data != get_sf(sf_map::counter_target).reg.data){
                set_sf(sf_map::clk, instruction.bits_16_8.p1);
            }
        }
        void push_block_core(Reg instruction,uint32_t off2,uint32_t off3){
            Core& core = cores[instruction.bits_8_8_8.p1];
            for(int i = 0 ; i < 3 ; i++){
                if(instruction.bits_8_8_8.p1+i < REG_SIZE  && instruction.bits_8_8_8.p2+i < REG_SIZE ){
                    core.set_reg(instruction.bits_8_8_8.p3+i+off3,get_reg(instruction.bits_8_8_8.p2+i+off2));
                }
            }
        }
        void pull_block_core(Reg instruction,uint32_t off2,uint32_t off3){
            Core& core = cores[instruction.bits_8_8_8.p1];
            for(int i = 0 ; i < 3 ; i++){
                if(instruction.bits_8_8_8.p1+i < REG_SIZE  && instruction.bits_8_8_8.p2+i < REG_SIZE ){
                    set_reg(instruction.bits_8_8_8.p3+i+off3,core.get_reg(instruction.bits_8_8_8.p2+i+off2));
                }
            }
        }
        void sub32(Reg instruction, uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.sub32(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void and32(Reg instruction, uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.AND32(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void NAND32(Reg instruction, uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.NAND32(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void OR32(Reg instruction, uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.OR32(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void XOR32(Reg instruction, uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.XOR32(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void ld_stack(Reg instruction ,uint32_t off1){
            set_stack(instruction.bits_8_16.p1+off1,instruction.bits_8_16.p2);
        }
        void ld_cash(Reg instruction, uint32_t off1){
            set_cash(instruction.bits_8_16.p1+off1,instruction.bits_8_16.p2);
        }
        void ld_byte(Reg instruction,uint32_t off1){
            Reg r = get_reg(instruction.bits_8_8_8.p1+off1);
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
            set_reg(instruction.bits_8_8_8.p1, r);
        }
        void mult_sined(Reg instruction, uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.mul(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
        }
        void syscall_ptr(Reg instruction, uint32_t off1){
            uint32_t jmp_addr = get_reg(get_reg(instruction.bits_8_16.p1+off1).bits_8_8_8.p1+get_sf(sf_map::sc_offset).reg.data).reg.data;
            set_sf(sf_map::clk,jmp_addr);
        }
        void set_flags(Reg instruction, uint32_t off1){
            Reg value = get_reg(instruction.bits_8_8_8.p1+off1);
            value.reg.flags = instruction.bits_8_8_8.p2;
            set_reg(instruction.bits_8_8_8.p1+off1,value);
        }
        void test(Reg instruction, uint32_t off1,uint32_t off2){
            Reg result = alu.AND(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_sf(sf_map::alu_flags,result.reg.flags);
        }
        void SWAP(Reg instruction, uint32_t off1, uint32_t off2){
            Reg r1 = get_reg(instruction.bits_8_8_8.p1+off1);
            Reg r2 = get_reg(instruction.bits_8_8_8.p2+off2);
            set_reg(instruction.bits_8_8_8.p1+off1, r2);
            set_reg(instruction.bits_8_8_8.p2+off2, r1);
        }
        void mdi(Reg instruction, uint32_t off1){
            Reg result = alu.mdi(get_reg(instruction.bits_8_16.p1+off1), instruction.bits_8_16.p2);
            set_reg(instruction.bits_8_16.p1+off1, result);
        }
        void ddi(Reg instruction, uint32_t off1){
            Reg result = alu.ddi(get_reg(instruction.bits_8_16.p1+off1), instruction.bits_8_16.p2);
            set_reg(instruction.bits_8_16.p1+off1, result);
        }
        void ld_sf_ptr(Reg instruction,uint32_t off2){
            set_sf(instruction.bits_8_8_8.p1,get_reg(instruction.bits_8_8_8.p2+off2));
        }
        void lock_cache(Reg instruction){
            Util::cache_locked = true;
        }
        void unlock_cache(Reg instruction){
            Util::cache_locked = true;
        }
        void lock_mem(Reg instruction){
            Core& core = cores[instruction.bits_8_8_8.p1];
            core.mem->locked = true;
        }
        void unlock_mem(Reg instruction){
            Core& core = cores[instruction.bits_8_8_8.p1];
            core.mem->locked = false;
        }
        void push_flags(Reg instruction, uint32_t off1){
            uint32_t flags;
            flags = get_reg(instruction.bits_8_8_8.p1+off1).reg.flags ^ instruction.bits_8_8_8.p2;
            set_sf(sf_map::alu_flags,flags);
        }
        void jmpf(Reg instruction){
            uint32_t requierd_flags = instruction.bits_16_8.p2;
            if((get_sf(sf_map::alu_flags).bits_16_8.p2 & requierd_flags) == requierd_flags){
                set_sf(sf_map::clk,instruction.bits_16_8.p1);
            }
        }
        void jmpf_ptr(Reg instruction,uint32_t off1){
            uint32_t requierd_flags = instruction.bits_8_8_8.p2;
            if((get_sf(sf_map::alu_flags).bits_8_8_8.p2 & requierd_flags) == requierd_flags){
                set_sf(sf_map::clk,get_reg(instruction.bits_8_8_8.p1+off1));
            }
        }
        void fadd(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.fadd(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void fsub(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.fsub(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void fmult(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.fmul(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void fdiv(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.fdiv(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void int_to_f(Reg instruction,uint32_t off1,uint32_t off2){
            Reg result;
            result.fdata = (float)get_reg(instruction.bits_8_8_8.p1+off1).sdata;
            set_reg(instruction.bits_8_8_8.p2+off2, result);
        }
        void f_to_int(Reg instruction,uint32_t off1,uint32_t off2){
            Reg result;
            result.sdata = (int)get_reg(instruction.bits_8_8_8.p1+off1).fdata;
            set_reg(instruction.bits_8_8_8.p2+off2, result);
        }
        void NULL_116(Reg instruction){

        }
        void clear(Reg instruction,uint32_t off1){
            set_reg(instruction.bits_8_16.p1+off1,nullreg);
        }
        void MIN(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            set_reg(instruction.bits_8_8_8.p3+off3,min(get_reg(instruction.bits_8_8_8.p1+off1).reg.data,get_reg(instruction.bits_8_8_8.p2+off2).reg.data));
        }
        void MAX(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            set_reg(instruction.bits_8_8_8.p3+off3,max(get_reg(instruction.bits_8_8_8.p1+off1).reg.data,get_reg(instruction.bits_8_8_8.p2+off2).reg.data));
        }
        void clear_block(Reg instruction,uint32_t off1){
            for(int i = 0 ; i < 3 ; i++){
                set_reg(instruction.bits_8_16.p1+off1+i,nullreg);
            }
        }
        void intrp_abs(Reg instruction){
            Core& target = cores[instruction.bits_8_16.p1];
            if(target.get_sf(sf_map::interruptable).reg.data){
                target.append_call_stack(target.get_sf(sf_map::clk).reg.data);
                target.set_sf(sf_map::clk,instruction.bits_8_16.p2);
                target.set_sf(sf_map::active,1);
            }
        }
        void wf_intrp(Reg instruction){
            set_sf(sf_map::active,0);
            set_sf(sf_map::interruptable, 1);
        }
        void fabs(Reg instruction,uint32_t off1,uint32_t off2){
            Reg result;
            result.fdata = abs(get_reg(instruction.bits_8_8_8.p1+off1).fdata);
            set_reg(instruction.bits_8_8_8.p2+off2, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        } 
        void fsqrt(Reg instruction,uint32_t off1,uint32_t off2){
            Reg result;
            result.fdata = sqrtf(get_reg(instruction.bits_8_8_8.p1+off1).fdata);
            set_reg(instruction.bits_8_8_8.p2+off2,result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void dump_reg(Reg instruction , uint32_t off1){
            Util::dump_reg_data(get_reg(instruction.bits_8_16.p1+off1),instruction.bits_8_16.p1+off1);
        }
        void dump_sf(Reg instruction){
            Util::dump_reg_data(get_sf(instruction.bits_8_16.p1),instruction.bits_8_16.p1);
        }
        void dump_cache(Reg instruction , uint32_t off1){
            Util::dump_reg_data(get_cash(instruction.bits_8_16.p1+off1),instruction.bits_8_16.p1+off1);
        }
        void dump_stack(Reg instruction,uint32_t off1){
            Util::dump_reg_data(get_stack(instruction.bits_8_16.p1+off1),instruction.bits_8_16.p1+off1);
        }
        void int_sqrt(Reg instruction,uint32_t off1,uint32_t off2){
            set_reg(instruction.bits_8_8_8.p2+off2,(uint32_t)sqrt(get_reg(instruction.bits_8_8_8.p1+off1).fdata));
        }
        void FMOD(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.FMOD(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
            set_sf(sf_map::alu_flags,alu.alu_flags);
        }
        void WFE(Reg instruction){
            execute = &Core::execute_wfe;
            core_vars.wating_on_event = true;
            core_vars.event_wating_on = instruction.bits_16_8.p1;
        }
        void WFE_ptr(Reg instruction,uint32_t off1){
            execute = &Core::execute_wfe;
            core_vars.wating_on_event = true;
            core_vars.event_wating_on = get_reg(instruction.bits_8_16.p1+off1).data;
        }
        void offset_to_block(Reg instruction,uint32_t off1){
            for(int i = 0 ; i < 2 ; i++){
                set_reg(instruction.bits_16_8.p1+i+off1,get_sf(sf_map::offset0+i));
            }
            set_reg(instruction.bits_16_8.p1+3+off1,get_sf(sf_map::sc_offset));
        }
        void jmpb(Reg instruction,uint32_t off1){
            if(get_reg(instruction.bits_8_16.p1+off1).reg.data){
                set_sf(sf_map::clk, instruction.bits_8_16.p2);
            }
        }
        void jmpb_ptr(Reg instruction,uint32_t off1){
            if(get_reg(instruction.bits_8_8_8.p1+off1).reg.data){
                set_sf(sf_map::clk, get_reg(instruction.bits_8_8_8.p2+off1).reg.data);
            }
        }
        void NULL_137(Reg instruction){}
        void NULL_138(Reg instruction){}
        void NULL_139(Reg instruction){}
        void NULL_140(Reg instruction){}
        void wfews(Reg instruction){
            execute = &Core::execute_WFEWS;
            core_vars.wating_for_external_write = true;
            core_vars.last_external_write_to_stack_target = instruction.bits_8_16.p1;
        }
        void send_pf(Reg instruction){

        }
        void set_pf_out(Reg instruction){

        }
        void send_pf_ptr(Reg instruction){

        }
        void cmp_reg(Reg instruction,uint32_t off1){
            Reg result = alu.cmp_reg(get_reg(instruction.bits_8_8_8.p1+off1), instruction.bits_8_8_8.p2, instruction.bits_8_8_8.p3);
            set_sf(sf_map::do_jmp, result);
        }
        void ccpus(Reg instruction){

        }
        void split_byte(Reg instruction,uint32_t off2,uint32_t off3){
            switch(instruction.bits_8_8_8.p1 % 4){
                case 0:
                    set_reg(instruction.bits_8_8_8.p3+off3,get_reg(instruction.bits_8_8_8.p2+off2).bits_8_8_8.p3);
                    break;
                case 1:
                    set_reg(instruction.bits_8_8_8.p3+off3,get_reg(instruction.bits_8_8_8.p2+off2).bits_8_8_8.p2);
                    break;
                case 2:
                    set_reg(instruction.bits_8_8_8.p3+off3,get_reg(instruction.bits_8_8_8.p2+off2).bits_8_8_8.p1);
                    break;
                case 3:
                    set_reg(instruction.bits_8_8_8.p3+off3,get_reg(instruction.bits_8_8_8.p2+off2).bits_8_8_8.opcode);
                    break;
                default:break;
            }
        }
        void shift_l(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.shiftl(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
        }
        void shift_l_abs(Reg instruction,uint32_t off1,uint32_t off3){
            Reg result = alu.shiftl_abs(get_reg(instruction.bits_8_8_8.p1+off1), instruction.bits_8_8_8.p2);
            set_reg(instruction.bits_8_8_8.p3+off3, result);
        }
        void shift32_l(Reg instruction,uint32_t off1,uint32_t off2,uint32_t off3){
            Reg result = alu.shift32l(get_reg(instruction.bits_8_8_8.p1+off1), get_reg(instruction.bits_8_8_8.p2+off2));
            set_reg(instruction.bits_8_8_8.p3+off3, result);
        }
        void shift32_l_abs(Reg instruction,uint32_t off1,uint32_t off3){
            Reg result = alu.shift32l_abs(get_reg(instruction.bits_8_8_8.p1+off1), instruction.bits_8_8_8.p2);
            set_reg(instruction.bits_8_8_8.p3+off3, result);
        }
        void shut_down(Reg instruction){
            Util::cpu_on = false;
            for(int i = 0 ; i < CORE_COUNT ; i++){
                
            }
        }
        
        // i was working on filling in instructions =======================================================================
    private:

    void init_sf_stats(uint32_t core_id){
        set_sf(sf_map::core_id, core_id);
        set_sf(sf_map::reg_size, REG_SIZE);
        set_sf(sf_map::stack_size, STACK_SIZE);
        set_sf(sf_map::cache_size, CACHE_SIZE);
        set_sf(sf_map::core_count, CORE_COUNT); // to be updated by main controller
        set_sf(sf_map::call_stack_depth, CALL_STACK_SIZE); // to be updated by main controller
    }
    public:
    inline
    void append_call_stack(uint32_t addr){
        mem->call_stack.push_back({addr});
    }
    inline
    Reg pop_call_stack(){
        Reg addr = mem->call_stack.back();
        mem->call_stack.pop_back();
        return addr;
    } 
    inline 
    Reg get_reg(uint32_t addr){return mem->regs[addr];}
    inline 
    void set_reg(uint32_t addr, Reg value){mem->regs[addr] = value;}
    inline
    void set_reg(uint32_t addr, uint32_t value){
        Reg reg_value;
        reg_value.data = value;
        mem->regs[addr] = reg_value;
    }
    inline
    Reg get_sf(uint32_t addr){return mem->sf[addr];}
    inline 
    void set_sf(uint32_t addr, Reg value){mem->sf[addr] = value;}
    inline 
    void set_sf(uint32_t addr, uint32_t value){
        Reg reg_value;
        reg_value.data = value;
        mem->sf[addr] = reg_value;
    }
    inline 
    Reg get_stack(uint32_t addr){return mem->stack[addr];}
    inline 
    void set_stack(uint32_t addr, Reg value){mem->stack[addr] = value;}
    inline 
    void set_stack(uint32_t addr , uint32_t value){
        Reg reg_value;
        reg_value.data = value;
        mem->stack[addr] = reg_value;
    }
    inline 
    Reg get_cash(uint32_t addr){return (*mem->cache)[addr];}
    inline 
    void set_cash(uint32_t addr, Reg value){(*mem->cache)[addr] = value;}
    inline 
    void set_cash(uint32_t addr, uint32_t value){
        Reg reg_value;
        reg_value.data = value;
        (*mem->cache)[addr] = reg_value;
    }
    inline void external_write_stack(uint32_t addr , Reg value){
        mem->stack[addr] = value;
        set_sf(sf_map::lpwt,addr);

    }
    inline void external_write_stack(uint32_t addr , uint32_t value){

    }

};


/*
inline
    void split_8_8_8(uint32_t value, uint32_t* op[3]) {
        *op[0] = (value >> 16) & 0xFF;
        *op[1] = (value >> 8)  & 0xFF;
        *op[2] =  value        & 0xFF;
    }
    inline
    void split_8_16(uint32_t value, uint32_t* op[2]) {
        *op[0] = (value >> 16) & 0xFF;
        *op[1] =  value        & 0xFFFF;
    }
    inline
    void split_16_8(uint32_t value, uint32_t* op[2]) {
        *op[0] = (value >> 16) & 0xFFFF;
        *op[1] =  value        & 0xFF;
    }










*/


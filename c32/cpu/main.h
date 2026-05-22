#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <stdfloat> 

#include <memory>
#include <functional>
#include <nlohmann/json.hpp>
#include <immintrin.h>

#include "config.h"
#include "global_utils.h"
#pragma once

using namespace std;
using json = nlohmann::json;


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
    lpwt = 20
};

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
        struct { // IEEE 754 32-bit Float Components
            uint32_t fraction : 23;
            uint32_t exponent : 8;
            uint32_t sign     : 1;
        } f_bits;
};

struct alignas(64) Memory_unit {
    
    //vector<uint32_t> regs;
    //vector<uint32_t> stack;
    //vector<uint32_t> sf;
    //vector<uint32_t>* cash;
    array<Reg, REG_SIZE> regs;
    array<Reg, STACK_SIZE> stack;
    array<Reg, SF_SIZE> sf;
    array<Reg, CACHE_SIZE>* cache = nullptr;
};
enum class ALUop {
    add, sub, mul, div,
    AND, NAND, OR, XOR,
    add32, sub32, OR32, AND32, NAND32, XOR32,
    shift, shiftl, shift32, shift32l,
    adi, sbi, mdi, ddi,
    abs, mod,
    sqrt  // leave as integer sqrt for now
};

class ALU {
    private:
        template<ALUop op>
        inline
        Reg alu16(Reg a, Reg b) {
            Reg result;
            uint32_t ra = a.reg.data;
            uint32_t rb = b.reg.data;
            if      constexpr (op == ALUop::add)    result.reg.data = ra + rb;
            else if constexpr (op == ALUop::sub)    result.reg.data = ra - rb;
            else if constexpr (op == ALUop::mul)    result.reg.data = ra * rb;
            else if constexpr (op == ALUop::div)    result.reg.data = ra / rb;
            else if constexpr (op == ALUop::AND)    result.reg.data = ra & rb;
            else if constexpr (op == ALUop::NAND)   result.reg.data = ~(ra & rb);
            else if constexpr (op == ALUop::OR)     result.reg.data = ra | rb;
            else if constexpr (op == ALUop::XOR)    result.reg.data = ra ^ rb;
            else if constexpr (op == ALUop::shift)  result.reg.data = ra << rb;
            else if constexpr (op == ALUop::shiftl) result.reg.data = ra >> rb;
            else if constexpr (op == ALUop::mod)    result.reg.data = ra % rb;
            else if constexpr (op == ALUop::adi)    result.reg.data = ra + rb;
            else if constexpr (op == ALUop::sbi)    result.reg.data = ra - rb;
            else if constexpr (op == ALUop::mdi)    result.reg.data = ra * rb;
            else if constexpr (op == ALUop::ddi)    result.reg.data = ra / rb;
            else result.reg.data = 0;
            alu_complete16(&result, a, b);
            return result;
        }
        template<ALUop op>
        inline
        Reg alu16(Reg a) {
            Reg result;
            uint32_t ra = a.reg.data;

            if constexpr (op == ALUop::abs)  result.reg.data = result.reg.data = (ra & 0x8000) ? (~ra + 1) & 0xFFFF : ra;;
            else if constexpr (op == ALUop::sqrt) result.reg.data = (uint32_t)sqrtf((float)ra);
            else result.reg.data = 0;
            alu_complete16(&result, a, a);
            return result;
        }
        template<ALUop op>
        inline
        Reg alu32(Reg a, Reg b) {
            Reg result;
            uint32_t ra = a.data;
            uint32_t rb = b.data;
            if constexpr (op == ALUop::add32)  result.data = ra + rb;
            else if constexpr (op == ALUop::sub32)  result.data = ra - rb;
            else if constexpr (op == ALUop::AND32)  result.data = ra & rb;
            else if constexpr (op == ALUop::NAND32) result.data = ~(ra & rb);
            else if constexpr (op == ALUop::OR32)   result.data = ra | rb;
            else if constexpr (op == ALUop::XOR32)  result.data = ra ^ rb;
            else if constexpr (op == ALUop::shift32)  result.data = ra << rb;
            else if constexpr (op == ALUop::shift32l) result.data = ra >> rb;
            else result.data = 0;
            alu_complete32(&result, a, b);
            return result;
        }

    private:

    private:
    inline 
    void alu_complete16(Reg *result , Reg a, Reg b){
        result->reg.flags = gen_flags_16(result->reg.data, a.reg.data, b.reg.data);
        result->reg.meta_data = a.reg.meta_data;
        //return result;
    }
    inline 
    void alu_complete32(Reg *result , Reg a, Reg b){
        result->reg.flags = gen_flags_32(result->reg.data, a.reg.data, b.reg.data);
        result->reg.meta_data = a.reg.meta_data;
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
            (0u) | // 16bit flag
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
            (1u) |  // 32bit flag
            (ra == rb) << 7
        );
    }

};
class Core {
    unique_ptr<Memory_unit> mem = make_unique<Memory_unit>();
    

    public:
        Core(json core_config) : mem(make_unique<Memory_unit>()){
            load_sf_from_json(core_config, mem.get());
        }
        void exacute(){
            Reg instruction = get_reg(get_sf(sf_map::clk).data);
            switch(instruction.opcode.opcode){
                case 0:break;
                case 1:break;
                case 2:break;
                case 3:break;
                case 4:break;
                case 5:break;
                case 6:break;
                case 7:break;
                case 8:break;
                case 9:break;
                case 10:break;
                case 11:break;
                case 12:break;
                case 13:break;
                case 14:break;
                case 15:break;
                case 16:break;
                case 17:break;
                case 18:break;
                case 19:break;
                case 20:break;
                case 21:break;
                case 22:break;
                case 23:break;
                case 24:break;
                case 25:break;
                case 26:break;
                case 27:break;
                case 28:break;
                case 29:break;
                case 30:break;
                case 31:break;
                case 32:break;
                case 33:break;
                case 34:break;
                case 35:break;
                case 36:break;
                case 37:break;
                case 38:break;
                case 39:break;
                case 40:break;
                case 41:break;
                case 42:break;
                case 43:break;
                case 44:break;
                case 45:break;
                case 46:break;
                case 47:break;
                case 48:break;
                case 49:break;
                case 50:break;
                case 51:break;
                case 52:break;
                case 53:break;
                case 54:break;
                case 55:break;
                case 56:break;
                case 57:break;
                case 58:break;
                case 59:break;
                case 60:break;
                case 61:break;
                case 62:break;
                case 63:break;
                case 64:break;
                case 65:break;
                case 66:break;
                case 67:break;
                case 68:break;
                case 69:break;
                case 70:break;
                case 71:break;
                case 72:break;
                case 73:break;
                case 74:break;
                case 75:break;
                case 76:break;
                case 77:break;
                case 78:break;
                case 79:break;
                case 80:break;
                case 81:break;
                case 82:break;
                case 83:break;
                case 84:break;
                case 85:break;
                case 86:break;
                case 87:break;
                case 88:break;
                case 89:break;
                case 90:break;
                case 91:break;
                case 92:break;
                case 93:break;
                case 94:break;
                case 95:break;
                case 96:break;
                case 97:break;
                case 98:break;
                case 99:break;
                case 100:break;
                case 101:break;
                case 102:break;
                case 103:break;
                case 104:break;
                case 105:break;
                case 106:break;
                case 107:break;
                case 108:break;
                case 109:break;
                case 110:break;
                case 111:break;
                case 112:break;
                case 113:break;
                case 114:break;
                case 115:break;
                case 116:break;
                case 117:break;
                case 118:break;
                case 119:break;
                case 120:break;
                case 121:break;
                case 122:break;
                case 123:break;
                case 124:break;
                case 125:break;
                case 126:break;
                case 127:break;
                case 128:break;
                case 129:break;
                case 130:break;
                case 131:break;
                case 132:break;
                case 133:break;
                case 134:break;
                case 135:break;
                case 136:break;
                case 137:break;
                case 138:break;
                case 139:break;
                case 140:break;
                case 141:break;
                case 142:break;
                case 143:break;
                case 144:break;
                case 145:break;
                case 146:break;
                case 147:break;
                case 148:break;
                case 149:break;
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
        }
    private: 

    void load_sf_from_json(json core_config, Memory_unit *mem){
        for (int i = 0 ; i < core_config["sf_size"].get<uint32_t>() ; i++){
            mem->sf[i].data = core_config["sf_preload"][i].get<uint32_t>();
        }
    }
    private: // memory accessors
    inline 
    Reg get_reg(uint32_t addr){return mem->regs[addr];}
    inline 
    void set_reg(uint32_t addr, Reg value){mem->regs[addr] = value;}
    inline 
    Reg get_sf(uint32_t addr){return mem->sf[addr];}
    inline 
    void set_sf(uint32_t addr, Reg value){mem->sf[addr] = value;}
    inline 
    Reg get_stack(uint32_t addr){return mem->stack[addr];}
    inline 
    void set_stack(uint32_t addr, Reg value){mem->stack[addr] = value;}
    inline 
    Reg get_cash(uint32_t addr){return (*mem->cache)[addr];}
    inline 
    void set_cash(uint32_t addr, Reg value){(*mem->cache)[addr] = value;}


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


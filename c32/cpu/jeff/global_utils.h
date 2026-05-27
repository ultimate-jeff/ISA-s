

#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono> 
#include "data.h"
#pragma once
using namespace std;


uint32_t rand32(uint32_t rand_state){
    rand_state ^= rand_state << 13;
    rand_state ^= rand_state >> 17;
    rand_state ^= rand_state << 5;
    return rand_state;
}

namespace Util{
     // prosesor vars holds stats and vars that act throughout the entire prosesor
    bool cpu_on = true;
    bool cache_locked = false;
    string print_que = "";


    inline void print(string msg){;
        print_que += msg + "\n";
    }
    inline void cout_print_que(){
        cout << print_que << endl;
    }
    inline void dump_reg_data(Reg r,uint32_t addr){
        print(
            "reg["  + to_string(addr) + "] "
            " int=" + to_string(r.reg.data)               +
            " sint="+ to_string((int16_t)r.reg.data)       +  // signed interpretation
            " full=" + to_string(r.data)                    +  // full 32 bit raw
            " float=" + to_string(r.fdata)                   +  // float interpretation
            " flg=" + to_string(r.reg.flags)               +  // flags byte
            " meta="+ to_string(r.reg.meta_data)              // metadata
        );
    }

};
::Reg nullreg = {0};



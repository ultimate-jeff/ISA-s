//////////////////// C32 ISA ////////////////////

#what is c32 :
    c32 is a 32 bit emulator ISA baced off of g20,x86 and arm64. It is soposto be faster and more powerfull than the g20 emulator

#stats :
- c32 can have up to 256(bc it has a 8 bit core id peramiters) core althow it was desighed for only 16-8 
- c32 can have up to 256 instructions (bc 8 bit opcode)
- c32 has 3 peramiter splitting maps : opcode(8)_p1(8)_p2(8)_p2(8) , opcode(8)_p1(8)_p2(16) , opcode(8)_p1(16)_p2(8)
- c32 has 4 memory units : regs,stack,cache,system_flags
- all memory units can have a max size of 65536 regesters/words in them exept the system_flags (also known as sf) which can only have 256 regs/words
- the cache in c32 is a chared intercore memory unit 
- the sf in c32 hold spesial varyables like the clock(clk) , do_jmp and interuptable system flags as well as the offsets

# To run code on the C32 emulator, follow these steps:
- 1) assemble your code into decimal
- 2) locate the boot json file (normally in c32/bios/boot.json)
- 3) then chose a memory unit to put your assembled code in (executing code goes in the "regs" )
- 4) finally, chose the core you want to load the data into, and run the emulator (or cimpile then run )
EXAMPLE OF BOOT JSON FILE :
```json
{
    "data":{
        "version":1.0,
        "core_to_boot":0
    },
    "preload":{
        "sf":{
            "size":256,
            "data":[1] // core should start with active flag of 1 so it can run
        },
        "regs":{
            "size":4096,
            "data":[17367039,1040187402,0 .... your code]
        },
        "stack":{
            "size":4096,
            "data":[0]
        },
        "cache":{
            "size":4096,
            "data":[0]
        }
    }    
}
```

#system flag map :
- active : this flag determans if a core is active or not
- interuptable : this flag determans if a core can be interupted 
- do_jmp : this flag determains whether a some instructions do there op (its mainly used for jmp instructions)
- offset0 : offset 0 is added to peramiter 1 before it is exacuted 
- offset1 : this offset is just liek offset0 but it is added to peramiter 2
- offset2 : this offset is the same as 0 and 1 but for peramiter 3
- clk : this is the clock / exacution pointer
- curent_loop : this just keeps track of which loop the curent core is on 
- sc_offset : this offset os for system calls only 
- counter : this sf is used for instructions like : loop+,loop-,while ...
- counter_target : this sf is used for instructions like : loop+,loop-,while ...
- var0 : this is an unalocated sf
- var1 : this is an unalocated sf
- var2 : this is an unalocated sf
- ALU_flag : this offset holds the flags from the ALU when doing a 32 bit alu op or a non 16 bit op which includes stuff like : 32 ops , float ops , random
- force_intrp_ptr : if a value is placed inside of this system flag that is not 0 then it will interupt to the addr inside and then set this sf back to 0
- force_sc_ptr : this flag is just like force_intrp_ptr but it uses the system flag table 
- event : this flag is used for the instruction WateForEvent / WFE
- pf_in : port flags in : this flag is used for external devices (it has not been used yet bc the emulator dose not yet have external device support)
- pf_out : unused
- lpwt : this sf sais the last external write to the stack (when the stack is being writen to from external devises it it then conciderd the ports)
- core_id : this is a system spesifacation flag
- reg_size : this sf is also a system spesifacation flag 
- stack_size : this is a system spesifacation flag
- cache_size : this is a system spesifacation flag
- core_count : this is a system spesifacation flag
- csd : this spesifies the cores call stack depth and it is also a system spesifacation flag

#instructions : 
- 0: hult
- 1: lr, reg(8), data(16)
- 2: move, reg_src(8), reg_dst(8)
- 3: move_abs, addr_src(8), addr_dst(8)
- 4: push_stack, reg_src(8), stack_dst(8)
- 5: pull_stack, stack_src(8), reg_dst(8)
- 6: push_cache, reg_src(8), cache_dst(8)
- 7: pull_cache, cache_src(8), reg_dst(8)
- 8: lr_block, reg(8), data(16)
- 9: move_block, reg_src(8), reg_dst(8)
- 10: push_bs, reg_src(8), stack_dst(8)
- 11: pull_bs, stack_src(8), reg_dst(8)
- 12: push_bc, reg_src(8), cache_dst(8)
- 13: pull_bc, cache_src(8), reg_dst(8)
- 14: add, reg_a(8), reg_b(8), reg_dst(8)
- 15: sub, reg_a(8), reg_b(8), reg_dst(8)
- 16: add32, reg_a(8), reg_b(8), reg_dst(8)
- 17: AND, reg_a(8), reg_b(8), reg_dst(8)
- 18: NAND, reg_a(8), reg_b(8), reg_dst(8)
- 19: OR, reg_a(8), reg_b(8), reg_dst(8)
- 20: XOR, reg_a(8), reg_b(8), reg_dst(8)
- 21: shift, reg_a(8), reg_b(8), reg_dst(8)
- 22: shift_abs, reg_src(8), imm(8), reg_dst(8)
- 23: shift32, reg_a(8), reg_b(8), reg_dst(8)
- 24: shift32_abs, reg_src(8), imm(8), reg_dst(8)
- 25: cmp, reg_src(8), flags(8), invert_mask(8)
- 26: jmpu, addr(16)
- 27: jmp, addr(16)
- 28: jmp_ptr, reg_src(8)
- 29: ccall, addr(16)
- 30: call, addr(16)
- 31: ret
- 32: append, reg_src(8)
- 33: pop, reg_dst(8)
- 34: push_cstack, reg_src(8), cstack_idx(8)
- 35: pull_cstack, cstack_idx(8), reg_dst(8)
- 36: ADI, reg(8), imm(16)
- 37: SDI, reg(8), imm(16)
- 38: ld_ptr, reg_src(8), reg_dst(8)
- 39: intrp, core_id(8), addr_reg(8)
- 40: mult, reg_a(8), reg_b(8), reg_dst(8)
- 41: push_clk, reg_src(8)
- 42: pull_clk, reg_dst(8)
- 43: push_sf, reg_src(8), sf_idx(8)
- 44: pull_sf, sf_idx(8), reg_dst(8)
- 45: ld_block_offset, reg_src(8)
- 46: ld_block_offset_stack, stack_src(8)
- 47: ld_block_offset_cache, cache_src(8)
- 48: set_sf, sf_idx(8), data(16)
- 49: cmp_and, reg_src(8), flags(8), invert_mask(8)
- 50: cmp_or, reg_src(8), flags(8), invert_mask(8)
- 51: cmove_abs, addr_src(8), addr_dst(8)
- 52: cmove_ptr, reg_src(8), reg_dst(8)
- 53: addr_to_block, addr(16), reg_dst(8)
- 54: block_to_addr, block(16), reg_dst(8)
- 55: ATB_ptr, reg_src(8), reg_dst(8)
- 56: BTA_ptr, reg_src(8), reg_dst(8)
- 57: syscall, reg_idx(8)
- 58: push_sc_table, reg_src(8), reg_val(8)
- 59: pull_sc_table, reg_dst(8)
- 60: ABS, reg_src(8), reg_dst(8)
- 61: nop
- 62: stall, reg_count(8)
- 63: jmp_ptr_u, reg_src(8)
- 64: push_cache_ptr, reg_src(8), reg_addr(8)
- 65: pull_cache_ptr, reg_dst(8), reg_addr(8)
- 66: push_stack_ptr, reg_src(8), reg_addr(8)
- 67: pull_stack_ptr, reg_dst(8), reg_addr(8)
- 68: push_cstate, core_id(8), reg_src(8), sf_idx(8)
- 69: pull_cstate, core_id(8), reg_dst(8), sf_idx(8)
- 70: push_core, core_id(8), reg_src(8), reg_dst(8)
- 71: pull_core, core_id(8), reg_src(8), reg_dst(8)
- 72: iret
- 73: cstall, core_id(8), reg_count(8)
- 74: RAND, reg_dst(8)
- 75: onecount, reg_src(8), reg_dst(8)
- 76: zerocount, reg_src(8), reg_dst(8)
- 77: MOD, reg_a(8), reg_b(8), reg_dst(8)
- 78: div, reg_a(8), reg_b(8), reg_dst(8)
- 79: loop_p, addr(16)
- 80: loop_m, addr(16)
- 81: loop_ptr_p, reg_addr(8)
- 82: loop_ptr_m, reg_addr(8)
- 83: WHILE, addr(16)
- 84: push_block_core, core_id(8), reg_src(8), reg_dst(8)
- 85: pull_block_core, core_id(8), reg_src(8), reg_dst(8)
- 86: sub32, reg_a(8), reg_b(8), reg_dst(8)
- 87: and32, reg_a(8), reg_b(8), reg_dst(8)
- 88: NAND32, reg_a(8), reg_b(8), reg_dst(8)
- 89: OR32, reg_a(8), reg_b(8), reg_dst(8)
- 90: XOR32, reg_a(8), reg_b(8), reg_dst(8)
- 91: ld_stack, stack_dst(8), data(16)
- 92: ld_cache, cache_dst(8), data(16)
- 93: ld_byte, reg(8), byte_idx(8), byte_val(8)
- 94: mult_signed, reg_a(8), reg_b(8), reg_dst(8)
- 95: syscall_ptr, reg_idx(8)
- 96: set_flags, reg(8), flags(8)
- 97: test, reg_a(8), reg_b(8)
- 98: SWAP, reg_a(8), reg_b(8)
- 99: mdi, reg(8), imm(16)
- 100: ddi, reg(8), imm(16)
- 101: ld_sf_ptr, sf_idx(8), reg_src(8)
- 102: lock_cache
- 103: unlock_cache
- 104: lock_mem, core_id(8)
- 105: unlock_mem, core_id(8)
- 106: push_flags, reg_src(8), xor_mask(8)
- 107: jmpf, addr(16), req_flags(8)
- 108: jmpf_ptr, reg_addr(8), req_flags(8)
- 109: fadd, reg_a(8), reg_b(8), reg_dst(8)
- 110: fsub, reg_a(8), reg_b(8), reg_dst(8)
- 111: fmult, reg_a(8), reg_b(8), reg_dst(8)
- 112: fdiv, reg_a(8), reg_b(8), reg_dst(8)
- 113: int_to_f, reg_src(8), reg_dst(8)
- 114: f_to_int, reg_src(8), reg_dst(8)
- 115: NULL
- 116: clear, reg(8)
- 117: MIN, reg_a(8), reg_b(8), reg_dst(8)
- 118: MAX, reg_a(8), reg_b(8), reg_dst(8)
- 119: clear_block, reg(8)
- 120: intrp_abs, core_id(8), addr(16)
- 121: wf_intrp
- 122: fabs, reg_src(8), reg_dst(8)
- 123: fsqrt, reg_src(8), reg_dst(8)
- 124: dump_reg, reg(8)
- 125: dump_sf, sf_idx(8)
- 126: dump_cache, cache_idx(8)
- 127: dump_stack, stack_idx(8)
- 128: int_sqrt, reg_src(8), reg_dst(8)
- 129: FMOD, reg_a(8), reg_b(8), reg_dst(8)
- 130: WFE, event_id(16)
- 131: WFE_ptr, reg_src(8)
- 132: offset_to_block, reg_dst(8)
- 133: jmpb, reg_cond(8), addr(16)
- 134: jmpb_ptr, reg_cond(8), reg_addr(8)
- 135: NULL
- 136: NULL
- 137: NULL
- 138: wfews, stack_addr(16)
- 139: send_pf
- 140: set_pf_out
- 141: send_pf_ptr
- 142: cmp_reg, reg_src(8), flags(8), invert_mask(8)
- 143: ccpus
- 144: split_byte, byte_idx(8), reg_src(8), reg_dst(8)
- 145: shift_l, reg_a(8), reg_b(8), reg_dst(8)
- 146: shift_l_abs, reg_src(8), imm(8), reg_dst(8)
- 147: shift32_l, reg_a(8), reg_b(8), reg_dst(8)
- 148: shift32_l_abs, reg_src(8), imm(8), reg_dst(8)
- 149: shut_down
- 150-255: (unimplemented/reserved)
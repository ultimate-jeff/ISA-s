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
0:hult
1:lr,reg(8),data(16)
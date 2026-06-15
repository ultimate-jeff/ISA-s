import string
import struct
import random
import re


prin_RED = '\033[91m'
prin_GREEN = '\033[92m'
prin_BLUE = '\033[94m'
prin_RESET = '\033[0m'
print_YELLOW = '\033[33m'
print_MAGENTA = '\033[35m'
print_CYAN = '\033[36m'
prin_ORANGE = '\033[38;5;208m'
prin_PINK = '\033[38;5;206m'
prin_PURPLE = '\033[38;5;129m'
prin_BROWN = '\033[38;5;94m'
prin_GOLD = '\033[38;5;220m'
prin_LIME = '\033[38;5;118m'
prin_TEAL = '\033[38;5;30m'
prin_NAVY = '\033[38;5;18m'
prin_SKY_BLUE = '\033[38;5;117m'
prin_HOT_PINK = '\033[38;5;198m'
prin_MAROON = '\033[38;5;88m'
prin_OLIVE = '\033[38;5;100m'
prin_VIOLET = '\033[38;5;93m'
prin_SALMON = '\033[38;5;209m'
prin_DARK_GREEN = '\033[38;5;22m'
# opcode perams  101010101001010101010

"""
basic
.lables
*keword
# # or \n
"aa"  ascii(97)  9797
'aa'  ascii(97)  97

"""

opcode_table = {
    "hult": 0,
    "lr": 1,
    "move": 2,
    "move_abs": 3,
    "push_stack": 4,
    "pull_stack": 5,
    "push_cache": 6,
    "pull_cache": 7,
    "lr_block": 8,
    "move_block": 9,
    "push_bs": 10,
    "pull_bs": 11,
    "push_bc": 12,
    "pull_bc": 13,
    "add": 14,
    "sub": 15,
    "add32": 16,
    "AND": 17,
    "NAND": 18,
    "OR": 19,
    "XOR": 20,
    "shift": 21,
    "shift_abs": 22,
    "shift32": 23,
    "shift32_abs": 24,
    "cmp": 25,
    "jmpu": 26,
    "jmp": 27,
    "jmp_ptr": 28,
    "ccall": 29,
    "call": 30,
    "ret": 31,
    "append": 32,
    "pop": 33,
    "push_cstack": 34,
    "pull_cstack": 35,
    "ADI": 36,
    "SDI": 37,
    "ld_ptr": 38,
    "intrp": 39,
    "mult": 40,
    "push_clk": 41,
    "pull_clk": 42,
    "push_sf": 43,
    "pull_sf": 44,
    "ld_block_offset": 45,
    "ld_block_offset_stack": 46,
    "ld_block_offset_cache": 47,
    "set_sf": 48,
    "cmp_and": 49,
    "cmp_or": 50,
    "cmove_abs": 51,
    "cmove_ptr": 52,
    "addr_to_block": 53,
    "block_to_addr": 54,
    "ATB_ptr": 55,
    "BTA_ptr": 56,
    "syscall": 57,
    "push_sc_table": 58,
    "pull_sc_table": 59,
    "ABS": 60,
    "nop": 61,
    "stall": 62,
    "jmp_ptr_u": 63,
    "push_cache_ptr": 64,
    "pull_cache_ptr": 65,
    "push_stack_ptr": 66,
    "pull_stack_ptr": 67,
    "push_cstate": 68,
    "pull_cstate": 69,
    "push_core": 70,
    "pull_core": 71,
    "iret": 72,
    "cstall": 73,
    "RAND": 74,
    "onecount": 75,
    "zerocount": 76,
    "MOD": 77,
    "div": 78,
    "loop_p": 79,
    "loop_m": 80,
    "loop_ptr_p": 81,
    "loop_ptr_m": 82,
    "WHILE": 83,
    "push_block_core": 84,
    "pull_block_core": 85,
    "sub32": 86,
    "and32": 87,
    "NAND32": 88,
    "OR32": 89,
    "XOR32": 90,
    "ld_stack": 91,
    "ld_cache": 92,
    "ld_byte": 93,
    "mult_signed": 94,
    "syscall_ptr": 95,
    "set_flags": 96,
    "test": 97,
    "SWAP": 98,
    "mdi": 99,
    "ddi": 100,
    "ld_sf_ptr": 101,
    "lock_cache": 102,
    "unlock_cache": 103,
    "lock_mem": 104,
    "unlock_mem": 105,
    "push_flags": 106,
    "jmpf": 107,
    "jmpf_ptr": 108,
    "fadd": 109,
    "fsub": 110,
    "fmult": 111,
    "fdiv": 112,
    "int_to_f": 113,
    "f_to_int": 114,
    "NULL_115": 115,
    "clear": 116,
    "MIN": 117,
    "MAX": 118,
    "clear_block": 119,
    "intrp_abs": 120,
    "wf_intrp": 121,
    "fabs": 122,
    "fsqrt": 123,
    "dump_reg": 124,
    "dump_sf": 125,
    "dump_cache": 126,
    "dump_stack": 127,
    "int_sqrt": 128,
    "FMOD": 129,
    "WFE": 130,
    "WFE_ptr": 131,
    "offset_to_block": 132,
    "jmpb": 133,
    "jmpb_ptr": 134,
    "NULL_135": 135,
    "NULL_136": 136,
    "NULL_137": 137,
    "wfews": 138,
    "send_pf": 139,
    "set_pf_out": 140,
    "send_pf_ptr": 141,
    "cmp_reg": 142,
    "ccpus": 143,
    "split_byte": 144,
    "shift_l": 145,
    "shift_l_abs": 146,
    "shift32_l": 147,
    "shift32_l_abs": 148,
    "shut_down": 149
}
        

def write_c32_bin(path:"str",inst:"list",sinst:"list",cinst:"list"):
    hs = 52
    # each item is 4 bytes so multiply by 4
    reg_start   = hs
    stack_start = hs + len(inst) * 4
    cache_start = hs + (len(inst) + len(sinst)) * 4
    sf_start    = hs + (len(inst) + len(sinst) + len(cinst)) * 4
    try:
        print(f"writing file {path}")
        with open(path, "wb") as f:
            f.write(b'C32\x00')
            f.write(struct.pack('<I', 1))    # version
            f.write(struct.pack('<I', 0))    # secondary version
            f.write(struct.pack('<I', 0))    # entry point
            f.write(struct.pack('<II', reg_start,   len(inst)))
            f.write(struct.pack('<II', stack_start, len(sinst)))
            f.write(struct.pack('<II', cache_start, len(cinst)))
            f.write(struct.pack('<II', sf_start,    255))
            f.write(struct.pack('<I', 0))    # spacing
            for i in inst:
                f.write(struct.pack('<I', i)) 
            for si in sinst:
                f.write(struct.pack("<I",si))
            for ci in cinst:
                f.write(struct.pack("<I",ci))
        print("done")
    except Exception as e:
        print(f"!!-error-!! : {e}")

class Assembler():
    error_msgs = ["-----ERROR (you're still a FAILURE 0~0)--{",
        "i can smell the FAILURE!",
        "MMM, Tastes like garbage","you call that code?",
        "you sure you know how to code?","YOU HAVE BECOME YOUR FATHER",
        "get that outa here! this ain't a DUMPSTER",
        "it's no use, this is a lost cause (:"
    ]

    def error(self,msg:"str",line_num:"int",e=None):
        if random.randint(0,100) > 10:
            self.errors.append(f"{prin_RED}{random.choice(Assembler.error_msgs)}{prin_RESET}")
        if e != None:
            self.errors.append(f"{prin_RED}!!-error-!! : {msg} : at line {line_num}{prin_RESET}")
        else:
            self.errors.append(f"{prin_RED}!!-error-!! : {msg} : at line {line_num} with python error of {e}{prin_RESET}")
    def log(self,msg:"str",line_num:"int"):
        self.logs.append(f"{prin_ORANGE}loged : {msg} on line {line_num}{prin_RESET}")

    def remove_coments(self,text:str):
        new_text = ""
        removing = False
        for char in text:
            if char == "#":
                removing = not removing
            if not removing:
                new_text += char
        new_text = new_text.replace("#","")
        return new_text
    

    def splitting_map_8_8_8(self,text:"list[str]",line_num:"int"):
        pass


    def tokenize(self,text):
        #tokens = re.split(r'[^a-zA-Z0-9_*;.]', text)
        tokens = re.findall(r'"[^"]*"|\'[^\']*\'|[a-zA-Z0-9_*;.]+', text)
        for t in range(len(tokens)):
            if '' in tokens:
                tokens.remove('')
        return 
    
    def set_mu(self,line_num,line)
    
    def __init__(self,text):
        self.text = text
        self.output = []
        self.errors = []
        self.logs = []
        self.lables = {}
        self.regs = []
        self.stack = []
        self.cache = []
        self.curent_mu
        self.ops = {
            0:{"reg":,
               
               },
            1:{},
            2:{}
        }
        self.mu_index = {
            "regs":0,
            "stack":None,
            "cache":None
        }
            



text = """

*regs

*include
lr 10 2
lr2 22 5
lr 'c' 3
lr "j ssds" 5
.lable
"""
a = Assembler("""

*reg 
*include "jeff.txt"

lr 20 2
lr2 300 2
lr"jeff sads" 33
.lable

""")

tokens = a.tokenize(a.text)
print(tokens)


#  add   r1 r2 r3 lkasdjk askdljlkdas
# ["add" ,"r1" , "r2" , "r3" , "lkasdjk" , "askdljlkdas"]

# opcode,perms
# 8_8_8
# 8_16
# 16_8

































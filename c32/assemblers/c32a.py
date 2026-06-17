
import struct
import random
import re
import typer
import sys
import json
from functools import wraps


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

prin_colors = (
    prin_RED, # 0
    prin_GREEN, # 1
    prin_BLUE, # 2
    prin_RESET, # 3
    print_YELLOW, # 4
    print_MAGENTA, # 5
    print_CYAN, # 6
    prin_ORANGE, # 7
    prin_PINK, # 8
    prin_PURPLE, # 9
    prin_BROWN, # 10
    prin_GOLD, # 11
    prin_LIME, # 12
    prin_TEAL, # 13
    prin_NAVY, # 14
    prin_SKY_BLUE, # 15
    prin_HOT_PINK, # 16
    prin_MAROON, # 17
    prin_OLIVE, # 18
    prin_VIOLET, # 19
    prin_SALMON, # 20
    prin_DARK_GREEN, # 21
)

key_chars = ["*","."]
commands = {
    "*!":0,
    "*stop":0,
    "*reg":1,
    "*stack":2,
    "*cache":3,
    "*include":4,
    "*import":4,
    ".":5,
    "*r":6
}

comparison_ops = {
    "==": {"flg": 128, "im": 0,   "op": "-", "cmp": "cmp"}, # == set        -> a == b
    "!=": {"flg": 128, "im": 128, "op": "-", "cmp": "cmp"}, # == inverted   -> a != b
    "<":  {"flg": 4,   "im": 0,   "op": "-", "cmp": "cmp"}, # carry set        -> a < b   (unsigned)
    ">=": {"flg": 4,   "im": 4,   "op": "-", "cmp": "cmp"}, # carry inverted   -> a >= b  (unsigned)
    ">":  {"flg": 4,   "im": 0,   "op": "-r","cmp": "cmp"}, # carry set on b-a -> a > b   (unsigned)
    "<=": {"flg": 4,   "im": 4,   "op": "-r","cmp": "cmp"}, # carry inverted   -> a <= b  (unsigned)
}

opcode_table = {
    "hult": {"value":0,"sm":888,"pc":0},
    "hlt": {"value":0,"sm":888,"pc":0},
    "lr": {"value":1,"sm":816,"pc":2},
    "ldi": {"value":1,"sm":816,"pc":2},
    "move": {"value":2,"sm":888,"pc":2},
    "move_abs": {"value":3,"sm":888,"pc":2},
    "push_stack": {"value":4,"sm":888,"pc":2},
    "pull_stack": {"value":5,"sm":888,"pc":2},
    "push_cache": {"value":6,"sm":888,"pc":2},
    "pull_cache": {"value":7,"sm":888,"pc":2},
    "lr_block": {"value":8,"sm":816,"pc":2},
    "move_block": {"value":9,"sm":888,"pc":2},
    "push_bs": {"value":10,"sm":88,"pc":28},
    "pull_bs": {"value":11,"sm":888,"pc":2},
    "push_bc": {"value":12,"sm":888,"pc":2},
    "pull_bc": {"value":13,"sm":888,"pc":2},
    "add": {"value":14,"sm":888,"pc":3},
    "+": {"value":14,"sm":888,"pc":3},
    "sub": {"value":15,"sm":888,"pc":3},
    "-": {"value":15,"sm":888,"pc":3},
    "add32": {"value":16,"sm":888,"pc":3},
    "AND": {"value":17,"sm":888,"pc":3},
    "&&": {"value":17,"sm":888,"pc":3},
    "NAND": {"value":18,"sm":888,"pc":3},
    "~&&": {"value":18,"sm":888,"pc":3},
    "OR": {"value":19,"sm":888,"pc":3},
    "||": {"value":19,"sm":888,"pc":3},
    "XOR": {"value":20,"sm":888,"pc":3},
    "^": {"value":20,"sm":888,"pc":8},
    "shift": {"value":21,"sm":888,"pc":3},
    "shift_abs": {"value":22,"sm":888,"pc":3},
    "shift32": {"value":23,"sm":888,"pc":3},
    "shift32_abs": {"value":24,"sm":888,"pc":3},
    "cmp": {"value":25,"sm":888,"pc":3},
    "jmpu": {"value":26,"sm":168,"pc":1},
    "jmp": {"value":27,"sm":168,"pc":1},
    "jmp_ptr": {"value":28,"sm":816,"pc":1},
    "ccall": {"value":29,"sm":168,"pc":1},
    "call": {"value":30,"sm":168,"pc":1},
    "ret": {"value":31,"sm":888,"pc":0},
    "append": {"value":32,"sm":816,"pc":1},
    "pop": {"value":33,"sm":816,"pc":0},
    "push_cstack": {"value":34,"sm":888,"pc":2},
    "pull_cstack": {"value":35,"sm":888,"pc":2},
    "ADI": {"value":36,"sm":816,"pc":2},
    "adi": {"value":36,"sm":816,"pc":2},
    "+=": {"value":36,"sm":816,"pc":2},
    "SDI": {"value":37,"sm":816,"pc":2},
    "sdi": {"value":37,"sm":816,"pc":2},
    "-=": {"value":37,"sm":816,"pc":2},
    "ld_ptr": {"value":38,"sm":888,"pc":2},
    "&": {"value":38,"sm":888,"pc":2},
    "intrp": {"value":39,"sm":888,"pc":2},
    "mult": {"value":40,"sm":888,"pc":3},
    "push_clk": {"value":41,"sm":888,"pc":1},
    "pull_clk": {"value":42,"sm":888,"pc":1},
    "push_sf": {"value":43,"sm":888,"pc":2},
    "pull_sf": {"value":44,"sm":888,"pc":2},
    "ld_block_offset": {"value":45,"sm":168,"pc":1},
    "ld_block_offset_stack": {"value":46,"sm":168,"pc":1},
    "ld_block_offset_cache": {"value":47,"sm":168,"pc":1},
    "set_sf": {"value":48,"sm":816,"pc":2},
    "cmp_and": {"value":49,"sm":888,"pc":3},
    "cmp_or": {"value":50,"sm":888,"pc":3},
    "cmove_abs": {"value":51,"sm":888,"pc":2},
    "cmove_ptr": {"value":52,"sm":888,"pc":2},
    "addr_to_block": {"value":53,"sm":168,"pc":2},
    "block_to_addr": {"value":54,"sm":168,"pc":2},
    "ATB_ptr": {"value":55,"sm":888,"pc":2},
    "BTA_ptr": {"value":56,"sm":888,"pc":2},
    "syscall": {"value":57,"sm":816,"pc":1},
    "push_sc_table": {"value":58,"sm":888,"pc":2},
    "pull_sc_table": {"value":59,"sm":888,"pc":2},
    "ABS": {"value":60,"sm":888,"pc":2},
    "nop": {"value":61,"sm":888,"pc":0},
    "stall": {"value":62,"sm":816,"pc":1},
    "jmp_ptr_u": {"value":63,"sm":816,"pc":1},
    "push_cache_ptr": {"value":64,"sm":888,"pc":2},
    "pull_cache_ptr": {"value":65,"sm":888,"pc":2},
    "push_stack_ptr": {"value":66,"sm":888,"pc":2},
    "pull_stack_ptr": {"value":67,"sm":888,"pc":2},
    "push_cstate": {"value":68,"sm":888,"pc":3},
    "pull_cstate": {"value":69,"sm":888,"pc":3},
    "push_core": {"value":70,"sm":888,"pc":3},
    "pull_core": {"value":71,"sm":888,"pc":3},
    "iret": {"value":72,"sm":888,"pc":0},
    "cstall": {"value":73,"sm":888,"pc":2},
    "RAND": {"value":74,"sm":816,"pc":1},
    "onecount": {"value":75,"sm":888,"pc":2},
    "zerocount": {"value":76,"sm":888,"pc":2},
    "MOD": {"value":77,"sm":888,"pc":3},
    "div": {"value":78,"sm":888,"pc":3},
    "loop_p": {"value":79,"sm":168,"pc":1},
    "for+": {"value":79,"sm":168,"pc":1},
    "loop_m": {"value":80,"sm":168,"pc":1},
    "for-": {"value":80,"sm":168,"pc":1},
    "loop_ptr_p": {"value":81,"sm":816,"pc":1},
    "loop_ptr_m": {"value":82,"sm":816,"pc":1},
    "while": {"value":83,"sm":168,"pc":1},
    "push_block_core": {"value":84,"sm":888,"pc":3},
    "pull_block_core": {"value":85,"sm":888,"pc":3},
    "sub32": {"value":86,"sm":888,"pc":3},
    "and32": {"value":87,"sm":888,"pc":3},
    "nand32": {"value":88,"sm":888,"pc":3},
    "or32": {"value":89,"sm":888,"pc":3},
    "xor32": {"value":90,"sm":888,"pc":3},
    "ld_stack": {"value":91,"sm":816,"pc":2},
    "ld_cache": {"value":92,"sm":816,"pc":2},
    "ld_byte": {"value":93,"sm":888,"pc":3},
    "mult_signed": {"value":94,"sm":888,"pc":3},
    "syscall_ptr": {"value":95,"sm":888,"pc":1},
    "set_flags": {"value":96,"sm":888,"pc":2},
    "test": {"value":97,"sm":888,"pc":2},
    "SWAP": {"value":98,"sm":888,"pc":2},
    "mdi": {"value":99,"sm":816,"pc":2},
    "ddi": {"value":100,"sm":816,"pc":2},
    "ld_sf_ptr": {"value":101,"sm":888,"pc":2},
    "lock_cache": {"value":102,"sm":888,"pc":0},
    "unlock_cache": {"value":103,"sm":888,"pc":0},
    "lock_mem": {"value":104,"sm":888,"pc":1},
    "unlock_mem": {"value":105,"sm":888,"pc":1},
    "push_flags": {"value":106,"sm":888,"pc":2},
    "jmpf": {"value":107,"sm":168,"pc":2},
    "jmpf_ptr": {"value":108,"sm":888,"pc":2},
    "fadd": {"value":109,"sm":888,"pc":3},
    "fsub": {"value":110,"sm":888,"pc":3},
    "fmult": {"value":111,"sm":888,"pc":3},
    "fdiv": {"value":112,"sm":888,"pc":3},
    "int_to_f": {"value":113,"sm":888,"pc":2},
    "f_to_int": {"value":114,"sm":888,"pc":2},
    "NULL_115": {"value":115,"sm":888,"pc":0},
    "clear": {"value":116,"sm":816,"pc":1},
    "MIN": {"value":117,"sm":888,"pc":3},
    "MAX": {"value":118,"sm":888,"pc":3},
    "clear_block": {"value":119,"sm":816,"pc":1},
    "intrp_abs": {"value":120,"sm":816,"pc":2},
    "wf_intrp": {"value":121,"sm":888,"pc":0},
    "fabs": {"value":122,"sm":888,"pc":2},
    "fsqrt": {"value":123,"sm":888,"pc":2},
    "dump_reg": {"value":124,"sm":816,"pc":1},
    "dump_sf": {"value":125,"sm":816,"pc":1},
    "dump_cache": {"value":126,"sm":816,"pc":1},
    "dump_stack": {"value":127,"sm":816,"pc":1},
    "int_sqrt": {"value":128,"sm":888,"pc":1},
    "FMOD": {"value":129,"sm":888,"pc":3},
    "WFE": {"value":130,"sm":168,"pc":1},
    "WFE_ptr": {"value":131,"sm":816,"pc":1},
    "offset_to_block": {"value":132,"sm":168,"pc":1},
    "jmpb": {"value":133,"sm":816,"pc":2},
    "jmpb_ptr": {"value":134,"sm":888,"pc":2},
    "NULL_135": {"value":135,"sm":888,"pc":0},
    "NULL_136": {"value":136,"sm":888,"pc":0},
    "NULL_137": {"value":137,"sm":888,"pc":0},
    "wfews": {"value":138,"sm":816,"pc":1},
    "send_pf": {"value":139,"sm":888,"pc":1},
    "set_pf_out": {"value":140,"sm":888,"pc":1},
    "send_pf_ptr": {"value":141,"sm":888,"pc":1},
    "cmp_reg": {"value":142,"sm":888,"pc":3},
    "ccpus": {"value":143,"sm":888,"pc":1},
    "split_byte": {"value":144,"sm":888,"pc":3},
    "shift_l": {"value":145,"sm":888,"pc":3},
    "shift_l_abs": {"value":146,"sm":888,"pc":3},
    "shift32_l": {"value":147,"sm":888,"pc":3},
    "shift32_l_abs": {"value":148,"sm":888,"pc":3},
    "shut_down": {"value":149,"sm":888,"pc":3},
}

opcode_table = {k.lower(): v for k, v in opcode_table.items()}

opcode_sm_table = {}
for k in opcode_table.keys():
    opcode_sm_table[opcode_table[k]["value"]] = opcode_table[k]["sm"]


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

def write_c32_json(path:"str",inst:"list",sinst:"list",cints:"list"):
    data = {
            "data":{
                "version":1.0,
                "core_to_boot":0,
                "boot_method":"preload"
            },
            "binary":{
                "binary_path":None
            },
            "preload":{
                "sf":{
                    "size":256,
                    "data":[1,0,1]
                },
                "regs":{
                    "size":4096,
                    "data":inst
                },
                "stack":{
                    "size":4096,
                    "data":sinst
                },
                "cache":{
                    "size":4096,
                    "data":cints
                }
            }    
        }
    try:
        with open(path,"w") as f:
            json.dump(data,f,indent=4)
    except Exception as e:
        print(f"!!-error-!! : {e}")


class Assembler:
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
            self.errors.append(f"{prin_RED}!!-error-!! : {msg} : at line {line_num} : python error {e}{prin_RESET}")
        else:
            self.errors.append(f"{prin_RED}!!-error-!! : {msg} : at line {line_num}{prin_RESET}")
    def log(self,msg:"str",line_num:"int",color_index=1):
        self.logs.append(f"{prin_colors[color_index]}loged : {msg} on line {line_num}{prin_RESET}")
    def print_data(self):
        if len(self.errors) > 0:
            print("----- folowing errors have occurred. code may not run as expected ----- ")
            for error in self.errors:
                print(error)
        if len(self.logs) > 0 :
            print("----- folowing data was loged during compalation -----")
            for log in self.logs:
                print(log)
        print()
        print("compalation complete")

    def open_file(self,path,ln=None):
        data = ""
        try:
            with open(path,"r") as f:
                data = f.read()
        except Exception as e:
            self.error("faled to open path : {path} ",ln,e)
        return data
    def loop_over_lines(func):
        def wrapper(self,text:"str"):
            data = []
            splitline = text.splitlines()
            for i in range(len(splitline)):
                line = splitline[i]
            data.append(func(self,line))
            return data
        return wrapper
    
    def clamp(self,value,limit):
        if value > limit:
            return limit
        return value
    
    def _tokenize_line(self,text):
        #tokens = re.findall(r'"[^"]*"|\'[^\']*\'|[a-zA-Z0-9_*.!=<>]+', text)
        tokens = re.findall(r'"[^"]*"|\'[^\']*\'|[a-zA-Z0-9_*.!=<>%]+|\S', text)
        for t in range(len(tokens)):
            if '' in tokens:
                tokens.remove('')
        return tokens
    def tokenize(self,text:"str"):
        out = []
        splitline = text.splitlines()
        for line in splitline:
            line_token = self._tokenize_line(line)
            if line_token != []:
                out.append(line_token)
        return out   
    def remove_coments(self,text:str):
        new_text = ""
        removing = False
        for i in range(len(text)):
            char = text[i]
            if char == "#":
                removing = not removing
            if not removing:
                new_text += char
            if removing and i == len(text)-1:
                self.log("un closed comment","idk but you should be able to figure it out",4)
        new_text = new_text.replace("#","")
        return new_text
    def merge_tokens(self, tokens_a: "list[list[str,int]]", index: "int", tokens_b: "list[list[str,int]]"):
        tokens_a[index:index+1] = tokens_b
        return tokens_a
    def detect_stops(self,text):
        new_text = ""
        adding = True
        splitline = text.splitlines()
        for i in range(len(splitline)):
            line = splitline[i]
            line_token = self._tokenize_line(line)
            if line_token != [] and commands.get(line_token[0]) == commands["*!"]:
                adding = not adding
            elif adding:
                new_text += line+"\n"
        return new_text
    def handle_includes(self,tokens:"list[list[str]]"):
        for li in range(len(tokens)):
            line_token = tokens[li]
            if commands.get(line_token[0]) == commands["*include"]:
                if len(line_token) >= 2:
                    path = line_token[1].replace("\"","")
                    data = self.open_file(path,li)
                    data = self.phase1(data)
                    tokens = self.merge_tokens(tokens,li,data)
                else:
                    self.error(f"faled to find {line_token[0]} path",li)
                    tokens.pop(li)
        return tokens


    def phase1(self,text):
        text = self.remove_coments(text)
        text = self.detect_stops(text)
        tokens = self.tokenize(text)
        tokens = self.handle_includes(tokens)
        return tokens
    def compile(self,text):
        tokens = self.phase1(text)

        return tokens
    def __init__(self):
        self.lables = {}
        self.errors = []
        self.logs = []


text = """

*reg

lr 'Z' 5;
.start
add 2 3 'Z'
cmp 'Z' !=
jmp .start


"""

a = Assembler()
out = a.compile(text)
a.print_data()
print()
for line in out:
    print(line)
print()
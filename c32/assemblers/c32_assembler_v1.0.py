
import struct
import random
import re
import typer
import sys


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

key_chars = ["*",".",";","%"]
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
    "hult": {"value":0,"sm":888},
    "hlt": {"value":0,"sm":888},
    "lr": {"value":1,"sm":816},
    "ldi": {"value":1,"sm":816},
    "move": {"value":2,"sm":888},
    "move_abs": {"value":3,"sm":888},
    "push_stack": {"value":4,"sm":888},
    "pull_stack": {"value":5,"sm":888},
    "push_cache": {"value":6,"sm":888},
    "pull_cache": {"value":7,"sm":888},
    "lr_block": {"value":8,"sm":816},
    "move_block": {"value":9,"sm":888},
    "push_bs": {"value":10,"sm":888},
    "pull_bs": {"value":11,"sm":888},
    "push_bc": {"value":12,"sm":888},
    "pull_bc": {"value":13,"sm":888},
    "add": {"value":14,"sm":888},
    "+": {"value":14,"sm":888},
    "sub": {"value":15,"sm":888},
    "-": {"value":15,"sm":888},
    "add32": {"value":16,"sm":888},
    "AND": {"value":17,"sm":888},
    "&&": {"value":17,"sm":888},
    "NAND": {"value":18,"sm":888},
    "~&&": {"value":18,"sm":888},
    "OR": {"value":19,"sm":888},
    "||": {"value":19,"sm":888},
    "XOR": {"value":20,"sm":888},
    "^": {"value":20,"sm":888},
    "shift": {"value":21,"sm":888},
    "shift_abs": {"value":22,"sm":888},
    "shift32": {"value":23,"sm":888},
    "shift32_abs": {"value":24,"sm":888},
    "cmp": {"value":25,"sm":888},
    "jmpu": {"value":26,"sm":168},
    "jmp": {"value":27,"sm":168},
    "jmp_ptr": {"value":28,"sm":816},
    "ccall": {"value":29,"sm":168},
    "call": {"value":30,"sm":168},
    "ret": {"value":31,"sm":888},
    "append": {"value":32,"sm":816},
    "pop": {"value":33,"sm":816},
    "push_cstack": {"value":34,"sm":888},
    "pull_cstack": {"value":35,"sm":888},
    "ADI": {"value":36,"sm":816},
    "adi": {"value":36,"sm":816},
    "+=": {"value":36,"sm":816},
    "SDI": {"value":37,"sm":816},
    "sdi": {"value":37,"sm":816},
    "-=": {"value":37,"sm":816},
    "ld_ptr": {"value":38,"sm":888},
    "&": {"value":38,"sm":888},
    "intrp": {"value":39,"sm":888},
    "mult": {"value":40,"sm":888},
    "push_clk": {"value":41,"sm":888},
    "pull_clk": {"value":42,"sm":888},
    "push_sf": {"value":43,"sm":888},
    "pull_sf": {"value":44,"sm":888},
    "ld_block_offset": {"value":45,"sm":168},
    "ld_block_offset_stack": {"value":46,"sm":168},
    "ld_block_offset_cache": {"value":47,"sm":168},
    "set_sf": {"value":48,"sm":816},
    "cmp_and": {"value":49,"sm":888},
    "cmp_or": {"value":50,"sm":888},
    "cmove_abs": {"value":51,"sm":888},
    "cmove_ptr": {"value":52,"sm":888},
    "addr_to_block": {"value":53,"sm":168},
    "block_to_addr": {"value":54,"sm":168},
    "ATB_ptr": {"value":55,"sm":888},
    "BTA_ptr": {"value":56,"sm":888},
    "syscall": {"value":57,"sm":816},
    "push_sc_table": {"value":58,"sm":888},
    "pull_sc_table": {"value":59,"sm":888},
    "ABS": {"value":60,"sm":888},
    "nop": {"value":61,"sm":888},
    "stall": {"value":62,"sm":816},
    "jmp_ptr_u": {"value":63,"sm":816},
    "push_cache_ptr": {"value":64,"sm":888},
    "pull_cache_ptr": {"value":65,"sm":888},
    "push_stack_ptr": {"value":66,"sm":888},
    "pull_stack_ptr": {"value":67,"sm":888},
    "push_cstate": {"value":68,"sm":888},
    "pull_cstate": {"value":69,"sm":888},
    "push_core": {"value":70,"sm":888},
    "pull_core": {"value":71,"sm":888},
    "iret": {"value":72,"sm":888},
    "cstall": {"value":73,"sm":888},
    "RAND": {"value":74,"sm":816},
    "onecount": {"value":75,"sm":888},
    "zerocount": {"value":76,"sm":888},
    "MOD": {"value":77,"sm":888},
    "div": {"value":78,"sm":888},
    "loop_p": {"value":79,"sm":168},
    "for+": {"value":79,"sm":168},
    "loop_m": {"value":80,"sm":168},
    "for-": {"value":80,"sm":168},
    "loop_ptr_p": {"value":81,"sm":816},
    "loop_ptr_m": {"value":82,"sm":816},
    "while": {"value":83,"sm":168},
    "push_block_core": {"value":84,"sm":888},
    "pull_block_core": {"value":85,"sm":888},
    "sub32": {"value":86,"sm":888},
    "and32": {"value":87,"sm":888},
    "nand32": {"value":88,"sm":888},
    "or32": {"value":89,"sm":888},
    "xor32": {"value":90,"sm":888},
    "ld_stack": {"value":91,"sm":816},
    "ld_cache": {"value":92,"sm":816},
    "ld_byte": {"value":93,"sm":888},
    "mult_signed": {"value":94,"sm":888},
    "syscall_ptr": {"value":95,"sm":888},
    "set_flags": {"value":96,"sm":888},
    "test": {"value":97,"sm":888},
    "SWAP": {"value":98,"sm":888},
    "mdi": {"value":99,"sm":816},
    "ddi": {"value":100,"sm":816},
    "ld_sf_ptr": {"value":101,"sm":888},
    "lock_cache": {"value":102,"sm":888},
    "unlock_cache": {"value":103,"sm":888},
    "lock_mem": {"value":104,"sm":888},
    "unlock_mem": {"value":105,"sm":888},
    "push_flags": {"value":106,"sm":888},
    "jmpf": {"value":107,"sm":168},
    "jmpf_ptr": {"value":108,"sm":888},
    "fadd": {"value":109,"sm":888},
    "fsub": {"value":110,"sm":888},
    "fmult": {"value":111,"sm":888},
    "fdiv": {"value":112,"sm":888},
    "int_to_f": {"value":113,"sm":888},
    "f_to_int": {"value":114,"sm":888},
    "NULL_115": {"value":115,"sm":888},
    "clear": {"value":116,"sm":816},
    "MIN": {"value":117,"sm":888},
    "MAX": {"value":118,"sm":888},
    "clear_block": {"value":119,"sm":816},
    "intrp_abs": {"value":120,"sm":816},
    "wf_intrp": {"value":121,"sm":888},
    "fabs": {"value":122,"sm":888},
    "fsqrt": {"value":123,"sm":888},
    "dump_reg": {"value":124,"sm":816},
    "dump_sf": {"value":125,"sm":816},
    "dump_cache": {"value":126,"sm":816},
    "dump_stack": {"value":127,"sm":816},
    "int_sqrt": {"value":128,"sm":888},
    "FMOD": {"value":129,"sm":888},
    "WFE": {"value":130,"sm":168},
    "WFE_ptr": {"value":131,"sm":816},
    "offset_to_block": {"value":132,"sm":168},
    "jmpb": {"value":133,"sm":816},
    "jmpb_ptr": {"value":134,"sm":888},
    "NULL_135": {"value":135,"sm":888},
    "NULL_136": {"value":136,"sm":888},
    "NULL_137": {"value":137,"sm":888},
    "wfews": {"value":138,"sm":816},
    "send_pf": {"value":139,"sm":888},
    "set_pf_out": {"value":140,"sm":888},
    "send_pf_ptr": {"value":141,"sm":888},
    "cmp_reg": {"value":142,"sm":888},
    "ccpus": {"value":143,"sm":888},
    "split_byte": {"value":144,"sm":888},
    "shift_l": {"value":145,"sm":888},
    "shift_l_abs": {"value":146,"sm":888},
    "shift32_l": {"value":147,"sm":888},
    "shift32_l_abs": {"value":148,"sm":888},
    "shut_down": {"value":149,"sm":888},
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

    def clamp(self,line_num,value,limit,msg=1):
        if value > limit:
            self.error(f"parameter {msg} is greater than the opcode's splitting map permits ({limit}), snapping to max value", line_num)
            return limit
        return value
    def map_8_8_8(self, tokens: list, ln):
        suppressed = ";" in tokens
        params = [t for t in tokens if t != ";"]
        expected = 4  # opcode + 3 params, change per map
        if len(params) < expected and not suppressed:
            self.error(f"parameters missing (888): filling with 0", ln)
        opcode = params[0] if len(params) > 0 else 0
        p1 = params[1] if len(params) > 1 else 0
        p2 = params[2] if len(params) > 2 else 0
        p3 = params[3] if len(params) > 3 else 0

        opcode = self.clamp(ln,opcode,255)
        p1 = self.clamp(ln,p1,255,1)
        p2 = self.clamp(ln,p2,255,2)
        p3 = self.clamp(ln,p3,255,3)
        value = (opcode << 24) | (p1 << 16) | (p2 << 8) | p3
        return value
    def map_8_16(self,tokens:"list[int,str]",ln):
        suppressed = ";" in tokens
        params = [t for t in tokens if t != ";"]
        expected = 3
        if len(params) < expected and not suppressed:
            self.error(f"parameters missing (888): filling with 0", ln)
        opcode = params[0] if len(params) > 0 else 0
        p1 = params[1] if len(params) > 1 else 0
        p2 = params[2] if len(params) > 2 else 0

        opcode = self.clamp(ln,opcode,255)
        p1 = self.clamp(ln,p1,255,1)
        p2 = self.clamp(ln,p2,(2**16)-1,2)
        value = (opcode << 24) | (p1 << 16) | p2
        return value
    def map_16_8(self,tokens:"list[int,str]",ln):
        suppressed = ";" in tokens
        params = [t for t in tokens if t != ";"]
        expected = 3
        if len(params) < expected and not suppressed:
            self.error(f"parameters missing (888): filling with 0", ln)
        opcode = params[0] if len(params) > 0 else 0
        p1 = params[1] if len(params) > 1 else 0
        p2 = params[2] if len(params) > 2 else 0

        opcode = self.clamp(ln,opcode,255)
        p2 = self.clamp(ln,p2,255,2)
        p1 = self.clamp(ln,p1,(2**16)-1,1)
        value = (opcode << 24) | (p2 << 16) | p1
        return value
    def map32(self,tokens:"list[list[int,str]]",ln):
        p1 = self.clamp(ln,tokens[0],(2**32)-1)
        return p1

    def splitting_map(self,tokens:"list[list[int,str]]"):
        new_tokens = []
        for i in range(len(tokens)):
            line_token = tokens[i]
            opcode = line_token[0]
            split_map = opcode_sm_table.get(opcode)
            if split_map == 888:
                curent_SM = self.map_8_8_8
            elif split_map == 816:
                curent_SM = self.map_8_16
            elif split_map == 168:
                curent_SM = self.map_16_8
            elif split_map == None:
                curent_SM = self.map32
            else:
                self.error(f"invalid splitting map {split_map} , falling back to defalt of 888","compiler error on like ~ 360 in function : splitting_map")
                curent_SM = self.map_8_8_8
            new_tokens.append(curent_SM(line_token,i))
        return new_tokens

    def tokenize_line(self,text):
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
            line_token = self.tokenize_line(line)
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
    def _fill_in_opcode(self,line_token,line_num):
        op = line_token[0]
        if op[0] in key_chars:
            return line_token
        opcode = opcode_table.get(op)["value"]
        if opcode == None:
            opcode = 0
            self.error(f"invalid opcode {op} , replacing with hult",line_num)
        line_token[0] = opcode
        return line_token   
    def fill_in_opcodes(self,tokens:"list[list[str]]"):
        for i in range(len(tokens)):
            tokens[i] = self._fill_in_opcode(tokens[i],i)
        return tokens
    def init_text(self,text:"str"):
        text.lower()
        return text
    def appy_comparison_ops(self,tokens:"list[list[str,int]]"):
        for i in range(len(tokens)):
            line_token = tokens[i]
            for j in range(len(line_token)):
                elm = line_token[j]
                comp_op = comparison_ops.get(elm)
                if comp_op != None: # im stands for invert mask and invert mask are applied before the comparison 
                    line_token[j:j+1] = [comp_op["flg"], comp_op["im"]]
        return tokens            
    def section_data(self,tokens:"list[list[str,int]]"): # this is in charge of the *! and *stop commands
        new_tokens = []
        adding = True
        for i in range(len(tokens)):
            line_token = tokens[i]
            command = commands.get(line_token[0])
            if command == 0:
                adding = not adding
            if adding and command != 0:
                new_tokens.append(line_token)
        return new_tokens
    def create_lables(self,tokens:"list[list[str,int]]"):
        new_tokens = []
        for i in range(len(tokens)):
            line_token = tokens[i]
            elm = line_token[0]
            if type(elm) == str and commands.get(elm[0]) == 5:
                self.lables[line_token[0]] = i
            else:
                new_tokens.append(line_token)
        return new_tokens
    def apply_lables(self,tokens:"list[list[str,int]]"):
        for i in range(len(tokens)):
            line_token = tokens[i]
            for j in range(len(line_token)):
                elm = line_token[j]
                if type(elm) == str and commands.get(elm[0]) == 5:
                    tokens[i][j] = self.lables.get(elm)
        return tokens
    def _include_file(self,perams:"list[str]",line_num):
        new_line_token = [[0]]  # i was working on includs
        try:
            perm1 = perams[1] == "r"
        except IndexError:
            perm1 = "r"
        if perm1:
            with open(perams[0].replace("\"",""),"r") as f:
                text = f.read()
                new_line_token = self.phase1(text)
        else:
            self.error(f"faled to include {perams[0].replace("\"","")} , try doing : *include \"{perams[0].replace("\"","")}\" r ",line_num)
        return new_line_token
    def merge_tokens(self, tokens_a: "list[list[str,int]]", index: "int", tokens_b: "list[list[str,int]]"):
        tokens_a[index:index+1] = tokens_b
        return tokens_a
    def handle_includes(self,tokens:"list[list[str,int]]"):
        for i in range(len(tokens)):
            line_token = tokens[i]
            if commands.get(line_token[0]) == 4:
                perams = line_token.copy()
                perams.pop(0)
                new_tokens = self._include_file(perams,i)
                self.merge_tokens(tokens,i,new_tokens)
        return tokens
    def string_eval(self,tokens:list[list[str,int]]):
        for i in range(len(tokens)):
            line_token = tokens[i]
            for j in range(len(line_token)):
                elm = line_token[j]
                try:
                    if type(elm) == str and ("\'" in elm):
                        tokens[i][j] = ord(elm[1])
                    elif type(elm) == str and ("\"" in elm):
                        value = 0
                        for char in elm[1:-1]:
                            value += ord(char)
                        tokens[i][j] = value
                except IndexError:
                    tokens[i][j] = 0
                    self.error(f"quotes not closed",i,IndexError)
        return tokens
    def apply_reg_preloading(self,tokens:"list[list[str,int]]"):
        for i in range(len(tokens)):
            line_token = tokens[i]
            op = line_token[0]
            if commands.get(op) == 6:
                try:
                    if int(line_token[1]) < 2**32:
                        value = int(line_token[1])
                except ValueError:
                    self.log("command *r was given a str instead of an int , using ascii sum of the str",i,4)
                    value = 0
                    for char in line_token[1]:
                        value += ord(char)
                except IndexError:
                    self.error("command *r peramiters not fillid in , filling in wiht 0",i,IndexError)
                    value = 0
                if value < (2**32)-1:
                    tokens[i] = [value]
                else:
                    self.log(f"command *r value {value} exceeds 32bit range, clamping to max", i, 4)
                    tokens[i] = [2**32 - 1]
        return tokens
    def separate_MU(self,tokens:"list[list[str,int]]"):
        curent_mu = "*reg"
        build = {"*reg":[],"*stack":[],"*cache":[]}
        build_keys = build.keys()
        for i in range(len(tokens)):
            line_token = tokens[i]
            op = line_token[0]
            if type(op) == str and op in build_keys:
                curent_mu = op
            if line_token[0] not in build_keys:
                build[curent_mu].append(line_token)
        return build
    def convert_to_int(self,tokens:"list[list[str,int]]"):
        for i in range(len(tokens)):
            line_token = tokens[i]
            for j in range(len(line_token)):
                elm = line_token[j]
                if type(elm) == str:
                    contains_key = False
                    for k in key_chars:
                        if k in elm:
                            contains_key = True
                    if not contains_key:
                        tokens[i][j] = int(elm)
        return tokens

    def phase1(self,text):
        text = self.init_text(text)
        text = self.remove_coments(text)
        tokens = self.tokenize(text)
        tokens = self.section_data(tokens)
        tokens = self.handle_includes(tokens)
        return tokens
    def phase2(self,tokens:"list[list[str,int]]"):
        tokens = self.create_lables(tokens)
        tokens = self.apply_lables(tokens)
        return tokens
    def phase3(self,tokens):
        tokens = self.string_eval(tokens)
        tokens = self.apply_reg_preloading(tokens)
        tokens = self.convert_to_int(tokens)
        build = self.separate_MU(tokens)
        for k in build.keys():
            build[k] = self.splitting_map(build[k])

        return build
           
    def compile(self,text):
        tokens = self.phase1(text)
        tokens = self.fill_in_opcodes(tokens)
        tokens = self.appy_comparison_ops(tokens)
        tokens = self.phase2(tokens)
        build = self.phase3(tokens)
        return build
    
    def __init__(self):
        self.output = []
        self.errors = []
        self.logs = []
        self.lables = {}
            

assembler = Assembler()
app = typer.Typer()

def compile(input_path,output_path):
    try:
        with open(input_path,"r") as f:
            text = f.read()
        output_data = assembler.compile(text)
        write_c32_bin(output_path,output_data["*reg"],output_data["*stack"],output_data["*cache"])
    except Exception as e:
        print(f"compiler error during compile {e} (probably do to a file not found)")
    
@app.command()
def cli_compile(input_path:str , output_path:str):
    compile(input_path,output_path)

print("-----")
if __name__ == "__mane__":
    if len(sys.argv) > 1:
        app()  # CLI mode: c32 cli-compile jeff.txt out.c32
    else:
        print("c32 v1.0.0 compiler")
        path = input("enter path u want to compile :")
        output_path = input("enter output_path :")
        compile(path, output_path)



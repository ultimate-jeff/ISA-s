#pragma once

#define EX_BIT_MAPING 0

// main cpu config 

#define CORE_COUNT 4

#define REG_SIZE 4096
#define STACK_SIZE 4096
#define CACHE_SIZE 4096
#define SF_SIZE 256
#define CALL_STACK_SIZE 256

#define DEBUG 4
/*
DEBUG:
1 or 2 shows u what instruction each core exacutes 

2 or 3 gives you speed data on how fast the emulator ran

4 just tells u whenever the cpu clockes every 1000 cycles
*/

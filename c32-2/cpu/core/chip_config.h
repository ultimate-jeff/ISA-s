#include "includes.h"
#pragma once

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;

#define CORE_COUNT 4
#define AVAILABLE_CIDS 64

#define DEBUG 1

//#define CORE_COUNT 4 // there can only be up to 256 cores
constexpr uint8_t CORE_MASK = AVAILABLE_CIDS- 1;

#define REG_SIZE 4096
#define STACK_SIZE 256
#define CACHE_SIZE 256
#define CALL_STACK_DEPTH 256
#define SF_SIZE 256
#define PORT_SIZE 256

constexpr uint16_t REG_MASK = REG_SIZE - 1;
constexpr uint16_t STACK_MASK = STACK_SIZE - 1;
constexpr uint16_t CACHE_MASK = CACHE_SIZE - 1;
constexpr uint16_t CALL_STACK_MASK = CALL_STACK_DEPTH - 1;
constexpr uint16_t SF_MASK = SF_SIZE - 1;
constexpr uint16_t PORT_MASK = PORT_SIZE - 1;

// note all MU sizes must be powers of 2.

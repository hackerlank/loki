#pragma once

#include <stdint.h>

typedef uint8_t     BYTE;
typedef uint16_t    WORD;
typedef uint32_t    DWORD;
typedef uint64_t    QWORD;
typedef int32_t		SDWORD;
typedef int16_t		SWORD;

#define SCREEN_WIDTH 13
#define SCREEN_HEIGHT 19
#define MAX_NAMESIZE 32


#define SAFE_DELETE(x) { if (x) { delete (x); (x) = nullptr; } }
#define SAFE_DELETE_VEC(x) { if (x) { delete [](x); (x) = nullptr; } }

#include "logger.h"

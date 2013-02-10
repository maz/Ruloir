#ifndef CRC_H
#define CRC_H

#include "prefix.h"

#define CRC32InitialValue	(0xFFFFFFFF)

uint32_t CRC32(uint32_t*,uint8_t);

#endif
#include "crc.h"
//Based on http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/bsd/libkern/crc32.c

#include "crc_table.h"

#define UPDC32(octet,crc) (crc32_tab[((crc)\
     ^ ((uint8_t)octet)) & 0xff] ^ ((crc) >> 8))

uint32_t CRC32(uint32_t *crc,uint8_t c){
	*crc=UPDC32(c, *crc);
	return ~(*crc);
}
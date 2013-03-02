// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
//
// Based on a CRC32 implementation by Finn Yannick Jacobs
// =============================================================================

#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

CHash::CHash() {
	uint32_t c;
	uint32_t p = 0xEDB88320L;
	int i, j;
	
	for (i = 0; i < 256; i++) {
		c = i;
		
		for (j = 8; j > 0; j--) {
			if (c & 1) c = (c >> 1) ^ p;
			else c >>= 1;
		}
		
		crc32_tab[i] = c;
	}
}

uint32_t CHash::crc32(uint8_t *data, uint32_t len) {
	uint32_t crc = 0xFFFFFFFF;
	
	for (uint32_t i = 0; i < len; i++) {
		crc = ((crc >> 8) & 0x00FFFFFF) ^ crc32_tab[(crc ^ *data++) & 0xFF];
	}
	
	return (crc ^ 0xFFFFFFFF);
}


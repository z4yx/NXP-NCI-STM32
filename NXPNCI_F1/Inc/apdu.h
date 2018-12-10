#pragma once
#include <stdint.h>

void handleAPDU(uint8_t* recv, uint8_t sz_recv, uint8_t* send, uint8_t* psz_send);
void resetAPDU();

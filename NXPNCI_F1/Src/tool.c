/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#include <stdint.h>
//#include "FreeRTOS.h"
//#include "semphr.h"

void Sleep(unsigned int ms)
{
//	SemaphoreHandle_t xSemaphore = NULL;
//
//	xSemaphore = xSemaphoreCreateBinary();
//	xSemaphoreTake( xSemaphore, portTICK_PERIOD_MS * ms);
    HAL_Delay(ms);
}

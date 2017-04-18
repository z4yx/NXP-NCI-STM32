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
#include "stm32f1xx_hal.h"
//#include "FreeRTOS.h"
//#include "semphr.h"
#include <tool.h>

#define NXPNCI_I2C_ADDR_8BIT        (0x28<<1)

//SemaphoreHandle_t IrqSem = NULL;
extern I2C_HandleTypeDef hi2c1;
volatile uint8_t irq_flag;
//i2c_master_transfer_t masterXfer;

//typedef enum {ERROR = 0, SUCCESS = !ERROR} Status;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//	if (GPIO_ReadPinInput(NXPNCI_IRQ_GPIO, NXPNCI_IRQ_PIN) == 1)
//	{
//	    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//		GPIO_ClearPinsInterruptFlags(NXPNCI_IRQ_GPIO, 1U << NXPNCI_IRQ_PIN);
//		xSemaphoreGiveFromISR(IrqSem, &xHigherPriorityTaskWoken);
//	}
    if(GPIO_Pin == NXPNCI_IRQ_Pin){
        irq_flag = 1;
//        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//        xSemaphoreGiveFromISR(IrqSem, &xHigherPriorityTaskWoken);
    }
}

static HAL_StatusTypeDef I2C_WRITE(uint8_t *pBuff, uint16_t buffLen)
{
//    printf("HAL_I2C_Master_Transmit %d bytes\r\n",buffLen);
    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit   (&hi2c1,
            NXPNCI_I2C_ADDR_8BIT,
            pBuff,
            buffLen,
            100
    );
//    printf("HAL_I2C_Master_Transmit=%d\r\n",ret);
    return ret;
//    masterXfer.direction = kI2C_Write;
//    masterXfer.data = pBuff;
//    masterXfer.dataSize = buffLen;
//
//    return I2C_MasterTransferBlocking(NXPNCI_I2C_INSTANCE, &masterXfer);
}

static HAL_StatusTypeDef I2C_READ(uint8_t *pBuff, uint16_t buffLen)
{
//    printf("HAL_I2C_Master_Receive %d bytes\r\n",buffLen);
    HAL_StatusTypeDef ret = HAL_I2C_Master_Receive   (&hi2c1,
            NXPNCI_I2C_ADDR_8BIT,
            pBuff,
            buffLen,
            100
    );
//    printf("HAL_I2C_Master_Receive=%d\r\n",ret);
    return ret;
//    masterXfer.direction = kI2C_Read;
//    masterXfer.data = pBuff;
//    masterXfer.dataSize = buffLen;
//
//    return I2C_MasterTransferBlocking(NXPNCI_I2C_INSTANCE, &masterXfer);
}

static ErrorStatus tml_Init(void) {
//    i2c_master_config_t masterConfig;
//    uint32_t sourceClock;
//
//    gpio_pin_config_t irq_config = {kGPIO_DigitalInput, 0,};
//    gpio_pin_config_t ven_config = {kGPIO_DigitalOutput, 0,};
//
//    GPIO_PinInit(NXPNCI_IRQ_GPIO, NXPNCI_IRQ_PIN, &irq_config);
//    GPIO_PinInit(NXPNCI_VEN_GPIO, NXPNCI_VEN_PIN, &ven_config);
//
//    I2C_MasterGetDefaultConfig(&masterConfig);
//    masterConfig.baudRate_Bps = NXPNCI_I2C_BAUDRATE;
//    sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);
//    masterXfer.slaveAddress = NXPNCI_I2C_ADDR_7BIT;
//    masterXfer.subaddress = 0;
//    masterXfer.subaddressSize = 0;
//    masterXfer.flags = kI2C_TransferDefaultFlag;
//    I2C_MasterInit(NXPNCI_I2C_INSTANCE, &masterConfig, sourceClock);

//    IrqSem = xSemaphoreCreateBinary();

    return SUCCESS;
}

static ErrorStatus tml_DeInit(void) {
//	vSemaphoreDelete(IrqSem);
	HAL_GPIO_WritePin(NXPNCI_VEN_GPIO_Port, NXPNCI_VEN_Pin, GPIO_PIN_RESET);
//	GPIO_ClearPinsOutput(NXPNCI_VEN_GPIO, 1U << NXPNCI_VEN_PIN);
    return SUCCESS;
}

static ErrorStatus tml_Reset(void) {
    HAL_GPIO_WritePin(NXPNCI_VEN_GPIO_Port, NXPNCI_VEN_Pin, GPIO_PIN_RESET);
//	GPIO_ClearPinsOutput(NXPNCI_VEN_GPIO, 1U << NXPNCI_VEN_PIN);
	Sleep(10);
    HAL_GPIO_WritePin(NXPNCI_VEN_GPIO_Port, NXPNCI_VEN_Pin, GPIO_PIN_SET);
//	GPIO_SetPinsOutput(NXPNCI_VEN_GPIO, 1U << NXPNCI_VEN_PIN);
	Sleep(10);
	return SUCCESS;
}

static ErrorStatus tml_Tx(uint8_t *pBuff, uint16_t buffLen) {
    if (I2C_WRITE(pBuff, buffLen) != HAL_OK)
    {
    	Sleep(10);
    	if(I2C_WRITE(pBuff, buffLen) != HAL_OK)
    	{
    	    printf("tml_Tx error!\r\n");
    		return ERROR;
    	}
    }

	return SUCCESS;
}

static ErrorStatus tml_Rx(uint8_t *pBuff, uint16_t buffLen, uint16_t *pBytesRead) {
    if(I2C_READ(pBuff, 3) == HAL_OK)
    {
    	if ((pBuff[2] + 3) <= buffLen)
    	{
			if (pBuff[2] > 0)
			{
				if(I2C_READ(&pBuff[3], pBuff[2]) == HAL_OK)
				{
					*pBytesRead = pBuff[2] + 3;
				}
				else return ERROR;
			} else
			{
				*pBytesRead = 3;
			}
    	}
		else return ERROR;
   }
    else return ERROR;

	return SUCCESS;
}

static ErrorStatus tml_WaitForRx(uint32_t timeout) {
    uint32_t tick = HAL_GetTick();
    while(!irq_flag){
        if(timeout!=0 && HAL_GetTick() - tick > timeout)
            return ERROR;
    }
    irq_flag = 0;
//	if (xSemaphoreTake(IrqSem, (timeout==0)?(portMAX_DELAY):(portTICK_PERIOD_MS*timeout)) != pdTRUE) return ERROR;
	return SUCCESS;
}

void tml_Connect(void) {
    printf("tml_Connect\r\n");
	tml_Init();
	tml_Reset();
}

void tml_Disconnect(void) {
    printf("tml_Disconnect\r\n");
	tml_DeInit();
}

void tml_Send(uint8_t *pBuffer, uint16_t BufferLen, uint16_t *pBytesSent) {
//    printf("tml_Send\r\n");
	if(tml_Tx(pBuffer, BufferLen) == ERROR) *pBytesSent = 0;
	else *pBytesSent = BufferLen;
}

void tml_Receive(uint8_t *pBuffer, uint16_t BufferLen, uint16_t *pBytes, uint16_t timeout) {
//    printf("tml_Receive\r\n");
	if (tml_WaitForRx(timeout) == ERROR) *pBytes = 0;
	else tml_Rx(pBuffer, BufferLen, pBytes);
}



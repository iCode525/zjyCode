/*
 * Copyright 2016-2018 NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    sensor.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "fsl_uart.h"
//#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"


#include "fsl_i2c.h"
#include "fsl_i2c_edma.h"
#include "fsl_dmamux.h"

#include "fsl_ftm.h"
#include "sreg.h"
#include "sensor.h"

#define DEMO_UART UART3

#define I2C_MASTER_CLK_SRC I2C0_CLK_SRC
#define I2C_MASTER_CLK_FREQ CLOCK_GetFreq(I2C0_CLK_SRC)
#define EXAMPLE_I2C_MASTER_BASEADDR I2C0
#define DMA_REQUEST_SRC kDmaRequestMux0I2C0
#define EXAMPLE_I2C_DMAMUX_BASEADDR DMAMUX0
#define EXAMPLE_I2C_DMA_BASEADDR DMA0
#define I2C_DMA_CHANNEL 0U


uint16_t hmi_value = 0;

uint16_t g_master_txBuff;

i2c_master_edma_handle_t g_m_dma_handle;
edma_handle_t edmaHandle;

i2c_master_config_t masterConfig;
i2c_master_transfer_t masterXfer;

volatile bool g_MasterCompletionFlag = true;

/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

 /******************************************************************************/
volatile uint32_t g_systickCounter;

/*******************************************************************************
 * Code
 ******************************************************************************/

static void i2c_master_callback(I2C_Type *base, i2c_master_edma_handle_t *handle, status_t status, void *userData)
{
    /* Signal transfer success when received success status. */
    if (status == kStatus_Success)
    {
    	hmi_value = g_master_txBuff;
        g_MasterCompletionFlag = true;
    }
}


void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
}

void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while(g_systickCounter != 0U)
    {
    }
}


void delay(unsigned int n)
{
	while (n-- > 0)
		;
}

char sbuf[128] = {0};
/*
 * @brief   Application entry point.
 */

void FTM0_IRQHandler(void)
{
	static int v = 1;
	/* Clear interrupt flag.*/
	FTM_ClearStatusFlags(FTM0, kFTM_TimeOverflowFlag);
	if (g_MasterCompletionFlag) {
		g_MasterCompletionFlag = false;
		I2C_MasterTransferEDMA(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dma_handle, &masterXfer);
	}
	GPIO_WritePinOutput(BOARD_INITPINS_ARM1_USER_LED1_GPIO, BOARD_INITPINS_ARM1_USER_LED1_PIN, v);
	v = !v;
}

int tick_count = 1000u;

static int __slip_read(void *ud, int cmd, uint16_t reg, uint16_t *data, int count)
{
	if (reg == SENSOR_HUMIDITY ) {
		switch (cmd) {
		case SREG_CMD_RD:
			data[0] = hmi_value;
			break;
		case SREG_CMD_WR:
			tick_count = data[0];
			break;
		}
	}
	return 0;
}

static int __slip_write(void *ud, const void *buf, int len)
{
	UART_WriteBlocking(ud, buf, len);
	return len;
}

#define DIR_IN  0
#define DIR_OUT 1
void rs485_direction(int dir)
{
	GPIO_WritePinOutput(BOARD_INITPINS_ARM1_UART3_DRE_GPIO, BOARD_INITPINS_ARM1_UART3_DRE_PIN, dir);
}

int main(void)
{
	uart_config_t config;
	edma_config_t config_edma;
	ftm_config_t ftmInfo;
	uint8_t ch;
	struct sreg *slip;
	int err;

//	g_master_txBuff[0] = 0X76;

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

#if 1
    FTM_GetDefaultConfig(&ftmInfo);

       /* Divide FTM clock by 1 */
       ftmInfo.prescale = kFTM_Prescale_Divide_128;

       /* Initialize FTM module */
       FTM_Init(FTM0, &ftmInfo);

       /*
        * Set timer period.
       */
       FTM_SetTimerPeriod(FTM0, MSEC_TO_COUNT(tick_count, CLOCK_GetFreq(kCLOCK_BusClk)/128));

       FTM_EnableInterrupts(FTM0, kFTM_TimeOverflowInterruptEnable);
#endif

    DMAMUX_Init(EXAMPLE_I2C_DMAMUX_BASEADDR);
    EDMA_GetDefaultConfig(&config_edma);
    EDMA_Init(EXAMPLE_I2C_DMA_BASEADDR, &config_edma);

    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = 115200;
    config.enableTx = true;
    config.enableRx = true;

    UART_Init(DEMO_UART, &config, CLOCK_GetFreq(UART3_CLK_SRC));

  //  UART_WriteBlocking(DEMO_UART, "Hello World\n",12);
   // UART_WriteBlocking(DEMO_UART, g_master_txBuff,1);

    printf("Hello World\n");

    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = 100000U;
    I2C_MasterInit(EXAMPLE_I2C_MASTER_BASEADDR, &masterConfig, I2C_MASTER_CLK_FREQ);

    memset(&g_m_dma_handle, 0, sizeof(g_m_dma_handle));
    memset(&masterXfer, 0, sizeof(masterXfer));

    /* subAddress = 0x01, data = g_master_txBuff - write to slave.
      start + slaveaddress(r)  + data buffer + stop*/
    //masterXfer.slaveAddress = I2C_MASTER_SLAVE_ADDR_7BIT;
    masterXfer.slaveAddress = 0x28;
    //masterXfer.direction = kI2C_Write;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress = 0;
    masterXfer.subaddressSize = 0;
    masterXfer.data = (uint8_t *)&g_master_txBuff;
    //masterXfer.dataSize = I2C_DATA_LENGTH;
    masterXfer.dataSize = 2;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    DMAMUX_SetSource(EXAMPLE_I2C_DMAMUX_BASEADDR, I2C_DMA_CHANNEL, DMA_REQUEST_SRC);
    EDMA_CreateHandle(&edmaHandle, EXAMPLE_I2C_DMA_BASEADDR, I2C_DMA_CHANNEL);
    DMAMUX_EnableChannel(EXAMPLE_I2C_DMAMUX_BASEADDR, I2C_DMA_CHANNEL);

    I2C_MasterCreateEDMAHandle(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dma_handle, i2c_master_callback, NULL, &edmaHandle);
#if 0
    I2C_MasterTransferEDMA(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dma_handle, &masterXfer);


    /*  Wait for transfer completed. */

    while (!g_MasterCompletionFlag)
    {
    }
//    g_MasterCompletionFlag = false;
#endif
   // sprintf(sbuf, "\r\n%04x\r\n", hmi_value & 0xffff);
   // UART_WriteBlocking(DEMO_UART, sbuf, strlen(sbuf));

    EnableIRQ(FTM0_IRQn);
    FTM_StartTimer(FTM0, kFTM_SystemClock);

//    UART_WriteBlocking(DEMO_UART, g_master_txBuff,2);
    //UART_WriteBlocking(DEMO_UART, g_master_txBuff[1],1);

    slip = sreg_alloc(DEMO_UART, __slip_read, __slip_write);

    /* Set systick reload value to generate 1ms interrupt */
    if(SysTick_Config(SystemCoreClock / 1000U))
    {
        while(1)
        {
        }
    }

    /* Enter an infinite loop, just incrementing a counter. */
   // GPIO_WritePinOutput(BOARD_INITPINS_ARM1_UART3_DRE_GPIO,  BOARD_INITPINS_ARM1_UART3_DRE_PIN, 1);
    while(1) {
    	//	if (!g_MasterCompletionFlag) {
    	//		I2C_MasterTransferEDMA(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dma_handle, &masterXfer);
    	//	}
    	    err = UART_ReadBlocking(DEMO_UART, &ch, 1);
    	    if (err == kStatus_Success) {
    	    	 sreg_push(slip, &ch, 1);
    	    	 //UART_WriteBlocking(DEMO_UART, &ch, 1);
    	    } else {
    	    	//printf("err = %d\r\n", err);
    	    	UART_ClearStatusFlags(DEMO_UART,UART_GetStatusFlags(DEMO_UART));
    	    }
    	 //   delay(32768);
 //

//    	    delay(0x300000);
           // UART_WriteBlocking(DEMO_UART, &ch, 1);
//            delay(0x300000);
         //   SysTick_DelayTicks(500u);
           // sprintf(sbuf, "\r\n%04x\r\n", hmi_value & 0xffff);
            //UART_WriteBlocking(DEMO_UART, sbuf, strlen(sbuf));

    }
    return 0 ;
}

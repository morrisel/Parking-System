/*
 * i2c_slave.c
 *
 *  Created on: Jul 21, 2024
 *      Author: go
 */


#include "main.h"
#include "i2c_slave.h"


extern I2C_HandleTypeDef hi2c1;

#define RxSIZE 6
uint8_t RxData[RxSIZE];
uint8_t TxData[RxSIZE] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6}; ////////////////////////////////

int count = 0;

///////////////////////////////////////////////////////////
void I2C_Slave_Init(void) {
    HAL_I2C_EnableListen_IT(&hi2c1);
}
void I2C_Slave_Receive(uint8_t* buffer, uint16_t size) {
    HAL_I2C_Slave_Receive_IT(&hi2c1, buffer, size);
}

void I2C_Slave_Transmit(uint8_t* buffer, uint16_t size) {
    HAL_I2C_Slave_Transmit_IT(&hi2c1, buffer, size);
}
/////////////////////////////////////////////////////////////


void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
	HAL_I2C_EnableListen_IT(hi2c);
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
	//////////////////////////////////////////////////////////////////////////////
    if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
        I2C_Slave_Receive(RxData, RxSIZE);
    } else {
        I2C_Slave_Transmit(TxData, RxSIZE);
    }
    ///////////////////////////////////////////////////////////////////////////////

	// here is the code focus on receiver from the master
	if (TransferDirection == I2C_DIRECTION_TRANSMIT)	// check the transmitting data to the slave
	{
		HAL_I2C_Slave_Sequential_Transmit_IT(hi2c, RxData, RxSIZE, I2C_FIRST_AND_LAST_FRAME);
	}
	else
	{
		Error_Handler();
	}
}


void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	count++;
	HAL_I2C_EnableListen_IT(hi2c); //////////////////////////////////////////////////////////////////
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	HAL_I2C_EnableListen_IT(hi2c);
}

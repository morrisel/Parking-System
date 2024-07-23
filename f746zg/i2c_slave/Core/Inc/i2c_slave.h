/*
 * i2c_slave.h
 *
 *  Created on: Jul 21, 2024
 *      Author: go
 */

#ifndef INC_I2C_SLAVE_H_
#define INC_I2C_SLAVE_H_

////////////////////////////////////////////////////////////
#include "stm32f7xx_hal.h"

void I2C_Slave_Init(void);
void I2C_Slave_Receive(uint8_t* buffer, uint16_t size);
void I2C_Slave_Transmit(uint8_t* buffer, uint16_t size);
////////////////////////////////////////////////////////////


#endif /* INC_I2C_SLAVE_H_ */

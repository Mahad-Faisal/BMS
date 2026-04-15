/*
 * ina226.h
 *
 *  Created on: Mar 30, 2026
 *      Author: mahad
 */

#ifndef INA226_INA226_H_
#define INA226_INA226_H_

#include "bsp_api.h"

void INA226_Init(void);
float INA226_GetBusVoltage(void);
float INA226_GetCurrent(void);
float INA226_GetPower(void);

#endif /* INA226_INA226_H_ */

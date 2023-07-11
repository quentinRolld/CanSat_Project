/*
 * bmp280.h
 *
 *  Created on: 17 juin 2023
 *      Author: quentinrolland
 */

#ifndef INC_BMP280_H_
#define INC_BMP280_H_

#include "Const.h"
#include "types.h"
#include "main.h"

#define ADDRESS_BMP 0x77 <<1  //in I2C communication the last bit of the address is left to define
#define REG_ID 0xD0
#define VAL_ID 0X58
#define REG_RESET 0XE0
#define VALUE_RESET 0XB6
#define ADDRESS_CTRL_MEAS 0XF4
#define MEASURING_CONFIG 0X57
#define TEMP1 0XFA
#define TEMP2 0XFB
#define TEMP3 0XFC
#define PRESS1 0XF7
#define PRESS2 0XF8
#define PRESS3 0XF9

/*! @name Calibration parameter register addresses*/
#define BMP280_DIG_T1_LSB_ADDR               UINT8_C(0x88)
#define BMP280_DIG_T1_MSB_ADDR               UINT8_C(0x89)
#define BMP280_DIG_T2_LSB_ADDR               UINT8_C(0x8A)
#define BMP280_DIG_T2_MSB_ADDR               UINT8_C(0x8B)
#define BMP280_DIG_T3_LSB_ADDR               UINT8_C(0x8C)
#define BMP280_DIG_T3_MSB_ADDR               UINT8_C(0x8D)
#define BMP280_DIG_P1_LSB_ADDR               UINT8_C(0x8E)
#define BMP280_DIG_P1_MSB_ADDR               UINT8_C(0x8F)
#define BMP280_DIG_P2_LSB_ADDR               UINT8_C(0x90)
#define BMP280_DIG_P2_MSB_ADDR               UINT8_C(0x91)
#define BMP280_DIG_P3_LSB_ADDR               UINT8_C(0x92)
#define BMP280_DIG_P3_MSB_ADDR               UINT8_C(0x93)
#define BMP280_DIG_P4_LSB_ADDR               UINT8_C(0x94)
#define BMP280_DIG_P4_MSB_ADDR               UINT8_C(0x95)
#define BMP280_DIG_P5_LSB_ADDR               UINT8_C(0x96)
#define BMP280_DIG_P5_MSB_ADDR               UINT8_C(0x97)
#define BMP280_DIG_P6_LSB_ADDR               UINT8_C(0x98)
#define BMP280_DIG_P6_MSB_ADDR               UINT8_C(0x99)
#define BMP280_DIG_P7_LSB_ADDR               UINT8_C(0x9A)
#define BMP280_DIG_P7_MSB_ADDR               UINT8_C(0x9B)
#define BMP280_DIG_P8_LSB_ADDR               UINT8_C(0x9C)
#define BMP280_DIG_P8_MSB_ADDR               UINT8_C(0x9D)
#define BMP280_DIG_P9_LSB_ADDR               UINT8_C(0x9E)
#define BMP280_DIG_P9_MSB_ADDR               UINT8_C(0x9F)


#define BMP280_CALIB_DATA_SIZE               UINT8_C(24)



extern I2C_HandleTypeDef hi2c1;

extern TypeDataCansat pDataCansat;


int8_t get_calib_param();

int8_t bmp280_get_comp_pres_double(double *pressure, uint32_t uncomp_pres);
int8_t bmp280_get_comp_pres_32bit(uint32_t *comp_pres, uint32_t uncomp_pres);

uint32_t bmp280_conv_raw(uint8_t pPress[5]);
double press_to_altitude(uint32_t pression_utile);

#endif /* INC_BMP280_H_ */

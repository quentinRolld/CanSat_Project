/*
 * bmp280.c
 *
 *  Created on: 17 juin 2023
 *      Author: quentinrolland
 */

/* https://github.com/boschsensortec/BMP280_driver/blob/master/bmp280.c
 * THIS IS A CODE THAT I GET FROM BOSH GITHUB
 */


#include "bmp280.h"
#include "math.h"


uint32_t bmp280_conv_raw(uint8_t pPress[5])
{
	uint32_t PressRaw = 0;
	PressRaw=pPress[0];
	PressRaw=PressRaw<<8;
	PressRaw=PressRaw+pPress[1];
	PressRaw=PressRaw<<4;
	PressRaw=PressRaw+(pPress[2]>>4);

	return PressRaw;

}

int8_t bmp280_get_comp_pres_32bit(uint32_t *comp_pres, uint32_t uncomp_pres)
{
    int32_t var1, var2;
    int8_t rslt;

    //rslt = null_ptr_check(dev);
    //if (rslt == BMP280_OK)
    //{
        var1 = (((int32_t) pDataCansat.DataFromBMP280.calib_param.t_fine) / 2) - (int32_t) 64000;
        var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t) pDataCansat.DataFromBMP280.calib_param.dig_p6);
        var2 = var2 + ((var1 * ((int32_t) pDataCansat.DataFromBMP280.calib_param.dig_p5)) * 2);
        var2 = (var2 / 4) + (((int32_t) pDataCansat.DataFromBMP280.calib_param.dig_p4) * 65536);
        var1 =
            (((pDataCansat.DataFromBMP280.calib_param.dig_p3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8) +
             ((((int32_t) pDataCansat.DataFromBMP280.calib_param.dig_p2) * var1) / 2)) / 262144;
        var1 = ((((32768 + var1)) * ((int32_t) pDataCansat.DataFromBMP280.calib_param.dig_p1)) / 32768);
        *comp_pres = (uint32_t)(((int32_t)(1048576 - uncomp_pres) - (var2 / 4096)) * 3125);

        /* Avoid exception caused by division with zero */
        if (var1 != 0)
        {
            /* Check for overflows against UINT32_MAX/2; if pres is left-shifted by 1 */
            if (*comp_pres < 0x80000000)
            {
                *comp_pres = (*comp_pres << 1) / ((uint32_t) var1);
            }
            else
            {
                *comp_pres = (*comp_pres / (uint32_t) var1) * 2;
            }
            var1 = (((int32_t) pDataCansat.DataFromBMP280.calib_param.dig_p9) * ((int32_t) (((*comp_pres / 8) * (*comp_pres / 8)) / 8192))) /
                   4096;
            var2 = (((int32_t) (*comp_pres / 4)) * ((int32_t) pDataCansat.DataFromBMP280.calib_param.dig_p8)) / 8192;
            *comp_pres = (uint32_t) ((int32_t) *comp_pres + ((var1 + var2 + pDataCansat.DataFromBMP280.calib_param.dig_p7) / 16));
            rslt = BMP280_OK;
        }
        else
        {
            *comp_pres = 0;
            rslt = BMP280_E_32BIT_COMP_PRESS;
        }
    //}

    return rslt;
}


/*!
 * @brief This API is used to get the compensated pressure from
 * uncompensated pressure. This API uses double floating precision.
 */
int8_t bmp280_get_comp_pres_double(double *pressure, uint32_t uncomp_pres)
{
    double var1, var2;
    int8_t rslt;

    rslt = 0;
    if (rslt == BMP280_OK)
    {
        var1 = ((double) pDataCansat.DataFromBMP280.calib_param.t_fine / 2.0) - 64000.0;
        var2 = var1 * var1 * ((double) pDataCansat.DataFromBMP280.calib_param.dig_p6) / 32768.0;
        var2 = var2 + var1 * ((double) pDataCansat.DataFromBMP280.calib_param.dig_p5) * 2.0;
        var2 = (var2 / 4.0) + (((double) pDataCansat.DataFromBMP280.calib_param.dig_p4) * 65536.0);
        var1 = (((double)pDataCansat.DataFromBMP280.calib_param.dig_p3) * var1 * var1 / 524288.0 + ((double)pDataCansat.DataFromBMP280.calib_param.dig_p2) * var1) /
               524288.0;
        var1 = (1.0 + var1 / 32768.0) * ((double) pDataCansat.DataFromBMP280.calib_param.dig_p1);

        *pressure = 1048576.0 - (double)uncomp_pres;
        if (var1 < 0 || var1 > 0)
        {
            *pressure = (*pressure - (var2 / 4096.0)) * 6250.0 / var1;
            var1 = ((double)pDataCansat.DataFromBMP280.calib_param.dig_p9) * (*pressure) * (*pressure) / 2147483648.0;
            var2 = (*pressure) * ((double)pDataCansat.DataFromBMP280.calib_param.dig_p8) / 32768.0;
            *pressure = *pressure + (var1 + var2 + ((double)pDataCansat.DataFromBMP280.calib_param.dig_p7)) / 16.0;
        }
        else
        {
            *pressure = 0;
            rslt = BMP280_E_DOUBLE_COMP_PRESS;
        }
    }

    return rslt;
}


 int8_t get_calib_param()
{
    int8_t rslt = 0;
    uint8_t temp[BMP280_CALIB_DATA_SIZE] = { 0 };


        //rslt = bmp280_get_regs(BMP280_DIG_T1_LSB_ADDR, temp, BMP280_CALIB_DATA_SIZE, dev);
    temp[0] = BMP280_DIG_T1_LSB_ADDR;
    HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_BMP, temp, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, ADDRESS_BMP, temp, 24, HAL_MAX_DELAY);


            pDataCansat.DataFromBMP280.calib_param.dig_t1 =
                (uint16_t) (((uint16_t) temp[BMP280_DIG_T1_MSB_POS] << 8) | ((uint16_t) temp[BMP280_DIG_T1_LSB_POS]));
            pDataCansat.DataFromBMP280.calib_param.dig_t2 =
                (int16_t) (((int16_t) temp[BMP280_DIG_T2_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_T2_LSB_POS]));
            pDataCansat.DataFromBMP280.calib_param.dig_t3 =
                (int16_t) (((int16_t) temp[BMP280_DIG_T3_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_T3_LSB_POS]));
            pDataCansat.DataFromBMP280.calib_param.dig_p1 =
                (uint16_t) (((uint16_t) temp[BMP280_DIG_P1_MSB_POS] << 8) | ((uint16_t) temp[BMP280_DIG_P1_LSB_POS]));
            pDataCansat.DataFromBMP280.calib_param.dig_p2 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P2_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P2_LSB_POS]));
            pDataCansat.DataFromBMP280.calib_param.dig_p3 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P3_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P3_LSB_POS]));
            pDataCansat.DataFromBMP280.calib_param.dig_p4 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P4_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P4_LSB_POS]));
            pDataCansat.DataFromBMP280.calib_param.dig_p5 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P5_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P5_LSB_POS]));
            pDataCansat.DataFromBMP280.calib_param.dig_p6 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P6_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P6_LSB_POS]));
            pDataCansat.DataFromBMP280.calib_param.dig_p7 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P7_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P7_LSB_POS]));
            pDataCansat.DataFromBMP280.calib_param.dig_p8 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P8_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P8_LSB_POS]));
            pDataCansat.DataFromBMP280.calib_param.dig_p9 =
                (int16_t) (((int16_t) temp[BMP280_DIG_P9_MSB_POS] << 8) | ((int16_t) temp[BMP280_DIG_P9_LSB_POS]));



    return rslt;
}


 double press_to_altitude(uint32_t pression_utile)
 {
	 double z = 0;
	 double p0 = 1013;
	 double g = 9.81;
	 double Cp = 1006;
	 double T0 = 25 + 273.15; // Température au sol !!!!! à compléter le jour du lancement !!!!


	 return (z = -2.0*Cp*T0/(7.0*g)*log(pression_utile/p0));
 }

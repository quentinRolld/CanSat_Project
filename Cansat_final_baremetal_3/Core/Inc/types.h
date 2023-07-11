/*
 * types.h
 *
 *  Created on: Oct 19, 2022
 *      Author: quentinrolland
 */

#ifndef INC_TYPES_H_
#define INC_TYPES_H_

#include "main.h"


typedef enum {
	ON,
	OFF,
	BROKEN,
}TypeState;

typedef enum {
	FLOOR,
	TAKE_OFF,
	DROPED,
	LANDED,
}TypeStage;

typedef enum
    {
        PASS,
        FAIL,
        PENDING,
        POLLING,
        TRIGGERED,
        //ABORTED,
        //UNDEFINED
    } OUTCOME;


/********* GPS ************/

typedef struct TypeDataGPS{

	double latitude_Cansat;
	double longitude_Cansat;
	double altitude_Cansat;
	double nombre_de_satellites_en_vue;
	double latitude_Target;
	double longitude_Target;

}TypeDataGPS;




/********** IMU ***********/


typedef struct TypeParametersIMU{

	TypeState IMU_State;

} TypeParametersIMU;


typedef struct TypeAccelerometer{

	uint8_t Brute_Accel_Xout_H;
	uint8_t Brute_Accel_Xout_L;
	uint8_t Brute_Accel_Yout_H;
	uint8_t Brute_Accel_Yout_L;
	uint8_t Brute_Accel_Zout_H;
	uint8_t Brute_Accel_Zout_L;
	uint8_t Accel_Sensitivity;
	uint16_t Accel_X;
	uint16_t Accel_Y;
	uint16_t Accel_Z;

	int x;
	int y;
	int z;

} TypeAccelerometer;


typedef struct TypeMagnetometer{

	double mag_raw[3];
	double offset[3];
	double coeff[3];
	double mag_calibrated[3];
	double degree_angle;

} TypeMagnetometer;


typedef struct TypeGyrometer{

	uint8_t Brute_Gyro_Xout_H;
	uint8_t Brute_Gyro_Xout_L;
	uint8_t Brute_Gyro_Yout_H;
	uint8_t Brute_Gyro_Yout_L;
	uint8_t Brute_Gyro_Zout_H;
	uint8_t Brute_Gyro_Zout_L;
	uint8_t Gyro_Sensitivity;
	uint16_t Gyro_X;
	uint16_t Gyro_Y;
	uint16_t Gyro_Z;

} TypeGyrometer;


typedef struct TypeDataIMU{

	uint8_t IMU_Device_ID;
	TypeParametersIMU Parameters;
	TypeAccelerometer AccelerometerData;
	TypeGyrometer GyrometerData;
	TypeMagnetometer MagnetometerData;

} TypeDataIMU;

/********** eCompass **********/

typedef struct TypeDataeCompass{

	double Delta_theta;

} TypeDataeCompass;

/********** BMP280 ***********/

typedef struct Temperature
    {
        uint8_t T_Brute_MSB;
        uint8_t T_Brute_LSB;
        uint8_t T_Brute_XLSB;
        uint32_t T_Brute;
        uint32_t Temp_finale;
    } Temperature;

typedef struct Pression
    {
        uint8_t P_Brute_MSB;
        uint8_t P_Brute_LSB;
        uint8_t P_Brute_XLSB;
        uint32_t P_Brute;
        uint32_t Pression_finale;
    } Pression;



typedef struct bmp280_calib_param
	{
	    uint16_t dig_t1;
	    int16_t dig_t2;
	    int16_t dig_t3;
	    uint16_t dig_p1;
	    int16_t dig_p2;
	    int16_t dig_p3;
	    int16_t dig_p4;
	    int16_t dig_p5;
	    int16_t dig_p6;
	    int16_t dig_p7;
	    int16_t dig_p8;
	    int16_t dig_p9;
	    int32_t t_fine;

	}bmp280_calib_param;

typedef struct bmp280_calib_param_raw
	{
		uint8_t MSB_T1;
		uint8_t LSB_T1;
		uint8_t MSB_T2;
		uint8_t LSB_T2;
		uint8_t MSB_T3;
		uint8_t LSB_T3;
		uint8_t MSB_P1;
		uint8_t LSB_P1;
		uint8_t MSB_P2;
		uint8_t LSB_P2;
		uint8_t MSB_P3;
		uint8_t LSB_P3;
		uint8_t MSB_P4;
		uint8_t LSB_P4;
		uint8_t MSB_P5;
		uint8_t LSB_P5;
		uint8_t MSB_P6;
		uint8_t LSB_P6;
		uint8_t MSB_P7;
		uint8_t LSB_P7;
		uint8_t MSB_P8;
		uint8_t LSB_P8;
		uint8_t MSB_P9;
		uint8_t LSB_P9;

	}bmp280_calib_param_raw;


typedef struct TypeDataBMP280
    {
        Temperature Temp;
        Pression Press;
        bmp280_calib_param calib_param;
        bmp280_calib_param_raw calib_param_raw;

    } TypeDataBMP280;

/********** General **********/

typedef struct TypeDataCansat{

	TypeDataGPS GPS;
	TypeDataIMU IMU;
	TypeDataeCompass eCompass;
	TypeDataBMP280 DataFromBMP280;

} TypeDataCansat;



#endif /* INC_TYPES_H_ */

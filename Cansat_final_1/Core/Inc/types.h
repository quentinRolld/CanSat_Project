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

/********** General **********/

typedef struct TypeDataCansat{

	TypeDataGPS GPS;
	TypeDataIMU IMU;

} TypeDataCansat;



#endif /* INC_TYPES_H_ */

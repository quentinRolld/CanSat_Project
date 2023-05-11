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
}TypeState;


/********* GPS ************/

typedef struct TypeDataGPS{

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

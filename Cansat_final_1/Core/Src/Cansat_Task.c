/*
 * Cansat_Task.c
 *
 *  Created on: 14 mai 2023
 *      Author: quentinrolland
 */


#include "Const.h"
#include "stdio.h"
#include <math.h>
#include "IMU_10DOF.h"
#include "data.h"
#include "types.h"
#include "servo.h"
#include "gps.h"
#include "compass.h"
#include "Cansat_Task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include "i2c.h"


extern TypeDataCansat pDataCansat;

/********* PV GPS **********/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern char uart_gps_rx[1];
extern char uart_pc_tx[1];

extern int it_rx_gps;

/******* Task Handler ********/
extern TaskHandle_t pxGPS_Handler;
extern TaskHandle_t pxDrop_detection;
extern TaskHandle_t pxLancement_Cansat;
extern TaskHandle_t pxMesure_M;
extern TaskHandle_t pxeCompass;
/****************************/



void Task_lancement_Cansat(){

	for(;;){

		xTaskCreate(Task_Mesure_M, "mesure champ magnetique", 500, NULL, osPriorityHigh, &pxMesure_M);
		//xTaskCreate(Task_Mesure_AetG, "mesure acceleration lineaire et angulaire", 500, NULL, osPriorityAboveNormal, pxCreatedTask);
		xTaskCreate(Task_eCompass, "eCompass", 500, NULL, osPriorityAboveNormal, &pxeCompass);

		vTaskDelete(NULL);
	}
}


void Task_Mesure_M(){

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 200;

  //Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;){

		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		printf("mesure champ magnetique \r\n");
		Measure_M(&hi2c1, pDataCansat.IMU.MagnetometerData.mag_raw, pDataCansat.IMU.MagnetometerData.offset, pDataCansat.IMU.MagnetometerData.coeff);
	}
}

void Task_Mesure_AetG(){

	for(;;){


	}
}

/* On démarre cette tâche dès la mise sous tension du Cansat car le GPS met un peu de temps à se lancer
 * Par la suite, elle tourne en tâche de fond toutes les secondes.
 */
void Task_GPS_data_reading(){

	int i = 0;
	int gps_data_ready_flag = 0;
	HAL_UART_Receive_IT(&huart1, (uint8_t*)&uart_gps_rx, 1);

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 20;

  //Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;){

		//ulTaskNotifyTake(pdTRUE, 500);

		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		GPS_data_reading(pDataCansat, i, gps_data_ready_flag);

		if(gps_data_ready_flag){
			printf("GPS read ***************** \r\n");
			printf("latitude : %lf N, longitude : %lf E, altitude : %lf m \r\n", pDataCansat.GPS.latitude_Cansat, pDataCansat.GPS.longitude_Cansat, pDataCansat.GPS.altitude_Cansat);
		}
	}
}



void Task_eCompass(){

	float Cansat_theta = 0;
	double Delta_theta = 0;

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1000;

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;){

		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		//1. Récupération des données de champ magnétique
		//

		//1 bis. compensation du tengage et du roulis pour le calcul du champ magnétique
		// ---> Mesure gyro
		// ---> Messure accéléro
		// ---> calcul

		//3. Conversion du champ magnétique en degré

		Cansat_theta = magnetic_field_to_degree(pDataCansat.IMU.MagnetometerData.mag_raw);
		pDataCansat.IMU.MagnetometerData.degree_angle = Cansat_theta;

		printf("Cansat theta computed * * * * * * * * * \r\n");

		//4. Calcul du Delta theta

		Delta_theta = Delta_theta_calculation(pDataCansat);

		printf("Delta theta computed * * * * * * * * * * \r\n");

		//5. Rotation des servos en conséquence

		choice_direction_intensity(Delta_theta);
	}

}

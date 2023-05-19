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
#include "main.h"
#include "compass.h"


extern TypeDataCansat pDataCansat;

/********* PV GPS **********/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern char uart_gps_rx[1];
extern char uart_pc_tx[1];


void Task_lancement_Cansat(){

	for(;;){



	}
}


void Task_Mesure_M(){

	for(;;){

		//Measure_M(&hi2c1, double* mag, double* offset, double* coeff);
	}
}

void Task_Mesure_AetG(){

	for(;;){


	}
}

void Task_GPS_data_reading(){

	for(;;){

		HAL_UART_Receive_IT(&huart1, uart_gps_rx, 1);
		GPS_data_reading(pDataCansat);

	}
}



void Task_eCompass(){


	for(;;){

		//1. Récupération des données de champ magnétique
		//

		//1 bis. compensation du tengage et du roulis pour le calcul du champ magnétique
		// ---> Mesure gyro
		// ---> Messure accéléro
		// ---> calcul

		//2. Récupération des données GPS

		//3. Conversion du champ magnétique en degré
		//magnetic_field_to_degree(double* mag_calibrated);

		//4. Calcul du Delta theta
		//Delta_theta_calculation(TypeDataCansat DataCansat);

		//5. Rotation des servos en conséquence
		//choice_direction_intensity(unsigned int delta_teta);
	}

}

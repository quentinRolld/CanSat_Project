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




void Task_eCompass(){


	for(;;){

		//1. Récupération des données de champ magnétique
		// Measure_M(&hi2c1, double* mag, double* offset, double* coeff);

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

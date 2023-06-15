/*
 * compass.c
 *
 *  Created on: May 10, 2023
 *      Author: quentinrolland
 */

#include "main.h"
#include "types.h"
#include <math.h>



float magnetic_field_to_degree(double* mag_calibrated)
{
	float heading = atan2(-mag_calibrated[1], mag_calibrated[0]); // atan2(y,x)


	  //float declinationAngle = 0.22; // Angle de déclinaison magnétique pour Paris
	  float declinationAngle = 0.21; // Angle de déclinaison magnétique pour Barcelone
	  heading += declinationAngle;   // site pour connaitre la déclinaison : https://www.ngdc.noaa.gov/geomag/calculators/magcalc


	  float headingDegrees = heading * 180 / M_PI; // Conversion en degrés

	  if(headingDegrees<=0){
			  headingDegrees+=360;
		  }



	  return headingDegrees;
}

double Delta_theta_calculation(TypeDataCansat DataCansat){

	double theta_target = 0;
	double latitude_vector = DataCansat.GPS.latitude_Target - DataCansat.GPS.latitude_Cansat;
	double longitude_vector = DataCansat.GPS.longitude_Target - DataCansat.GPS.longitude_Cansat;

	theta_target = atan2(longitude_vector, latitude_vector);
	theta_target = theta_target * 180/M_PI;  // Conversion en degrés
	if(theta_target<=0)
	{
		theta_target+=360;
	}

	double Delta_theta = fabs(theta_target - DataCansat.IMU.MagnetometerData.degree_angle);

	return Delta_theta;
}

/*


void Tilt_compensation(){

	// Calculate pitch and roll, in the range (-pi,pi)
			pitch = atan2((double)-acc_x, sqrt((long)acc_z*(long)acc_z + (long)acc_y*(long)acc_y));
			roll = atan2((double)acc_y, sqrt((long)acc_z*(long)acc_z  + (long)acc_x*(long)acc_x));

			 // Calculate Azimuth:
			 // Magnetic horizontal components, after compensating for Roll(r) and Pitch(p) are:
			 // X_h = X*cos(p) + Y*sin(r)*sin(p) + Z*cos(r)*sin(p)
			 // Y_h = Y*cos(r) - Z*sin(r)
			 // Azimuth = arcTan(Y_h/X_h)

			X_h = (double)mag_x*cos(pitch) + (double)mag_y*sin(roll)*sin(pitch) + (double)mag_z*cos(roll)*sin(pitch);
			Y_h = (double)mag_y*cos(roll) - (double)mag_z*sin(roll);
			azimuth = atan2(Y_h, X_h);
			if(azimuth < 0) {	// Convert Azimuth in the range (0, 2pi)
				azimuth = 2*M_PI + azimuth;
			}
}

*/

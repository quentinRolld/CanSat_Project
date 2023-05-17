/*
 * compas.c
 *
 *  Created on: May 10, 2023
 *      Author: quentinrolland
 */

#include "main.h"
#include "types.h"
#include <math.h>

float magnetic_field_to_degree(double* mag_calibrated)
{
	float heading = atan2(mag_calibrated[1], mag_calibrated[0]); // atan2(y,x)

	  if(heading < 0) {
	    heading += 2 * M_PI;
	  }
	  if(heading > 2 * M_PI) {
	    heading -= 2 * M_PI;
	  }

	  //float declinationAngle = 0.22; // Angle de déclinaison magnétique pour Paris
	  float declinationAngle = 0.21; // Angle de déclinaison magnétique pour Barcelone
	  heading += declinationAngle;   // site pour connaitre la déclinaison : https://www.ngdc.noaa.gov/geomag/calculators/magcalc

	  if(heading < 0) {
	    heading += 2 * M_PI;
	  }
	  if(heading > 2 * M_PI) {
	    heading -= 2 * M_PI;
	  }

	  float headingDegrees = heading * 180 / M_PI; // Conversion en degrés

	  return headingDegrees;
}

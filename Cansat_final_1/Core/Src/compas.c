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
	float heading = atan2(mag_calibrated[1], mag_calibrated[0]);

	  if(heading < 0) {
	    heading += 2 * M_PI;
	  }
	  if(heading > 2 * M_PI) {
	    heading -= 2 * M_PI;
	  }

	  float declinationAngle = 0.22; // Angle de déclinaison magnétique pour Paris
	  heading += declinationAngle;

	  if(heading < 0) {
	    heading += 2 * M_PI;
	  }
	  if(heading > 2 * M_PI) {
	    heading -= 2 * M_PI;
	  }

	  float headingDegrees = heading * 180 / M_PI; // Conversion en degrés

	  return headingDegrees;
}

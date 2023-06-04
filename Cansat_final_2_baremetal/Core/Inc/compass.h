/*
 * compas.h
 *
 *  Created on: May 10, 2023
 *      Author: quentinrolland
 */

#ifndef INC_COMPASS_H_
#define INC_COMPASS_H_


#include "types.h"

float magnetic_field_to_degree(double* mag_calibrated);
double Delta_theta_calculation(TypeDataCansat DataCansat);

#endif /* INC_COMPASS_H_ */

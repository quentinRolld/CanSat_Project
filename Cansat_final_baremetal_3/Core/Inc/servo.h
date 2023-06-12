/*
 * servo.h
 *
 *  Created on: 11 mai 2023
 *      Author: quentinrolland
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_


#include "main.h"
#define minAngle 70
#define maxAngle 431
void servoSetAngleLeft(unsigned int angle);
void servoSetAngleRight(unsigned int angle);
void servoSetPositionLeft (int position);
void servoSetPositionRight (int position);
void choice_direction_intensity(unsigned int delta_teta);
void declenchement_structure_gonflable();



#endif /* INC_SERVO_H_ */

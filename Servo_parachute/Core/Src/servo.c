/*
 * servo.c
 *
 *  Created on: Oct 28, 2022
 *      Author: cheye
 */
#include "servo.h"

#define POSITION_0_LEFT 80
#define POSITION_1_LEFT 110
#define POSITION_2_LEFT 170
#define POSITION_3_LEFT 250

#define POSITION_0_RIGHT 220
#define POSITION_1_RIGHT 190
#define POSITION_2_RIGHT 130
#define POSITION_3_RIGHT 60

#define epsilon 5  //erreur acceptable
#define teta_low 30  //valeur en degrés
#define teta_high 100 //valeur en degrés



/*void servoSetAngleLeft(unsigned int angle){
	if(angle>180) angle = 180;
	int pwm = (angle*((maxAngle-minAngle)/180) + minAngle);
	  TIM1->CCR1 = pwm;
}


void servoSetAngleRight(unsigned int angle){
	if(angle>180) angle = 180;
	int pwm = (angle*((maxAngle-minAngle)/180) + minAngle);
	  TIM1->CCR1 = pwm;
}

*/


/**
 	 * @brief Positionnement du servo gauche
 	 * @Note Cette fonction permet de placer le servo comme on le souhaite avec 4 positions différentes
	 * @param int position : 0,1,2 ou 3
	 * @retval None
	 */
void servoSetPositionLeft (int position){
	if (position == 0){
		TIM1->CCR1 = POSITION_0_LEFT;
	}
	else if (position == 1){
			TIM1->CCR1 = POSITION_1_LEFT;
	}
	else if (position == 2){
			TIM1->CCR1 = POSITION_2_LEFT;
	}
	else if (position == 3){
			TIM1->CCR1 = POSITION_3_LEFT;
	}
	else {
		printf("Vous ne devez mettre en entree que 0,1,2 ou 3 \r\n");
	}
}

/**
 	 * @brief Positionnement du servo droit
 	 * @Note Cette fonction permet de placer le servo comme on le souhaite avec 4 positions différentes
	 * @param int position : 0,1,2 ou 3
	 * @retval None
	 */
void servoSetPositionRight (int position){
	if (position == 0){
		TIM1->CCR2 = POSITION_0_RIGHT;
	}
	else if (position == 1){
			TIM1->CCR2 = POSITION_1_RIGHT;
	}
	else if (position == 2){
			TIM1->CCR2 = POSITION_2_RIGHT;
	}
	else if (position == 3){
			TIM1->CCR2 = POSITION_3_RIGHT;
	}
	else {
		printf("Vous ne devez mettre en entree 0,1,2 ou 3 \r\n");
	}
}



/**
 	 * @brief Choix de la direction et de l'intensité de la rotation
 	 * @Note Cette fonction permet de choisir la direction dans laquelle le cansat doit aller ainsi que son intensité grâce à une différence d'angle
	 * @param int delta_teta : difference d'angle comprise entre 0 et 360 degrés
	 * @retval None
	 */

void choice_direction_intensity(unsigned int delta_teta){
	if ((delta_teta<epsilon) || (delta_teta>360-epsilon)){
		servoSetPositionLeft(0);
		servoSetPositionRight(0);
	}
	else if (delta_teta<teta_low){
		servoSetPositionLeft(0);
		servoSetPositionRight(1);
	}
	else if (delta_teta>360-teta_low){
		servoSetPositionLeft(1);
		servoSetPositionRight(0);
	}

	else if (delta_teta<teta_high){
		servoSetPositionLeft(0);
		servoSetPositionRight(2);
	}

	else if (delta_teta>360-teta_high){
		servoSetPositionLeft(2);
		servoSetPositionRight(0);
	}

	else if (delta_teta<=180){
		servoSetPositionLeft(0);
		servoSetPositionRight(3);
	}

	else if (delta_teta>180){
		servoSetPositionLeft(3);
		servoSetPositionRight(0);
	}
	else {
		printf("cas non traité \r\n");
	}

}








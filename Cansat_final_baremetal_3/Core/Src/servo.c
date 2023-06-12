/*
 * servo.c
 *
 *  Created on: Oct 28, 2022
 *      Author: cheye
 */
#include "main.h"
#include "servo.h"
#include "Const.h"
#include "stdio.h"
#include <math.h>
#include "IMU_10DOF.h"
#include "data.h"
#include "types.h"

/************* external variable ****************/

extern I2C_HandleTypeDef hi2c1;

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

extern UART_HandleTypeDef huart2;


/********** Define MISSION PRINCIPALE ***********/

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


/********** Define MISSION SECONDAIRE ***********/

#define SERVO_MIN_PULSE_WIDTH 1000
#define SERVO_MAX_PULSE_WIDTH 2000

/********** fonctions MISSION PRINCIPALE ***********/


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
		//TIM2->CCR1 = POSITION_0_LEFT;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 400);
	}
	else if (position == 1){
			//TIM2->CCR1 = POSITION_1_LEFT;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 600);
	}
	else if (position == 2){
			//TIM2->CCR1 = POSITION_2_LEFT;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1000);
	}
	else if (position == 3){
			//TIM2->CCR1 = POSITION_3_LEFT;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1500);
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
		//TIM2->CCR2 = POSITION_0_RIGHT;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 400);
	}
	else if (position == 1){
			//TIM2->CCR2 = POSITION_1_RIGHT;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 600);              //   Attention, Channel 1 --> servo gauche
	}															 	   	//   			Channel 2 --> servo droit
	else if (position == 2){
			//TIM2->CCR2 = POSITION_2_RIGHT;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1000);
	}
	else if (position == 3){
			//TIM2->CCR2 = POSITION_3_RIGHT;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1500);
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


/********** fonctions MISSION SECONDAIRE ***********/


void declenchement_structure_gonflable(){

	  int x;
	  TIM4->CCR1 = 1000;

	  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

	  for(x=1000;x<2500;x=x+1){
	  			  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, x);

	  		  }

}














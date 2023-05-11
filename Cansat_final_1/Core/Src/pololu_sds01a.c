/*
 * pololu_sds01a.c
 *
 *  Created on: May 10, 2023
 *      Author: quentinrolland
 */



#include "main.h"



/**
 	 * @brief Drop detection
 	 * @Note Cette fonction permet de detecter le largage du Cansat depuis le tube de largage. Lorsque le capteur ne detecte plus le tube, il passe gpio_value à 1
	 * @param Drop_flag Drapeau qui permet d'indiquer que le Cansat a été largué, ce qui permet de lancer le démarrage des missions.
	 * @retval None
	 */

void Drop_detection(int Drop_flag)
{
	uint16_t gpio_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);  // prend la valeur 1 lorsque pas d'obstacles
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	 // prend la valeur 0 lorsque detection d'un obstacle
	if(gpio_value == 1)
	{
		Drop_flag = 1;
	}
}

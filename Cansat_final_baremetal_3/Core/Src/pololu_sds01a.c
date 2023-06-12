/*
 * pololu_sds01a.c
 *
 *  Created on: May 10, 2023
 *      Author: quentinrolland
 */



#include "main.h"

#include "stdlib.h"
#include <stdio.h>



/**
 	 * @brief Drop detection
 	 * @Note Cette fonction permet de detecter le largage du Cansat depuis le tube de largage.
 	 * Lorsque le capteur ne detecte plus le tube, il passe gpio_value à 1. La fonction est appelée
 	 * périodiquement toutes les 500ms à partir du moment où le bluebutton a été pressé et jusqu'à
 	 * ce que le capteur ne détecte plus le tube de largage. Une notification est envoyée à la tâche
 	 * Task_lancement_Cansat, ce qui permet de lancer le démarrage des missions.
	 * @param Drop_flag Drapeau qui permet d'indiquer que le Cansat a été largué
	 * @retval None
	 */

int Drop_detection()
{
		int flag = 0;
		uint16_t gpio_value = 0;

		gpio_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);  // prend la valeur 1 lorsque pas d'obstacles
															// prend la valeur 0 lorsque detection d'un obstacle
		//printf("valeur du capteur IR : %d \r\n", gpio_value);

		if(gpio_value == 1)
		{
			flag = 1;
			printf("*** \r\n *** \r\n *** \r\n *** \r\n *** \r\n *** \r\n largage effectue \r\n *** \r\n *** \r\n *** \r\n *** \r\n *** \r\n *** \r\n");
		}

		return flag;
}

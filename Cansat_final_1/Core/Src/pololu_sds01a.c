/*
 * pololu_sds01a.c
 *
 *  Created on: May 10, 2023
 *      Author: quentinrolland
 */



#include "main.h"
#include "cmsis_os.h"
#include "Cansat_Task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include <stdio.h>

extern int Drop_flag;
extern TaskHandle_t pxDrop_detection;
extern TaskHandle_t pxLancement_Cansat;

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

void Task_Drop_detection(void * pvParameters)
{
	uint16_t gpio_value = 0;
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500;

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{

		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		gpio_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);  // prend la valeur 1 lorsque pas d'obstacles
															// prend la valeur 0 lorsque detection d'un obstacle
		printf("valeur du capteur IR : %d \r\n", gpio_value);

		if(gpio_value == 1)
		{
			Drop_flag = 1;
			printf("largage effectue \r\n");
			xTaskCreate(Task_lancement_Cansat, "Lancement du Cansat", 500, NULL, osPriorityHigh, &pxLancement_Cansat);
			vTaskDelete(NULL);
		}

	}
}

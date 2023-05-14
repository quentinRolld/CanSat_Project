/*
 * pololu_sds01a.c
 *
 *  Created on: May 10, 2023
 *      Author: quentinrolland
 */



#include "main.h"
#include "cmsis_os.h"

extern int Drop_flag;
extern TaskHandle_t pxDrop_detection;

/**
 	 * @brief Drop detection
 	 * @Note Cette fonction permet de detecter le largage du Cansat depuis le tube de largage.
 	 * Lorsque le capteur ne detecte plus le tube, il passe gpio_value à 1. La fonction est appelée
 	 * périodiquement toutes les 500ms à partir du moment où le bluebutton a été pressé et jusqu'à
 	 * ce que le capteur ne détecte plus le tube de largage.
	 * @param Drop_flag Drapeau qui permet d'indiquer que le Cansat a été largué, ce qui permet de
	 * lancer le démarrage des missions.
	 * @retval None
	 */

void Task_Drop_detection(void * pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = portTICK_PERIOD_MS/500;

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{

		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		uint16_t gpio_value = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);  // prend la valeur 1 lorsque pas d'obstacles
																	// prend la valeur 0 lorsque detection d'un obstacle
		if(gpio_value == 1)
		{
			Drop_flag = 1;
			vTaskDelete(pxDrop_detection);
		}

	}
}

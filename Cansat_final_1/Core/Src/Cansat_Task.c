/*
 * Cansat_Task.c
 *
 *  Created on: 14 mai 2023
 *      Author: quentinrolland
 */


#include "Const.h"
#include "stdio.h"
#include <math.h>
#include "IMU_10DOF.h"
#include "data.h"
#include "types.h"
#include "servo.h"
#include "gps.h"
#include "compass.h"
#include "Cansat_Task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include "i2c.h"
#include "stream_buffer.h"
#include "string.h"

extern TypeDataCansat pDataCansat;

/********* PV GPS **********/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern char uart_gps_rx[GPS_TRAME_SIZE];
extern char uart_pc_tx[GPS_TRAME_SIZE];

extern int it_rx_gps;

static const char * pcStringToReceive = "$GNGGA.191048.000.4122.93680,N.00208.49967.E.1.05,8.0.4.4.M.0.0.M..*74";


/******* Task Handler ********/
extern TaskHandle_t pxGPS_Handler;
extern TaskHandle_t pxDrop_detection;
extern TaskHandle_t pxLancement_Cansat;
extern TaskHandle_t pxMesure_M;
extern TaskHandle_t pxeCompass;
/****************************/

/********* StreamBuffer *********/
extern StreamBufferHandle_t xGPS_StreamBuffer;



void Task_lancement_Cansat(){

	for(;;){

		xTaskCreate(Task_Mesure_M, "mesure champ magnetique", 500, NULL, osPriorityHigh, &pxMesure_M);
		//xTaskCreate(Task_Mesure_AetG, "mesure acceleration lineaire et angulaire", 500, NULL, osPriorityAboveNormal, pxCreatedTask);
		xTaskCreate(Task_eCompass, "eCompass", 500, NULL, osPriorityAboveNormal, &pxeCompass);

		vTaskDelete(NULL);
	}
}


void Task_Mesure_M(){

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 200;

  //Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;){

		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		printf("mesure champ magnetique \r\n");
		Measure_M(&hi2c1, pDataCansat.IMU.MagnetometerData.mag_raw, pDataCansat.IMU.MagnetometerData.offset, pDataCansat.IMU.MagnetometerData.coeff);
	}
}

void Task_Mesure_AetG(){

	for(;;){


	}
}

void prvGPS_ReceivingTask( void *pvParameters )
{

	char ReceivingTask_rx[GPS_TRAME_SIZE];
	char DataGPS[GPS_TRAME_SIZE];
	int i = 0;
	BaseType_t xDemoStatus = pdPASS;

/*
    char cRxBuffer[ 20 ];
    BaseType_t xNextByte = 0;

	// Remove warning about unused parameters.
	( void ) pvParameters;

	// Make sure the string will fit in the Rx buffer, including the NULL terminator.
	configASSERT( sizeof( cRxBuffer ) > strlen( pcStringToReceive ) );

	// Make sure the stream buffer has been created.
	configASSERT( xStreamBuffer != NULL );

	// Start with the Rx buffer in a known state.
	memset( cRxBuffer, 0x00, sizeof( cRxBuffer ) );
*/
	 HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart_gps_rx, 80);

	for( ;; )
	{
		/* Keep receiving characters until the end of the string is received.
		Note:  An infinite block time is used to simplify the example.  Infinite
		block times are not recommended in production code as they do not allow
		for error recovery. */
		xStreamBufferReceive( /* The stream buffer data is being received from. */
							  xGPS_StreamBuffer,
							  /* Where to place received data. */
							  &ReceivingTask_rx,
							  /* The number of bytes to receive. */
							  80,
							  /* The time to wait for the next data if the buffer
							  is empty. */
							  portMAX_DELAY );



		if( i == 0 )
		{
			if( ReceivingTask_rx[i] == '$');
			i++;
		}

		/*
		if(i==999)
		{
			i=0;
			printf("Data GPS received --------------- GPS --------------- GPS ---- \r\n");
		}
		*/

		else
				{
					// Receiving characters while looking for the end of the GNSGGA string,
					// which is an '\n'.
					if( ReceivingTask_rx[ i ] == '\n' )
					{
						// The string has now been received.  Check its validity.
						if( strcmp( ReceivingTask_rx, pcStringToReceive ) != 0 )
						{
							xDemoStatus = pdFAIL;

						}

						// Return to start looking for the beginning of the string again.
						memset( ReceivingTask_rx, 0x00, sizeof( DataGPS ) );
						i = 0;

						printf("Data GPS received --------------- GPS --------------- GPS ---- \r\n");


					}
					else
					{
						// Receive the next character the next time around, while
						// continuing to look for the end of the string.
						i++;

						//configASSERT( i < sizeof( DataGPS ) );
					}
				 }

		HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart_gps_rx, 80);

/*
		// If xNextByte is 0 then this task is looking for the start of the string, which is 'H'.
		if( xNextByte == 0 )
		{
			if( cRxBuffer[ xNextByte ] == 'H' )
			{
				// The start of the string has been found.  Now receive
				//characters until the end of the string is found.
				xNextByte++;
			}
		}
		else
		{
			// Receiving characters while looking for the end of the string,
			// which is an 'S'.
			if( cRxBuffer[ xNextByte ] == 'S' )
			{
				// The string has now been received.  Check its validity.
				if( strcmp( cRxBuffer, pcStringToReceive ) != 0 )
				{
					xDemoStatus = pdFAIL;
				}

				// Return to start looking for the beginning of the string again.
				memset( cRxBuffer, 0x00, sizeof( cRxBuffer ) );
				xNextByte = 0;

				// Increment the cycle count as an indication to the check task
				// that this demo is still running.
				if( xDemoStatus == pdPASS )
				{
					ulCycleCount++;
				}
			}
			else
			{
				// Receive the next character the next time around, while
				// continuing to look for the end of the string.
				xNextByte++;

				configASSERT( xNextByte < sizeof( cRxBuffer ) );
			}
		}

	*/
	}
}

/* On démarre cette tâche dès la mise sous tension du Cansat car le GPS met un peu de temps à se lancer
 * Par la suite, elle tourne en tâche de fond toutes les secondes.
 */
void Task_GPS_data_reading(){

	int i = 0;
	int gps_data_ready_flag = 0;
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)&uart_gps_rx, 1);

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 20;

  //Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;){

		//ulTaskNotifyTake(pdTRUE, 500);

		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		GPS_data_reading(pDataCansat, i, gps_data_ready_flag);

		if(gps_data_ready_flag){
			printf("GPS read ***************** \r\n");
			printf("latitude : %lf N, longitude : %lf E, altitude : %lf m \r\n", pDataCansat.GPS.latitude_Cansat, pDataCansat.GPS.longitude_Cansat, pDataCansat.GPS.altitude_Cansat);
		}
	}
}




void Task_eCompass(){

	float Cansat_theta = 0;
	double Delta_theta = 0;

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1000;

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;){

		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		//1. Récupération des données de champ magnétique
		//

		//1 bis. compensation du tengage et du roulis pour le calcul du champ magnétique
		// ---> Mesure gyro
		// ---> Messure accéléro
		// ---> calcul

		//3. Conversion du champ magnétique en degré

		Cansat_theta = magnetic_field_to_degree(pDataCansat.IMU.MagnetometerData.mag_raw);
		pDataCansat.IMU.MagnetometerData.degree_angle = Cansat_theta;

		printf("Cansat theta computed * * * * * * * * * \r\n");

		//4. Calcul du Delta theta

		Delta_theta = Delta_theta_calculation(pDataCansat);

		printf("Delta theta computed * * * * * * * * * * \r\n");

		//5. Rotation des servos en conséquence

		choice_direction_intensity(Delta_theta);
		printf("-----------------------------> Rotation en cours ! \r\n");
	}

}

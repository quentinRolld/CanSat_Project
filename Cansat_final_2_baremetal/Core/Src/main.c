/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "Const.h"
#include "stdio.h"
#include <math.h>
#include "IMU_10DOF.h"
#include "data.h"
#include "types.h"
#include "servo.h"
#include "gps.h"
#include "pololu_sds01a.h"
#include "compass.h"
#include "stdlib.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


/******* Data structure *******/
TypeDataCansat pDataCansat;
/******************************/


/******* GPS ********/

/*
char uart_gps_rx[1];
char uart_pc_tx[1];
char coordonnees[GPS_TRAME_SIZE];

uint8_t it_rx_gps=0;
int first_data = 0;
int data_rdy = 0;
int i = 0;
*/

char uart_gps_rx[1];
char uart_pc_tx[1];
uint8_t it_rx_gps=0;

char coordonnees[GPS_TRAME_SIZE];
char longitude_data[10];
char latitude_data[10];
char altitude_data[5];
char position[49];
char lat2[2];
char lat7[7];
char long3[3];
char long7[7];
double lata;
double latb;
double latitude;
double longitude;
double altitude;

char satellites[2];

int i = 0;
int k = 0;

int first_data = 0;
int data_rdy = 0;
char uart_tx_buffer[128];
/********************/


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int __io_putchar(int ch) {
HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
return ch;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){ // fonction de callback de l'UART


	if(huart == &huart1){

		it_rx_gps = 1;
		HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart_gps_rx, 1);

	}

}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	int gps_state = 0;


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */


  printf("------------------------\r\n");
  printf("------------------------\r\n");
  printf("------------------------\r\n");
  printf("------------------------\r\n");
  printf("initialisation du CanSat\r\n");
  printf("------------------------\r\n");
  printf("------------------------\r\n");
  printf("------------------------\r\n");
  printf("------------------------\r\n");

    int x=0;
    int i=0;
    int Devices[100]={0};

   uint8_t pData[100] = {0};

   /**********                           *********
    * ******** INIT MPU-9250 (IMU_10DOF) *********
    * ********                           *********
    */

   printf("initialisation de l'IMU \r\n \r\n");

   // Initialisation du capteur MPU-9250

   Init(&hi2c1);

   // vérification identité capteur MPU-9250

    pData[0]= WHO_AM_I_MPU9250;
    if(HAL_I2C_Master_Transmit(&hi2c1, MPU_ADD, pData, 1, HAL_MAX_DELAY) != HAL_OK )
    {
  	  printf("il y a une erreur avec I2C Master Transmit \r\n");
    }
    if(HAL_I2C_Master_Receive(&hi2c1, MPU_ADD, pData, 1, HAL_MAX_DELAY) != HAL_OK )
    {
  	  printf("il y a une erreur avec I2C Master Receive \r\n");
    }


    printf(" L'identifiant du capteur est : %x \r\n", pData[0]);
    if((pData[0] =! 0x71))
    {
  	  printf("ce n'est pas le bon capteur \r\n");
    }
    else
  	  printf("MPU-9250 identified \r\n");

    // vérification identité AK8963C (magnétomètre)

    pData[0]= WHO_AM_I_AK8963;
      if(HAL_I2C_Master_Transmit(&hi2c1, MAGNETO_ADD, pData, 1, HAL_MAX_DELAY) != HAL_OK )
      {
    	  printf("il y a une erreur avec I2C Master Transmit \r\n");
      }
      if(HAL_I2C_Master_Receive(&hi2c1, MAGNETO_ADD, pData, 1, HAL_MAX_DELAY) != HAL_OK )
      {
    	  printf("il y a une erreur avec I2C Master Receive \r\n");
      }


      printf(" L'identifiant du magnétomètre est : %x \r\n", pData[0]);
      if((pData[0] =! 0x48))
      {
    	  printf("ce n'est pas le bon capteur \r\n  ****** \r\n ****** \r\n ****** \r\n");
      }
      else
    	  printf("AK8963C identified \r\n  ****** \r\n ****** \r\n ****** \r\n");



  // check les périphériques i2c disponibles

    for (i=0;i<256;i++){
         	  if(HAL_I2C_IsDeviceReady(&hi2c1, i, 4, 20)==HAL_OK){
         		  Devices[x]=i;
         		  printf("%d \r\n",Devices[x]);
         		  x=x+1;
         	  }

           }

    /*********** Calibration du magnétomètre *************/
 /*
    printf("debut de la calibration : faire des 8 \r\n");
    HAL_Delay(2000);
    printf("calibration du magnetometre en cours... \r\n");
    Calib_mag(&hi2c1, mag_offset, mag_coeff);
    printf("calibration du magnetometre terminee \r\n *\r\n *\r\n *\r\n");
    HAL_Delay(2000);
 */

    /**********                    *********
     * ******** INITIALISATION GPS *********
     * ********                    *********
     */

    	 /***********************************************************
    	  *            entrée des données GPS de la cible
    	  ***********************************************************/

    	  //Position GPS cible : 43°13'18.7"N 0°03'10.0"W  --> données telles que décrites dans le règlement 2023

    	  pDataCansat.GPS.latitude_Target = 43.2218611;
    	  pDataCansat.GPS.longitude_Target = -0.05277777777777778;



    /**********                              *********
     * ******** INITIALISATION Pololu_sds01a *********
     * ********                              *********
     */

    uint16_t gpio_value = 0;
    int Drop_flag = 0; // flag that indicates if the Cansat probe has been launch, in order to begin the missions

    /**********                       *********
     * ******** INITIALISATION Servos *********
     * ********                       *********
     */

    int arm_deployment_flag = 1; // pour le déploiement des bras lors du largage

    int altitude_ouverture_ballons = 30; // altitude à partir de laquelle on démarre l'opération
    									 // d'ouverture de la structure gonflable
    									 // à déterminer expérimentalement --> !!!prendre en compte l'altitude locale

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1100);   //repliement des bras pour rentrer dans la capsule de largage


    /************* INITIALISATION TIMER3 *************/ //pour calculer l'angle et déclencher le mouvement des servos toutes les secondes

    int demarrage_tim3 = 1;


    HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart_gps_rx, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /************** Récupération des données GPS *************/

	  //GPS_data_reading(it_rx_gps, data_rdy, first_data, pDataCansat, coordonnees, uart_gps_rx, i);

	  if(it_rx_gps){
	  	  		if(data_rdy == 1){
	  	  				for(int j = 0; j<=50;j++){
	  	  					position[j] = coordonnees[j+18]; //position[] comprend la latitude et la longitude telles que sur la trame reçue
	  	  					//En effet, les coordonnes sont d'abord de cette forme : "$GNGGA,121933.000,4902.36627,N,00204.31287,E,1,05,13.3,0.0,M,0.0,M,,*4D"
	  	  					//On sélectionne donc la partie qui est après l'identifieur et l'heure mais avant le ",E,".
	  	  				}

	  	  				for(int k = 0; k<=9;k++){
	  	  					latitude_data[k] = position[k+1]; //latitude non convertie
	  	  				}
	  	  				for(int l = 0; l<=10;l++){
	  	  					longitude_data[l] = position[l+14]; //longitude non convertie
	  	  				}
	  	  				for(int h = 0; h<=4; h++){
	  	  					altitude_data[h] = position[h+37]; //altitude non convertie
	  	  				}

	  	  				lat2[0] = latitude_data[0];
	  	  				lat2[1] = latitude_data[1]; //lat2 -> les deux digits avant la virgue du degré de la lat.

	  	  				for(int m = 0; m<=6;m++){
	  	  					lat7[m] = latitude_data[m+2]; //lat 7 -> ce qui reste après la virgule de la lat.
	  	  				}

	  	  				long3[0] = longitude_data[0];
	  	  				long3[1] = longitude_data[1];
	  	  				long3[2] = longitude_data[2]; //long3 -> les trois digits avant la virgule du degré de la long.

	  	  				for(int n = 0; n<=6;n++){
	  	  					long7[n] = longitude_data[n+3]; //long7 -> ce qui reste après la virgule de la long.
	  	  				}

	  	  				float deg_lat = atof(lat2);
	  	  				float reste_lat = atof(lat7); //conversion du char[lat] en float[lat]

	  	  				float deg_long = atof(long3);
	  	  				float reste_long = atof(long7); //conversion du char[long] en float[long]

	  	  				pDataCansat.GPS.latitude_Cansat = deg_lat + (reste_lat/60); //latitude convertie
	  	  				pDataCansat.GPS.longitude_Cansat = deg_long + (reste_long/60); //longitude convertie
	  	  				pDataCansat.GPS.altitude_Cansat = atof(altitude_data);

	  	  				//printf("donnees du GPS : latitude = %f, longitude = %f, altitude = %f, - - - - - - - - GPS \r\n", pDataCansat.GPS.latitude_Cansat, pDataCansat.GPS.longitude_Cansat, pDataCansat.GPS.altitude_Cansat);

	  	  				//nombre de satellites
	  	  				for(int k = 0; k<2;k++){
	  	  				satellites[k] = coordonnees[k+45]; //satellites[] comprend le nombre de stellites que sur la trame reçue
	  	  				}
	  	  		}
	  	  		if(uart_gps_rx[0]==10){
	  	  			//HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY); //Arrangement de la trame
	  	  		}
	  	  		else{
	  	  			uart_pc_tx[0]=uart_gps_rx[0];
	  	  			//HAL_UART_Transmit(&huart2, uart_pc_tx, 1, HAL_MAX_DELAY);
	  	  			coordonnees[i] = uart_pc_tx[0]; //On copie ce qui passe dans l'UART dans un tableau coordonnees[i].
	  	  			if(i >= 4){
	  	  				if(strncmp("OK*35",&coordonnees[i-4],5) == 0){
	  	  						i = 0; //Si on trouve "OK", on réinitialise i à 0 pour synchroniser notre trame.
	  	  						if(first_data == 1){
	  	  							data_rdy = 1; //Si la trame est initialisée, le premier caractère du tableau voulu est prêt.
	  	  						}
	  	  						else first_data = 1; //Permet d'enregistrer le tableau désiré.
	  	  					}
	  	  				else{
	  	  						if(i == GPS_TRAME_SIZE-1){
	  	  							i = 0;
	  	  						}
	  	  						else{
	  	  							i++;
	  	  					}

	  	  				}
	  	  			}
	  	  			else i++;
	  	  		}
	  	  	it_rx_gps = 0;
	  	  	}


	  /************** Détection du largage ***************/

	  if( Drop_flag == 0 )
	  {
	  Drop_flag = Drop_detection();
	  }

	  	  	  if(Drop_flag) // largage détecté, démarrage des missions
	  	  	  {
	  	  		 /*************** déploiement des bras *******************************/

	  	  		  if(arm_deployment_flag)
	  	  		  {
	  	  			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 400); // déploiement bras gauche
	  	  		//	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, ?); // déploiement bras droit
	  	  			arm_deployment_flag = 0;
	  	  		  }

	  	  		 /*************** Lecture du champ magnétique terrestre **************/

	  	  		  Measure_M(&hi2c1, pDataCansat.IMU.MagnetometerData.mag_raw, pDataCansat.IMU.MagnetometerData.offset, pDataCansat.IMU.MagnetometerData.offset);
	  	  		  //printf("champ magnetique --- selon x : %f , selon y : %f \r\n", pDataCansat.IMU.MagnetometerData.mag_raw[0], pDataCansat.IMU.MagnetometerData.mag_raw[1]);
	  	  		  HAL_Delay(50);

	  	  		 /*************** Conversion du champ magnétique en degrées **************/

	  	  		  pDataCansat.IMU.MagnetometerData.degree_angle = magnetic_field_to_degree(pDataCansat.IMU.MagnetometerData.mag_raw);

	  	  		  if( demarrage_tim3 )
	  	  		  {
	  	  			if(HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
	  	  			{
	  	  				printf("defaut d'initialisation du tim3");
	  	  			}
	  	  			demarrage_tim3 = 0;
	  	  		  }



	  	  		  if(pDataCansat.GPS.altitude_Cansat <= altitude_ouverture_ballons )
	  	  		  {
	  	  			  // fonction d'ouverture des ballons
	  	  			 declenchement_structure_gonflable();
	  	  		  }
	  	  	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

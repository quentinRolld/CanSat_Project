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
#include "cmsis_os.h"
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
#include "Cansat_Task.h"
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

/******* Task Handler ********/
extern TaskHandle_t pxGPS_Handler;
extern TaskHandle_t pxDrop_detection;
extern TaskHandle_t pxLancement_Cansat;
extern TaskHandle_t pxMesure_M;
extern TaskHandle_t pxeCompass;
/****************************/

int Drop_flag = 0; // flag that indicates if the Cansat probe has been launch, in order to begin the missions

/******* GPS ********/
char uart_gps_rx[1];
char uart_pc_tx[1];
/********************/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int __io_putchar(int ch) {
HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
return ch;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){ // fonction de callback de l'UART

	if(&huart == &huart1){

		HAL_UART_Receive_IT(&huart1, &uart_gps_rx, 1);

		portYIELD_FROM_ISR(Task_GPS_data_reading);

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

	  /******************************************
	  *          création structure
	  ******************************************/
/*
	  TypeDataCansat *pDataCansat;
	  pDataCansat = DATA_getDataCansat();
*/
	  /***********************************************************
	  *            entrée des données GPS de la cible
	  ***********************************************************/

	  //Position GPS cible : 43°13'18.7"N 0°03'10.0"W  --> données telles que décrites dans le règlement 2023

	  pDataCansat.GPS.latitude_Target = 43.2218611;
	  pDataCansat.GPS.longitude_Target = -0.05277777777777778;




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
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
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

    HAL_UART_Receive_IT(&huart1, uart_gps_rx, 1);

    /**********                              *********
     * ******** INITIALISATION Pololu_sds01a *********
     * ********                              *********
     */


    /**********                       *********
     * ******** INITIALISATION Servos *********
     * ********                       *********
     */



  vTaskStartScheduler();
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

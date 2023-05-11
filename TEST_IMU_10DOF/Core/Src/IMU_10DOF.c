/*
 * IMU_10DOF.c
 *
 *  Created on: Oct 21, 2022
 *      Author: quentinrolland
 */

#include "IMU_10DOF.h"
#include "math.h"
#include "types.h"
#include "main.h"


 /**
  * \fn Init(I2C_HandleTypeDef* p_hi2c1)
  * \brief Cette fonction met à 1 le bit 7 de PWR_MGMT pour faire un reset puis attend
	 * 100ms et choisit une horlorge
	 * \param p_hi2c1
	 */
void Init(I2C_HandleTypeDef* p_hi2c1){

	uint8_t buff[6];
	buff[0] = 0x80;
	//Reset the internal registers and restores the default settings
	HAL_I2C_Mem_Write ( p_hi2c1, MPU_ADD, PWR_MGMT_1,  1, &buff[0], 1, 10);
	HAL_Delay(100);
	buff[0] = 0x1;
	//Auto selects the best available clock source – PLL if ready, else use the Internal oscillator
	HAL_I2C_Mem_Write ( p_hi2c1, MPU_ADD, PWR_MGMT_1,  1, &buff[0], 1, 10);


	buff[0] =0b0011;  // changement de la sensibilité de l'accélérometre  00=2g 10=4g 01=8g 11=16g
		HAL_I2C_Mem_Write ( p_hi2c1, MPU_ADD,  ACCEL_CONFIG,  1, &buff[0], 1, 10);


	buff[0]=0x2;
	//When asserted, the i2c_master interface pins(ES_CL and ES_DA) will go into ‘bypass mode’ when the i2c master interface is disabled.
	//The pins will float high due to the internal pull-up if not enabled and the i2c master interface is disabled.
		  if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,INT_PIN_CFG,1,&buff[0],1,10)!=HAL_OK){
			  printf("erreur d'initialisation INT_PIN_CFG \r\n");
		  	  Error_Handler();
		  }

	buff[0]=0x16; //continuous measurement mode 2 ; 16-bit output
		 	  if(HAL_I2C_Mem_Write(p_hi2c1,MAGNETO_ADD,AK8963_CNTL,1,&buff[0],1,10)!=HAL_OK){
		 		  printf("erreur d'initialisation magnetometre \r\n");
		 	  	  Error_Handler();
		 	  }

	buff[0]=0x3;
	if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,CONFIG,1,buff,1,10)!=HAL_OK){
		Error_Handler();
	}

	buff[0]=0xFF;
	if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,SMPLRT_DIV,1,buff,1,10)!=HAL_OK){
		Error_Handler();

	}
}
/**
 *\fn  Measure_T(I2C_HandleTypeDef* p_hi2c1,double* theta)
 *  \brief Cette fonction récupère les données de température et les transforme en
	 une valeur décimale
		 * \param p_hi2c1
		 * \param theta
		 */
void Measure_T(I2C_HandleTypeDef* p_hi2c1,double* theta){

	uint8_t buff[2];
	int16_t Temp;
	if ( HAL_I2C_Mem_Read ( p_hi2c1, MPU_ADD, TEMP_OUT_H, 1, buff, 2, 10) == HAL_OK ){
		Temp = 256*buff[0] + buff[1];
		*theta = ( Temp - 21 )/333.87 + 12;
	}
}
/**
* \fn Measure_A(I2C_HandleTypeDef* p_hi2c1,double* accel)
 * \brief Cette fonction récupére les données d'accélération et les transforme en
	 une valeur décimale
	 * \param p_hi2c1
	 * \param accel
	 */
void Measure_A(I2C_HandleTypeDef* p_hi2c1,TypeDataCansat *pDataCansat){

	uint8_t buff[6];
	uint8_t sens[1];
	uint16_t x;
	uint16_t y;
	uint16_t z;
	uint16_t s;
	if((HAL_I2C_Mem_Read(p_hi2c1, MPU_ADD, ACCEL_XOUT_H, 1, buff, 6, 10)==HAL_OK) &
			(HAL_I2C_Mem_Read(p_hi2c1, MPU_ADD, ACCEL_CONFIG, 1, sens, 1, 10)==HAL_OK)){

		sens[0] = ((sens[0])&(0b11000))>>3;

		if(sens[0]==00){
			s =2;
		}
		else if(sens[0]==10){
			s =4;
		}
		else if(sens[0]==01){
			s =8;
		}
		else {
			s =16;
		}
		x=256*buff[0]+ buff[1];
		y=256*buff[2]+ buff[3];
		z=256*buff[4]+ buff[5];
		/*
		*accel=(x*s)/32767.0;
		*(accel+1)=(y*s)/32767.0;
		*(accel+2)=(z*s)/32767.0;
		*/
		double accel_x = (x*s)/32767.0;
		double accel_y = (y*s)/32767.0;
		double accel_z = (z*s)/32767.0;
		pDataCansat->IMU.AccelerometerData.x = (int)(accel_x*1000);
		pDataCansat->IMU.AccelerometerData.y = (int)(accel_y*1000);
		pDataCansat->IMU.AccelerometerData.z = (int)(accel_z*1000);
	}
}
/**
* \fn Measure_Gyro(I2C_HandleTypeDef* p_hi2c1,double* pt)
 * \brief Cette fonction récupére les données de vitesse angulaire et les transforme en
	 une valeur décimale
	 * \param p_hi2c1
	 * \param  pt
	 */
void Measure_Gyro(I2C_HandleTypeDef* p_hi2c1,double* pt){
		uint8_t buff[6];
		uint8_t sens[1];
		uint16_t x;
		uint16_t y;
		uint16_t z;
		uint16_t s;
		double gyr[3];
		if((HAL_I2C_Mem_Read(p_hi2c1, MPU_ADD, GYRO_XOUT_H, 1, buff, 6, 10)==HAL_OK) &
					(HAL_I2C_Mem_Read(p_hi2c1, MPU_ADD, GYRO_CONFIG, 1, sens, 1, 10)==HAL_OK)){

				sens[0] = ((sens[0])&(0b11000))>>3;

				if(sens[0]==00){
					s =250;
				}
				else if(sens[0]==10){
					s =500;
				}
				else if(sens[0]==01){
					s =1000;
				}
				else {
					s =2000;
				}
				x=256*buff[0]+ buff[1];
				y=256*buff[2]+ buff[3];
				z=256*buff[4]+ buff[5];
				*pt=(x*s)/32767.0;
				*(pt+1)=(y*s)/32767.0;
				*(pt+2)=(z*s)/32767.0;

				*gyr=(x*s)/32767.0;
				*(gyr+1)=(y*s)/32767.0;
				*(gyr+2)=(z*s)/32767.0;

			}
}

/**
* \fn Measure_M(I2C_HandleTypeDef* p_hi2c1,double* mag)
 * \brief Cette fonction récupére les données de champ magnétique et les transforme en
	 une valeur décimale
	 * \param p_hi2c1
	 * \param  mag
	 */
void Measure_M(I2C_HandleTypeDef* p_hi2c1,double* mag){
		int16_t x;
		int16_t y;
		int16_t z;
		int16_t asax;
		int16_t asay;
		int16_t asaz;

		uint8_t buff[6];
		uint8_t asa[3];
		uint8_t drdy[1];


		HAL_I2C_Mem_Read(p_hi2c1,MAGNETO_ADD,AK8963_ST1,1,drdy,1,20);
		drdy[0]=((drdy[0])&(00000001));
		if(drdy[0]==1){

			if((HAL_I2C_Mem_Read(p_hi2c1,MAGNETO_ADD,AK8963_XOUT_L,1,buff,7,20)==HAL_OK)
					&(HAL_I2C_Mem_Read(p_hi2c1,MAGNETO_ADD,AK8963_ASAX,1,asa,3,20)==HAL_OK)){


						x=256*buff[1]+ buff[0];
						y=256*buff[3]+ buff[2];
						z=256*buff[5]+ buff[4];
						asax=asa[0];
						asay=asa[1];
						asaz=asa[2];
						*mag=(x*((((asax-128)*0.5)/128)+1)*4912)/32760;
						*(mag+1)=(y*((((asay-128)*0.5)/128)+1)*4912)/32760;
						*(mag+2)=(z*((((asaz-128)*0.5)/128)+1)*4912)/32760;
			}






		}





}


/**
* \fn Noise_G(I2C_HandleTypeDef* p_hi2c1,double* noise)
 * \brief Cette fonction récupére les valeur de bruit du gyromètre
	 * \param p_hi2c1
	 * \param  noice
	 */
void Noise_G(I2C_HandleTypeDef* p_hi2c1,double* noise){
		double xm;
		double ym;
		double zm;

		double xe;
		double ye;
		double ze;


		double gyro[3];
		int i =0;
		int N=100;
		for(i=0;i<N;i++){
				Measure_Gyro(p_hi2c1, gyro);
				xm += gyro[0]/N;
				ym += gyro[1]/N;
				zm += gyro[2]/N;

		}

		for(i=0;i<N;i++){
			Measure_Gyro(p_hi2c1, gyro);
			xe += ((gyro[0]-xm)*(gyro[0]-xm))/N;
			ye += ((gyro[1]-ym)*(gyro[1]-ym))/N;
			ze += ((gyro[2]-zm)*(gyro[2]-zm))/N;



				}
		*noise = sqrt(xe);
		*(noise+1) = sqrt(ye);
		*(noise+2) = sqrt(ze);
}
/**
* \fn Calib_Gyro(I2C_HandleTypeDef* p_hi2c1,double* offset)
 * \brief Cette fonction calibre le gyromètre
	 * \param p_hi2c1
	 * \param  offset
	 */
void Calib_Gyro(I2C_HandleTypeDef* p_hi2c1,double* offset){
	double offsetx;
	double offsety;
	double offsetz;
	double gyro[3];

	int i =0;
	int N=200;
	for(i=0;i<N;i++){
		Measure_Gyro(p_hi2c1,gyro);
		offsetx+=gyro[0]/N;
		offsety+=gyro[1]/N;
		offsetz+= gyro[2]/N;
	}
	*offset=offsetx;
	*(offset+1)=offsety;
	*(offset+2)=offsetz;


}
/**
* \fn Average(I2C_HandleTypeDef* p_hi2c1,double* average)
 * \brief Cette fonction calcul les moyennes des  vitesses angulaires pour les trois axes x y et z
	 * \param p_hi2c1
	 * \param  average
	 */
void Average(I2C_HandleTypeDef* p_hi2c1,double* average){
	double xm;
	double ym;
	double zm;
	int i =0;
	int N=200;
	double gyro[3];
	double offset[3];
	for(i=0;i<N;i++){
			Measure_Gyro(p_hi2c1,gyro);
			Calib_Gyro(p_hi2c1,offset);
			xm+=(gyro[0]-offset[0])/N;
			ym+=(gyro[1]-offset[1])/N;
			zm+=(gyro[2]-offset[2])/N;

		}
		*average=xm;
		*(average+1)=ym;
		*(average+2)=zm;




}






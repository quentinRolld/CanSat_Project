/*
 ******************************************************************************
 * @file : function.c
 *
 * @Note Created on: Nov 30, 2022
 *      Author: quentinrolland
 ******************************************************************************
 */


#include <IMU_10DOF.h>
#include "Const.h"
#include "math.h"
#include <stdio.h>



/**
 	 * @brief Init I2C1
 	 * @Note Cette fonction met à 1 le bit 7 de PWR_MGMT_1 pour faire un reset de l'ensemble des registres du capteur, puis attend
	 * 100ms et choisit une horlorge. De plus, elle désactive l'interface master I2C du MPU-9250 pour permettre la lecture des données du magnétomètre
	 * et elle configure ce dernier pour qu'il réalise des mesures en continu.
	 * @param p_hi2c1 Pointeur vers une structure I2C qui contient l'information de configuration pour un i2c particulier
	 * @retval None
	 */
void Init(I2C_HandleTypeDef* p_hi2c1)
{

	uint8_t buff[6];
	buff[0] = 0x80;
	HAL_I2C_Mem_Write ( p_hi2c1, MPU_ADD,  PWR_MGMT_1,  1, &buff[0], 1, 10);
	HAL_Delay(100);
	buff[0] = 0x1;
	HAL_I2C_Mem_Write ( p_hi2c1, MPU_ADD, PWR_MGMT_1,  1, &buff[0], 1, 10);


	/********* changement de la sensibilité de l'accélérometre  00=2g 01=4g 10=8g 11=16g *********/
	HAL_I2C_Mem_Read ( p_hi2c1, MPU_ADD,  ACCEL_CONFIG,  1, &buff[0], 1, 10);

	//Pour mettre 00
	buff[0] =(buff[0] & (0b11100111)) ;
	//Pour mettre 01
	//buff[0] =(buff[0] & (0b11101111)) ;
	//buff[0] =(buff[0] | (0b00001000)) ;
	//Pour mettre 10
	//buff[0] =(buff[0] & (0b11110111)) ;
	//buff[0] =(buff[0] | (0b00010000)) ;
	//Pour mettre 11
	//buff[0] =(buff[0] | (0b00011000)) ;

	HAL_I2C_Mem_Write ( p_hi2c1, MPU_ADD,  ACCEL_CONFIG,  1, &buff[0], 1, 10);


	buff[0]=0x2; // Bypass pour activer le magnétomètre
		  if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,INT_PIN_CFG,1,&buff[0],1,10)!=HAL_OK){
			  printf("probleme activation magnetometre --> fct Init \r\n");
		  	  Error_Handler();
		  }
	buff[0]=0x16; // configuration mesure en continu du magnétomètre
		 	  if(HAL_I2C_Mem_Write(p_hi2c1,MAGNETO_ADD,AK8963_CNTL,1,&buff[0],1,10)!=HAL_OK){
		 		  printf("probleme configuration mesure magnetometre --> fct Init \r\n");
		 	  	  Error_Handler();

		 	  }

	/********** Réglage de la bande passante **********/

	buff[0]=0x3;
	if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,CONFIG,1,&buff[0],1,10)!=HAL_OK){
		printf("probleme bande passante --> fct Init \r\n");
		Error_Handler();
	}

/*
	// bande passante à 3600Hz : FCHOICE = 01 => FCHOICE_b = 10, DLPF_CFG = x
		HAL_I2C_Mem_Read ( p_hi2c1, MPU_ADD,  GYRO_CONFIG,  1, &buff[0], 1, 10);
		//Pour mettre 10 dans FCHOICE_b
		buff[0] =(buff[0] & (0b11111110)) ;
		buff[0] =(buff[0] | (0b00000010)) ;
		if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,GYRO_CONFIG,1,&buff[0],1,10)!=HAL_OK){
			Error_Handler();
		}
*/

	// bande passante à 250Hz : FCHOICE = 11 => FCHOICE_b = 00, DLPF_CFG = 000
		HAL_I2C_Mem_Read ( p_hi2c1, MPU_ADD,  GYRO_CONFIG,  1, &buff[0], 1, 10);
		//Pour mettre 00 dans FCHOICE_b
		buff[0] =(buff[0] & (0b11111100)) ;
		if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,GYRO_CONFIG,1,&buff[0],1,10)!=HAL_OK){
				Error_Handler(); }
		//Pour mettre 000 dans DLPF_CFG
		HAL_I2C_Mem_Read ( p_hi2c1, MPU_ADD, CONFIG,  1, &buff[0], 1, 10);
		buff[0] =(buff[0] & (0b11111000)) ;
		if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,CONFIG,1,&buff[0],1,10)!=HAL_OK){
				Error_Handler();
		}

/*
	// bande passante à 92Hz : FCHOICE = 11 => FCHOICE_b = 00, DLPF_CFG = 010
		HAL_I2C_Mem_Read ( p_hi2c1, MPU_ADD,  GYRO_CONFIG,  1, &buff[0], 1, 10);
		//Pour mettre 00 dans FCHOICE_b
		buff[0] =(buff[0] & (0b11111100)) ;
		if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,GYRO_CONFIG,1,&buff[0],1,10)!=HAL_OK){
				Error_Handler(); }
		//Pour mettre 010 dans DLPF_CFG
		HAL_I2C_Mem_Read ( p_hi2c1, MPU_ADD, CONFIG, 1, &buff[0], 1, 10);
		buff[0] =(buff[0] & (0b11111010)) ;
		buff[0] =(buff[0] | (0b00000010)) ;
		if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,CONFIG,1,&buff[0],1,10)!=HAL_OK){
				Error_Handler();
		}
*/
/*
	// bande passante à 20Hz : FCHOICE = 11 => FCHOICE_b = 00, DLPF_CFG = 100
		HAL_I2C_Mem_Read ( p_hi2c1, MPU_ADD,  GYRO_CONFIG,  1, &buff[0], 1, 10);
		//Pour mettre 00 dans FCHOICE_b
		buff[0] =(buff[0] & (0b11111100)) ;
		if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,GYRO_CONFIG,1,&buff[0],1,10)!=HAL_OK){
				Error_Handler(); }
		//Pour mettre 110 dans DLPF_CFG
		HAL_I2C_Mem_Read ( p_hi2c1, MPU_ADD, CONFIG,  1, &buff[0], 1, 10);
		buff[0] =(buff[0] & (0b11111100)) ;
		buff[0] =(buff[0] | (0b00000100)) ;
		if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,CONFIG,1,&buff[0],1,10)!=HAL_OK){
				Error_Handler();
		}
*/
/*
	// bande passante à 5Hz : FCHOICE = 11 => FCHOICE_b = 00, DLPF_CFG = 110
		HAL_I2C_Mem_Read ( p_hi2c1, MPU_ADD,  GYRO_CONFIG,  1, &buff[0], 1, 10);
		//Pour mettre 00 dans FCHOICE_b
		buff[0] =(buff[0] & (0b11111100)) ;
		if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,GYRO_CONFIG,1,&buff[0],1,10)!=HAL_OK){
				Error_Handler(); }
		//Pour mettre 000 dans DLPF_CFG
		HAL_I2C_Mem_Read ( p_hi2c1, MPU_ADD, CONFIG,  1, &buff[0], 1, 10);
		buff[0] =(buff[0] & (0b11111110)) ;
		buff[0] =(buff[0] | (0b00000110)) ;
		if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,CONFIG,1,&buff[0],1,10)!=HAL_OK){
				Error_Handler();
		}
*/

	/*********** Réglage de la fréquence d'échantillonnage **********/

	buff[0]=0xFF;
	if(HAL_I2C_Mem_Write(p_hi2c1,MPU_ADD,SMPLRT_DIV,1,&buff[0],1,10)!=HAL_OK){
		printf("probleme reglage frequence echantillonnage --> Init \r\n");
		Error_Handler();
	}
	HAL_I2C_Mem_Read(p_hi2c1,MPU_ADD,LP_ACCEL_ODR,1,&buff[0],1,10);
	// a faire : changer la valeur de DLPF_CFG à 2 par exemple et voir en mode debug si la valeur des 4 derniers bits de LP_ACCEL_ODR est à 4.

}


/**
 	 * @brief Mesure temperature
 	 * @Note Cette fonction permet de lire la temperature du capteur puis de l'enregistrer dans une variable globale de type double
	 * @param hi2cx Pointeur vers une structure I2C qui contient l'information de configuration pour un i2c particulier.
	 * @param Temperature Pointeur vers une zone mémoire de type double contenant l’information de température
	 * @retval None
	 */

void Measure_T(I2C_HandleTypeDef* hi2cx, double* Temperature)
{
	uint8_t buff[6];
	HAL_I2C_Mem_Read ( hi2cx, MPU_ADD, TEMP_OUT_H, 1, &buff[0], 1, 10);
	HAL_I2C_Mem_Read ( hi2cx, MPU_ADD, TEMP_OUT_L, 1, &buff[1], 1, 10);

	uint16_t Temp = 0;
	Temp = ((buff[0]<< 8) + buff[1]);

	*Temperature = ((Temp - ROOM_TEMP_OFFSET)/TEMP_SENS) + 21;

}


/**
 	 * @brief Mesure acceleration
 	 * @Note Cette fonction permet de lire l'acceleration du capteur sur les axes x,y et z puis de l'enregistrer dans une variable globale de type double
	 * @param hi2cx Pointeur vers une structure I2C qui contient l'information de configuration pour un i2c particulier
	 * @param Acceleration Pointeur vers une zone mémoire de type double contenant l’information d'accélération
	 * @retval None
	 */

void Measure_A(I2C_HandleTypeDef* hi2cx,double* Acceleration){
	uint8_t buff[6];
	HAL_I2C_Mem_Read ( hi2cx, MPU_ADD, ACCEL_XOUT_H, 1, &buff[0], 6, 10);

	uint8_t buffer_scale[1];
	HAL_I2C_Mem_Read ( hi2cx, MPU_ADD,  ACCEL_CONFIG,  1, &buffer_scale[0], 1, 10);


	uint16_t ax = 0;
		ax = ((buff[0]<< 8) + buff[1]);

	uint16_t ay = 0;
		ay = ((buff[2]<< 8) + buff[3]);

	uint16_t az = 0;
		az = ((buff[4]<< 8) + buff[5]);

		//Check de la sensibilité

	buffer_scale[0] = ((buffer_scale[0])&(0b11000))>>3;
	int sensibility =0;

	if (buffer_scale[0]==00){
		sensibility=2;
	}
	else if (buffer_scale[0]==01){
		sensibility=4;
	}
	else if (buffer_scale[0]==10){
		sensibility=8;
	}
	else if (buffer_scale[0]==11){
		sensibility=16;
	}

	Acceleration[0] = ax*9.81*sensibility/32767.0;
	Acceleration[1] = ay*9.81*sensibility/32767.0;
	Acceleration[2] = az*9.81*sensibility/32767.0;


}

/**
 	 * @brief Mesure vitesse angulaire
 	 * @Note Cette fonction permet de lire la vitesse angulaire du capteur sur les axes x,y et z puis de l'enregistrer dans une variable globale de type double
	 * @param hi2cx Pointeur vers une structure I2C qui contient l'information de configuration pour un i2c particulier
	 * @param tableau_donnee_utiles Pointeur vers une zone mémoire de type double contenant l’information de vitesse angulaire
	 * @retval None
	 */

void Measure_Vitesse_angulaire(I2C_HandleTypeDef* hi2cx, double* tableau_donnee_utiles)
{
	uint8_t buffer[6];
	uint8_t plage[1];
	uint16_t gyro_x;
	uint16_t gyro_y;
	uint16_t gyro_z;
	int p = 0;

	if( HAL_I2C_Mem_Read( hi2cx, MPU_ADD, GYRO_XOUT_H, 1, &buffer[0], 6, 10) != HAL_OK){
		printf("probleme lecture donnees gyro \r\n");
		Error_Handler();
	}

	if( HAL_I2C_Mem_Read( hi2cx, MPU_ADD, GYRO_CONFIG, 1, &plage[0], 1, 10) != HAL_OK){
		printf("probleme lecture plage echelle gyro \r\n");
		Error_Handler();
	}

	plage[0] = ((plage[0])&(0b11000))>>3;

                if(plage[0]==00)
		{
                    p =250;
                }
                else if(plage[0]==10)
		{
                    p =500;
                }
                else if(plage[0]==01)
		{
                    p =1000;
                }
                else {
                    p =2000;
                }


	gyro_x = (uint16_t)((buffer[0]<<8) + buffer[1]);
	gyro_y = (uint16_t)((buffer[2]<<8) + buffer[3]);
	gyro_z = (uint16_t)((buffer[4]<<8) + buffer[5]);

	tableau_donnee_utiles[0] =  gyro_x * p / 32767.0;
	tableau_donnee_utiles[1] =  gyro_y * p / 32767.0;
	tableau_donnee_utiles[2] =  gyro_z * p / 32767.0;
}




/**
 	 * @brief Mesure champ magnétique
 	 * @Note Cette fonction permet de lire la valeur du champ magnétique selon un repère d'axes x,y et z puis de l'enregistrer dans une variable globale de type double
	 * @param hi2cx Pointeur vers une structure I2C qui contient l'information de configuration pour un i2c particulier
	 * @param tableau_donnee_utiles Pointeur vers une zone mémoire de type double contenant l’information de champ magnétique
	 * @retval None
	 */

void Measure_M(I2C_HandleTypeDef* p_hi2c1,double* mag, double* offset, double* coeff){

		int16_t x;
		int16_t y;
		int16_t z;
		int16_t asax;
		int16_t asay;
		int16_t asaz;

		uint8_t buffer[6];
		uint8_t asa[3];
		uint8_t drdy[1];


		HAL_I2C_Mem_Read(p_hi2c1,MAGNETO_ADD,AK8963_ST1,1,drdy,1,20);
		drdy[0]=((drdy[0])&(00000001));
		if(drdy[0]==1){

			if((HAL_I2C_Mem_Read(p_hi2c1,MAGNETO_ADD,AK8963_XOUT_L,1,buffer,7,20)==HAL_OK)
					&(HAL_I2C_Mem_Read(p_hi2c1,MAGNETO_ADD,AK8963_ASAX,1,asa,3,20)==HAL_OK)){


						x = (uint16_t)((buffer[1]<<8)+ buffer[0]);
						y = (uint16_t)((buffer[3]<<8)+ buffer[2]);
						z = (uint16_t)((buffer[5]<<8)+ buffer[4]);
						asax=asa[0];
						asay=asa[1];
						asaz=asa[2];
						mag[0] = (x*((((asax-128)*0.5)/128)+1)*4912)/32760;
						mag[1] = (y*((((asay-128)*0.5)/128)+1)*4912)/32760;
						mag[2] = (z*((((asaz-128)*0.5)/128)+1)*4912)/32760;

						// calibration

						mag[0] = (mag[0]-offset[0])*coeff[0];
						mag[1] = (mag[1]-offset[1])*coeff[1];
						mag[2] = (mag[2]-offset[2])*coeff[2];
					/*
						mag[0] = (mag[0]*coeff[0])-offset[0];
						mag[1] = (mag[1]*coeff[1])-offset[1];
						mag[2] = (mag[2]*coeff[2])-offset[2];
					*/
			}
		}
}

/**
 	 * @brief Mesure de la valeur efficace du bruit
 	 * @Note Cette fonction permet de calculer la valeur efficace du bruit du gyromètre selon une approche probabiliste, en calculant la moyenne d'un échantillon de données de vitesse angulaire
	 * @param hi2cx Pointeur vers une structure I2C qui contient l'information de configuration pour un i2c particulier
	 * @param noise Pointeur vers une zone mémoire de type double contenant l’information de valeur efficace du bruit
	 * @retval None
	 */
void Noise_G(I2C_HandleTypeDef* hi2cx, double* noise){

		double x_m;
		double y_m;
		double z_m;
		double x_b;
		double y_b;
		double z_b;
		int i =0;
		double Buffer_gyrometre[3];
		int Plage_mesure=100;

		for(i=0;i<Plage_mesure;i++){

                Measure_Vitesse_angulaire(hi2cx, Buffer_gyrometre);  //	         N
				x_m += Buffer_gyrometre[0]/Plage_mesure;//  		  xm = 1/N * ∑ x
				y_m += Buffer_gyrometre[1]/Plage_mesure;//	      			    i=0
				z_m += Buffer_gyrometre[2]/Plage_mesure;// moyenne des valeurs

		}
											//			   N
		for(i=0;i<Plage_mesure;i++){        //  xb = 1/N * ∑ (x-xm)^2
											//			  i=0
			Measure_Vitesse_angulaire(hi2cx, Buffer_gyrometre);		//  = (valeur efficace du bruit)^2
			x_b += ((Buffer_gyrometre[0]-x_m)*(Buffer_gyrometre[0]-x_m))/Plage_mesure;
			y_b += ((Buffer_gyrometre[1]-y_m)*(Buffer_gyrometre[1]-y_m))/Plage_mesure;
			z_b += ((Buffer_gyrometre[2]-z_m)*(Buffer_gyrometre[2]-z_m))/Plage_mesure;

		}

		noise[0] = sqrt(x_b); // Valeur efficace du bruit
		noise[1] = sqrt(y_b);
		noise[2] = sqrt(z_b);

}



/**
 	 * @brief Calibration du gyromètre
 	 * @Note Cette fonction permet de calculer l'offset des composantes de la vitesse angulaire en se basant sur la méthode du single point
	 * @param hi2cx Pointeur vers une structure I2C qui contient l'information de configuration pour un i2c particulier
	 * @param gyro_offset Pointeur vers une zone mémoire de type double contenant l’information de l'offset du gyromètre
	 * @retval None
	 */
void Calib_gyro(I2C_HandleTypeDef* hi2cx,double* gyro_offset){

	double wx_offset = 0;
	double wy_offset = 0;
	double wz_offset = 0;

	double tableau_donnees_utiles[3];

	int i = 0;

	for(i=0;i<100;i++){
		Measure_Vitesse_angulaire(hi2cx, tableau_donnees_utiles);
		wx_offset += tableau_donnees_utiles[0]/100;
		wy_offset += tableau_donnees_utiles[1]/100;
		wz_offset += tableau_donnees_utiles[2]/100;
	}


	gyro_offset[0] = wx_offset;
	gyro_offset[1] = wy_offset;
	gyro_offset[2] = wz_offset;

}


/**
 	 * @brief Vérification du fonctionnement de la calibration
 	 * @Note Cette fonction permet de tester la fonction Cali_gyro en calculant la moyenne de la vitesse angulaire moins son offset lorsque le capteur est immobile
	 * @param hi2cx Pointeur vers une structure I2C qui contient l'information de configuration pour un i2c particulier
	 * @param tableau_moyenne Pointeur vers une zone mémoire de type double contenant l’information de valeur moyenne de la vitesse angulaire calibrée et capteur immobile (censée valoir 0)
	 * @retval None
	 */
void Average(I2C_HandleTypeDef* hi2cx, double* tableau_moyenne){

	double tableau_donnees_utiles[3] = {0};
	double wx_moyen = 0;
	double wy_moyen = 0;
	double wz_moyen = 0;
	int i = 0;
	double gyro_offset[3]={0};

	for (i=0;i<100;i++){ // i=100 car 100 données sur chaque axe

		Measure_Vitesse_angulaire(hi2cx, tableau_donnees_utiles);
		Calib_gyro(hi2cx, gyro_offset);

		wx_moyen += (tableau_donnees_utiles[0]-gyro_offset[0])/100;
		wy_moyen += (tableau_donnees_utiles[1]-gyro_offset[1])/100;
		wz_moyen += (tableau_donnees_utiles[2]-gyro_offset[2])/100;
	}

	tableau_moyenne[0] = wx_moyen;
	tableau_moyenne[1] = wy_moyen;
	tableau_moyenne[2] = wz_moyen;

}

// fonction pour convertir les données reçues par le magnétomètre (µT) en °
double convDegre(double* donne_mag){

	double theta = 0;

	if (donne_mag[0] > 30){ // si on se trouve dans la zone 0 à 180 degrés
		theta = -4.1*donne_mag[1]+197.88;
	}

	if (donne_mag[0] <= 30){  // si on se trouve dans la zone 180 à 360 degrés
		theta = 4.1*donne_mag[1]+162.12;
	}

	return theta;
}

// fonction pour calculer le maximum d'un tableau

double Maximum(double* data){
	int i = 0;
	int N = sizeof(data);
	double max = 0;

	for (i=0;i<N;i++)
	{
		if(data[i]>max)
		{
			max = data[i];
		}
	}
	return max;
}

// fonction pour calculer le minimum d'un tableau

double Minimum(double* data){
	int i = 0;
	int N = sizeof(data);
	double min = 0;

	for (i=0;i<N;i++)
	{
		if(data[i]<min)
		{
			min = data[i];
		}
	}
	return min;
}

// fonction pour calculer l'offset du magnétomètre

void Calib_mag(I2C_HandleTypeDef* hi2cx, double* offset, double* coeff){

	double donnees_mag_Calib[3]={0};
	double donnees_B[300][3];
	double max_Bx = 0;
	double min_Bx = 0;
	double max_By = 0;
	double min_By = 0;
	double max_Bz = 0;
	double min_Bz = 0;
	double Rx = 0;
	double Ry = 0;
	double Rz = 0;
	double R = 0;
	double Somme_Bx;
	double Somme_By;
	double Somme_Bz;

	int i = 0;
	int N = 300;

	for(i=0;i<N;i++){
		  	  		Measure_M(hi2cx, donnees_mag_Calib, offset, coeff);

		  	  		// Bx
		  	  		donnees_B[i][0] = donnees_mag_Calib[0];

		  	  		if(i==0){
		  	  			max_Bx = donnees_B[0][0];
		  	  			min_Bx = donnees_B[0][0];
		  	  			Somme_Bx = donnees_B[0][0];
		  	  		}

		  	  		else{

		  	  			Somme_Bx += donnees_B[i][0]; // Calcul de la somme de Bx pour la moyenne

		  	  			if(donnees_B[i][0] > donnees_B[i-1][0]) //Calcul du Maximum de Bx
		  	  			{
		  	  				max_Bx = donnees_B[i][0];
		  	  			}
		  	  			if(donnees_B[i][0] < donnees_B[i-1][0]) // Calcul du Minimum de Bx
		  	  			{
		  	  				min_Bx = donnees_B[i][0];
		  	  			}
		  	  		}

		  	  		//By
		  	  		donnees_B[i][1] = donnees_mag_Calib[1];

		  	  		if(i==0){
		  	  			max_By = donnees_B[0][1];
		  	  			min_By = donnees_B[0][1];
		  	  			Somme_By = donnees_B[0][1];
		  	  		}

		  	  		else{

		  	  			Somme_By += donnees_B[i][1];  // Calcul de la somme de By pour la moyenne

		  	  			if(donnees_B[i][1] > donnees_B[i-1][1]) //Calcul du Maximum de By
		  	  			{
		  	  				max_By = donnees_B[i][1];
		  	  			}
		  	  			if(donnees_B[i][1] < donnees_B[i-1][1]) // Calcul du Minimum de By
		  	  			{
		  	  				min_By = donnees_B[i][1];
		  	  			}
		  	  		}

		  	  		//Bz
		  	  		donnees_B[i][2] = donnees_mag_Calib[2];

		  	  		if(i==0){
		  	  			max_Bz = donnees_B[0][2];
		  	  			min_Bz = donnees_B[0][2];
		  	  			Somme_Bz = donnees_B[0][2];
		  	  		}

		  	  		else{

		  	  			Somme_Bz += donnees_B[i][2];  // Calcul de la somme de Bz pour la moyenne

		  	  			if(donnees_B[i][2] > donnees_B[i-1][2]) //Calcul du Maximum de Bz
		  	  			{
		  	  				max_Bz = donnees_B[i][2];
		  	  			}
		  	  			if(donnees_B[i][2] < donnees_B[i-1][2]) // Calcul du Minimum de Bz
		  	  			{
		  	  				min_Bz = donnees_B[i][2];
		  	  			}
		  	  		}

		  	  		HAL_Delay(50);
	}

	//calcul des 3 offsets
/*
	offset[0] = (max_Bx + min_Bx)/2;
	offset[1] = (max_By + min_By)/2;
	offset[2] = (max_Bz + min_Bz)/2;
*/
	offset[0] = Somme_Bx/N;
	offset[1] = Somme_By/N;
	offset[2] = Somme_Bz/N;

	//calcul des rayons maximum

	Rx = (min_Bx - max_Bx)/2;
	Ry = (min_By - max_By)/2;
	Rz = (min_Bz - max_Bz)/2;

	//calcul rayon moyen

	R = (Rx + Ry + Rz)/3;

	//calcul coeff correcteur

	coeff[0] = R/Rx;
	coeff[1] = R/Ry;
	coeff[2] = R/Rz;



}












/*
 * gps.c
 *
 *  Created on: 13 mai 2023
 *      Author: quentinrolland
 */

#include "main.h"
#include "Const.h"
#include "stdio.h"
#include <math.h>
#include "IMU_10DOF.h"
#include "data.h"
#include "types.h"
#include "servo.h"
#include "stdlib.h"
#include "string.h"
#include "gps.h"



extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


extern char uart_gps_rx[1];
extern char uart_pc_tx[1];
extern char coordonnees[GPS_TRAME_SIZE];

extern uint8_t it_rx_gps;
extern int first_data;
extern int data_rdy;
extern int i;

extern TypeDataCansat pDataCansat;


OUTCOME GPS_data_reading(){

	  if(it_rx_gps){
	  		if(data_rdy == 1){

	  			char longitude_data[10];
	  			char latitude_data[10];
	  			char altitude_data[5];
	  			char position[23];
	  			char lat2[2];
	  			char lat7[7];
	  			char long3[3];
	  			char long7[7];
	  			char satellites[2];

	  			for(int j = 0; j<=47;j++){
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
	  			lat2[1] = latitude_data[1]; //lat2 -> les deux digits avant la virgule du degré de la lat.

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
	  			pDataCansat.GPS.altitude_Cansat = atof(altitude_data); // altitude convertie

	  			printf("donnees du GPS : latitude = %f, longitude = %f, altitude = %f, - - - - - - - - GPS \r\n", pDataCansat.GPS.latitude_Cansat, pDataCansat.GPS.longitude_Cansat, pDataCansat.GPS.altitude_Cansat);


	  				//nombre de satellites
	  				for(int k = 0; k<2;k++){
	  				satellites[k] = coordonnees[k+45]; //satellites[] comprend le nombre de satellites utilisés par le modul GPS
	  				}
	  		}
	  		if(uart_gps_rx[0]==10){
	  			//HAL_UART_Transmit(&huart2, "\r\n", 2, HAL_MAX_DELAY); //Arrangement de la trame
	  		}
	  		else{
	  			//uart_pc_tx[0]=uart_gps_rx[0];
	  			//HAL_UART_Transmit(&huart2, uart_pc_tx, 1, HAL_MAX_DELAY);
	  			coordonnees[i] = uart_gps_rx[0]; //On copie ce qui passe dans l'UART dans un tableau coordonnees[i].
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

	  return PASS;
}

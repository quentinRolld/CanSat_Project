/*
 * @file : function.h
 *
 *  Created on: Nov 30, 2022
 *      Author: quentinrolland
 */

#ifndef INC_IMU_10DOF_H_
#define INC_IMU_10DOF_H_

#include "main.h"


void Init(I2C_HandleTypeDef* hi2cx);
void Measure_T(I2C_HandleTypeDef* hi2cx, double* Temperature);
void Measure_A(I2C_HandleTypeDef* hi2cx, double* Acceleration);
void Measure_Vitesse_angulaire(I2C_HandleTypeDef* hi2cx, double* tableau_donnee_utiles);
void Measure_M(I2C_HandleTypeDef* p_hi2c1,double* mag, double* offset, double* coeff);
void Noise_G(I2C_HandleTypeDef* hi2cx,double* noise);
void Calib_gyro(I2C_HandleTypeDef* hi2cx,double* gyro_offset);
void Average(I2C_HandleTypeDef* hi2cx, double* tableau_moyenne);
double convDegre(double* donne_mag);
double Minimum(double* data);
double Maximum(double* data);
void Calib_mag(I2C_HandleTypeDef* hi2cx, double* offset, double* coeff);

#endif /* INC_IMU_10DOF_H_ */

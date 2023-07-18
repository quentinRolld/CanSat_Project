/*
 * balance.c
 *
 *  Created on: Jul 18, 2023
 *      Author: quentinrolland
 *      Source : NXP AN4248
 */

#include "main.h"
#include "math.h"


const uint16_t MINDELTATRIG = 1;             /* final step size for iTrig */

const uint16_t MINDELTADIV = 1; /* final step size for iDivide */

/* fifth order of polynomial approximation giving 0.05 deg max error */
const uint16_t K1 = 5701;
const uint16_t K2 = -1645;
const uint16_t K3 = 446;


/* roll pitch and yaw angles computed by iecompass */
static uint16_t iPhi, iThe, iPsi;
/* magnetic field readings corrected for hard iron effects and PCB orientation */
static uint16_t iBfx, iBfy, iBfz;



/* function to calculate ir = iy / ix with iy <= ix, and ix, iy both > 0 */
static uint16_t iDivide(uint16_t iy, uint16_t ix)
{
uint16_t itmp;
uint16_t ir;
uint16_t idelta;
/* scratch */
/* result = iy / ix range 0., 1. returned in range 0 to 32767 */
/* delta on candidate result dividing each stage by factor of 2 */
/* set result r to zero and binary search step to 16384 = 0.5 */
ir = 0;
idelta = 16384;                      /* set as 2^14 = 0.5 */
/* to reduce quantization effects, boost ix and iy to the maximum signed 16 bit value */
while ((ix < 16384) && (iy < 16384))
{
         ix = (uint16_t)(ix + ix);
         iy = (uint16_t)(iy + iy);
}
/* loop over binary sub-division algorithm solving for ir*ix = iy */
do
{

/* generate new candidate solution for ir and test if we are too high or too low */
itmp = (uint16_t)(ir + idelta);       /* itmp=ir+delta, the candidate solution */
itmp = (uint16_t)((itmp * ix) >> 15);
if (itmp <= iy) ir += idelta;
         idelta = (uint16_t)(idelta >> 1);     /* divide by 2 using right shift one bit */
} while (idelta >= MINDELTADIV);   /* last loop is performed for idelta=MINDELTADIV */
return (ir);
}



/* function to calculate ir = ix / sqrt(ix*ix+iy*iy) using binary division */
static uint16_t iTrig(uint16_t ix, uint16_t iy)
{
uint32_t itmp;
uint32_t ixsq;
uint16_t isignx;
uint32_t ihypsq;
uint16_t ir;
uint16_t idelta;
/* scratch */
/* ix * ix */
/* storage for sign of x. algorithm assumes x >= 0 then corrects later */
/* (ix * ix) + (iy * iy) */
/* result = ix / sqrt(ix*ix+iy*iy) range -1, 1 returned as signed uint16_t */
/* delta on candidate result dividing each stage by factor of 2 */
/* stack variables */
/* ix, iy: signed 16 bit integers representing sensor reading in range -32768 to 32767 */ /* function returns signed uint16_t as signed fraction (ie +32767=0.99997, -32768=-1.0000) */ /* algorithm solves for ir*ir*(ix*ix+iy*iy)=ix*ix */
/* correct for pathological case: ix==iy==0 */
if ((ix == 0) && (iy == 0)) ix = iy = 1;
/* check for -32768 which is not handled correctly */
if (ix == -32768) ix = -32767;
if (iy == -32768) iy = -32767;
/* store the sign for later use. algorithm assumes x is positive for convenience */
isignx = 1;
if (ix < 0)
{
         ix = (uint16_t)-ix;
         isignx = -1;
}
/* for convenience in the boosting set iy to be positive as well as ix */
iy = (uint16_t)fabs(iy);
/* to reduce quantization effects, boost ix and iy but keep below maximum signed 16 bit */
while ((ix < 16384) && (iy < 16384))
{
         ix = (uint16_t)(ix + ix);
         iy = (uint16_t)(iy + iy);
}
/* calculate ix*ix and the hypotenuse squared */
ixsq = (uint32_t)(ix * ix); /* ixsq=ix*ix: 0 to 32767^2 = 1073676289 */
ihypsq = (uint32_t)(ixsq + iy * iy); /* ihypsq=(ix*ix+iy*iy) 0 to 2*32767*32767=2147352578 */
/* set result r to zero and binary search step to 16384 = 0.5 */
ir = 0;
idelta = 16384;                      /* set as 2^14 = 0.5 */
/* loop over binary sub-division algorithm */
do
{

  /* generate new candidate solution for ir and test if we are too high or too low */
  /* itmp=(ir+delta)^2, range 0 to 32767*32767 = 2^30 = 1073676289 */
  itmp = (uint32_t)((ir + idelta) * (ir + idelta));
  /* itmp=(ir+delta)^2*(ix*ix+iy*iy), range 0 to 2^31 = 2147221516 */
  itmp = (itmp >> 15) * (ihypsq >> 15);
  if (itmp <= ixsq) ir += idelta;
  idelta = (uint16_t)(idelta >> 1);       /* divide by 2 using right shift one bit */
 }while (idelta >= MINDELTATRIG);    /* last loop is performed for idelta=MINDELTATRIG */
/* correct the sign before returning */
return (uint16_t)(ir * isignx);
}




/* calculates 100*atan(iy/ix) range 0 to 9000 for all ix, iy positive in range 0 to 32767 */
static uint16_t iHundredAtanDeg(uint16_t iy, uint16_t ix)
{
uint32_t iAngle;
uint16_t iRatio;
uint32_t iTmp;
/* angle in degrees times 100 */
/* ratio of iy / ix or vice versa */
/* temporary variable */
/* check for pathological cases */
if ((ix == 0) && (iy == 0)) return (0);
if ((ix == 0) && (iy != 0)) return (9000);
/* check for non-pathological cases */
if (iy <= ix)
         iRatio = iDivide(iy, ix); /* return a fraction in range 0. to 32767 = 0. to 1. */
else
         iRatio = iDivide(ix, iy); /* return a fraction in range 0. to 32767 = 0. to 1. */
/* first, third and fifth order polynomial approximation */
iAngle = (uint32_t) K1 * (uint32_t) iRatio;
iTmp = ((uint32_t) iRatio >> 5) * ((uint32_t) iRatio >> 5) * ((uint32_t) iRatio >> 5);
iAngle += (iTmp >> 15) * (uint32_t) K2;
iTmp = (iTmp >> 20) * ((uint32_t) iRatio >> 5) * ((uint32_t) iRatio >> 5);
iAngle += (iTmp >> 15) * (uint32_t) K3;
iAngle = iAngle >> 15;
/* check if above 45 degrees */
if (iy > ix) iAngle = (uint16_t)(9000 - iAngle);
/* for tidiness, limit result to range 0 to 9000 equals 0.0 to 90.0 degrees */
if (iAngle < 0) iAngle = 0;
if (iAngle > 9000) iAngle = 9000;
return ((uint16_t) iAngle);
}



/* calculates 100*atan2(iy/ix)=100*atan2(iy,ix) in deg for ix, iy in range -32768 to 32767 */
static uint16_t iHundredAtan2Deg(uint16_t iy, uint16_t ix)
{
uint16_t iResult;    /* angle in degrees times 100 */
/* check for -32768 which is not handled correctly */
if (ix == -32768) ix = -32767;
if (iy == -32768) iy = -32767;
/* check for quadrants */
if ((ix >= 0) && (iy >= 0)) /* range 0 to 90 degrees */
iResult = iHundredAtanDeg(iy, ix);
else if ((ix <= 0) && (iy >= 0)) /* range 90 to 180 degrees */
iResult = (uint16_t)(18000 - (uint16_t)iHundredAtanDeg(iy, (uint16_t)-ix)); else if ((ix <= 0) && (iy <= 0)) /* range -180 to -90 degrees */
iResult = (uint16_t)((uint16_t)-18000 + iHundredAtanDeg((uint16_t)-iy, (uint16_t)-ix));
else /* ix >=0 and iy <= 0 giving range -90 to 0 degrees */
         iResult = (uint16_t)(-iHundredAtanDeg((uint16_t)-iy, ix));
return (iResult);
}


/* tilt-compensated e-Compass code */
static void iecompass(uint16_t iBpx, uint16_t iBpy, uint16_t iBpz,
                          uint16_t iGpx, uint16_t iGpy, uint16_t iGpz)
{
/* stack variables */
/* iBpx, iBpy, iBpz: the three components of the magnetometer sensor */
/* iGpx, iGpy, iGpz: the three components of the accelerometer sensor */
/* local variables */
uint16_t iSin, iCos;         /* sine and cosine */
/* calculate current roll angle Phi */
iPhi = iHundredAtan2Deg(iGpy, iGpz);/* Eq 13 */
/* calculate sin and cosine of roll angle Phi */
iSin = iTrig(iGpy, iGpz);  /* Eq 13: sin = opposite / hypotenuse */
iCos = iTrig(iGpz, iGpy);  /* Eq 13: cos = adjacent / hypotenuse */
/* de-rotate by roll angle Phi */
iBfy = (uint16_t)((iBpy * iCos - iBpz * iSin) >> 15);/* Eq 19 y component */
iBpz = (uint16_t)((iBpy * iSin + iBpz * iCos) >> 15);/* Bpy*sin(Phi)+Bpz*cos(Phi)*/
iGpz = (uint16_t)((iGpy * iSin + iGpz * iCos) >> 15);/* Eq 15 denominator */
/* calculate current pitch angle Theta */
iThe = iHundredAtan2Deg((uint16_t)-iGpx, iGpz);/* Eq 15 */
/* restrict pitch angle to range -90 to 90 degrees */
if (iThe > 9000) iThe = (uint16_t) (18000 - iThe);
if (iThe < -9000) iThe = (uint16_t) (-18000 - iThe);
/* calculate sin and cosine of pitch angle Theta */
iSin = (uint16_t)-iTrig(iGpx, iGpz);    /* Eq 15: sin = opposite / hypotenuse */
iCos = iTrig(iGpz, iGpx);            /* Eq 15: cos = adjacent / hypotenuse */
/* correct cosine if pitch not in range -90 to 90 degrees */
if (iCos < 0) iCos = (uint16_t)-iCos;
/* de-rotate by pitch angle Theta */
iBfx = (uint16_t)((iBpx * iCos + iBpz * iSin) >> 15); /* Eq 19: x component */
iBfz = (uint16_t)((-iBpx * iSin + iBpz * iCos) >> 15);/* Eq 19: z component */
/* calculate current yaw = e-compass angle Psi */
iPsi = iHundredAtan2Deg((uint16_t)-iBfy, iBfx);  /* Eq 22 */
}

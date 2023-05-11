/*
 * data.c
 *
 *  Created on: Nov 1, 2022
 *      Author: quentinrolland
 */

#include "data.h"
#include "types.h"

static TypeDataCansat systemData;


TypeDataCansat *DATA_getDataCansat(void) { return &systemData; }

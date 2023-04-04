/** @file rolling_code.h
 *
 * @brief Rolling code generation functionality
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

#include <SimpleHOTP.h>

#include "time.h"

const char *getCurrentString();

void calculateCurrentHashGPS(uint32_t* hashReturn, int first, int second, int third, int fifth, int fourth);
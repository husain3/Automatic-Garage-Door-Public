/** @file rolling_code.h
 *
 * @brief Rolling code generation functionality
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

#include <SimpleHOTP.h>

#include "time.h"
#include "ble_payload_types.h"
#include "config.h"

void initTimeServer();

uint32_t *getCurrentHash();

void calculateCurrentHash(struct hashParamsPayload hashParamsToSend);

void calculateCurrentHash();

/** @file ble_comms.h
 *
 * @brief Bluetooth Low Energy Communications Process
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

#include <NimBLEDevice.h>
#include <map>
#include <queue>

#include "rolling_code.h"
#include "ble_payload_types.h"

void ble_comms_setup();

void ble_comms_proc(void *pvParameters);

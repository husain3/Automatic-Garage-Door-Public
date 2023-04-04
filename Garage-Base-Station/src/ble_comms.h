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
#include "vehicle_door_control.h"
#include "config.h"

void ble_comms_setup();

double get_door_close_distance_threshold();

void ble_comms_proc(void *pvParameters);

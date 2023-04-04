/** @file vehicle_door_control.h
 *
 * @brief Automatic Door Control from Vehicle
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

#include "door_control.h"
#include "rolling_code.h"
#include "ble_payload_types.h"

uint32_t getLastSuccessfulVerify();

bool authenticate_received_hash_ble(struct authenticationPayload authPayload);

bool authenticate_received_hash_nrf24(struct hashPayload *authPayload);

void disallowDoorOpen();

void allowDoorOpen();

bool isDoorOpenAllowed();

void disallowDoorClose();

void allowDoorClose();

bool isDoorCloseAllowed();

bool isDoorOpen();

bool isDoorClosed();

bool openGarageDoor();

bool closeGarageDoor();
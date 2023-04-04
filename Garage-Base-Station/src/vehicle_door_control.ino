/** @file vehicle_door_control.ino
 *
 * @brief Automatic Door Control from Vehicle
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

#include "vehicle_door_control.h"

bool doorOpenAllowed = true;
bool doorCloseAllowed = true;

unsigned long lastSuccessfulVerify = 0;

uint32_t last_authenticated_hash[5] = {};

uint32_t getLastSuccessfulVerify()
{
	return lastSuccessfulVerify;
}

bool authenticate_received_hash_ble(struct authenticationPayload authPayload)
{
	uint32_t *current_hash = getCurrentHash();
	if (current_hash[0] == 0)
	{
		return false;
	}

	if(current_hash[0] == authPayload.hash[0] && current_hash[1] == authPayload.hash[1] && current_hash[2] == authPayload.hash[2] && current_hash[3] == authPayload.hash[3] && current_hash[4] == authPayload.hash[4]) {
		if(last_authenticated_hash[0] != authPayload.hash[0] && last_authenticated_hash[1] != authPayload.hash[1] && last_authenticated_hash[2] != authPayload.hash[2] && last_authenticated_hash[3] != authPayload.hash[3] && last_authenticated_hash[4] != authPayload.hash[4]) {
			Serial.println("AUTHENTICATED");
			last_authenticated_hash[0] = authPayload.hash[0];
			last_authenticated_hash[1] = authPayload.hash[1];
			last_authenticated_hash[2] = authPayload.hash[2];
			last_authenticated_hash[3] = authPayload.hash[3];
			last_authenticated_hash[4] = authPayload.hash[4];
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

void disallowDoorOpen()
{
	doorOpenAllowed = false;
}

void allowDoorOpen()
{
	doorOpenAllowed = true;
}

bool isDoorOpenAllowed()
{
	return doorOpenAllowed;
}

void disallowDoorClose()
{
	doorCloseAllowed = false;
}

void allowDoorClose()
{
	doorCloseAllowed = true;
}

bool isDoorCloseAllowed()
{
	return doorCloseAllowed;
}

bool isDoorClosed()
{
	return (lowerSensorStatus == HIGH && upperSensorStatus == LOW);
}

bool isDoorOpen()
{
	return (lowerSensorStatus == LOW && upperSensorStatus == HIGH);
}

bool openGarageDoor()
{
	Serial.printf("isDoorClosed() %d, isDoorOpen() %d, isDoorOpenAllowed() %d\n", isDoorClosed(), isDoorOpen(), isDoorOpenAllowed());
	if (isDoorClosed() && isDoorOpenAllowed())
	{
		Serial.println("OPEN GARAGE DOOR");
		Serial.println("OPEN GARAGE DOOR");
		Serial.println("OPEN GARAGE DOOR");
		Serial.println("OPEN GARAGE DOOR");

		toggleGarageDoor();
		disallowDoorOpen();
		return true;
	}
	return false;
}

bool closeGarageDoor()
{
	if (isDoorOpen() && isDoorCloseAllowed())
	{
		Serial.println("CLOSE GARAGE DOOR");
		Serial.println("CLOSE GARAGE DOOR");
		Serial.println("CLOSE GARAGE DOOR");
		Serial.println("CLOSE GARAGE DOOR");

		toggleGarageDoor();

		disallowDoorOpen();
		disallowDoorClose();
		return true;
	}
	return false;
}
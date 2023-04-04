/** @file door_control.ino
 *
 * @brief Door control functionality
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

#include "door_control.h"

void toggleGarageDoor()
{
	digitalWrite(relay, LOW);
	delay(1000);
	digitalWrite(relay, HIGH);
}

// MQTT Garage door control
unsigned long prevMillis = 0;
void handleToggleMsg(const String &payload)
{
	unsigned long currentMillis = millis();

	/* If first call OR over 5 seconds since last MQTT toggle */
	if (prevMillis == 0 || (currentMillis - prevMillis > 5000))
	{
		/* If garage door is currently closed */
		if (lowerSensorStatus == HIGH)
		{
			Serial.println(payload);
			toggleGarageDoor();
			prevMillis = currentMillis;
		}
	}
}
/** @file wifi_connect.ino
 *
 * @brief Wifi connection functionality
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

// #define DEBUG_WIFI //Uncomment to enable debug statements

#include "wifi_connect.h"
#include "config.h"

void connectToWifi()
{

#ifdef DEBUG_WIFI
	Serial.print("Connecting to WiFi");
#endif

	WiFi.begin(wifi_hotspot, wifi_password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(100);

#ifdef DEBUG_WIFI
		Serial.print(".");
#endif
	}

#ifdef DEBUG_WIFI
	Serial.println(" Connected!");
	Serial.println(WiFi.localIP());
#endif
}

void checkAndReconnect()
{

#ifdef DEBUG_WIFI
	Serial.println("Checking WIFI status");
#endif

	if ((WiFi.status() != WL_CONNECTED))
	{

#ifdef DEBUG_WIFI
		Serial.print(millis());
		Serial.println("Reconnecting to WiFi...");
#endif

		WiFi.disconnect();
		WiFi.reconnect();
	}
}
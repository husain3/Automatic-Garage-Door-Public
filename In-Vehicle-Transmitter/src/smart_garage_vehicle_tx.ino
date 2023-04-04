/** @file smart_garage_vehicle_tx.ino
 *
 * @brief Smart Garage Vehicle TX Main Thread
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

// #define DEBUG_MAIN //Uncomment to enable debug statements
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include "wifi_connect.h"
#include "rolling_code.h"
#include "ble_comms.h"

int counter = 0;

AsyncWebServer server(80);

unsigned long previousWifiConnectedCheckMillis = 0;
const long interval = 10000;

void setup() {
	xTaskCreate(&ble_comms_proc, "ble_comms_proc", 3072, NULL, 3, NULL);
	Serial.begin(115200);

	connectToWifi();

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
		{ request->send(200, "text/plain", "Hi! I am the in-vehicle transmitter."); });

	AsyncElegantOTA.begin(&server); // Start ElegantOTA
	server.begin();
}

void loop() {
	/////////////////////////////WIFICHEK////////////////////////////////
	unsigned long currentWifiConnectedCheckMillis = millis();
	if (currentWifiConnectedCheckMillis - previousWifiConnectedCheckMillis >= interval + 500 + 9000)
	{
		previousWifiConnectedCheckMillis = currentWifiConnectedCheckMillis;
		checkAndReconnect();
	}
	/////////////////////////////WIFICHEK////////////////////////////////
}

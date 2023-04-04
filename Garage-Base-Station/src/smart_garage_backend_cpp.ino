/** @file smart_garage_backend_cpp.ino
 *
 * @brief Smart Garage Backend Main Thread
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

// #define DEBUG_MAIN //Uncomment to enable debug statements
#include <esp_rom_sys.h>
#include <EspMQTTClient.h>
#include <WiFiUdp.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include "wifi_connect.h"
#include "http_server.h"
#include "rolling_code.h"
#include "door_control.h"
#include "ble_comms.h"

#include "config.h"

const int relay = 26;

const int lowerSensorInterruptPin = 17;
volatile int lowerSensorinterruptOccurrences = 0;
int lowerSensorStatus;

const int upperSensorInterruptPin = 16;
volatile int upperSensorinterruptOccurrences = 0;
int upperSensorStatus;

unsigned long previousMillis = 0;
unsigned long previousCodeGenMillis = 0;
unsigned long previousWifiConnectedCheckMillis = 0;
const long interval = 500;

// OTA firmware update web server
AsyncWebServer server(80);

portMUX_TYPE mux1 = portMUX_INITIALIZER_UNLOCKED;

EspMQTTClient client(
	mqttServerIp, // MQTT Broker server ip
	mqttServerPort,
	mqttUsername,  // Can be omitted if not needed
	mqttPassword,  // Can be omitted if not needed
	mqttClientName // Client name that uniquely identify your device
);

void IRAM_ATTR handleLowerSensorInterrupt()
{
	portENTER_CRITICAL_ISR(&mux1);
	lowerSensorinterruptOccurrences++;
	portEXIT_CRITICAL_ISR(&mux1);
}

portMUX_TYPE mux2 = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR handleUpperSensorInterrupt()
{
	portENTER_CRITICAL_ISR(&mux2);
	upperSensorinterruptOccurrences++;
	portEXIT_CRITICAL_ISR(&mux2);
}

void setup()
{
	Serial.begin(115200);
	Serial.println("Smart Garage Backend");

	lowerSensorStatus = digitalRead(lowerSensorInterruptPin);
	upperSensorStatus = digitalRead(upperSensorInterruptPin);

	xTaskCreate(&ble_comms_proc, "ble_comms_proc", 3072, NULL, 3, NULL);

	connectToWifi();

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(200, "text/plain", "Hi! I am the smart garage door opener."); });

	AsyncElegantOTA.begin(&server); // Start ElegantOTA
	server.begin();

	xTaskCreate(&generate_json, "json", 4096, NULL, 0, NULL);
	xTaskCreate(&http_server, "http_server", 4096, NULL, 1, NULL);

	pinMode(relay, OUTPUT);
	digitalWrite(relay, HIGH);
	pinMode(lowerSensorInterruptPin, INPUT_PULLUP);
	pinMode(upperSensorInterruptPin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(lowerSensorInterruptPin), handleLowerSensorInterrupt, CHANGE);
	attachInterrupt(digitalPinToInterrupt(upperSensorInterruptPin), handleUpperSensorInterrupt, CHANGE);
}

void onConnectionEstablished()
{

	client.subscribe("homeassistant/garageespcontrol", handleToggleMsg);
	client.publish("homeassistant/garageespnotify", "GarageESP32 now online!");
}

void loop()
{
	unsigned long currentMillis = millis();
	if ((lowerSensorinterruptOccurrences > 0 || upperSensorinterruptOccurrences > 0) && currentMillis - previousMillis >= interval)
	{
		previousMillis = currentMillis;

		if (lowerSensorinterruptOccurrences > 0)
		{
			portENTER_CRITICAL(&mux1);
			lowerSensorinterruptOccurrences = 0;
			portEXIT_CRITICAL(&mux1);

			if (digitalRead(lowerSensorInterruptPin) != lowerSensorStatus)
			{
				lowerSensorStatus = digitalRead(lowerSensorInterruptPin);
				if (lowerSensorStatus == HIGH)
				{

#ifdef DEBUG_MAIN
					Serial.println("lowerSensor HIGH");
#endif

					client.publish("homeassistant/garageespnotify", "Garage Door Open");
				}
				else
				{

#ifdef DEBUG_MAIN
					Serial.println("lowerSensor LOW");
#endif

					client.publish("homeassistant/garageespnotify", "Garage Door Closed");
				}
			}
		}
		else
		{
			portENTER_CRITICAL(&mux2);
			upperSensorinterruptOccurrences = 0;
			portEXIT_CRITICAL(&mux2);

			if (digitalRead(upperSensorInterruptPin) != upperSensorStatus)
			{
#ifdef DEBUG_MAIN
				Serial.println("upperSensor");
#endif

				upperSensorStatus = digitalRead(upperSensorInterruptPin);
				if (upperSensorStatus == HIGH)
				{

#ifdef DEBUG_MAIN
					Serial.println("upperSensor HIGH");
#endif
				}
				else
				{

#ifdef DEBUG_MAIN
					Serial.println("upperSensor LOW");
#endif
				}
			}
		}
	}

	client.loop();

	/////////////////////////////WIFICHEK////////////////////////////////
	unsigned long currentWifiConnectedCheckMillis = millis();
	if (currentWifiConnectedCheckMillis - previousWifiConnectedCheckMillis >= interval + 500 + 9000)
	{
		previousWifiConnectedCheckMillis = currentWifiConnectedCheckMillis;
		checkAndReconnect();
	}
	/////////////////////////////WIFICHEK////////////////////////////////
	delay(1000);
}

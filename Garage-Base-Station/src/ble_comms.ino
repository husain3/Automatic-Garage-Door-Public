/** @file ble_comms.ino
 *
 * @brief Bluetooth Low Energy Communications Process
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

// #define DEBUG_BLE //Uncomment to enable debug statements
#include "ble_comms.h"

// The remote service we wish to connect to.
static BLEUUID authorizedVehicleServiceUUID("AAAA");

// The characteristics of the remote service we are interested in.
static BLEUUID dateTimeCharacteristicUUID("BBBB");
static BLEUUID hashCharacteristicUUID("CCCC");
static BLEUUID aliveTimeCharacteristicUUID("DDDD");

NimBLEScan *pBLEScan;

std::queue<NimBLEAddress> deviceToConnect;
std::list<NimBLEClient *> connectedDevices;

struct hashParamsPayload hashParamsToSend;

// TODO: Need to read distance from non-volatile memory to persist between reboots
double door_close_distance_threshold = 18.0;

double get_door_close_distance_threshold()
{
	return door_close_distance_threshold;
}

static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
#ifdef DEBUG_BLE
	Serial.println("Notify callback for characteristic ");
	Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
	Serial.print(" of data length ");
	Serial.println(length);
#endif

	authenticationPayload retrievedKey;
	memcpy(retrievedKey.hash, pData, length);

	if (authenticate_received_hash_ble(retrievedKey))
	{
#ifdef DEBUG_BLE
		Serial.println("OPEN DOOR BLE");
		Serial.println("OPEN DOOR BLE");
		Serial.println("OPEN DOOR BLE");
		Serial.println("OPEN DOOR BLE");
		Serial.println("OPEN DOOR BLE");
#endif
		openGarageDoor();
	}
}

static constexpr float reference_power = -50;
static constexpr float distance_factor = 3.5;

int rssi_average = 0;
int consecPassedThreshold = 0;
void rssiToDistance(int current_rssi)
{
#ifdef VERBOSE
	show_address(remote_addr);
#endif

	// Adjust damping_factor to lower values to have a more reactive response
	const float damping_factor = 1;
	rssi_average = rssi_average * damping_factor +
				   current_rssi * (1 - damping_factor);

	float avg_distance = pow(10, (reference_power - rssi_average) / (10 * distance_factor));

#ifdef VERBOSE
	Serial.printf(", rssi=%hi",
#else
	Serial.printf("%hi\n",
#endif
				  current_rssi);

	if (avg_distance > door_close_distance_threshold)
	{
		consecPassedThreshold++;
		if (consecPassedThreshold >= 3 && isDoorCloseAllowed() == true)
		{
			unsigned long passedThresholdTime = millis();

			while (millis() - passedThresholdTime < 3000)
			{
				Serial.println("passedThresholdTime");
				Serial.println(passedThresholdTime);
				Serial.println("getLastSuccessfulVerify()");
				Serial.println(getLastSuccessfulVerify());
				if (getLastSuccessfulVerify() > passedThresholdTime)
				{
					// closeGarageDoor();
					break;
				}
				delay(500);
			}
		}
	}
	else
	{
		consecPassedThreshold = 0;
	}

#ifdef VERBOSE
	Serial.printf("\n");
#endif
}

class Monitor : public BLEClientCallbacks
{
public:
	int16_t connection_id;

	/* dBm to distance parameters; How to update distance_factor 1.place the
	 * phone at a known distance (2m, 3m, 5m, 10m) 2.average about 10 RSSI
	 * values for each of these distances, Set distance_factor so that the
	 * calculated distance approaches the actual distances, e.g. at 5m. */
	static constexpr float reference_power = -50;
	static constexpr float distance_factor = 3.5;

	static float get_distance(const int8_t rssi)
	{
		return pow(10, (reference_power - rssi) / (10 * distance_factor));
	}

	void onConnect(BLEClient *pClient)
	{
		Serial.println("onConnect");
		connectedDevices.push_back(pClient);
		pClient->updateConnParams(240, 240, 5, 60);
		if (connectedDevices.size() < 2)
		{
			rssi_average = pClient->getRssi();
		}
	}

	void onDisconnect(BLEClient *pClient)
	{
		Serial.println("onDisconnect");
	}
	/***************** New - Security handled here ********************
	****** Note: these are the same return values as defaults ********/
	uint32_t onPassKeyRequest()
	{
		Serial.println("Client PassKeyRequest");
		return 123456;
	}
	bool onConfirmPIN(uint32_t pass_key)
	{
		Serial.print("The passkey YES/NO number: ");
		Serial.println(pass_key);
		return true;
	}

	void onAuthenticationComplete(ble_gap_conn_desc *desc)
	{
		Serial.println("Starting BLE work!");
	}
	/*******************************************************************/
};

class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks
{
	void onResult(NimBLEAdvertisedDevice *advertisedDevice)
	{
#ifdef DEBUG_BLE
		Serial.printf("Advertised Device: %s \n", advertisedDevice->toString().c_str());
#endif
		if (advertisedDevice->haveServiceUUID() && advertisedDevice->isAdvertisingService(authorizedVehicleServiceUUID))
		{
			deviceToConnect.push(advertisedDevice->getAddress());
		}
	}
};

bool connectToServer()
{
	esp_task_wdt_reset();
	NimBLEClient *pClient = nullptr;

	NimBLEAddress currentDevice = deviceToConnect.front();
	deviceToConnect.pop();

	Serial.print("Forming a connection to ");
	Serial.println(currentDevice.toString().c_str());

	/** Check if we have a client we should reuse first **/
	if (NimBLEDevice::getClientListSize())
	{
		/** Special case when we already know this device, we send false as the
		 *  second argument in connect() to prevent refreshing the service database.
		 *  This saves considerable time and power.
		 */
		pClient = NimBLEDevice::getClientByPeerAddress(currentDevice);
		if (pClient)
		{
			if (!pClient->connect(currentDevice, false))
			{
				Serial.println("Reconnect failed");
				return false;
			}
			Serial.println("Reconnected client");
		}
	}

	if (!pClient)
	{
		pClient = BLEDevice::createClient();
		pClient->setClientCallbacks(new Monitor());
		Serial.println(" - Created client");

		if (pClient->connect(currentDevice))
			Serial.println(" - Connected to server");
	}

	if(lowerSensorStatus == LOW) {
		Serial.printf("isDoorOpenAllowed() %d\n", isDoorOpenAllowed());
		Serial.println("DOOR ALREADY OPEN. DISABLING DOOR OPEN..");
		disallowDoorOpen();
		Serial.printf("isDoorOpenAllowed() %d\n", isDoorOpenAllowed());
	}


	// Obtain a reference to the service we are after in the remote BLE server.
	BLERemoteService *pAuthorizedVehicleService = pClient->getService(authorizedVehicleServiceUUID);
	if (pAuthorizedVehicleService == nullptr)
	{
		Serial.print("Failed to find our service UUID: ");
		Serial.println(authorizedVehicleServiceUUID.toString().c_str());
		pClient->disconnect();
		return false;
	}
	Serial.println(" - Found our service");

	// Obtain a reference to the characteristic in the service of the remote BLE server.
	NimBLERemoteCharacteristic *pDateTimeCharacteristic;
	NimBLERemoteCharacteristic *pHashCharacteristic;
	NimBLERemoteCharacteristic *pAliveTimeCharacteristic;
	if (pAuthorizedVehicleService != nullptr)
	{
		pDateTimeCharacteristic = pAuthorizedVehicleService->getCharacteristic(dateTimeCharacteristicUUID);
		pHashCharacteristic = pAuthorizedVehicleService->getCharacteristic(hashCharacteristicUUID);
		pAliveTimeCharacteristic = pAuthorizedVehicleService->getCharacteristic(aliveTimeCharacteristicUUID);
	}
	else
	{
		return false;
	}

	if (pDateTimeCharacteristic == nullptr || pHashCharacteristic == nullptr || pAliveTimeCharacteristic == nullptr)
	{
		Serial.print("Failed to find all characteristics: ");
		if (pDateTimeCharacteristic == nullptr)
			Serial.println("BBBB");
		if (pHashCharacteristic == nullptr)
			Serial.println("CCCC");
		if (pAliveTimeCharacteristic == nullptr)
			Serial.println("DDDD");

		pClient->disconnect();
		return false;
	}
	Serial.println(" - Found our characteristics");

	if (pAliveTimeCharacteristic->canRead())
	{
		if (pAliveTimeCharacteristic->readValue<long>() > 20000)
		{
			Serial.println("VEHICLE APPROACHING... DISABLE DOOR CLOSE");
			disallowDoorClose();
		}
	}

	if (pDateTimeCharacteristic->canWrite())
	{
		if (pDateTimeCharacteristic->writeValue<hashParamsPayload>(hashParamsToSend, true))
			Serial.println("Hash params payload sent!");
	}

	if (pHashCharacteristic->canNotify())
	{
		if (!pHashCharacteristic->subscribe(true, notifyCallback))
		{
			pClient->disconnect();
			return false;
		}
		Serial.println(" - Found our callback characteristic");
	}

	return true;
}

#define SCAN_DURATION 120

void ble_comms_setup()
{
	Serial.println("Scanning...");

	NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DEVICE);

	NimBLEDevice::init("");

	/** Optional: set the transmit power, default is 3db */
#ifdef ESP_PLATFORM
	NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
#else
	NimBLEDevice::setPower(9); /** +9db */
#endif
	esp_task_wdt_init(SCAN_DURATION + 2, true);
	esp_task_wdt_add(NULL);

	if(lowerSensorStatus == 0) {
		Serial.printf("isDoorOpenAllowed() %d\n", isDoorOpenAllowed());
		Serial.println("DOOR ALREADY OPEN. DISABLING DOOR OPEN..");
		Serial.println(lowerSensorStatus);
		disallowDoorOpen();
		Serial.printf("isDoorOpenAllowed() %d\n", isDoorOpenAllowed());
	}

	pBLEScan = NimBLEDevice::getScan(); // create new scan
	// Set the callback for when devices are discovered, no duplicates.
	pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), false);
	pBLEScan->setActiveScan(true); // Set active scanning, this will get more data from the advertiser.
	pBLEScan->setInterval(75);	   // How often the scan occurs / switches channels; in milliseconds,
	pBLEScan->setWindow(50);	   // How long to scan during the interval; in milliseconds.
	pBLEScan->setMaxResults(0);	   // do not store the scan results, use callback only.
}

long lastWrittenTime = 0;
int offlineTick = 0;
void ble_comms_proc(void *pvParameters)
{
	ble_comms_setup();
	int wifi_timeout_counter = 0;
	while (1)
	{
		if (!deviceToConnect.empty())
		{
			if (connectToServer())
			{
				Serial.println("We are now connected to the BLE Server.");
			}
			else
			{
				Serial.println("We have failed to connect to the server; there is nothin more we will do.");
			}
		}

		// If an error occurs that stops the scan, it will be restarted here.
		if (pBLEScan->isScanning() == false)
		{
			Serial.println("RESTARTING BLE SCAN");
			// Start scan with: duration = 0 seconds(forever), no scan end callback, not a continuation of a previous scan.
			pBLEScan->start(SCAN_DURATION, nullptr, false);
			esp_task_wdt_reset();
		}

		long currentTime = millis();
		if (currentTime - lastWrittenTime > 1000)
		{
			hashParamsToSend.first = 1900 + (rand() % 200);
			hashParamsToSend.second = rand() % (int) (sizeof(rollingSaltSet1)/sizeof(rollingSaltSet1[0]));
			hashParamsToSend.third = rand() % (int) (sizeof(rollingSaltSet2)/sizeof(rollingSaltSet2[0]));
			hashParamsToSend.fourth = rand() % 59;
			hashParamsToSend.fifth = rand() % (int) (sizeof(rollingSaltSet3)/sizeof(rollingSaltSet3[0]));;
			hashParamsToSend.sixth = rand() % 59;

#ifdef DEBUG_BLE
			Serial.println(hashParamsToSend.first);
			Serial.println(hashParamsToSend.second);
			Serial.println(hashParamsToSend.third);
			Serial.println(hashParamsToSend.fourth);
			Serial.println(hashParamsToSend.fifth);
			Serial.println(hashParamsToSend.sixth);
			Serial.println(rollingSaltSet1[hashParamsToSend.second]);
			Serial.println(rollingSaltSet2[hashParamsToSend.third]);
			Serial.println(rollingSaltSet3[hashParamsToSend.fifth]);
			Serial.println();
#endif

			if (!connectedDevices.empty())
			{
				esp_task_wdt_reset();
				offlineTick = 0;
				calculateCurrentHash(hashParamsToSend);
				for (std::list<NimBLEClient *>::iterator it = connectedDevices.begin(); it != connectedDevices.end(); it++)
				{
					NimBLEClient *pClient = *it;
					if (pClient->isConnected())
					{
						NimBLERemoteService *pSvc = pClient->getService("AAAA");
						if (pSvc)
						{
							NimBLERemoteCharacteristic *pChr = pSvc->getCharacteristic("BBBB");
							if (pChr->canWrite())
							{
								if (pChr->writeValue<hashParamsPayload>(hashParamsToSend, true))
									// Serial.println("Time written to client");
									Serial.print("");
							}
						}
					}
					else
					{
						connectedDevices.erase(it);
					}
				}
				lastWrittenTime = currentTime;
			}
			else
			{
				if(lowerSensorStatus == HIGH) {
					offlineTick++;
					if(offlineTick > 30) {
						if (!isDoorOpenAllowed()) {
							Serial.println("UNLOCK DOOR OPEN");
							allowDoorOpen();
						}

						if (!isDoorCloseAllowed()) {
							Serial.println("UNLOCK DOOR CLOSE");
							allowDoorClose();
						}
					}
				}
			}
		}

		if (connectedDevices.size() == 1 && isDoorOpen())
		{
			// Serial.println("RSSI Reading");
			Serial.println(connectedDevices.front()->getRssi());
			delay(62);
		}
		else
		{
			delay(500);
		}

		if(isDoorOpenAllowed() && connectedDevices.size() == 0 && millis() > 480000) {
			ESP.restart();
		}
	}
}
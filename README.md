# Automatic-Garage-Door
Public demo code for ESP32 BLE-based garage door opener + HomeAssistant + REST API

## Features

* Bluetooth-based rolling-code authentication between In-Vehicle-Transmitter and Garage-Base-Station
* MQTT backend for integration with Home Automation
* HTTP backend for monitoring and control

## Instructions

1. Add your wifi, MQTT, and/or webpage button credentials to `server_creds.yaml`

2. Install python script dependencies with `python3 -m pip install -r requirements.txt`

3. Run config.h generator script using `python3 config-gen.py`

4. Open Garage-Base-Station in Visual Studio Code with platformio extension to build project

5. Open In-Vehicle-Tramsitter in Visual Studio Code with platformio extension to build project

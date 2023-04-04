/** @file wifi_connect.h
 *
 * @brief Wifi connection functionality
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <WiFi.h>

extern "C" void connectToWifi();

extern "C" void checkAndReconnect();

#endif /* WIFI_CONNECT_H */

/*** end of file ***/

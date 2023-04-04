/** @file ble_payload_types.h
 *
 * @brief Payload types used in nrf24 communications
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

#ifndef BLE_PAYLOAD_TYPES_H
#define BLE_PAYLOAD_TYPES_H

struct ackPayload
{
	uint16_t year;
	uint8_t mon;
	uint8_t day;
	uint8_t min;
	uint8_t hour;
	uint8_t sec;
};

struct hashPayload
{
	uint32_t hash[5];
	uint32_t time_since_boot;
};

struct hashParamsPayload
{
	uint16_t first;
	uint8_t second;
	uint8_t third;
	uint8_t fourth;
	uint8_t fifth;
	uint8_t sixth;
};

struct authenticationPayload
{
	uint32_t hash[5];
};

#endif

/* BLE_PAYLOAD_TYPES_H */

/*** end of file ***/
/** @file rolling_code.ino
 *
 * @brief Rolling code generation functionality
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

//#define DEBUG_ROLLINGCODE //Uncomment to enable debug statements

#include "rolling_code.h"

uint32_t current_hash[5] = {};
struct tm current_timeinfo;

uint32_t *getCurrentHash()
{
	return current_hash;
}

void calculateCurrentHash(struct hashParamsPayload hashParamsToSend)
{
	String concatenate = rollingSaltSet1[hashParamsToSend.second] + hashParamsToSend.second + hashParamsToSend.first + rollingSaltSet3[hashParamsToSend.fifth] + hashParamsToSend.third + hashParamsToSend.fourth + hashParamsToSend.fifth + rollingSaltSet2[hashParamsToSend.third];

#ifdef DEBUG_ROLLINGCODE
	Serial.println(concatenate);
#endif

	int m2 = concatenate.length(); // number of BYTES of the message
	char msgArray[m2 + 1];
	concatenate.toCharArray(msgArray, m2 + 1);
	m2 *= 8;

	uint32_t hash[5] = {};
	hashParamsToSend.second += 1;
	SimpleSHA1::generateSHA((uint8_t *)msgArray, m2, hash);
	memcpy(current_hash, hash, sizeof(hash));

#ifdef DEBUG_ROLLINGCODE
	for (int i = 0; i < 5; i++)
	{
		Serial.print(hash[i], HEX);
	}
#endif
}

/** @file rolling_code.ino
 *
 * @brief Rolling code generation functionality
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

//#define DEBUG_ROLLINGCODE //Uncomment to enable debug statements

#include "rolling_code.h"
#include "config.h"

String concatenate;

const char * getCurrentString() {
	return concatenate.c_str();
}

void calculateCurrentHashGPS(uint32_t* hashReturn, int first, int second, int third, int fifth, int fourth) {
	concatenate = rollingSaltSet1[second] + second + first + rollingSaltSet3[fifth] + third + fourth + fifth + rollingSaltSet2[third];

	#ifdef DEBUG_ROLLINGCODE
	Serial.println(concatenate);
	#endif

	int m2 = concatenate.length(); // number of BYTES of the message
	char msgArray[m2+1];
	concatenate.toCharArray(msgArray, m2+1);
	m2 *= 8;

	uint32_t hash[5] = {};
	SimpleSHA1::generateSHA((uint8_t*) msgArray, m2, hash);
	memcpy(hashReturn, hash, sizeof(hash));
}

/** @file http_server.ino
 *
 * @brief HTTP Server functionality
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

// #define DEBUG_HTTP_SERVER //Uncomment to enable debug statements
#include "http_server.h"
#include "door_control.h"
#include "vehicle_door_control.h"
#include "config.h"
#include "ble_comms.h"

#define LED_BUILTIN GPIO_NUM_1
char *json_unformatted;

const static char http_html_hdr[] =
	"HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-type: text/html\r\n\r\n";

const static char http_index_hml[] = "<!DOCTYPE html>"
									 "<html>\n"
									 "<head>\n"
									 "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
									 "  <style type=\"text/css\">\n"
									 "    html, body, iframe { margin: 0; padding: 0; height: 100%; }\n"
									 "    iframe { display: block; width: 100%; border: none; }\n"
									 "  </style>\n"
									 "<title>Smart Garage Backend 2.0</title>\n"
									 "</head>\n"
									 "<body>\n"
									 "<h1>Hello there, I'm the Smart Garage Backend 2.0 :) </h1>\n"
									 "</body>\n"
									 "</html>\n";

void http_server_netconn_serve(struct netconn *conn)
{
	struct netbuf *inbuf;
	char *buf;
	u16_t buflen;
	err_t err;

	/* Read the data from the port, blocking if nothing yet there.
	We assume the request (the part we care about) is in one netbuf */
	err = netconn_recv(conn, &inbuf);

	if (err == ERR_OK)
	{
		netbuf_data(inbuf, (void **)&buf, &buflen);

		/* Is this an HTTP GET command? (only check the first 5 chars, since
		there are other formats for GET, and we're keeping it very simple )*/
		printf("buffer = %s \n", buf);
		if (buflen >= 5 &&
			buf[0] == 'G' &&
			buf[1] == 'E' &&
			buf[2] == 'T' &&
			buf[3] == ' ' &&
			buf[4] == '/')
		{
#ifdef DEBUG_HTTP_SERVER
			Serial.println("INSIDE GET");
#endif
			printf("buf[5] = %c\n", buf[5]);
			/* Send the HTML header
			 * subtract 1 from the size, since we dont send the \0 in the string
			 * NETCONN_NOCOPY: our data is const static, so no need to copy it
			 */

			netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_NOCOPY);
			if (buflen >= 5 &&
				buf[5] == 'a' &&
				buf[6] == 'l' &&
				buf[7] == 'l')
			{
#ifdef DEBUG_HTTP_SERVER
				Serial.println("INSIDE /all");
#endif
				netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_NOCOPY);
			}
			if (buf[5] == 'h')
			{
				gpio_set_level(LED_BUILTIN, GPIO_NUM_1);
				/* Send our HTML page */
				netconn_write(conn, http_index_hml, sizeof(http_index_hml) - 1, NETCONN_NOCOPY);
			}
			else if (buf[5] == 'l')
			{
				gpio_set_level(LED_BUILTIN, GPIO_NUM_1);
				/* Send our HTML page */
				netconn_write(conn, http_index_hml, sizeof(http_index_hml) - 1, NETCONN_NOCOPY);
			}
			else if (buf[5] == 'j')
			{
				netconn_write(conn, json_unformatted, strlen(json_unformatted), NETCONN_NOCOPY);
			}
			else
			{
				netconn_write(conn, http_index_hml, sizeof(http_index_hml) - 1, NETCONN_NOCOPY);
			}
		}
		else if (buflen >= 5 &&
				 buf[0] == 'P' &&
				 buf[1] == 'O' &&
				 buf[2] == 'S' &&
				 buf[3] == 'T' &&
				 buf[4] == ' ' &&
				 buf[5] == '/')
		{
			Serial.println("INSIDE POST");
			if (buflen >= 5 &&
				buf[6] == 'o' &&
				buf[7] == 'p' &&
				buf[8] == 'e' &&
				buf[9] == 'n' &&
				buf[10] == 'c' &&
				buf[11] == 'l' &&
				buf[12] == 'o' &&
				buf[13] == 's' &&
				buf[14] == 'e')
			{
				Serial.println("INSIDE /openclose");
				netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_NOCOPY);

				if (strstr(buf, http_api_key) != NULL)
				{
					toggleGarageDoor();
				}
			}
		}
		else if (buflen >= 5 &&
				 buf[0] == 'O' &&
				 buf[1] == 'P' &&
				 buf[2] == 'T' &&
				 buf[3] == 'I' &&
				 buf[4] == 'O' &&
				 buf[5] == 'N' &&
				 buf[6] == 'S' &&
				 buf[7] == ' ' &&
				 buf[8] == '/')
		{
			Serial.println("INSIDE OPTIONS");
			const static char http_html_hdr1[] =
				"HTTP/1.1 200 OK\r\nAccess-Control-Allow-Headers: *\r\nAccess-Control-Allow-Origin: *\r\nContent-type: text/html\r\n\r\n";
			netconn_write(conn, http_html_hdr1, sizeof(http_html_hdr1) - 1, NETCONN_NOCOPY);
		}
	}
	/* Close the connection (server closes in HTTP) */
	netconn_close(conn);

	/* Delete the buffer (netconn_recv gives us ownership,
	so we have to make sure to deallocate the buffer) */
	netbuf_delete(inbuf);
}

void http_server(void *pvParameters)
{
#ifdef DEBUG_HTTP_SERVER
		Serial.println("Starting http_server...");
#endif
	struct netconn *conn, *newconn;
	err_t err;
	conn = netconn_new(NETCONN_TCP);
	netconn_bind(conn, NULL, 8000);
	netconn_listen(conn);
	do
	{
		err = netconn_accept(conn, &newconn);
		if (err == ERR_OK)
		{
			http_server_netconn_serve(newconn);
			netconn_delete(newconn);
		}
		delay(3000);
	} while (err == ERR_OK);
	netconn_close(conn);
	netconn_delete(conn);
}

void generate_json(void *pvParameters)
{
	cJSON *root, *info, *d;
	root = cJSON_CreateObject();

	extern int lowerSensorStatus;
	extern int upperSensorStatus;

	/*Add door sensor information to JSON*/
	cJSON_AddItemToObject(root, "door_sensor", d = cJSON_CreateObject());
	if (lowerSensorStatus == HIGH)
	{
		cJSON_AddStringToObject(d, "lowerSensorStatus", "HIGH");
	}
	else
	{
		cJSON_AddStringToObject(d, "lowerSensorStatus", "LOW");
	}

	if (upperSensorStatus == HIGH)
	{
		cJSON_AddStringToObject(d, "upperSensorStatus", "HIGH");
	}
	else
	{
		cJSON_AddStringToObject(d, "upperSensorStatus", "LOW");
	}
	/*Add BLE doorclose distance information to JSON*/
	cJSON_AddItemToObject(root, "ble_doorclose_distance", cJSON_CreateNumber(get_door_close_distance_threshold()));

	/*Add time_since_boot information to JSON*/
	cJSON_AddItemToObject(root, "time_since_boot", cJSON_CreateNumber((double)millis()));
	while (1)
	{
		/*Update door sensor information to JSON*/
		if (lowerSensorStatus == HIGH)
		{
			cJSON_ReplaceItemInObject(d, "lowerSensorStatus", cJSON_CreateString("HIGH"));
		}
		else
		{
			cJSON_ReplaceItemInObject(d, "lowerSensorStatus", cJSON_CreateString("LOW"));
		}

		if (upperSensorStatus == HIGH)
		{
			cJSON_ReplaceItemInObject(d, "upperSensorStatus", cJSON_CreateString("HIGH"));
		}
		else
		{
			cJSON_ReplaceItemInObject(d, "upperSensorStatus", cJSON_CreateString("LOW"));
		}

		/*Update BLE doorclose distance information to JSON*/
		cJSON_ReplaceItemInObject(root, "ble_doorclose_distance", cJSON_CreateNumber(get_door_close_distance_threshold()));

		/*Update time_since_boot information to JSON*/
		cJSON_ReplaceItemInObject(root, "time_since_boot", cJSON_CreateNumber((double)millis()));

		json_unformatted = cJSON_PrintUnformatted(root);
#ifdef DEBUG_HTTP_SERVER
		printf("[len = %d]  ", strlen(json_unformatted));
		printf("[isDoorOpenAllowed = %d]  [isDoorCloseAllowed = %d]\n", isDoorOpenAllowed(), isDoorCloseAllowed());
		printf("\n");
#endif

#ifdef DEBUG_HTTP_SERVER
		for (int var = 0; var < strlen(json_unformatted); ++var)
		{
			putc(json_unformatted[var], stdout);
		}
		printf("\n");
#endif
		fflush(stdout);
		delay(4000);
		free(json_unformatted);
	}
}

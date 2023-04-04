/** @file http_server.h
 *
 * @brief HTTP Server functionality
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "lwip/api.h"
#include "lwip/err.h"

#include "cJSON.h"
// #include <cjson/cJSON.h>

extern "C" void http_server_netconn_serve(struct netconn *conn);

extern "C" void http_server(void *pvParameters);

extern "C" void generate_json(void *pvParameters);

#endif

/* HTTP_SERVER_H */

/*** end of file ***/

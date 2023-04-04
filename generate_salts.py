from random import choice
from string import ascii_uppercase, ascii_lowercase, digits
import yaml
import base64

def rollingSaltGenerator(name, size):
	rollingSaltSet = f"String {name}[] = {{"
	for i in range(size):
		rollingSaltSet += "\""
		rollingSaltSet +=''.join(choice(ascii_uppercase + ascii_lowercase + digits) for i in range(12))
		rollingSaltSet += "\""

		if i < size - 1:
			rollingSaltSet += ", "

		if(i % 4 == 0):
			rollingSaltSet += "\n"
	rollingSaltSet += "};"

	return rollingSaltSet

with open('server_creds.yaml', 'r') as creds_yaml_file:
	creds_info = yaml.safe_load(creds_yaml_file);


username_pass = f"{creds_info['webapp_credentials']['username']}:{creds_info['webapp_credentials']['password']}"
http_api_key = base64.b64encode(str.encode(username_pass)).decode("ascii")

wifi_network_name = creds_info['wifi_credentials']['network']
wifi_password = creds_info['wifi_credentials']['password']


mqtt_server_ip = creds_info['mqtt_server_info']['ip']
mqtt_server_port = creds_info['mqtt_server_info']['port']
mqtt_username = creds_info['mqtt_server_info']['username']
mqtt_password = creds_info['mqtt_server_info']['password']
mqtt_client_name = creds_info['mqtt_server_info']['clientname']

#Zero indexing
number_of_months = 11
number_of_days = 31
number_of_hours = 23


rollingSaltSet1 = rollingSaltGenerator("rollingSaltSet1", number_of_months)
rollingSaltSet2 = rollingSaltGenerator("rollingSaltSet2", number_of_days)
rollingSaltSet3 = rollingSaltGenerator("rollingSaltSet3", number_of_hours)

file_prepend = f"""
/** @file config.h
 *
 * @brief Configuration file for project
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Arshad Husain.  All rights reserved.
 */

#ifndef CONFIG_H
#define CONFIG_H
"""

file_mqtt_creds = f"""
const char *mqttServerIp = \"{mqtt_server_ip}\";
const short mqttServerPort = {mqtt_server_port};
const char *mqttUsername = \"{mqtt_username}\";
const char *mqttPassword = \"{mqtt_password}\";
const char *mqttClientName = \"{mqtt_client_name}\";
"""

wifi_creds = f"""
char wifi_hotspot[] = \"{wifi_network_name}\";
char wifi_password[] = \"{wifi_password}\";
"""

http_api_key = f"""
char http_api_key[] = \"{http_api_key}\";
"""

file_append = f"""
#endif /* CONFIG_H */

/*** end of file ***/
"""

with open('config.h', 'w') as config_file:
	config_file.write(file_prepend + "\n")

	config_file.write(file_mqtt_creds + "\n")
	config_file.write(wifi_creds + "\n")
	config_file.write(http_api_key + "\n")
	config_file.write(rollingSaltSet1 + "\n\n")
	config_file.write(rollingSaltSet2 + "\n\n")
	config_file.write(rollingSaltSet3 + "\n\n")
	config_file.write(file_append + "\n")



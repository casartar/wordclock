/*
 * esp8266.c
 *
 *  Created on: 22.05.2016
 *      Author: Carsten
 */

/*
// func access point -> master
	AT+CWMODE=2	// access point mode: open own wifi
	AT+RST		// reset
	AT+CWSAP=“wordclock“,“PW123456“,7,3		// ssid max 10, pw min 8, channel 7, 3= WPA2
	AT+RST		// reset
	AT+CIPMUX=1			// multiple TCP connections necessary for server mode
	AT+CIPSERVER=1,80	// open server on port80

// func station mode -> slave
 	AT+CWMODE=1	// station mode: connect to wifi
	AT+RST   	// reset
	AT+CWJAP=“ssid_2connect2“,“wifi_password“ 		// connect to access point

	AT+CWQAP	// quit access point

// func get ntp time
 	AT+CIPNTP=1		// set request for GMT +1

	AT+CIPNTP?		// read time

// func config mode -> webserver input

// func summer/winter time change date
 */
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "esp8266.h"
#include "queue.h"
#include "uart2.h"
#include "uart1.h"
#include "timer.h"
#include "config.h"

#define MAX_STRING_LEN 100

#define ERR_TIMEOUT		4
#define ERR_RESP		3
#define ERR_RESP_CMD	2
#define ERR_RESP_OK		1
#define ERR_OK			0

//#define DEBUG
uint8_t sendCWMODE(uint8_t cwmode, uint32_t timeout);
uint8_t sendRST(uint32_t timeout);
uint8_t sendCWSAP(char* ssid, char* pw, uint8_t channel, uint8_t encryption, uint32_t timeout);
uint8_t sendCIPMUX(uint8_t cipmux, uint32_t timeout);
uint8_t sendCIPSERVER(uint8_t mode, uint16_t port, uint32_t timeout);
uint8_t sendCIPSEND(uint8_t id, char* html, uint32_t timeout);
uint8_t sendCIPCLOSE(uint8_t id, uint32_t timeout);
uint8_t sendCWJAP(char* ssid, char* pw, uint32_t timeout);
uint8_t sendCIPNTPconf(int8_t gmtOffset, uint32_t timeout);
uint8_t sendCIPNTPget(uint32_t timeout);
uint8_t receiveRequest(uint8_t* id, uint8_t* methode, char* str, char* search, uint32_t timeout);
uint8_t receiveCmd(char* cmdString);

typedef enum {
	NTP_STATE_RST1,
	NTP_STATE_CWMODE,
	NTP_STATE_RST2,
	NTP_STATE_CWJAP,
	NTP_STATE_CIPNTP_CONF,
	NTP_STATE_CIPNTP_GET,
	NTP_STATE_CWQAP,
	NTP_STATE_IDLE,
}NTP_STATE;

typedef enum {
	ACCESS_POINT_CWMODE,
	ACCESS_POINT_RST1,
	ACCESS_POINT_CWSAP,
	ACCESS_POINT_RST2,
	ACCESS_POINT_CIPMUX,
	ACCESS_POINT_CIPSERVER,
	ACCESS_POINT_RECEIVE,
	ACCESS_POINT_SEND_FORMULAR,	//send config page
	ACCESS_POINT_SEND_ACK,		// send OK page
	ACCESS_POINT_SEND_CLOSE,
	ACCESS_POINT_SEND_CLOSE2,
	ACCESS_POINT_IDLE,
}AP_STATE;

uint8_t esp8266_requestTime(void){

	static NTP_STATE ntpState = NTP_STATE_RST1;

	uint8_t tmpGmtOffset = 0;

	switch(ntpState){
		case NTP_STATE_RST1:
			switch(sendRST(5000)){
				case ERR_OK:
					ntpState = NTP_STATE_CWMODE;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					ntpState = NTP_STATE_RST1;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					ntpState = NTP_STATE_RST1;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					ntpState = NTP_STATE_RST1;
					break;
				default:
					ntpState = NTP_STATE_RST1;
					break;
			}
			break;
		case NTP_STATE_CWMODE:
			switch(sendCWMODE(1,500)){
				case ERR_OK:
					ntpState = NTP_STATE_RST2;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					ntpState = NTP_STATE_RST1;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					ntpState = NTP_STATE_RST1;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					ntpState = NTP_STATE_RST1;
					break;
				default:
					ntpState = NTP_STATE_RST1;
					break;
			}
			break;
		case NTP_STATE_RST2:
			switch(sendRST(5000)){
				case ERR_OK:
					ntpState = NTP_STATE_CWJAP;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					ntpState = NTP_STATE_RST1;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					ntpState = NTP_STATE_RST1;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					ntpState = NTP_STATE_RST1;
					break;
				default:
					ntpState = NTP_STATE_RST1;
					break;
			}
			break;
		case NTP_STATE_CWJAP:
			switch(sendCWJAP(config.ssid, config.pw, 20000)){
				case ERR_OK:
					ntpState = NTP_STATE_CIPNTP_CONF;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					ntpState = NTP_STATE_RST1;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					ntpState = NTP_STATE_RST1;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					ntpState = NTP_STATE_RST1;
					break;
				default:
					ntpState = NTP_STATE_RST1;
					break;
			}
			break;
		case NTP_STATE_CIPNTP_CONF:
			if (config.summertimeFlag) tmpGmtOffset = config.gmtOffsetSummertime;
			else tmpGmtOffset = config.gmtOffsetWintertime;
			switch(sendCIPNTPconf(tmpGmtOffset, 8000)){
				case ERR_OK:
					ntpState = NTP_STATE_CIPNTP_GET;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					ntpState = NTP_STATE_RST1;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					ntpState = NTP_STATE_RST1;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					ntpState = NTP_STATE_RST1;
					break;
				default:
					ntpState = NTP_STATE_RST1;
					break;
			}
			break;
		case NTP_STATE_CIPNTP_GET:
			switch(sendCIPNTPget(1000)){
				case ERR_OK:
					if(ctime.month > 3 && ctime.month < 10) config.summertimeFlag = 1;
					else if (ctime.month == 3 && ctime.day < 27) config.summertimeFlag = 1;
					else if (ctime.month == 10 && ctime.day < 30) config.summertimeFlag = 1;
					else if (ctime.month == 3 && ctime.day == 27 && ctime.hours > 2) config.summertimeFlag = 1;
					else if (ctime.month == 10 && ctime.day == 30 && ctime.hours < 2) config.summertimeFlag = 1;
					else config.summertimeFlag = 0;
					ntpState = NTP_STATE_IDLE;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					ntpState = NTP_STATE_RST1;
					break;
				default:
					ntpState = NTP_STATE_RST1;
					break;
			}
			break;
		case NTP_STATE_IDLE:
			if (ctime.minutes == 0) ntpState = NTP_STATE_RST1;
			else ntpState = NTP_STATE_IDLE;
			break;
	}
	if (ntpState == NTP_STATE_IDLE) return 0;
	else return 1;
}

uint8_t esp8266_accessPoint(void){

	char string [100];
	static uint8_t methode = 0;
	static uint8_t id = 0;
	char ssid[SSID_MAX_LEN];
	char pw[PW_MAX_LEN];
	char* needle;

	*string = '\0';
	*ssid = '\0';
	*pw = '\0';

	static AP_STATE apState = ACCESS_POINT_RST1;

	switch (apState){

		case ACCESS_POINT_RST1:
			switch(sendRST(5000)){
				case ERR_OK:
					apState = ACCESS_POINT_CWMODE;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_RST1;
					break;
				default:
					apState = ACCESS_POINT_RST1;
					break;
			}
			break;
		case ACCESS_POINT_CWMODE:
			switch(sendCWMODE(2,100)){
				case ERR_OK:
					apState = ACCESS_POINT_RST2;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_RST1;
					break;
				default:
					apState = ACCESS_POINT_RST1;
					break;
			}
			break;
		case ACCESS_POINT_RST2:
			switch(sendRST(5000)){
				case ERR_OK:
					apState = ACCESS_POINT_CWSAP;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_RST1;
					break;
				default:
					apState = ACCESS_POINT_RST1;
					break;
			}
			break;
		case ACCESS_POINT_CWSAP:
			switch(sendCWSAP("wordclock", "PW123456", 7, 3, 100)){
				case ERR_OK:
					apState = ACCESS_POINT_CIPMUX;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_RST1;
					break;
				default:
					apState = ACCESS_POINT_RST1;
					break;
			}
			break;
		case ACCESS_POINT_CIPMUX:
			switch(sendCIPMUX(1, 100)){
				case ERR_OK:
					apState = ACCESS_POINT_CIPSERVER;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_RST1;
					break;
				default:
					apState = ACCESS_POINT_RST1;
					break;
			}
			break;
		case ACCESS_POINT_CIPSERVER:
			switch(sendCIPSERVER(1, 80, 100)){
				case ERR_OK:
					apState = ACCESS_POINT_RECEIVE;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					apState = ACCESS_POINT_RST1;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_RST1;
					break;
				default:
					apState = ACCESS_POINT_RST1;
					break;
			}
			break;
		case ACCESS_POINT_RECEIVE:
			uart1_sendString("Receive\n");
			switch(receiveRequest(&id, &methode, string, "ssid", 1000)){
				case ERR_OK:
					if (methode == 2){
						// GET
						if (string[0] == ' '){
							// Empty GET request
							apState = ACCESS_POINT_SEND_FORMULAR;
						}
						else if (strstr(string,"favicon") != NULL){
							apState = ACCESS_POINT_SEND_CLOSE;
						}
						else{
							apState = ACCESS_POINT_SEND_CLOSE;
						}
					}
					else if (methode == 1){
						// POST
						needle = strstr(string,"ssid=");
						needle = needle + 5;
						for (uint8_t index = 0; index < SSID_MAX_LEN; index++){
							if (*needle == '&'){
								ssid[index] = '\0';
								break;
							}
							else if (*needle == '+') ssid[index] = ' ';
							else if (*needle == '%'){
								if (*(needle+1) == '2' && *(needle+2) == '1'){
									// %21 means !
									ssid[index] = '!';
									needle++;
									needle++;
								}
							}
							else ssid[index] = *needle;
							needle++;
						}
						needle = strstr(string,"pwd=");
						needle = needle + 4;
						for (uint8_t index = 0; index < PW_MAX_LEN; index++){
							if (*needle == '\0'){
								pw[index] = '\0';
								break;
							}
							else if (*needle == '+') pw[index] = ' ';
							else pw[index] = *needle;
							needle++;
						}
						uart1_sendString("SSID = ");
						uart1_sendString(ssid);
						uart1_sendString("\n");
						uart1_sendString("PWD = ");
						uart1_sendString(pw);
						uart1_sendString("\n");
						if (pw[0] == '\0' || ssid[0] == '\0'){
							apState = ACCESS_POINT_SEND_FORMULAR;
						}
						else{
							strcpy(config.ssid, ssid);
							strcpy(config.pw, pw);
							apState = ACCESS_POINT_SEND_ACK;
						}
					}
					else apState = ACCESS_POINT_RECEIVE;
					break;
				case ERR_RESP:
					uart1_sendString("Error Response\n");
					apState = ACCESS_POINT_RECEIVE;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_RECEIVE;
					break;
				default:
					apState = ACCESS_POINT_RST1;
					break;
			}
			break;
		case ACCESS_POINT_SEND_FORMULAR:
			switch(sendCIPSEND(id,"<!DOCTYPE html><html><body><h1>Devcon Wordclock</h1><p>Konfiguration:</p><br><form action=\"\" method=\"POST\">SSID: <input type=\"text\" name=\"ssid\"><br><br>PWD: <input type=\"text\" name=\"pwd\"><br><br><input type=\"submit\" value=\"Submit\"></form></body></html>", 10000)){
				case ERR_OK:
					apState = ACCESS_POINT_SEND_CLOSE;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					apState = ACCESS_POINT_SEND_CLOSE;
					break;
				case ERR_RESP:
					uart1_sendString("Error Response OK missing\n");
					apState = ACCESS_POINT_SEND_CLOSE;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_SEND_CLOSE;
					break;
				default:
					apState = ACCESS_POINT_RST1;
					break;
			}
			break;
		case ACCESS_POINT_SEND_ACK:
			switch(sendCIPSEND(id,"<!DOCTYPE html><html><body><h1>Devcon Wordclock</h1><p>OK:</p></body></html>", 10000)){
				case ERR_OK:
					apState = ACCESS_POINT_SEND_CLOSE2;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					apState = ACCESS_POINT_SEND_CLOSE;
					break;
				case ERR_RESP:
					uart1_sendString("Error Response\n");
					apState = ACCESS_POINT_SEND_CLOSE;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_SEND_CLOSE;
					break;
				default:
					apState = ACCESS_POINT_RST1;
					break;
			}
			break;
		case ACCESS_POINT_SEND_CLOSE:
			uart1_sendString("Close1\n");
			switch(sendCIPCLOSE(id,1000)){
				case ERR_OK:
					apState = ACCESS_POINT_RECEIVE;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					apState = ACCESS_POINT_RECEIVE;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					apState = ACCESS_POINT_RECEIVE;
					break;
				case ERR_RESP:
					uart1_sendString("Error Response\n");
					apState = ACCESS_POINT_RECEIVE;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_RECEIVE;
					break;
				default:
					uart1_sendString("Unknown Error\n");
					apState = ACCESS_POINT_RST1;
					break;
			}
			break;
		case ACCESS_POINT_SEND_CLOSE2:
			/*uart1_sendString("Close2\n");
			switch(sendCIPCLOSE(id,1000)){
				case ERR_OK:
					apState = ACCESS_POINT_IDLE;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					apState = ACCESS_POINT_IDLE;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					apState = ACCESS_POINT_IDLE;
					break;
				case ERR_RESP:
					uart1_sendString("Error Response\n");
					apState = ACCESS_POINT_IDLE;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_IDLE;
					break;
				default:
					uart1_sendString("Unknown Error\n");
					apState = ACCESS_POINT_IDLE;
					break;
			}*/
			// I don't know why but it works
			switch(sendCIPSERVER(0, 80, 100)){
				case ERR_OK:
					apState = ACCESS_POINT_IDLE;
					break;
				case ERR_RESP_CMD:
					uart1_sendString("Error Response Command\n");
					apState = ACCESS_POINT_IDLE;
					break;
				case ERR_RESP_OK:
					uart1_sendString("Error Response OK missing\n");
					apState = ACCESS_POINT_IDLE;
					break;
				case ERR_TIMEOUT:
					uart1_sendString("Error Timeout\n");
					apState = ACCESS_POINT_IDLE;
					break;
				default:
					apState = ACCESS_POINT_IDLE;
					break;
			}
			break;
		case ACCESS_POINT_IDLE:
			apState = ACCESS_POINT_IDLE;
			break;
	}

	if (apState == ACCESS_POINT_IDLE) return 0;
	else return 1;
}
uint8_t sendCWMODE(uint8_t cwmode, uint32_t timeout){

	char recString[MAX_STRING_LEN];
	char command[11] = "AT+CWMODE=";
	char cwmodeString[2];

	esp8266_Timeout = timeout;
	itoa(cwmode, cwmodeString, 10);
	strcat(command, cwmodeString);
	// SEND COMMAND
	uart2_sendString(command);
	uart2_sendString("\r\n");
#ifdef DEBUG
	uart1_sendString("--> ");
	uart1_sendString(command);
	uart1_sendString("\r\n");
#endif
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString, command) != 0 ) return ERR_RESP_CMD;
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString,"OK") != 0 ) return ERR_RESP_OK;
	return ERR_OK;
}

uint8_t sendRST(uint32_t timeout){

	char recString[MAX_STRING_LEN];
	char command[11] = "AT+RST";

	esp8266_Timeout = timeout;

	// SEND COMMAND
	uart2_sendString(command);
	uart2_sendString("\r\n");
#ifdef DEBUG
	uart1_sendString("--> ");
	uart1_sendString(command);
	uart1_sendString("\r\n");
#endif
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString, command) != 0 ) return ERR_RESP_CMD;
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString,"OK") != 0 ) return ERR_RESP_OK;
	// WAIT FOR READY
	while(esp8266_Timeout){
		// WAIT FOR RESPONSE
		while(receiveCmd(recString) && esp8266_Timeout);
		if(!esp8266_Timeout) return ERR_TIMEOUT;
		// CHECK RESPONSE
		if ( strcmp(recString,"ready") == 0 ) return ERR_OK;
	}
	return ERR_TIMEOUT;
}

uint8_t sendCWSAP(char* ssid, char* pw, uint8_t channel, uint8_t encryption, uint32_t timeout){

	char recString[MAX_STRING_LEN];
	char command[100] = "AT+CWSAP=";
	char channelString[2];
	char encryptionString[2];

	esp8266_Timeout = timeout;

	itoa(encryption, encryptionString, 10);
	itoa(channel, channelString, 10);
	strcat(command, "\"");
	strcat(command, ssid);
	strcat(command, "\",\"");
	strcat(command, pw);
	strcat(command, "\",");
	strcat(command, channelString);
	strcat(command, ",");
	strcat(command, encryptionString);
	// SEND COMMAND
	uart2_sendString(command);
	uart2_sendString("\r\n");
#ifdef DEBUG
	uart1_sendString("--> ");
	uart1_sendString(command);
	uart1_sendString("\r\n");
#endif
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString, command) != 0 ) return ERR_RESP_CMD;
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString,"OK") != 0 ) return ERR_RESP_OK;
	return ERR_OK;
}


uint8_t sendCIPMUX(uint8_t cipmux, uint32_t timeout){

	char recString[MAX_STRING_LEN];
	char command[12] = "AT+CIPMUX=";
	char cipmuxString[2];

	esp8266_Timeout = timeout;

	itoa(cipmux, cipmuxString, 10);
	strcat(command, cipmuxString);
	// SEND COMMAND
	uart2_sendString(command);
	uart2_sendString("\r\n");
#ifdef DEBUG
	uart1_sendString("--> ");
	uart1_sendString(command);
	uart1_sendString("\r\n");
#endif
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString, command) != 0 ) return ERR_RESP_CMD;
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString,"OK") != 0 ) return ERR_RESP_OK;
	return ERR_OK;
}

uint8_t sendCIPSERVER(uint8_t mode, uint16_t port, uint32_t timeout){

	char recString[MAX_STRING_LEN];
	char command[21] = "AT+CIPSERVER=";
	char modeString[2];
	char portString[6];

	esp8266_Timeout = timeout;

	itoa(mode, modeString, 10);
	itoa(port, portString, 10);
	strcat(command, modeString);
	strcat(command, ",");
	strcat(command, portString);
	// SEND COMMAND
	uart2_sendString(command);
	uart2_sendString("\r\n");
#ifdef DEBUG
	uart1_sendString("--> ");
	uart1_sendString(command);
	uart1_sendString("\r\n");
#endif
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString, command) != 0 ) return ERR_RESP_CMD;
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString,"OK") != 0 ) return ERR_RESP_OK;
	return ERR_OK;
}

uint8_t receiveRequest(uint8_t* id, uint8_t* methode, char* str, char* search, uint32_t timeout){

	char recString[MAX_STRING_LEN];
	char* needle = NULL;

	esp8266_Timeout = timeout;

	// WAIT FOR +IPD
	while(esp8266_Timeout){
		// WAIT FOR RESPONSE
		while(receiveCmd(recString) && esp8266_Timeout);
		if(!esp8266_Timeout) return ERR_TIMEOUT;
		// CHECK RESPONSES
		if (strncmp(recString,"+IPD,",5) == 0 ){
			esp8266_Timeout = timeout;
			uart1_sendString("+IPD gefunden\n");
			*id = atoi(&recString[5]);
			uart1_sendString("ID=");
			uart1_sendChar(*id+'0');
			uart1_sendString("\r\n");
			if ((needle = strstr(recString,"GET /")) != NULL){
				uart1_sendString("GET\r\n");
				char* ptr = str;
				needle = needle+5;
				while(*needle){
					*ptr++ = *needle++;
				}
				*ptr = '\0';
				uart1_sendString("String: ");
				uart1_sendString(str);
				uart1_sendString("\r\n");
				*methode = 2;
			}
			else if ((needle = strstr(recString,"POST /")) != NULL){
				uart1_sendString("POST\r\n");
				*methode = 1;
			}
			else{
				uart1_sendString("ERR_RESP\r\n");
				return ERR_RESP;
			}
		}
		else if (*methode && strncmp(recString,search,strlen(search)) == 0){
			uint8_t index = 0;
			while(recString[index]){
				str[index] = recString[index];
				index++;
			}
			str[index] = '\0';
			uart1_sendString("String: ");
			uart1_sendString(str);
			uart1_sendString("\r\n");
		}
		else if(strncmp(recString,"OK",4) == 0 ) return ERR_OK;
	}
	return ERR_TIMEOUT;
}

uint8_t sendCIPSEND(uint8_t id, char* html, uint32_t timeout){

	char recString[MAX_STRING_LEN];
	char command[21] = "AT+CIPSEND=";
	char idString[2];
	char lengthString[6];


	esp8266_Timeout = timeout;

	itoa(id, idString, 10);
	itoa(strlen(html), lengthString, 10);
	strcat(command, idString);
	strcat(command, ",");
	strcat(command, lengthString);
	// SEND COMMAND
	uart2_sendString(command);
	uart2_sendString("\r\n");
#ifdef DEBUG
	uart1_sendString("--> ");
	uart1_sendString(command);
	uart1_sendString("\r\n");
#endif
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString, command) != 0 ) return ERR_RESP_CMD;

	while(esp8266_Timeout){
		if( !queue_read(&uart2RecQueue, recString) ){
#ifdef DEBUG
			uart1_sendString("<-- ");
			uart1_sendChar(*recString);
			uart1_sendString("\r\n");
#endif
			if (*recString == '\r' || *recString == '\n') continue;
			if(*recString == 'l'){
				// link is not - receive hole string and leave
				while(receiveCmd(recString) && esp8266_Timeout);
				if(!esp8266_Timeout) return ERR_TIMEOUT;
				return ERR_RESP;
			}
			else if(*recString != '>') return ERR_RESP;
			else break;
		}
	}
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	while(esp8266_Timeout){
		if( !queue_read(&uart2RecQueue, recString) ){
#ifdef DEBUG
			uart1_sendString("<-- ");
			uart1_sendChar(*recString);
			uart1_sendString("\r\n");
#endif
			if(*recString != ' ') return ERR_RESP;
			else break;
		}
	}
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// SEND WEBSITE
	uart2_sendString(html);
	uart2_sendString("\r\n");
#ifdef DEBUG
	uart1_sendString("--> ");
	uart1_sendString(html);
	uart1_sendString("\r\n");
#endif
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// WAIT FOR RESPONSE
	while(esp8266_Timeout){
		// WAIT FOR SEND OK
		while(receiveCmd(recString) && esp8266_Timeout);
		if(!esp8266_Timeout) return ERR_TIMEOUT;
		// CHECK RESPONSE
		if ( strcmp(recString,"SEND OK") == 0 ) return ERR_OK;
	}
	return ERR_TIMEOUT;

	return 0;
}

uint8_t sendCIPCLOSE(uint8_t id, uint32_t timeout){

	char command[21] = "AT+CIPCLOSE=";
	char idString[2];
	char recString[MAX_STRING_LEN];
	char closedString[10];

	esp8266_Timeout = timeout;

	itoa(id, idString, 10);
	strcat(command, idString);
	strcpy(closedString,idString);
	strcat(closedString,",CLOSED");

	// SEND COMMAND
	uart2_sendString(command);
	uart2_sendString("\r\n");
#ifdef DEBUG
	uart1_sendString("--> ");
	uart1_sendString(command);
	uart1_sendString("\r\n");
#endif
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString, command) != 0 ) {
		uart1_sendString("ERR_RESP\r\n");
		return ERR_RESP_CMD;
	}
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString, closedString) != 0 ) {
		uart1_sendString("ERR_RESP\r\n");
		return ERR_RESP;
	}
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString,"OK") != 0 ) return ERR_RESP_OK;
	return ERR_OK;

}

uint8_t sendCWJAP(char* ssid, char* pw, uint32_t timeout){

	char recString[MAX_STRING_LEN];
	char command[SSID_MAX_LEN+PW_MAX_LEN+20] = "AT+CWJAP=\"";

	esp8266_Timeout = timeout;

	strcat(command, ssid);
	strcat(command, "\",\"");
	strcat(command, pw);
	strcat(command, "\"");
	// SEND COMMAND
	uart2_sendString(command);
	uart2_sendString("\r\n");
#ifdef DEBUG
	uart1_sendString("--> ");
	uart1_sendString(command);
	uart1_sendString("\r\n");
#endif
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString, command) != 0 ) return ERR_RESP_CMD;
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString,"OK") != 0 ) return ERR_RESP_OK;
	return ERR_OK;

}

uint8_t sendCIPNTPconf(int8_t gmtOffset, uint32_t timeout){

	char recString[MAX_STRING_LEN];
	char command[21] = "AT+CIPNTP=";
	char gmtOffsetString[4];

	esp8266_Timeout = timeout;

	itoa(gmtOffset, gmtOffsetString, 10);
	strcat(command, gmtOffsetString);
	// SEND COMMAND
	uart2_sendString(command);
	uart2_sendString("\r\n");
#ifdef DEBUG
	uart1_sendString("--> ");
	uart1_sendString(command);
	uart1_sendString("\r\n");
#endif
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString, command) != 0 ) return ERR_RESP_CMD;
	// WAIT FOR RESPONSE
	while(esp8266_Timeout){
		// WAIT FOR process
		while(receiveCmd(recString) && esp8266_Timeout);
		if(!esp8266_Timeout) return ERR_TIMEOUT;
		// CHECK RESPONSE
		if ( strstr(recString,"sntp_process:") ) return ERR_OK;
	}
	return ERR_TIMEOUT;

}

uint8_t sendCIPNTPget(uint32_t timeout){

	char recString[MAX_STRING_LEN];
	char command[21] = "AT+CIPNTP?";
	char Buffer[50];
	esp8266_Timeout = timeout;

	// SEND COMMAND
	uart2_sendString(command);
	uart2_sendString("\r\n");
#ifdef DEBUG
	uart1_sendString("--> ");
	uart1_sendString(command);
	uart1_sendString("\r\n");
#endif
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString, command) != 0 ) return ERR_RESP_CMD;
	// WAIT FOR RESPONSE
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strncmp(recString,"Time: ",6) == 0 ){
		ctime.hours = atoi(&recString[6]);
		ctime.minutes = atoi(&recString[9]);
		ctime.seconds = atoi(&recString[12]);
		ctime.month = atoi(&recString[15]);
		ctime.day = atoi(&recString[18]);
		ctime.year = atoi(&recString[21]);

		sprintf(Buffer,"H: %u, M: %u, S: %u, D: %u, M: %u, Y: %u\n",
				ctime.hours,ctime.minutes,ctime.seconds,ctime.day,ctime.month,ctime.year);
		uart1_sendString(Buffer);
	}
	else return ERR_RESP;
	while(receiveCmd(recString) && esp8266_Timeout);
	if(!esp8266_Timeout) return ERR_TIMEOUT;
	// CHECK RESPONSE
	if ( strcmp(recString,"OK") != 0 ) return ERR_RESP_OK;
	return ERR_OK;

}

uint8_t receiveCmd(char* cmdString) {

	static uint8_t count = 0;
	char tmp;
	if (count == 0){
		while(!queue_read(&uart2RecQueue, &tmp)){
			// Read as long as there are character in queue
			if (tmp != '\r' && tmp != '\n'){
				// First character of command detected
				cmdString[count++] = tmp;
				break;
			}
		}
	}
	if (count > 0){
		while(!queue_read(&uart2RecQueue, &tmp)){
			// Read as long as there are character in queue
			if (tmp != '\r' && tmp != '\n'){
				// Command has not yet ended
				if(count < MAX_STRING_LEN){
					cmdString[count++] = tmp;
				}
				else{
					// Buffer is full
					cmdString[count] = '\0';
				}
			}
			else{
				// Command is completed
				cmdString[count] = '\0';
#ifdef DEBUG
				uart1_sendString("<-- ");
				uart1_sendString(cmdString);
				uart1_sendString("\r\n");
#endif
				count = 0;
				return 0;
			}
		}
	}
	// Queue is empty and Command not yet completed
	return 1;
	/*char tmp;

	while(1){
		if( !queue_read(&uart2RecQueue, &tmp) ){
			// There is data in Buffer
			if(tmp != '\r' && tmp != '\n'){
				// Ignore all \r and \n
				break;
			}
		}
	}
	*cmdString++ = tmp;
	while(tmp != '\r' && tmp != '\n'){
		if(!queue_read(&uart2RecQueue, &tmp)){
			*cmdString++ = tmp;
		}
	}
	*--cmdString = '\0';
	return 0;
	*/
}



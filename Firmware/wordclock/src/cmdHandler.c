/*
 * =============================== Includes ===================================
 */
#include "cmdHandler.h"
#include "uart1.h"
#include "config.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/*
 * =============================== Defines ====================================
 */

/*
 * ========================= Function Prototypes ==============================
 */

/*
 * =========================== Global Functions ===============================
 */
void cmdHandler(void)
{
	uint8_t response = 0;
	uint8_t saveConfig = 0;
	char outputBuffer[100];
	char* ptr = NULL;
	char tmp = 0;
	uint8_t tmpTimeH = 0;
	uint8_t tmpTimeM = 0;
	uint8_t tmpTimeS = 0;
	int8_t  tmpOffset = 0;
	uint32_t tmpBrightness = 0;
	uint32_t tmpColorRed = 0;
	uint32_t tmpColorGreen = 0;
	uint32_t tmpColorBlue = 0;

	if (uart1RecFlag){
		switch (uart1RecBuf[0]){
		case '\0':
			response = 1;
			break;
		case '?':
			if (uart1RecBuf[1] == '\0'){
				sprintf(outputBuffer, "SSID:             %s\r\n", config.ssid);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, "Password:         %s\r\n", config.pw);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, "UTC Offset:       %d\r\n", config.utcOffset);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, "Brightness:       %u\r\n", config.brightness);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, "Background Color: %u,%u,%u\r\n",
						config.colorRed, config.colorGreen, config.colorBlue);
				uart1_sendString(outputBuffer);

				response = 1;
			}
			break;
		case 'S':
		case 's':
			if ((uart1RecBuf[1] == 'S' || uart1RecBuf[1] == 's')
			  && (uart1RecBuf[2] == 'I' || uart1RecBuf[2] == 'i')
			  && (uart1RecBuf[3] == 'D' || uart1RecBuf[3] == 'd')
			  && uart1RecBuf[4] == '='){
				if (strlen(&uart1RecBuf[5]) > 32){
					// SSID is too long
					response = 0;
					break;
				}
				strcpy(config.ssid, &uart1RecBuf[5]);
				response = 1;
				saveConfig = 1;
			}
			break;
		case 'P':
		case 'p':
			if ((uart1RecBuf[1] == 'W' || uart1RecBuf[1] == 'w')
			  && uart1RecBuf[2] == '='){
				if (strlen(&uart1RecBuf[3]) > 63){
					// Password is too long
					response = 0;
					break;
				}
				strcpy(config.pw, &uart1RecBuf[3]);
				response = 1;
				saveConfig = 1;
			}
			break;
		case 'T':
		case 't':
			if (uart1RecBuf[1] != '='){
				// Invalid command
				break;
			}
			if (sscanf(&uart1RecBuf[2],"%hhu:%hhu:%hhu",
				(unsigned char*)&tmpTimeH, (unsigned char*)&tmpTimeM, (unsigned char*)&tmpTimeS) != 3){
				// Parsing error
				break;
			}
			if (tmpTimeH < 24) ctime.hours = tmpTimeH;
			else break;
			if (tmpTimeM < 60) ctime.minutes = tmpTimeM;
			else break;
			if (tmpTimeS < 60) ctime.seconds = tmpTimeS;
			else break;
			response = 1;
			break;
		case 'O':
		case 'o':
			if (uart1RecBuf[1] != '='){
				// Invalid command
				break;
			}
			if (sscanf(&uart1RecBuf[2],"%3hhd", (char*)&tmpOffset) != 1){
				// Parsing error
				break;
			}
			if (tmpOffset < 13 && tmpOffset > -13) config.utcOffset = tmpOffset;
			else break;
			response = 1;
			saveConfig = 1;
			break;
		case 'B':
		case 'b':
			if (uart1RecBuf[1] != '='){
				// Invalid command
				break;
			}
			tmpBrightness = strtoul(&uart1RecBuf[2], &ptr, 10);
			if (tmpBrightness == 0 && uart1RecBuf[2] != '0') break;
			if (tmpBrightness > 255) break;
			config.brightness = (uint8_t)tmpBrightness;
			response = 1;
			saveConfig = 1;
			break;
		case 'C':
		case 'c':
			if (uart1RecBuf[1] != '='){
				// Invalid command
				break;
			}
			tmpColorRed = strtoul(&uart1RecBuf[2], &ptr, 10);
			if (tmpColorRed == 0 && uart1RecBuf[2] != '0') break;
			ptr++;
			tmp = *ptr;
			tmpColorGreen = strtoul(ptr, &ptr, 10);
			if (tmpColorGreen == 0 && tmp != '0') break;
			ptr++;
			tmp = *ptr;
			tmpColorBlue = strtoul(ptr, &ptr, 10);
			if (tmpColorBlue == 0 && tmp != '0') break;

			if (tmpColorRed   > 255) break;
			if (tmpColorGreen > 255) break;
			if (tmpColorBlue  > 255) break;
			config.colorRed = tmpColorRed;
			config.colorGreen = tmpColorGreen;
			config.colorBlue = tmpColorBlue;
			response = 1;
			saveConfig = 1;
			break;

		} // switch
		if (response == 1){
			uart1_sendString("OK\r\n");
		}
		else{
			uart1_sendString("ERROR\r\n");
		}
		if (saveConfig){
			config_write();
		}
		uart1RecFlag = 0;
	}

}
/*
 * ============================ Local Functions ===============================
 */

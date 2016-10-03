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
#include "word_clock_draw.h"
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
	uint32_t tmpWordNumber = 0;

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

				uart1_sendString("w0 = es\r\n");
				uart1_sendString("w1 = ist\r\n");
				uart1_sendString("w2 = vor\r\n");
				uart1_sendString("w3 = nach\r\n");
				uart1_sendString("w4 = Uhr\r\n");
				uart1_sendString("w5 = fuenf\r\n");
				uart1_sendString("w6 = zehn\r\n");
				uart1_sendString("w7 = viertel\r\n");
				uart1_sendString("w8 = zwanzig\r\n");
				uart1_sendString("w9 = halb\r\n");
				uart1_sendString("w10 = dreiviertel\r\n");
				uart1_sendString("w11 = ein\r\n");
				uart1_sendString("w12 = eins\r\n");
				uart1_sendString("w13 = zwei\r\n");
				uart1_sendString("w14 = drei\r\n");
				uart1_sendString("w15 = vier\r\n");
				uart1_sendString("w16 = fuenf\r\n");
				uart1_sendString("w17 = sechs\r\n");
				uart1_sendString("w18 = sieben\r\n");
				uart1_sendString("w19 = acht\r\n");
				uart1_sendString("w20 = neun\r\n");
				uart1_sendString("w21 = zehn\r\n");
				uart1_sendString("w22 = elf\r\n");
				uart1_sendString("w23 = zwoelf\r\n");
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
			/*
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
			*/
		case 'O':
		case 'o':
			if (uart1RecBuf[1] != '='){
				// Invalid command
				break;
			}
			tmpOffset = strtol(&uart1RecBuf[2], &ptr, 10);
			if (tmpBrightness == 0 && uart1RecBuf[2] != '0') break;
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
			for (uint8_t index= 0; index < sizeof(config.wordColor); index++){
				config.wordColor[index].r = config.colorRed;
				config.wordColor[index].g = config.colorGreen;
				config.wordColor[index].b = config.colorBlue;
			}
			Word_Clock_Init(config.colorRed, config.colorGreen, config.colorBlue, 255);
			response = 1;
			saveConfig = 1;
			break;
		case 'W':
		case 'w':
			if (uart1RecBuf[1] != '='){
				// Invalid command
				break;
			}
			tmpWordNumber = strtoul(&uart1RecBuf[2], &ptr, 10);
			if (tmpWordNumber == 0 && uart1RecBuf[2] != '0') break;
			ptr++;
			tmp = *ptr;
			tmpColorRed = strtoul(ptr, &ptr, 10);
			if (tmpColorRed == 0 && tmp != '0') break;
			ptr++;
			tmp = *ptr;
			tmpColorGreen = strtoul(ptr, &ptr, 10);
			if (tmpColorGreen == 0 && tmp != '0') break;
			ptr++;
			tmp = *ptr;
			tmpColorBlue = strtoul(ptr, &ptr, 10);
			if (tmpColorBlue == 0 && tmp != '0') break;

			if (tmpWordNumber > 23) break;
			if (tmpColorRed   > 255) break;
			if (tmpColorGreen > 255) break;
			if (tmpColorBlue  > 255) break;

			config.wordColor[tmpWordNumber].r = tmpColorRed;
			config.wordColor[tmpWordNumber].g = tmpColorGreen;
			config.wordColor[tmpWordNumber].b = tmpColorBlue;

			switch(tmpWordNumber){
				case 0: Word_Clock_Set_Color(&clock_word_dict.it, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 1: Word_Clock_Set_Color(&clock_word_dict.is, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 2: Word_Clock_Set_Color(&clock_word_dict.before, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 3: Word_Clock_Set_Color(&clock_word_dict.after, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 4: Word_Clock_Set_Color(&clock_word_dict.min_0, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 5: Word_Clock_Set_Color(&clock_word_dict.min_5, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 6: Word_Clock_Set_Color(&clock_word_dict.min_10, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 7: Word_Clock_Set_Color(&clock_word_dict.min_15, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 8: Word_Clock_Set_Color(&clock_word_dict.min_20, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 9: Word_Clock_Set_Color(&clock_word_dict.min_30, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 10: Word_Clock_Set_Color(&clock_word_dict.min_45, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 11: Word_Clock_Set_Color(&clock_word_dict.h_1, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 12: Word_Clock_Set_Color(&clock_word_dict.h_1s, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 13: Word_Clock_Set_Color(&clock_word_dict.h_2, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 14: Word_Clock_Set_Color(&clock_word_dict.h_3, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 15: Word_Clock_Set_Color(&clock_word_dict.h_4, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 16: Word_Clock_Set_Color(&clock_word_dict.h_5, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 17: Word_Clock_Set_Color(&clock_word_dict.h_6, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 18: Word_Clock_Set_Color(&clock_word_dict.h_7, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 19: Word_Clock_Set_Color(&clock_word_dict.h_8, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 20: Word_Clock_Set_Color(&clock_word_dict.h_9, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 21: Word_Clock_Set_Color(&clock_word_dict.h_10, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 22: Word_Clock_Set_Color(&clock_word_dict.h_11, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
				case 23: Word_Clock_Set_Color(&clock_word_dict.h_12, tmpColorRed, tmpColorGreen, tmpColorBlue,255);
						break;
			}
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

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

				uart1_sendString("###############################################################\r\n");
				uart1_sendString("                       WordClock Config: \r\n");
				uart1_sendString("---------------------------------------------------------------\r\n");

				sprintf(outputBuffer, "WLAN-SSID:        %s\r\n", config.ssid);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, "WLAN-Password:    %s\r\n", config.pw);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, "UTC Offset:       %d\r\n", config.utcOffset);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, "Brightness:       %u\r\n", config.brightness);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, "Background Color: %u,%u,%u\r\n",
						config.colorRed, config.colorGreen, config.colorBlue);
				uart1_sendString(outputBuffer);

				uart1_sendString("---------------------------------------------------------------\r\n\r\n");
				uart1_sendString("         Word Colors: (CMD: w=Number,red,green,blue) \r\n\r\n");

				sprintf(outputBuffer, " 0 <Es>          %03u,%03u,%03u  |  1 <ist>     %03u,%03u,%03u \r\n",
						config.wordColor[0].r, config.wordColor[0].g, config.wordColor[0].b,
						config.wordColor[1].r, config.wordColor[1].g, config.wordColor[1].b);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, " 2 <vor>         %03u,%03u,%03u  |  3 <nach>    %03u,%03u,%03u \r\n",
						config.wordColor[2].r, config.wordColor[2].g, config.wordColor[2].b,
						config.wordColor[3].r, config.wordColor[3].g, config.wordColor[3].b);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, " 4 <Uhr>         %03u,%03u,%03u  |  5 <fuenf>   %03u,%03u,%03u \r\n",
						config.wordColor[4].r, config.wordColor[4].g, config.wordColor[4].b,
						config.wordColor[5].r, config.wordColor[5].g, config.wordColor[5].b);
				uart1_sendString(outputBuffer);


				sprintf(outputBuffer, " 6 <zehn>        %03u,%03u,%03u  |  7 <viertel> %03u,%03u,%03u\r\n",
						config.wordColor[6].r, config.wordColor[6].g, config.wordColor[6].b,
						config.wordColor[7].r, config.wordColor[7].g, config.wordColor[7].b);
				uart1_sendString(outputBuffer);


				sprintf(outputBuffer, " 8 <zwanzig>     %03u,%03u,%03u  |  9 <halb>    %03u,%03u,%03u\r\n",
						config.wordColor[8].r, config.wordColor[8].g, config.wordColor[8].b,
						config.wordColor[9].r, config.wordColor[9].g, config.wordColor[9].b);
				uart1_sendString(outputBuffer);


				sprintf(outputBuffer, "10 <dreiviertel> %03u,%03u,%03u  | 11 <ein>     %03u,%03u,%03u\r\n",
						config.wordColor[10].r, config.wordColor[10].g, config.wordColor[10].b,
						config.wordColor[11].r, config.wordColor[11].g, config.wordColor[11].b);
				uart1_sendString(outputBuffer);


				sprintf(outputBuffer, "12 <eins>        %03u,%03u,%03u  | 13 <zwei>    %03u,%03u,%03u\r\n",
						config.wordColor[12].r, config.wordColor[12].g, config.wordColor[12].b,
						config.wordColor[13].r, config.wordColor[13].g, config.wordColor[13].b);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, "14 <drei>        %03u,%03u,%03u  | 15 <vier>    %03u,%03u,%03u\r\n",
						config.wordColor[14].r, config.wordColor[14].g, config.wordColor[14].b,
						config.wordColor[15].r, config.wordColor[15].g, config.wordColor[15].b);
				uart1_sendString(outputBuffer);


				sprintf(outputBuffer, "16 <fuenf>       %03u,%03u,%03u  | 17 <sechs>   %03u,%03u,%03u\r\n",
						config.wordColor[16].r, config.wordColor[16].g, config.wordColor[16].b,
						config.wordColor[17].r, config.wordColor[17].g, config.wordColor[17].b);
				uart1_sendString(outputBuffer);


				sprintf(outputBuffer, "18 <sieben>      %03u,%03u,%03u  | 19 <acht>    %03u,%03u,%03u\r\n",
						config.wordColor[18].r, config.wordColor[18].g, config.wordColor[18].b,
						config.wordColor[19].r, config.wordColor[19].g, config.wordColor[19].b);
				uart1_sendString(outputBuffer);


				sprintf(outputBuffer, "20 <neun>        %03u,%03u,%03u  | 21 <zehn>    %03u,%03u,%03u\r\n",
						config.wordColor[20].r, config.wordColor[20].g, config.wordColor[20].b,
						config.wordColor[21].r, config.wordColor[21].g, config.wordColor[21].b);
				uart1_sendString(outputBuffer);

				sprintf(outputBuffer, "22 <elf>         %03u,%03u,%03u  | 23 <zwoelf>  %03u,%03u,%03u\r\n",
						config.wordColor[22].r, config.wordColor[22].g, config.wordColor[22].b,
						config.wordColor[23].r, config.wordColor[23].g, config.wordColor[23].b);
				uart1_sendString(outputBuffer);
				uart1_sendString("_______________________________________________________________\r\n");
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

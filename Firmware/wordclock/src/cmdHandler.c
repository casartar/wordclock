/*
 * =============================== Includes ===================================
 */
#include "cmdHandler.h"
#include "uart1.h"
#include "config.h"
#include <string.h>
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
	if (uart1RecFlag){
		switch (uart1RecBuf[0]){
		case '\0':
			response = 1;
			break;
		case '?':
			if (uart1RecBuf[1] == '\0'){
				uart1_sendString("SSID: ");
				uart1_sendString(config.ssid);
				uart1_sendString("\r\n");

				uart1_sendString("PW: ");
				uart1_sendString(config.pw);
				uart1_sendString("\r\n");

				response = 1;
			}
			break;
		case 'S':
		case 's':
			if (uart1RecBuf[1] == 'S' || uart1RecBuf[1] == 's'){
				// No check if SSID is to long - so be careful!!!
				strcpy(config.ssid, &uart1RecBuf[2]);
				response = 1;
			}
			break;
		case 'P':
		case 'p':
			// No check if PW is to long - so be careful!!!
			strcpy(config.pw, &uart1RecBuf[1]);
			response = 1;
			break;
		} // switch
		if (response == 1){
			uart1_sendString("OK\r\n");
		}
		else{
			uart1_sendString("ERROR\r\n");
		}
		uart1RecFlag = 0;
	}
}
/*
 * ============================ Local Functions ===============================
 */

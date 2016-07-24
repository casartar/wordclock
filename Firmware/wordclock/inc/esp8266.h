/*
 * esp8266.h
 *
 *  Created on: 22.05.2016
 *      Author: Carsten
 */

#ifndef ESP8266_H_
#define ESP8266_H_

#define SSID_MAX_LEN   65
#define PW_MAX_LEN     65

//void esp8266_init(void);
uint8_t esp8266_accessPoint(void);
uint8_t esp8266_requestTime(void);

#endif /* ESP8266_H_ */

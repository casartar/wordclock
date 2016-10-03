/*
 * cinfig.h
 *
 *  Created on: 06.07.2016
 *      Author: Carsten
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "esp8266.h"
#include <inttypes.h>
#include "led_matrix.h"

struct _config{
 uint16_t header;
 char    ssid[SSID_MAX_LEN];
 char    pw[PW_MAX_LEN];
 int8_t	 utcOffset;
 uint8_t brightness;
 uint8_t colorRed;
 uint8_t colorGreen;
 uint8_t colorBlue;
 uint8_t summertimeFlag;
 rgb_t wordColor[24];


}config;



void config_setDefault(void);
uint8_t config_init(void);
uint8_t config_read(void);
uint8_t config_write(void);

#endif /* CONFIG_H_ */

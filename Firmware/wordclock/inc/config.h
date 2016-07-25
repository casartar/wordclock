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

struct _config{
 uint16_t header;
 int8_t  gmtOffsetSummertime;
 int8_t  gmtOffsetWintertime;
 char    ssid[SSID_MAX_LEN];
 char    pw[PW_MAX_LEN];
 uint8_t summertimeFlag;
}config;

void config_setDefault(void);
uint8_t config_init(void);
uint8_t config_read(void);
uint8_t config_write(void);

#endif /* CONFIG_H_ */

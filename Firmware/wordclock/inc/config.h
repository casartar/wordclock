/*
 * cinfig.h
 *
 *  Created on: 06.07.2016
 *      Author: Carsten
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "esp8266.h"

struct _config{
 int8_t  gmtOffsetSummertime;
 int8_t  gmtOffsetWintertime;
 char    ssid[SSID_MAX_LEN];
 char    pw[PW_MAX_LEN];
 uint8_t summertimeFlag;
}config;

#endif /* CONFIG_H_ */

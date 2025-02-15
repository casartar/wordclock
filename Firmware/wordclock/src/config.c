/*
 * config.c
 *
 *  Created on: 06.07.2016
 *      Author: Carsten
 */
#include "config.h"
#include "eeprom.h"
#include "string.h"
#include "uart1.h"

#define DEFAULT_HEADER 0x1234

uint16_t VirtAddVarTab[NB_OF_VAR];

void config_setDefault(void){

	config.header = DEFAULT_HEADER;

	config.summertimeFlag = 0;
	strcpy (config.ssid,"Elektron.B");
	strcpy (config.pw, "www.google.de");
	config.utcOffset = 1;
	config.brightness = 50;
	config.colorRed = 0;
	config.colorGreen = 255;
	config.colorBlue = 0;
	for (uint8_t index= 0; index < sizeof(config.wordColor); index++){
		config.wordColor[index].r = config.colorRed;
		config.wordColor[index].g = config.colorGreen;
		config.wordColor[index].b = config.colorBlue;
	}
}

uint8_t config_init(void){

	for (uint16_t index = 0; index < sizeof(config); index++){
		VirtAddVarTab[index] = index;
	}
	FLASH_Unlock();
	if (EE_Init() != FLASH_COMPLETE){
		uart1_sendString("EEPROM init not OK \r\n");
		return 1;
	}
	else return 0;
}

uint8_t config_read(void){

	uint8_t* ptr;
	uint16_t tmpData;

	ptr = (uint8_t*)&config;
	for (uint16_t index = 0; index < sizeof(config); index++){
		EE_ReadVariable(index,&tmpData);
		*ptr++ = (uint8_t)tmpData;
	}
	if (DEFAULT_HEADER == config.header){
		// There is a configuration in EEPROM
		uart1_sendString("EEPROM header OK \r\n");
	}
	else{
		uart1_sendString("EEPROM header not OK \r\n");
		return 1;
	}
	return 0;
}

uint8_t config_write(void){

	uint8_t* ptr;

	ptr = (uint8_t*)&config;
	for (uint16_t index = 0; index < sizeof(config); index++){
		EE_WriteVariable(index,*ptr++);
	}
	return 0;
}

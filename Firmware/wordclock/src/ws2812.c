/*
 * ws2812.c
 *
 *  Created on: 28.05.2016
 *      Author: sascha
 */
#include "ws2812.h"

/* Private variables ---------------------------------------------------------*/
static volatile uint8_t  TX_Buffer[24*WS2812_TX_BUFFER_COUNT];	/*DAM Buffer for sending the WS2812 Puleses*/
static volatile uint8_t *TX_Tmp;
static volatile RgbLed_t LED_Buffer[WS2812_LED_COUNT];			/*LED Frame Buffer*/
static volatile uint16_t NumberLedsToUpdate	= 0;				/*Send counter used inside the ISR*/

static uint8_t TimerPeriod 					= 0;				/*Timer Periode to generate the 800kHz Signal*/
static volatile uint8_t TimerPulseTX0 		= 0;				/*Timer Periode to send a logic 0*/
static volatile uint8_t TimerPulseTX1 		= 0;				/*Timer Periode to send a Logic 1*/


/* Privte define ------------------------------------------------------------*/
const uint16_t TX_BUFFER_SIZE 		= sizeof(TX_Buffer)  / sizeof(TX_Buffer[0]);
const uint16_t LED_BUFFER_SIZE 		= sizeof(LED_Buffer) / sizeof(LED_Buffer[0]);

/* Private function prototypes -----------------------------------------------*/
static void TIM_Config(void);
static void DMA_Config(void);
static volatile uint8_t* addToTransmitBuffer(volatile uint8_t *bufferStart, volatile RgbLed_t led);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Configure the TIM1 Pins.
  * @param  None
  * @retval None
  */
static void TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
//	uint16_t CCR4_Value;

	/* GPIO Clock enable */
	WS2812_GPIO_CLK_CMD( WS2812_GPIO_CLK, ENABLE);

	/* TIM1 clock enable */
	WS2812_TIMER_CLK_CMD(WS2812_TIMER_CLK , ENABLE);

	/* DMA clock enable */
	WS2812_DMA_CLK_CMD(WS2812_DMA_CLK, ENABLE);


	/* GPIO Configuration: Channel 3 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin 	= WS2812_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode 	= WS2812_GPIO_MODE;
	GPIO_InitStructure.GPIO_Speed 	= WS2812_GPIO_SPEED;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP ;
	GPIO_Init(WS2812_GPIO_PORT, &GPIO_InitStructure);


#ifdef WS2812_STM32F0
	/* Connect TIM pins to AF2 */
	GPIO_PinAFConfig(WS2812_GPIO_PORT, WS2812_GPIO_SOURCE, WS2812_GPIO_AF);
#endif

	/* TIM1 Configuration ---------------------------------------------------
	 Generate 1 PWM signal with period of BAUDRATE:

	 The objective is to generate 1 PWM signal with period of BAUDRATE:
	   - TIM1_Period = (SystemCoreClock / 800000) - 1
	 The channel 1 duty cycle follows WS2812
	 The Timer pulse is calculated as follows:
	   - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100

	----------------------------------------------------------------------- */
	/* Compute the value to be set in ARR register to generate signal frequency at BAUDRATE */
	TimerPeriod = (SystemCoreClock / WS2812_BAUDRATE ) - 1;
	/* Compute CCR1 value to generate the timing for WS2812 */
	TimerPulseTX0 = (uint16_t) ((uint32_t) WS2812_DUTYCYCLE * TimerPeriod / 100);
	TimerPulseTX1 = TimerPeriod - TimerPulseTX0;

	/* Compute CCR4 value as DMA Trigger to update the Timer compare value */
//	CCR4_Value =  (uint16_t) ((TimerPeriod * 8) / 10);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler 		= 0;
	TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period 			= TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision 	= 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_DeInit(WS2812_TIMER);

	TIM_TimeBaseInit(WS2812_TIMER, &TIM_TimeBaseStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);

	/* Channel 3 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode 			= TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState 	= TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse 			= 0;
	TIM_OCInitStructure.TIM_OCPolarity 		= TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState 	= TIM_OCIdleState_Reset;

	WS2812_TIMER_OC_CMD(WS2812_TIMER, &TIM_OCInitStructure);

	/* Config DMA to update TIM1 Compare Register */
	TIM_DMAConfig(WS2812_TIMER, TIM_DMABase_CCR1, TIM_DMABurstLength_1Transfer);

	/* TIM1 DMA Update enable */
	TIM_DMACmd(WS2812_TIMER, TIM_DMA_CC4, ENABLE);

	/* TIM1 counter enable */
	TIM_Cmd(WS2812_TIMER, ENABLE);

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(WS2812_TIMER, ENABLE);
}


/**
  * @brief  Configure DMA1 CH4 to Update TIM1 CCR1.
  * @param  None
  * @retval None
  */
static void DMA_Config(void)
{
	DMA_InitTypeDef   DMA_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	/* DeInitialize the DMA1 Stream4 */
	DMA_DeInit(WS2812_DMA_CH);

	DMA_InitStructure.DMA_PeripheralBaseAddr 		= (uint32_t) &(WS2812_TIMER->WS2812_TIMER_OUT_CH);
	DMA_InitStructure.DMA_MemoryBaseAddr 			= (uint32_t)TX_Buffer;
	DMA_InitStructure.DMA_DIR 					    = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize 				= TX_BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc 				= DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize 		= DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode 					    = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority 				    = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M 					    = DMA_M2M_Disable;
	DMA_Init(WS2812_DMA_CH, &DMA_InitStructure);

	/* Enable DMA1 Stream4  */
	DMA_Cmd(WS2812_DMA_CH, ENABLE);

	/* Half full & Full interrupt ENABLE */
	DMA_ITConfig(WS2812_DMA_CH, DMA_IT_TC, ENABLE);
	DMA_ITConfig(WS2812_DMA_CH, DMA_IT_HT, ENABLE);

	/* Enable DMA1 channel IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = WS2812_DMA_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Calulate bitstream from given RGB Value
  * @param  None
  * @retval None
  */
static volatile uint8_t* addToTransmitBuffer(volatile uint8_t *bufferStart, volatile RgbLed_t led)
{
	volatile uint8_t* bufferNew = bufferStart;

	if( bufferStart == 0 )
	{
		return 0;
	}

	int i = 0, j = 0;
	uint8_t tmp = 0;

	for( j = 0; j < 3; ++j ){

		switch(j){
			case 0:	tmp = led.g; break;
			case 1: tmp = led.r; break;
			case 2: tmp = led.b; break;
		}

		for( i = 0; i < 8; ++i )
		{
			if( tmp & 0x80 )
			{
				*bufferNew++ = TimerPulseTX1;
			}
			else
			{
				*bufferNew++ = TimerPulseTX0;
			}
			tmp <<= 1;
		}
	}
	return bufferNew;
}


/* Public functions ---------------------------------------------------------*/
void WS2812_Init(){
	NumberLedsToUpdate	= 0;
	TimerPeriod 			= 0;
	TimerPulseTX0 		= 0;
	TimerPulseTX1 		= 0;

	uint16_t i = 0;
	for( i = 0; i < TX_BUFFER_SIZE; ++i )
	{
	  TX_Buffer[i] 		= 0;
	}

	/* TIM Configuration */
	TIM_Config();

	/* DMA Configuration */
	DMA_Config();
}

void WS2812_WirteBuffer(uint16_t buffer_pos, uint8_t r, uint8_t g, uint8_t b){
	if(buffer_pos <= LED_BUFFER_SIZE){
		LED_Buffer[buffer_pos].r 	= r;
		LED_Buffer[buffer_pos].g 	= g;
		LED_Buffer[buffer_pos].b 	= b;
	}
}

void WS2812_ReadBuffer(uint16_t buffer_pos, uint8_t *r, uint8_t *g, uint8_t *b){
	if(buffer_pos <= LED_BUFFER_SIZE){
		*r = LED_Buffer[buffer_pos].r;
		*g = LED_Buffer[buffer_pos].g;
		*b = LED_Buffer[buffer_pos].b;
	}else{
		*r = 0;
		*g = 0;
		*b = 0;
	}
}

void WS2812_Update(void){
	if(WS2812_Ready()){
		NumberLedsToUpdate = WS2812_LED_COUNT;
	}
}

uint8_t WS2812_Ready(void){
	if(NumberLedsToUpdate == 0){
		return 1;
	}else{
		return 0;
	}
}


void WS2812_DMA_IRQHandler(void){
	// check if half of DMA Transfer is complete
	if( DMA_GetITStatus(WS2812_DMA_IRQ_HT) ) {
		DMA_ClearITPendingBit(WS2812_DMA_IRQ_HT);
		// put new data in TX_Buffer starting at beginning and end at half of buffer
		TX_Tmp = TX_Buffer;
	}

	// check if DMA Transfer is complete
	else if( DMA_GetITStatus(WS2812_DMA_IRQ_TC) ) {
		DMA_ClearITPendingBit(WS2812_DMA_IRQ_TC);
		// put new data in TX_Buffer starting at half and end at end of buffer
		TX_Tmp = TX_Buffer + (TX_BUFFER_SIZE / 2);
	}

	int i = 0;
	for( i = 0; i < TX_BUFFER_SIZE / (2*24); ++i )
	{
		// if all LEDs from LED_Buffer are transmitted, send zeros
		if( NumberLedsToUpdate == 0 )
		{
			int i = 0;
			for( i = 0; i < 24; ++i ) {
				*TX_Tmp++ = 0;
			}
		}
		else {
			TX_Tmp = addToTransmitBuffer(TX_Tmp, LED_Buffer[LED_BUFFER_SIZE - NumberLedsToUpdate]);
			--NumberLedsToUpdate;
		}
	}
}

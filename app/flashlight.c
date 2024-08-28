#ifdef ENABLE_FLASHLIGHT

#include "driver/gpio.h"
#include "bsp/dp32g030/gpio.h"
#include "bsp/dp32g030/pwmplus.h"
#include "bsp/dp32g030/portcon.h"

#include "flashlight.h"

enum FlashlightMode_t  gFlashLightState;

#ifndef ENABLE_FLASHLIGHT_MOD

void FlashlightTimeSlice()
{
	if (gFlashLightState == FLASHLIGHT_BLINK && (gFlashLightBlinkCounter & 15u) == 0) {
		GPIO_FlipBit(&GPIOC->DATA, GPIOC_PIN_FLASHLIGHT);
		return;
	}

	if (gFlashLightState == FLASHLIGHT_SOS) {
		const uint16_t u = 15;
		static uint8_t c;
		static uint16_t next;

		if (gFlashLightBlinkCounter - next > 7 * u) {
			c = 0;
			next = gFlashLightBlinkCounter + 1;
			return;
		}

		if (gFlashLightBlinkCounter == next) {
			if (c==0) {
				GPIO_ClearBit(&GPIOC->DATA, GPIOC_PIN_FLASHLIGHT);
			} else {
				GPIO_FlipBit(&GPIOC->DATA, GPIOC_PIN_FLASHLIGHT);
			}

			if (c >= 18) {
				next = gFlashLightBlinkCounter + 7 * u;
				c = 0;
			} else if(c==7 || c==9 || c==11) {
				next = gFlashLightBlinkCounter + 3 * u;
			} else {
				next = gFlashLightBlinkCounter + u;
			}
			c++;
		}
	}
}

void ACTION_FlashLight(void)
{
	switch (gFlashLightState) {
		case FLASHLIGHT_OFF:
			gFlashLightState++;
			GPIO_SetBit(&GPIOC->DATA, GPIOC_PIN_FLASHLIGHT);
			break;
		case FLASHLIGHT_ON:
		case FLASHLIGHT_BLINK:
			gFlashLightState++;
			break;
		case FLASHLIGHT_SOS:
		default:
			gFlashLightState = 0;
			GPIO_ClearBit(&GPIOC->DATA, GPIOC_PIN_FLASHLIGHT);
	}
}

#else // ENABLE_FLASHLIGHT_MOD

void FLASHLIGHT_InitHardware()
{
	// Setup PWM on Flashlight pin
	// 48MHz / 94 / 1024 ~ 500Hz
	const uint32_t PWM_FREQUENCY_HZ =  1000;
	PWM_PLUS1_CLKSRC |= ((48000000 / 1024 / PWM_FREQUENCY_HZ) << 16);
	PWM_PLUS1_PERIOD = 1023;

	PORTCON_PORTC_SEL0 &= ~(0
		// Flashlight
		| PORTCON_PORTC_SEL0_C3_MASK
		);
	PORTCON_PORTC_SEL0 |= 0
		// Flashlight PWM
		| PORTCON_PORTC_SEL0_C3_BITS_PWMP1_CH1N
		;

	PWM_PLUS1_GEN = 	
		PWMPLUS_GEN_CH1N_OE_BITS_ENABLE |
		//PWMPLUS_GEN_CH1N_OUTINV_BITS_ENABLE |
		0;

	PWM_PLUS1_CFG =  	
		PWMPLUS_CFG_CNT_REP_BITS_ENABLE |
		PWMPLUS_CFG_COUNTER_EN_BITS_ENABLE |
		0;
}


void ACTION_FlashLight(void)
{
	// cycle through flashlight duty cycles
	switch (gFlashLightState) {
		case FLASHLIGHT_OFF:
			gFlashLightState++; // next state
			FLASHLIGHT_SetBrightness(0);
			break;
		case FLASHLIGHT_LOW:
			gFlashLightState++; // next state
			FLASHLIGHT_SetBrightness(5);
			break;
		case FLASHLIGHT_MED:
			gFlashLightState++; // next state
			FLASHLIGHT_SetBrightness(8);
			break;
		case FLASHLIGHT_HI:
		default:
			gFlashLightState = FLASHLIGHT_OFF; // next state
			FLASHLIGHT_SetBrightness(16);
			break;		
	}
}

void FLASHLIGHT_SetBrightness(uint8_t brightness)
{
	PWM_PLUS1_CH1_COMP = (1 << brightness) - 1; // 16 bits
	//PWM_PLUS1_SWLOAD = 1;
}
#endif

#endif


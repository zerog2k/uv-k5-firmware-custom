#ifndef APP_FLASHLIGHT_H
#define APP_FLASHLIGHT_H

#ifdef ENABLE_FLASHLIGHT

#include <stdint.h>

#ifndef ENABLE_FLASHLIGHT_MOD
enum FlashlightMode_t {
    FLASHLIGHT_OFF = 0,
    FLASHLIGHT_ON,
    FLASHLIGHT_BLINK,
    FLASHLIGHT_SOS
};
#else
enum FlashlightMode_t {
    FLASHLIGHT_OFF = 0,
    FLASHLIGHT_LOW,
    FLASHLIGHT_MED,
    FLASHLIGHT_HI
};

void FLASHLIGHT_SetBrightness(uint8_t brightness);
void FLASHLIGHT_InitHardware();
#endif

extern enum FlashlightMode_t gFlashLightState;
extern volatile uint16_t     gFlashLightBlinkCounter;

void FlashlightTimeSlice(void);
void ACTION_FlashLight(void);


#endif

#endif
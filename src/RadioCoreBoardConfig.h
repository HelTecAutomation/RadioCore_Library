#ifndef RADIOCORE_BOARD_CONFIG_H
#define RADIOCORE_BOARD_CONFIG_H

#if defined(HELTEC_RC52)
#include "boards/heltec_rc52.h"
#elif defined(HELTEC_RC32)
#include "boards/heltec_rc32.h"
#elif defined(HELTEC_RCC6)
#include "boards/heltec_rcc6.h"
#else
#define RADIOCORE_HAS_SENSOR_I2C 0
#define RADIOCORE_HAS_SENSOR_POWER_CTRL 0
#define RADIOCORE_HAS_CH01_ADC 0
#define RADIOCORE_HAS_CH01_POWER_CTRL 0
#define RADIOCORE_HAS_VO01_ADC 0
#define RADIOCORE_HAS_VO01_POWER_CTRL 0
#define RADIOCORE_HAS_CO01_ADC 0
#define RADIOCORE_HAS_CO01_POWER_CTRL 0
#endif

#endif // RADIOCORE_BOARD_CONFIG_H

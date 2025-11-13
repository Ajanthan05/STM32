#ifndef __DAC_CTRL_H
#define __DAC_CTRL_H

//#ifdef __cplusplus
//extern "C" {
//#endif


#include <stdint.h>
#include <stdbool.h>

#include "board.h"
//#include "stm32l0xx_hal.h"
#include <math.h>
#include "main.h"

/* Status codes */
typedef enum {
    DAC_CTRL_OK = 0,
    DAC_CTRL_ERR = -1,
} dac_ctrl_status_t;

dac_ctrl_status_t dac_init(void);

// It set channel outputs in volts. Volts will be clipped to [0, BOARD_VREF_V].
dac_ctrl_status_t dac_set_voltage_ch1(float volts);
dac_ctrl_status_t dac_set_voltage_ch2(float volts);

/* Convenience: set channel from an engineering range.
   value: input engineering value (e.g. pressure in kPa)
   in_min/in_max: expected input range
   out_min/out_max: output voltage range in volts (typically 0..BOARD_VREF_V)
   result is clipped to out_min/out_max then converted to DAC code */
dac_ctrl_status_t dac_set_scaled_ch1(float value, float in_min, float in_max, float out_min, float out_max);
dac_ctrl_status_t dac_set_scaled_ch2(float value, float in_min, float in_max, float out_min, float out_max);

#endif

#include "dac_ctrl.h"

//#include "stm32l0xx_hal.h"

extern DAC_HandleTypeDef hdac;

static inline uint32_t volts_to_code(float volts)
{
    if (volts <= 0.0f) return 0u;
    if (volts >= BOARD_VREF_V) return DAC_MAX_CODE;
    /* compute and round */
    const float ratio = volts / BOARD_VREF_V;
    const float codef = ratio * (float)DAC_MAX_CODE;
    uint32_t code = (uint32_t)(codef + 0.5f);
    if (code > DAC_MAX_CODE) code = DAC_MAX_CODE;
    return code;
}

static inline float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

dac_ctrl_status_t dac_init(void)
{
    HAL_StatusTypeDef st;

    st = HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    if (st != HAL_OK) return DAC_CTRL_ERR;

    st = HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
    if (st != HAL_OK) return DAC_CTRL_ERR;

    return DAC_CTRL_OK;
}

/* Write code to channel with 12-bit right alignment */
static dac_ctrl_status_t write_dac_channel(uint32_t code, uint32_t channel)
{
    HAL_StatusTypeDef st;
    st = HAL_DAC_SetValue(&hdac, channel, DAC_ALIGN_12B_R, code);
    if (st != HAL_OK) return DAC_CTRL_ERR;
    /* Optionally restart or ensure channel running - not needed if started in dac_init */
    return DAC_CTRL_OK;
}

dac_ctrl_status_t dac_set_voltage_ch1(float volts)
{
    volts = clampf(volts, 0.0f, BOARD_VREF_V);
    uint32_t code = volts_to_code(volts);
    return write_dac_channel(code, DAC_CHANNEL_1);
}

dac_ctrl_status_t dac_set_voltage_ch2(float volts)
{
    volts = clampf(volts, 0.0f, BOARD_VREF_V);
    uint32_t code = volts_to_code(volts);
    return write_dac_channel(code, DAC_CHANNEL_2);
}

/* Map engineering value -> voltage -> DAC code, and write it */
static float map_linear(float x, float in_min, float in_max, float out_min, float out_max)
{
    if (in_max == in_min) return out_min; /* avoid div by 0 */
    float t = (x - in_min) / (in_max - in_min);
    return out_min + t * (out_max - out_min);
}

dac_ctrl_status_t dac_set_scaled_ch1(float value, float in_min, float in_max, float out_min, float out_max)
{
    /* Map value to volts, then clip to out_min/out_max and finally clip to ASIC VREF */
    float volts = map_linear(value, in_min, in_max, out_min, out_max);
    volts = clampf(volts, out_min, out_max);
    /* ensure we don't exceed board VREF */
    volts = clampf(volts, 0.0f, BOARD_VREF_V);
    return dac_set_voltage_ch1(volts);
}

dac_ctrl_status_t dac_set_scaled_ch2(float value, float in_min, float in_max, float out_min, float out_max)
{
    float volts = map_linear(value, in_min, in_max, out_min, out_max);
    volts = clampf(volts, out_min, out_max);
    volts = clampf(volts, 0.0f, BOARD_VREF_V);
    return dac_set_voltage_ch2(volts);
}

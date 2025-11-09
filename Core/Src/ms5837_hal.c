#include "main.h"
#include "ms5837_hal.h"
#include <stdio.h>

#define MS5837_8BIT_ADDR (MS5837_ADDR << 1) // HAL expects addr<<1 for TX/RX
extern I2C_HandleTypeDef hi2c1;

static ms583730ba01_err_t hal_write_cmd(uint8_t cmd) {
    HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(&hi2c1, MS5837_8BIT_ADDR, &cmd, 1, 50);
    return (st == HAL_OK) ? E_MS58370BA01_SUCCESS : E_MS58370BA01_COM_ERR;
}

static ms583730ba01_err_t hal_read_data(uint8_t *buf, uint32_t n) {
    HAL_StatusTypeDef st = HAL_I2C_Master_Receive(&hi2c1, MS5837_8BIT_ADDR, buf, n, 100);
    return (st == HAL_OK) ? E_MS58370BA01_SUCCESS : E_MS58370BA01_COM_ERR;
}

static void hal_delay(uint16_t ms) {
    HAL_Delay(ms);
}

/* This function builds the handle struct you pass to ms5837 functions */
void ms5837_hal_handle_init(ms583730ba01_h *h) {
    h->write_cmd = hal_write_cmd;
    h->read_data = hal_read_data;
    h->delay = hal_delay;
}

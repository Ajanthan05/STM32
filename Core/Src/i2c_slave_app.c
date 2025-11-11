#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#include "i2c_slave_app.h"

extern I2C_HandleTypeDef hi2c2;

static uint8_t  i2c2_rx_buf[4];
static uint8_t  i2c2_tx_buf[4];


volatile bool    i2c2_rx_complete   = false;   
volatile uint32_t received_cmd_value = 0;      
volatile uint32_t response_value     = 12345678; 

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c != &hi2c2) return;

    HAL_I2C_EnableListen_IT(hi2c);
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c,
                          uint8_t TransferDirection,
                          uint16_t AddrMatchCode)
{
    if (hi2c != &hi2c2) return;

    if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
        HAL_I2C_Slave_Receive_IT(hi2c, i2c2_rx_buf, 4);
    } else { /* master wants to read from us */
        /* Prepare tx buffer (big-endian MSB first) */
        i2c2_tx_buf[0] = (uint8_t)((response_value >> 24) & 0xFF);
        i2c2_tx_buf[1] = (uint8_t)((response_value >> 16) & 0xFF);
        i2c2_tx_buf[2] = (uint8_t)((response_value >>  8) & 0xFF);
        i2c2_tx_buf[3] = (uint8_t)((response_value >>  0) & 0xFF);

        HAL_I2C_Slave_Transmit_IT(hi2c, i2c2_tx_buf, 4);
    }
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c != &hi2c2) return;

    uint32_t val =  ((uint32_t)i2c2_rx_buf[0] << 24) |
                    ((uint32_t)i2c2_rx_buf[1] << 16) |
                    ((uint32_t)i2c2_rx_buf[2] <<  8) |
                    ((uint32_t)i2c2_rx_buf[3] <<  0);

    received_cmd_value = val;

    i2c2_rx_complete = true;
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c != &hi2c2) return;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c != &hi2c2) return;

    uint32_t err = HAL_I2C_GetError(hi2c);

    if (err & HAL_I2C_ERROR_AF) {
        i2c2_rx_complete = true;
    }

    HAL_I2C_EnableListen_IT(hi2c);
}

void process_received_command_from_main(void)
{
    response_value = received_cmd_value + 1;
}

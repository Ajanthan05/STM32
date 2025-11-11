#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#include "i2c_slave_app.h"

extern I2C_HandleTypeDef hi2c2;

static uint8_t  i2c2_rx_buf[4];
static uint8_t  i2c2_tx_buf[4];


volatile bool    i2c2_rx_complete   = false;   /* signaled to main when 4-bytes received */
volatile uint32_t received_cmd_value = 0;      /* populated by ISR on RX complete */
volatile uint32_t response_value     = 12345678; /* value returned to master on read */


/* Called when STOP or Listen sequence finished (end of a listen transaction) */
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c != &hi2c2) return;

    /* Re-enter listen mode so we accept next master request */
    HAL_I2C_EnableListen_IT(hi2c);
}

/* Called on address match (master started transfer to/from us) */
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c,
                          uint8_t TransferDirection,
                          uint16_t AddrMatchCode)
{
    if (hi2c != &hi2c2) return;

    if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
        /* Master will write to us: arm HAL to receive 4 bytes (interrupt mode) */
        HAL_I2C_Slave_Receive_IT(hi2c, i2c2_rx_buf, 4);
    } else { /* master wants to read from us */
        /* Prepare tx buffer (big-endian MSB first) */
        i2c2_tx_buf[0] = (uint8_t)((response_value >> 24) & 0xFF);
        i2c2_tx_buf[1] = (uint8_t)((response_value >> 16) & 0xFF);
        i2c2_tx_buf[2] = (uint8_t)((response_value >>  8) & 0xFF);
        i2c2_tx_buf[3] = (uint8_t)((response_value >>  0) & 0xFF);

        /* Start the interrupt-driven transmit of 4 bytes */
        HAL_I2C_Slave_Transmit_IT(hi2c, i2c2_tx_buf, 4);
    }
}

/* Called when the HAL completed receiving the configured RX length (4 bytes here) */
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c != &hi2c2) return;

    /* Assemble received 4 bytes into 32-bit (big-endian ordering) */
    uint32_t val =  ((uint32_t)i2c2_rx_buf[0] << 24) |
                    ((uint32_t)i2c2_rx_buf[1] << 16) |
                    ((uint32_t)i2c2_rx_buf[2] <<  8) |
                    ((uint32_t)i2c2_rx_buf[3] <<  0);

    /* Publish atomically to shared volatile variable (32-bit write is atomic on Cortex-M) */
    received_cmd_value = val;

    /* Signal main loop to process value (do NOT do heavy work here) */
    i2c2_rx_complete = true;

    /* Note:
       - We do NOT immediately re-arm another HAL_I2C_Slave_Receive_IT here.
       - AddrCallback will arm receive on next write from master.
       - Listen mode (EnableListen_IT) handles address matching and STOP/RESTART semantics.
    */
}

/* Called when a slave transmit completes */
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c != &hi2c2) return;
    /* Nothing else required here. Keep short. */
}

/* Correct error callback name to override HAL weak function */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c != &hi2c2) return;

    uint32_t err = HAL_I2C_GetError(hi2c);

    /* If ACK failure (master NACK) or other end-of-transfer semantics occur,
       choose whether to treat it as "end of transfer" for partial processing.
       Here: treat AF as end-of-transfer and signal main to process partial data.
    */
    if (err & HAL_I2C_ERROR_AF) {
        /* The master NACKed — consider that transfer finished; process partial data if needed */
        i2c2_rx_complete = true;
    }

    /* Re-enter listen mode for future transactions */
    HAL_I2C_EnableListen_IT(hi2c);
}

void process_received_command_from_main(void)
{
    // Example: update response based on received command
    response_value = received_cmd_value + 1;
}

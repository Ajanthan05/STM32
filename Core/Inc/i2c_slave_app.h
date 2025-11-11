#ifndef I2C_SLAVE_APP_H
#define I2C_SLAVE_APP_H

#include "main.h"
#include <stdbool.h>

extern volatile bool     i2c2_rx_complete;
extern volatile uint32_t received_cmd_value;
extern volatile uint32_t response_value;

void process_received_command_from_main(void);

#endif

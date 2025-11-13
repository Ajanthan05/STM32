#ifndef DAC_CALIB_H
#define DAC_CALIB_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float gain;
    float offset;
    bool  valid;
} dac_calib_t;

void dac_calib_init(dac_calib_t *c);
bool dac_calib_compute_from_points(dac_calib_t *c,
                                   float cmd1, float meas1,
                                   float cmd2, float meas2);
float dac_calib_apply(const dac_calib_t *c, float desired_volts);

#endif

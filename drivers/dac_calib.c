#include "dac_calib.h"
#include <math.h>

void dac_calib_init(dac_calib_t *c)
{
    if (!c) return;
    c->gain   = 1.0f;
    c->offset = 0.0f;
    c->valid  = true;
}

bool dac_calib_compute_from_points(dac_calib_t *c,
                                   float cmd1, float meas1,
                                   float cmd2, float meas2)
{
    if (!c) return false;
    if (fabsf(cmd2 - cmd1) < 1e-6f) {
        c->valid = false;
        return false;
    }

    c->gain   = (meas2 - meas1) / (cmd2 - cmd1);
    c->offset = meas1 - c->gain * cmd1;

    if (fabsf(c->gain) < 1e-6f) {
        c->valid = false;
        return false;
    }

    c->valid = true;
    return true;
}

float dac_calib_apply(const dac_calib_t *c, float desired_volts)
{
    if (!c || !c->valid || fabsf(c->gain) < 1e-6f) {
        return desired_volts;
    }
    float cmd = (desired_volts - c->offset) / c->gain;
    return cmd;
}

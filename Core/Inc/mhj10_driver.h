
#ifndef MHJ10_DRIVER_H
#define MHJ10_DRIVER_H

#include "tim.h"
#include "gpio.h"

struct MHJ10_Valve_Params {
    TIM_HandleTypeDef valve_timer_handler;
    uint32_t inlet_timer_channel;
    uint32_t outlet_timer_channel;
};


#endif //MHJ10_DRIVER_H

#ifndef _A4988_stepper_H_
#define _A4988_stepper_H_

#include "Arduino.h"

class A4988_stepper {
public:
    A4988_stepper(uint8_t dir_pin, uint8_t step_pin, byte micro_step);
    void set_period(uint16_t period);
    int get_period();
    void set_dir(bool dir); //0 : CCW, 1 : CW
    void rotate_step(uint8_t step);
    void rotate_step_with_delay(uint8_t step, unsigned long mdelay);


private:
    uint8_t _dir_pin, _step_pin;
    byte _micro_step;
    uint16_t _period;
};

#endif
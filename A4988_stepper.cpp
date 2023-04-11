#include "A4988_stepper.h"

A4988_stepper::A4988_stepper(uint8_t dir_pin, uint8_t step_pin, byte micro_step) : _dir_pin(dir_pin), _step_pin(step_pin), _micro_step(micro_step) {
    _period = 0;
    pinMode(_step_pin, OUTPUT);
    pinMode(_dir_pin, OUTPUT);
}

void A4988_stepper::set_period(uint16_t period) {
    _period = period;
}

int A4988_stepper::get_period() {
    return _period;
}

void A4988_stepper::set_dir(bool dir) {
    digitalWrite(_dir_pin, !dir);
}

void A4988_stepper::rotate_step(uint8_t step) {
    for(int i=0; i<step; i++) {  
        digitalWrite(_step_pin, HIGH);  
        delayMicroseconds(_period);  
        digitalWrite(_step_pin, LOW);  
        delayMicroseconds(_period);  
    }  
}

void A4988_stepper::rotate_step_with_delay(uint8_t step, unsigned long mdelay) {
    Serial.println(mdelay);
    for(int i=0; i<step-1; i++) {  
        digitalWrite(_step_pin, HIGH);  
        delayMicroseconds(_period);  
        digitalWrite(_step_pin, LOW);  
        delayMicroseconds(_period);  
    }  
    digitalWrite(_step_pin, HIGH);  
    delayMicroseconds(_period);  
    digitalWrite(_step_pin, LOW);  
    if(1000*mdelay < _period) delayMicroseconds(_period-1000*mdelay);
}
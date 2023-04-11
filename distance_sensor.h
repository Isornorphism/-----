#ifndef _distance_sensor_H_
#define _distance_sensor_H_

#include "Arduino.h"

class distance_sensor {
public:
    distance_sensor(uint8_t pin) : _pin(pin) {}
    float get_distance();

private:
    uint8_t _pin;
};

#endif
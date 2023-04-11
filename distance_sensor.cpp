#include "distance_sensor.h"

float distance_sensor::get_distance() {
    double val = 0;
    for(int i=0; i<5; i++) {
        val += 299.88 * pow(analogRead(_pin)*5./1024., -1.173);
    }
    return val /= 5.;
}
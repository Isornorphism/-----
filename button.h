#ifndef _button_H_
#define _button_H_

#include "Arduino.h"

class button {
public:
    button(uint8_t pin, bool pull_type); //pull_type = 0 : pull_down, 1 : pull_up
    byte check_button();
    bool is_clicked();
    byte is_single_or_double_clicked();
    bool is_pushed();

private:
    uint8_t _pin;
    bool _pull_type;

    unsigned long _debounce_time;
    unsigned long _time_out;
    enum { None, SingleClick, DoubleClick, YesSingle};

    unsigned long _last_msec;
    unsigned long _deb_start_msec;
    bool _last_but_state;
};

#endif
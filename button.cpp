#include "button.h"

button::button(uint8_t pin, bool pull_type) {
    _pin = pin;
    _debounce_time = 100;
    _time_out = 250;
    _last_msec = 0;
    _deb_start_msec = 0;
    _last_but_state = 0;
    if(pull_type) pinMode(_pin, INPUT_PULLUP);
    else pinMode(_pin, INPUT);
}


byte button::check_button() {
    unsigned long now_msec = millis();
    if (_last_msec && (now_msec - _last_msec) > _time_out)  {
        _last_msec = 0;
        return YesSingle;
    }
    bool but = digitalRead(_pin);
    if(but != _last_but_state)  {
        if (millis() - _deb_start_msec < _debounce_time) return None;
        _deb_start_msec = millis();
        _last_but_state = but;

        if(!but) {
            if(_last_msec) {
                _last_msec = 0;
                return DoubleClick;
            }
            else {
                _last_msec = now_msec==0 ? 0 : now_msec;
                return SingleClick;
            }
        }
    }
    return None;
}

bool button::is_clicked() {
    if(check_button()==SingleClick) return true;
    else return false;
}

byte button::is_single_or_double_clicked() { //0 : n0, 1 : single, 2 : double
    if(check_button()==SingleClick) {
        while(true) {
            byte state = check_button();
            if(state == YesSingle) return SingleClick;
            else if(state == DoubleClick) return DoubleClick;
        }
    }
    return None;
}

bool button::is_pushed() {
    return !digitalRead(_pin);
}
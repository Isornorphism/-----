#ifndef _lcd_H_
#define _lcd_H_

#include "Arduino.h"

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

class lcd : public LiquidCrystal_I2C {
public:
    lcd();
    void custom_scroll(String text, bool line, unsigned long mdelay);
    void bar_graph_with_blick(float progress, byte char_num, byte start, bool line, unsigned long mdelay);
    void bar_graph(float progress, byte char_num, byte start, bool line);
    void createChar_P(uint8_t addr, const uint8_t* bitmap);
    void createChar_P(uint8_t addr, const char* bitmap);

private:
    byte _scroll_cnt;
    String _scroll_text;
    unsigned long _last_time;
    bool _blink_state;
};

#endif
#include "lcd.h"

lcd::lcd() : LiquidCrystal_I2C(0x27, 16, 2) {
    _scroll_cnt = 0;
    _scroll_text = "";
    _last_time = 0;
    _blink_state = 0;
}

void lcd::createChar_P(uint8_t addr, const uint8_t* bitmap) {
    uint8_t n;
    command(LCD_SETCGRAMADDR | ((addr % 8) * 8));
    for(n=0; n<8; n++) {
        write(pgm_read_byte(bitmap+n));
    }
}

void lcd::createChar_P(uint8_t addr, const char* bitmap) {
    createChar_P(addr, (const uint8_t*)(bitmap));
}

void lcd::custom_scroll(String text, bool line, unsigned long mdelay) {
    if(text!=_scroll_text) {
        _scroll_cnt = 0;
        _scroll_text = text;
    }
    if(millis() - _last_time > mdelay) {
        String print_text = "                ";
        print_text += _scroll_text;
        print_text += "                ";
        setCursor(0, line);
        print(print_text.substring(_scroll_cnt, _scroll_cnt+16));
        _last_time = millis();
        _scroll_cnt = (_scroll_cnt + 1) % (16+_scroll_text.length());
    }
}

void lcd::bar_graph_with_blick(float progress, byte char_num, byte start, bool line, unsigned long mdelay) {
    if(millis() - _last_time > mdelay) {
        byte total_block = 2*char_num-2;
        byte now_block = byte(progress * float(total_block));
        _blink_state = 1 - _blink_state;
        setCursor(start, line);
        if(_blink_state) {
            write(byte(1));
            if(now_block==0) {
                for(int i=0; i<char_num-2; i++) write(byte(2));
                write(byte(5));
            }
            else if(now_block==total_block-1) {
                for(int i=0; i<char_num-2; i++) write(byte(4));
                write(byte(6));
            }
            else {
                for(int i=0; i<now_block/2; i++) write(byte(4));
                if(now_block != total_block-2) write(byte(2+now_block%2));
                for(int i=0; i<char_num-now_block/2-3; i++) write(byte(2));
                write(byte(5));
            }
        }
        else {
            if(now_block==0) {
                write(byte(0));
                for(int i=0; i<char_num-2; i++) write(byte(2));
            }
            else if(now_block==total_block-1) {
                write(byte(1));
                for(int i=0; i<char_num-2; i++) write(byte(4));
            }
            else {
                write(byte(1));
                for(int i=0; i<(now_block-1)/2; i++) write(byte(4));
                write(byte(3-now_block%2));
                for(int i=0; i<char_num-(now_block-1)/2-3; i++) write(byte(2));
            }
            write(byte(5));
        }
        _last_time = millis();
    }
}

void lcd::bar_graph(float progress, byte char_num, byte start, bool line) {
    byte total_block = 2*char_num-2;
    byte now_block = byte(progress * float(total_block));
    setCursor(start, line);
    if(now_block==0) {
        write(byte(0));
        for(int i=0; i<char_num-2; i++) write(byte(2));
    }
    else if(now_block==total_block-1) {
        write(byte(1));
        for(int i=0; i<char_num-2; i++) write(byte(4));
    }
    else {
        write(byte(1));
        for(int i=0; i<(now_block-1)/2; i++) write(byte(4));
        write(byte(3-now_block%2));
        for(int i=0; i<char_num-(now_block-1)/2-3; i++) write(byte(2));
    }
    write(byte(5));
}
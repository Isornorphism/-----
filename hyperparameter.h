enum SCENE {INIT=0, SPEED_SET, THETA_RESOL_SET, Z_RESOL_SET, ORIGIN_SET, OBJ_SET, PRE_SCAN, SCAN};

#define SCREW_SPEED 150
#define STEPS_PER_REV 200
#define PLATE_MICRO_STEP 16
#define SD_CHECK_DURATION 5000
#define CCW 0
#define CW 1
#define CENTER_DISTANCE 158
#define MAX_DISTANCE 250

String plate_speed_text[4] = {"SLOW", "MEDIUM", "FAST", "VERY FAST"};
const uint16_t plate_speed_list[4] PROGMEM = {4000, 3000, 2000, 1000};
String resol_text[4] = {"LOW", "MEDIUM", "HIGH"};
const uint16_t theta_resol_list[3] PROGMEM = {STEPS_PER_REV/2, STEPS_PER_REV, STEPS_PER_REV*2}; //3.6deg, 1.8deg, 0.9deg
const uint16_t z_resol_list[3] PROGMEM = {STEPS_PER_REV*3/8, STEPS_PER_REV/4, STEPS_PER_REV/8}; //3mm, 2mm, 1mm

const byte _custom_char[7][8] PROGMEM = { 
    {
        B01111,
        B10000,
        B10000,
        B10000,
        B10000,
        B10000,
        B10000,
        B01111
    },
    {
        B01111,
        B10000,
        B10111,
        B10111,
        B10111,
        B10111,
        B10000,
        B01111
    },
    {
        B11111,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B11111
    },
    {
        B11111,
        B00000,
        B11000,
        B11000,
        B11000,
        B11000,
        B00000,
        B11111
    },
    {
        B11111,
        B00000,
        B11011,
        B11011,
        B11011,
        B11011,
        B00000,
        B11111
    },
    {
        B11110,
        B00001,
        B00001,
        B00001,
        B00001,
        B00001,
        B00001,
        B11110
    },
    {
        B11110,
        B00001,
        B11101,
        B11101,
        B11101,
        B11101,
        B00001,
        B11110
    }
};
#include <Encoder.h>
#include <SPI.h>
#include <SD.h>
#include "button.h"
#include "lcd.h"
#include "distance_sensor.h"
#include <Stepper.h>
#include "A4988_stepper.h"
#include "hyperparameter.h"

lcd mylcd;
Encoder myEnc(2, 3);
button rotary_sw(A0, 1);
button limit_sw(A2, 0);
distance_sensor dis_sensor(A1);
Stepper screw_stepper = Stepper(STEPS_PER_REV, 4, 5, 6, 7);
A4988_stepper plate_stepper(9, 8, PLATE_MICRO_STEP);

File myFile;

unsigned long last_time = 0;
bool SD_flag = 0;
bool origin_flag = 0;
bool pre_scan_flag = 0;
SCENE scene_num = INIT;
long enc_new_pos, enc_old_pos = 0;
uint16_t total_z_slices = 0;
uint16_t init_z = 0;

byte plate_speed = 1;
byte theta_resol = 1;
byte z_resol = 1;

void stage_control(SCENE next_scene, SCENE prev_scene) {
    byte rotary_sw_state = rotary_sw.is_single_or_double_clicked();
    if(rotary_sw_state==1) {
        scene_num = next_scene;
        mylcd.clear();
    }
    else if(rotary_sw_state==2) {
        scene_num = prev_scene;
        mylcd.clear();
    }
}

uint8_t print_file_num(File dir) {
    uint8_t num = 0;
    while (true) {
        File entry =  dir.openNextFile();
        if (!entry) return num;
        else num++;
        entry.close();
    }
}

void setup() {
    mylcd.init();
  	mylcd.backlight();
    mylcd.clear();
    for(byte i=0; i<7; i++) {
        mylcd.createChar_P(i, _custom_char[i]);
    }
    screw_stepper.setSpeed(SCREW_SPEED);
}

void loop() {
    switch(scene_num) {
        case INIT:
        mylcd.custom_scroll(F("Press button to start scanning!!"), 0, 200);
        if(millis()-last_time > SD_CHECK_DURATION) {
            last_time = millis();
            if(!SD.begin()) {
                mylcd.setCursor(0, 1);
                mylcd.print(F(" * SD Card  X *"));
                SD_flag = 0;
            }
            else {
                mylcd.setCursor(0, 1);
                mylcd.print(F(" * SD Card  O *"));
                SD_flag = 1;
            }
        }

        if(SD_flag == 1 && rotary_sw.is_clicked()) {
            scene_num = SPEED_SET;
            SD_flag = 0;
            mylcd.clear();
        }
        break;


        case SPEED_SET:
        mylcd.custom_scroll(F("1. Set plate speed"), 0, 200);
        mylcd.setCursor(0, 1);
        mylcd.print(plate_speed_text[plate_speed]);

        enc_new_pos = myEnc.read();
        if(enc_new_pos < enc_old_pos-2) { //CW
            enc_old_pos = enc_new_pos;
            if(plate_speed<3) {
                plate_speed++;
                mylcd.clear();
            }
        }
        else if(enc_new_pos > enc_old_pos+2) { //CCW
            enc_old_pos = enc_new_pos;
            if(plate_speed>0) {
                plate_speed--;
                mylcd.clear();
            }
        }

        stage_control(THETA_RESOL_SET, INIT);
        break;


        case THETA_RESOL_SET:
        mylcd.custom_scroll(F("2. Set resolution of theta-dir"), 0, 200);
        mylcd.setCursor(0, 1);
        mylcd.print(resol_text[theta_resol]);

        enc_new_pos = myEnc.read();
        if(enc_new_pos < enc_old_pos-2) { //CW
            enc_old_pos = enc_new_pos;
            if(theta_resol<2) {
                theta_resol++;
                mylcd.clear();
            }
        }
        else if(enc_new_pos > enc_old_pos+2) { //CCW
            enc_old_pos = enc_new_pos;
            if(theta_resol>0) {
                theta_resol--;
                mylcd.clear();
            }
        }
        
        stage_control(Z_RESOL_SET, SPEED_SET);
        break;


        case Z_RESOL_SET:
        origin_flag = 0;
        mylcd.custom_scroll(F("3. Set resolution of z-dir"), 0, 200);
        mylcd.setCursor(0, 1);
        mylcd.print(resol_text[z_resol]);

        enc_new_pos = myEnc.read();
        if(enc_new_pos < enc_old_pos-2) { //CW
            enc_old_pos = enc_new_pos;
            if(z_resol<2) {
                z_resol++;
                mylcd.clear();
            }
        }
        else if(enc_new_pos > enc_old_pos+2) { //CCW
            enc_old_pos = enc_new_pos;
            if(z_resol>0) {
                z_resol--;
                mylcd.clear();
            }
        }
        
        stage_control(ORIGIN_SET, THETA_RESOL_SET);
        break;


        case ORIGIN_SET:
        if(origin_flag==0) {
            mylcd.setCursor(0, 0);
            mylcd.print(F("Move to"));
            mylcd.setCursor(0, 1);
            mylcd.print(F("the lowest pt"));
            while(!limit_sw.is_pushed()) {
                screw_stepper.step(-1);
                delay(1);
            }
            origin_flag = 1;
            mylcd.clear();
        }
        else {
            mylcd.setCursor(0, 0);
            mylcd.print(F("Set z=0 point"));
            mylcd.custom_scroll(F("CW : up, CCW : down"), 1, 200);

            enc_new_pos = myEnc.read();
            if(enc_new_pos < enc_old_pos-2) { //CW
                enc_old_pos = enc_new_pos;
                screw_stepper.step(STEPS_PER_REV/8);
                init_z++;
            }
            else if(enc_new_pos > enc_old_pos+2) { //CCW
                enc_old_pos = enc_new_pos;
                if(!limit_sw.is_pushed()) screw_stepper.step(-STEPS_PER_REV/8);
                init_z--;
            }

            stage_control(OBJ_SET, Z_RESOL_SET);
        }
        break;


        case OBJ_SET:
        origin_flag = 0;
        pre_scan_flag = 0;
        mylcd.setCursor(0, 0);
        mylcd.print(F("Place the object"));
        mylcd.setCursor(0, 1);
        mylcd.print(F("on the plate."));

        stage_control(PRE_SCAN, ORIGIN_SET);
        break;


        case PRE_SCAN:
        if(pre_scan_flag==0) {
            mylcd.setCursor(0, 0);
            mylcd.print(F("Pre-scanning..."));
            plate_stepper.set_dir(CW);
            plate_stepper.set_period(pgm_read_word_near(plate_speed_list+plate_speed));
            while(!pre_scan_flag) {
                while(dis_sensor.get_distance() < MAX_DISTANCE) {
                    screw_stepper.step(pgm_read_word_near(z_resol_list+z_resol));
                    total_z_slices++;
                }
                for(uint16_t i=0; i<pgm_read_word_near(theta_resol_list+theta_resol); i++) {
                    plate_stepper.rotate_step(STEPS_PER_REV*PLATE_MICRO_STEP/pgm_read_word_near(theta_resol_list+theta_resol));
                    if(dis_sensor.get_distance() < MAX_DISTANCE) break;
                    if(i==pgm_read_word_near(theta_resol_list+theta_resol)-1) {
                        pre_scan_flag = 1;
                        mylcd.clear();
                    }
                }
            }
        }
        else {
            mylcd.setCursor(0, 0);
            mylcd.print(F("Total z-slices:"));
            mylcd.setCursor(0, 1);
            mylcd.print(total_z_slices);

            stage_control(SCAN, OBJ_SET);
        }
        break;


        case SCAN:
        pre_scan_flag = 0;
        while(!limit_sw.is_pushed()) {
            screw_stepper.step(-1);
            delay(1);
        }
        for(uint16_t i=0; i<init_z; i++) screw_stepper.step(STEPS_PER_REV/8);
        
        myFile = SD.open("/");
        String file_name = String(print_file_num(myFile))+".txt";
        mylcd.setCursor(0, 0);
        mylcd.print(file_name);

        myFile = SD.open(file_name, FILE_WRITE);
        if(!myFile) {
            mylcd.setCursor(0, 0);
            mylcd.print("SD card");
            mylcd.setCursor(0, 1);
            mylcd.print("dosen't exist!!");
            delay(5000);
            scene_num = INIT;
            total_z_slices = 0;
            init_z = 0;
            return;
        }

        float measure_dis, z, theta, progress;
        for(uint16_t i=0; i<total_z_slices; i++) {
            z = (float)i * pgm_read_word_near(z_resol_list+z_resol) * 8./STEPS_PER_REV;
            progress = (float)i/total_z_slices;
            mylcd.setCursor(0, 1);
            mylcd.print(String(round(progress*100))+"%");
            mylcd.bar_graph(progress, 12, 4, 1);
            for(uint16_t j=0; j<pgm_read_word_near(theta_resol_list+theta_resol); j++) {
                last_time = millis();
                measure_dis = dis_sensor.get_distance();
                if(measure_dis < MAX_DISTANCE) {
                    theta = (float)j * 2.*PI / pgm_read_word_near(theta_resol_list+theta_resol);
                    myFile.print((CENTER_DISTANCE - measure_dis) * cos(theta));
                    myFile.print(", ");
                    myFile.print((CENTER_DISTANCE - measure_dis) * sin(theta));
                    myFile.print(", ");
                    myFile.println(z);
                }
                plate_stepper.rotate_step_with_delay(STEPS_PER_REV*PLATE_MICRO_STEP/pgm_read_word_near(theta_resol_list+theta_resol), millis()-last_time);
            }
            screw_stepper.step(pgm_read_word_near(z_resol_list+z_resol));
        }
        myFile.close();

        mylcd.clear();
        mylcd.setCursor(0, 0);
        mylcd.print("Done!!!");
        delay(5000);
        scene_num = INIT;
        total_z_slices = 0;
        init_z = 0;
        break;
    }
}

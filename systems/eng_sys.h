#pragma once
#ifndef ENGSYS
#define ENGSYS


#include "../common_sys.h"

class Engine {
private:
public:
    void init() {
        //TODO
    }
    void update() {
        
    }
};

class APUEngine {
private:
    const int apu_flap_delay = 3 + rand() % 14;
    const int bleed_pressure_drop = 2 + rand() % 2;
    const double apu_cooling_coef = 2;          //2 degrees per sec
    const double apu_spool_down_coef = 2;       //3% per sec
    const double apu_N1_const = 0;
    const double apu_N1_x = 2.375010484;
    const double apu_N1_x2 = 0.034236847;
    const double apu_N1_x3 = -0.007404136;
    const double apu_N1_x4 = 0.000268;
    const double apu_N1_x5 = -0.000002438;
    const double apu_n1_temp_const = -105.565;
    const double apu_n1_temp_x = 28.571;
    const double apu_n1_temp_x2 = 0.0884;
    const double apu_n1_temp_x3 = -0.0081;
    const double apu_n1_temp_x4 = 0.00005;

    double start_time;
    double stop_time;
    double shutdown_bleed_cooldown_timer;

    void openFlap(const double currentAbsTime) {
        if (lSimVarsValue[APU_FLAP_OPEN] <= 100) {
            lSimVarsValue[APU_FLAP_OPEN] += 100 * (deltaT * 0.001 / apu_flap_delay);	//pct time finished for opening flap
        }
    }
    
    void closeFlap(const double currentAbsTime) {
        if (lSimVarsValue[APU_FLAP_OPEN] >= 0) {
            lSimVarsValue[APU_FLAP_OPEN] -= 100 * (deltaT * 0.001 / apu_flap_delay);	//pct time finished for closing flap
        }
    }
    
    void startup(const double currentAbsTime) {
        trigger_key_event(KEY_APU_STARTER, 1);      //activate apu starter only after full flap opening
        if (start_time == -1) {
            start_time = currentAbsTime;
        }
        if (start_time != -1) {
            if (lSimVarsValue[APU_N1] < 100) {
                const double time_since_start = (currentAbsTime - start_time) * 0.001;
                lSimVarsValue[APU_N1] = (apu_N1_x5 * pow(time_since_start, 5)) + (apu_N1_x4 * pow(time_since_start, 4)) + (apu_N1_x3 * pow(time_since_start, 3)) + (apu_N1_x2 * pow(time_since_start, 2)) + (apu_N1_x * time_since_start) + apu_N1_const;
            }
        }
        updateEGT(true, currentAbsTime);
    }
    
    void shutdown(const double currentAbsTime) {
        if (shutdown_bleed_cooldown_timer <= 0) {
            start_time = -1;
            trigger_key_event(KEY_APU_OFF_SWITCH, 1);   //do we even need this anymore?
            if (lSimVarsValue[APU_N1] > 0) {
                lSimVarsValue[APU_N1] -= apu_spool_down_coef * deltaT * 0.001;
            }
            updateEGT(false, currentAbsTime);
        }
    }
    
    void updateEGT(bool startup, const double currentAbsTime) {
        const double N1 = lSimVarsValue[APU_N1];
        const double ambient = aSimVarsValue[AMB_TEMP];
        if (startup) {
            double apu_egt = (apu_n1_temp_x4 * pow(N1, 4)) + (apu_n1_temp_x3 * pow(N1, 3)) + (apu_n1_temp_x2 * pow(N1, 2)) + (apu_n1_temp_x * N1) + apu_n1_temp_const;
            if (apu_egt > 500) {
                apu_egt = 495 + rand() % 3;
            }
            lSimVarsValue[APU_EGT] = max(ambient, apu_egt);
            if (lSimVarsValue[APU_EGT] > 500) {
                lSimVarsValue[APU_EGT] -= 0.4 * deltaT * 0.001;
            }
        }
        else {
            if (shutdown_bleed_cooldown_timer > 0 && lSimVarsValue[APU_EGT] > 300) {
                lSimVarsValue[APU_EGT] -= apu_cooling_coef * deltaT * 0.001;
            }
            else if (lSimVarsValue[APU_EGT] >= ambient) {
                lSimVarsValue[APU_EGT] -= apu_cooling_coef * deltaT * 0.001;
            }
        }
    }
    
    void updateEGTWarn() {
        const double n = lSimVarsValue[APU_N1];
        if (n < 11) {
            lSimVarsValue[APU_EGT_WARN] = 1200;
        }
        else if (n <= 15) {
            lSimVarsValue[APU_EGT_WARN] = (-50 * n) + 1750;
        }
        else if (n <= 65) {
            lSimVarsValue[APU_EGT_WARN] = (-3 * n) + 1045;
        }
        else {
            lSimVarsValue[APU_EGT_WARN] = (-30 / 7 * n) + 1128.6;
        }//ōīṇḥūiūṇō => I somehow managed to type this while I was asleep on the keyboard, any explanation would be nice.
    }

public:
    
    void init() {
        for (int i = APU_FLAP_OPEN; i <= APU_EGT_WARN; i++) {
            lSimVarsValue[i] = 0;
        }
        start_time = -1;
        stop_time = -1;
        shutdown_bleed_cooldown_timer = 0;
    }
    
    void update(const double currentAbsTime) {
        if (aSimVarsValue[APU_MASTER]) {
            openFlap(currentAbsTime);
            if (lSimVarsValue[APU_FLAP_OPEN] >= 100 && lSimVarsValue[APU_START] && !(lSimVarsValue[FIRE_APU_PUSH])) {
                startup(currentAbsTime);
            } else {
                shutdown(currentAbsTime);
            }
        }
        else {
            if (lSimVarsValue[APU_BLEED_VALVE] == 0 && lSimVarsValue[APU_BLEED_TOGGLE_OFF]) {
                shutdown_bleed_cooldown_timer = 120;
            }
            if (shutdown_bleed_cooldown_timer >= 0) {
                shutdown_bleed_cooldown_timer -= deltaT * 0.001;
            }
            closeFlap(currentAbsTime);
            shutdown(currentAbsTime);
        }
        updateEGTWarn();
    }
};

class EngSys {
private:
    Engine eng1;
    Engine eng2;
    APUEngine apuEng;
public:
    
    void init() {
        eng1.init();
        eng2.init();
        apuEng.init();
    }
    
    void update(double const currentAbsTime) {
        eng1.update();
        eng2.update();
        apuEng.update(currentAbsTime);
    }
};

#endif // !ENGSYS
#pragma once
#include "../common_sys.h"


class PressSys {
private:
    bool takeoff;
    bool landed;
    double landing_timer;

    void incCabinAlt(double inc) {
    
    }
    void decCabinAlt(double dec) {

    }
    void setCabinAlt(double alt) {

    }
    void incCabinVS(double alt) {

    }
    void decCabinVS(double alt){
    
    }
    void setLDGELEV(double alt) {

    }
    void autoSetCabinAlt(double alt) {

    }
    void autoSetCabinVS(double alt) {

    }

public:
    void init() {
        for (int i = OUTFLOW_VALVE; i <= CPC_SYS2; i++) {
            lSimVarsValue[i] = 0;
        }
        takeoff = false;
        landed = false;
        lSimVarsValue[OUTFLOW_VALVE] = 100;
        lSimVarsValue[CPC_SYS1] = 1;
        lSimVarsValue[CPC_SYS2] = 1;
        lSimVarsValue[CABIN_ALTITUDE] = aSimVarsValue[ALTITUDE];
        lSimVarsValue[CABIN_ALTITUDE_GOAL] = aSimVarsValue[ALTITUDE];
    }
    void update() {
        
    }
};
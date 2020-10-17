#pragma once
#include "../common_sys.h"


class PressSys {
private:
    bool takeoff;
    bool landed;
    double landing_timer;
    const int max_normal_cabin_alt = 8000;   //feet
    const int max_operating_alt = 39800;     //feet
    const int cab_alt_warn = 9200;
    const int outflow_max_alt = 15000;
    const int ldg_max_cabin_vs = 750;
    const int norm_max_cabin_vs = 2000;
    int max_cabin_vs;
    const int inc_dec_rate = 50;       //(+-50fpm/s)
    const int ofv_inc_dec_rate = 2;    //%opening ^results in the above vs rate
    
    double cabin_pressure;
    double last_cabin_pressure;

    void pressureToCabVS() {
        lSimVarsValue[CABIN_ALTITUDE_RATE] = (altitude_AtPressure(cabin_pressure) - altitude_AtPressure(last_cabin_pressure)) * deltaT;
    }

    void deltaP() {
        lSimVarsValue[DELTA_PRESSURE] = pressure_AtAltitude(lSimVarsValue[CABIN_ALTITUDE]) - aSimVarsValue[AMB_PRESS];
    }

    void pressureChangeToOFV() {
        
    }

    void setCabinAlt(double alt) {
        lSimVarsValue[CABIN_ALTITUDE_GOAL] = alt;
    }
    void incCabinVS(const double currentAbsTime) {
        lSimVarsValue[CABIN_ALTITUDE_RATE] += inc_dec_rate * (currentAbsTime - lastAbsTime) * 0.001;
    }
    void decCabinVS(const double currentAbsTime){
        lSimVarsValue[CABIN_ALTITUDE_RATE] -= inc_dec_rate * (currentAbsTime - lastAbsTime) * 0.001;
    }
    void setLDGELEV(double alt) {
    //need to get feild elev from cdu f-pln
    }
    void setMaxVS() {
        if ((aSimVarsValue[GEAR_POS] == 2 || aSimVarsValue[ALTITUDE_ABV_GND] <= 5000) && aSimVarsValue[CURRENT_VSPEED] < 0) {
            max_cabin_vs = ldg_max_cabin_vs;
        } else {
            max_cabin_vs = norm_max_cabin_vs;
        }
    }
    void autoSetCabinAltTarget(double alt) {
        lSimVarsValue[CABIN_ALTITUDE_GOAL] = ((alt + aSimVarsValue[CURRENT_VSPEED]) * max_normal_cabin_alt/max_operating_alt);
    }
    void autoSetCabinVS(double alt) {
        lSimVarsValue[CABIN_ALTITUDE_RATE] = max_cabin_vs * (lSimVarsValue[CABIN_ALTITUDE_GOAL] - lSimVarsValue[CABIN_ALTITUDE]) / max_normal_cabin_alt;
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
    void update(const double currentAbsTime) {
        deltaT = (currentAbsTime - lastAbsTime) * 0.001;
    }
};
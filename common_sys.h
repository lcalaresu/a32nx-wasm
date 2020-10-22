#pragma once

#ifndef SYS_DEF
#define SYS_DEF

#include <MSFS/MSFS.h>
#include <MSFS/MSFS_Render.h>
#include <MSFS/Legacy/gauges.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <ratio>
#include <chrono>
#include <vector>

#include "utils/utils.h"

#include "data/data.h"

#define REFRESH_RATE 500        //update refresh rate in milliseconds
uint64_t lastAbsTime;         //last time the update function was run
uint64_t deltaT;

#define DEBUG


int flightPhase() {
    // 0 = ON_GROUND, 1 = TAKEOFF, 2 = CLIMB, 3 = CRUISE, 4 = DESCENT 5 = APPROACH/LANDING, 6 = JUSTLANDED
    //this can be further improved / moved to LGCIU and other computer modules later on
    if ((aSimVarsValue[ENG1_THROTTLE] >= 95 || aSimVarsValue[ENG2_THROTTLE] >= 95) && aSimVarsValue[ON_GROUND]) {
        return 1;
    } else if (aSimVarsValue[ON_GROUND]) {
        if (!lSimVarsValue[LANDED]) {
            lSimVarsValue[LANDED] = 1;
            return 6;
        }
        return 0;
    } else {
        lSimVarsValue[LANDED] = 0;
    }
    if ((aSimVarsValue[GEAR_POS] == 2 || aSimVarsValue[ALTITUDE_ABV_GND] <= 6000) && aSimVarsValue[CURRENT_VSPEED] < 0) {
        return 5;
    }
    if (aSimVarsValue[CURRENT_VSPEED] > 200) {
        return 2;
    }
    if(aSimVarsValue[CURRENT_VSPEED] < -200) {
        return 4;
    }
    if (aSimVarsValue[ALTITUDE_ABV_GND] > 0) {
        return 3;
    }
    return 0;
}

#endif

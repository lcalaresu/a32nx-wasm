#pragma once
#ifndef UTILS
#define UTILS

#include "PID.h"
#include "bit_oper.h"

#ifndef DEBUG
#define DEBUG
#endif // !DEBUG
/*
* ============== *
* UTIL FUNCTIONS *
* ============== *
*/
double convert_rankineToCelcius(const double rankine);
double convert_inHgToPSI(const double inHg);

double convert_rankineToCelcius(const double rankine) {
    return ((rankine - 491.67) * (0.5556));
}
double convert_inHgToPSI(const double inHg) {
    return (inHg * 0.491);
}

void debug_print(const char* message) {
    #ifdef DEBUG
    printf("%s\n", message);
    #endif
}
#endif // !UTILS
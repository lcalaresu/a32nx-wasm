#pragma once

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
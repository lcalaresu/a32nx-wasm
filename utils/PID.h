#pragma once
#ifndef PIDDEF
#define PIDDEF

/*
* === *
* PID *
* === *
*/
class PIDClass {
private:
    double _Kp;         //Proportional constant
    double _Ki;         //Integral constant
    double _Kd;         //Differential constant
    double Ep;          //current error
    double Ei;          //predicted error
    double Ed;          //rate of error change
    double lastEp;      //last proportional error
    double _max_pos_change;  //max +ve change the system is capable of outputting 
    double _max_neg_change;  //max -ve change the system is capable of outputting
    double lastUpdateTime;

    bool crossZero() {
        if ((lastEp <= 0 && Ep >= 0) || (lastEp >= 0 && Ep <= 0)) {
            return true;
        }
        return false;
    }
    void updateError(const double currentError, const double currentTime) {
        const double deltaT = currentTime - lastUpdateTime;
        Ep = currentError;
        if (crossZero()) {
            Ei = 0;                 //predicted(integral) error is reset to 0 when Ep(currentError) crosses zero.
        }
        Ei += (Ep * deltaT);
        if (deltaT != 0) {
            Ed = (Ep - lastEp) / deltaT;
        }
        lastEp = Ep;
        lastUpdateTime = currentTime;
    }
public:
    void init(const double Kp, const double Ki, const double Kd, const double currentAbsTime, const double max_pos_change, const double max_neg_change) {
        _Kp = Kp;
        _Ki = Ki;
        _Kd = Kd;
        Ep = 0;
        Ei = 0;
        Ed = 0;
        _max_pos_change = max_pos_change;
        _max_neg_change = max_neg_change;
        lastEp = 0;

        lastUpdateTime = currentAbsTime;
    }
    /*
    * add result of PID control to your variable
    */
    double control(const double currentError, const double currentAbsTime) {
        updateError(currentError, currentAbsTime);
        double adjust_var = (_Kp * Ep + _Ki * Ei + _Kd * Ed);
        if (adjust_var > _max_pos_change) {
            adjust_var = _max_pos_change;
        }
        if (adjust_var < _max_neg_change) {
            adjust_var = _max_neg_change;
        }
        return adjust_var;
    }
};

#endif // !PIDDEF
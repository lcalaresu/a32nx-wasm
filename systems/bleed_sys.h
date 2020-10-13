#pragma once
#ifndef BLEEDSYS
#define BLEEDSYS

#include "../common_sys.h"

class EngineBleed {
private:
public:
    void init() {
        for (int i = ENG1_BLEED_PRESSURE; i <= ENG2_BLEED_TEMPERATURE; i++) {
            lSimVarsValue[i] = 0;
        }
    }
    void update(const double currentAbsTime) {
        //TODO
    }
    void updateSimVars() {
        for (int i = ENG1_BLEED_PRESSURE; i <= ENG2_BLEED_TEMPERATURE; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};

class APUBleed {
private:
public:
    void init() {
        for (int i = APU_BLEED_PRESSURE; i <= APU_BLEED_TEMPERATURE; i++) {
            lSimVarsValue[i] = 0;
        }
    }
    void update(const double currentAbsTime) {
        //TODO
    }
    void updateSimVars() {
        for (int i = APU_BLEED_PRESSURE; i <= APU_BLEED_TEMPERATURE; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};

class GPUBleed {
private:
public:
    void init() {
        for (int i = GPU_BLEED_PRESSURE; i <= GPU_BLEED_TEMPERATURE; i++) {
            lSimVarsValue[i] = 0;
        }
    }
    void update(const double currentAbsTime) {
        //TODO
    }
    void updateSimVars() {
        for (int i = GPU_BLEED_PRESSURE; i <= GPU_BLEED_TEMPERATURE; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};

class Valves {
private:
    const double valveDelay = 2 + rand()%1-0.5;            //2-3s
    const double valveOpenRate = 1/valveDelay;
    double ip1_valve_open_pct = 0;
    double hp1_valve_open_pct = 0;
    double ip2_valve_open_pct = 0;
    double hp2_valve_open_pct = 0;
    double apu_valve_open_pct = 0;
    double gpu_valve_open_pct = 0;
    double eng1_bleed_valve_open_pct = 0;
    double eng2_bleed_valve_open_pct = 0;
    double anti_ice_valve_open_pct = 0;
    double xbleed_valve_open_pct = 0;
    const double apu_max_pressure = 40;
    const double apu_max_temperature = 200;
    const double gpu_max_pressure = 40;
    const double gpu_max_temperature = 200;
    const double eng_bleed_max_temperature = 260;
    const double eng_bleed_max_pressure = 40;

public:
    void init() {
        for (int i = ENG1_IP_VALVE; i <= X_BLEED_VALVE; i++) {
            lSimVarsValue[i] = 0;
        }
    }
    void update(const double currentAbsTime) {
        //ENG1
        if (aSimVarsValue[ENG1_STARTER] && aSimVarsValue[ENG1_N2] <= 20) {
            lSimVarsValue[ENG1_BLEED_STARTER_VALVE] = 1;
            lSimVarsValue[ENG1_IP_VALVE] = 0;
            lSimVarsValue[ENG1_HP_VALVE] = 0;
        } else if (aSimVarsValue[ENG1_N1] < 84) {
            lSimVarsValue[ENG1_BLEED_STARTER_VALVE] = 0;
            if (hp1_valve_open_pct < 1) {
                hp1_valve_open_pct += (currentAbsTime - lastAbsTime)*0.001*valveOpenRate;
            }
            if(hp1_valve_open_pct >= 1){
                lSimVarsValue[ENG1_HP_VALVE] = 1;
            }
            lSimVarsValue[ENG1_IP_VALVE] = 0;
        } else {
            lSimVarsValue[ENG1_BLEED_STARTER_VALVE] = 0;
            if (ip1_valve_open_pct < 1) {
                ip1_valve_open_pct += (currentAbsTime - lastAbsTime) * 0.001 * valveOpenRate;
            }
            if (ip1_valve_open_pct >= 1) {
                lSimVarsValue[ENG1_IP_VALVE] = 1;
            }
            lSimVarsValue[ENG1_HP_VALVE] = 0;
        }
        if (aSimVarsValue[ENG1_BLEED_SW] && lSimVarsValue[ENG1_BLEED_TEMPERATURE] <= eng_bleed_max_temperature && lSimVarsValue[ENG1_BLEED_PRESSURE] <= eng_bleed_max_pressure) {
            if (eng1_bleed_valve_open_pct < 1) {
                eng1_bleed_valve_open_pct += (currentAbsTime - lastAbsTime) * 0.001 * valveOpenRate;
            }
            if (eng1_bleed_valve_open_pct >= 1) {
                lSimVarsValue[ENG1_BLEED_VALVE] = 1;
            }
        } else {
            lSimVarsValue[ENG1_BLEED_VALVE] = 0;
        }


        //ENG2
        if (aSimVarsValue[ENG2_STARTER] && aSimVarsValue[ENG2_N2] <= 20) {
            lSimVarsValue[ENG2_BLEED_STARTER_VALVE] = 1;
            lSimVarsValue[ENG2_IP_VALVE] = 0;
            lSimVarsValue[ENG2_HP_VALVE] = 0;
        } else if (aSimVarsValue[ENG2_N1] < 84) {
            lSimVarsValue[ENG2_BLEED_STARTER_VALVE] = 0;
            if (hp2_valve_open_pct < 1) {
                hp2_valve_open_pct += (currentAbsTime - lastAbsTime) * 0.001 * valveOpenRate;
            }
            if (hp2_valve_open_pct >= 1) {
                lSimVarsValue[ENG2_HP_VALVE] = 1;
            }
            lSimVarsValue[ENG2_IP_VALVE] = 0;
        } else {
            lSimVarsValue[ENG2_BLEED_STARTER_VALVE] = 0;
            if (ip2_valve_open_pct < 1) {
                ip2_valve_open_pct += (currentAbsTime - lastAbsTime) * 0.001 * valveOpenRate;
            }
            if (ip2_valve_open_pct >= 1) {
                lSimVarsValue[ENG2_IP_VALVE] = 1;
            }
            lSimVarsValue[ENG2_HP_VALVE] = 0;
        }
        if (aSimVarsValue[ENG2_BLEED_SW] && lSimVarsValue[ENG2_BLEED_TEMPERATURE] <= eng_bleed_max_temperature && lSimVarsValue[ENG1_BLEED_PRESSURE] <= eng_bleed_max_pressure) {
            if (eng2_bleed_valve_open_pct < 1) {
                eng2_bleed_valve_open_pct += (currentAbsTime - lastAbsTime) * 0.001 * valveOpenRate;
            }
            if (eng2_bleed_valve_open_pct >= 1) {
                lSimVarsValue[ENG1_BLEED_VALVE] = 1;
            }
        } else {
            lSimVarsValue[ENG2_BLEED_VALVE] = 0;
        }
        //AUTO PITOT HEAT
        if (aSimVarsValue[ENG1_N2] >= 30 || aSimVarsValue[ENG2_N2] >= 30) {
            execute_calculator_code("1 (&gt;K:PITOT_HEAT_TOGGLE)", nullptr, nullptr, nullptr);
        } else {
            execute_calculator_code("0 (&gt;K:PITOT_HEAT_TOGGLE)", nullptr, nullptr, nullptr);
        }

        //APU
        if (aSimVarsValue[APU_BLEED_SW] && lSimVarsValue[APU_N1] >= 95 && lSimVarsValue[APU_BLEED_PRESSURE] <= apu_max_pressure && lSimVarsValue[APU_BLEED_TEMPERATURE] <= apu_max_temperature) {
            if (apu_valve_open_pct < 1) {
                apu_valve_open_pct += (currentAbsTime - lastAbsTime) * 0.001 * valveOpenRate;
            }
            if (apu_valve_open_pct >= 1) {
                lSimVarsValue[APU_BLEED_VALVE] = 1;
            }
        } else {
            lSimVarsValue[APU_BLEED_VALVE] = 0;
        }


        //GPU
        if (aSimVarsValue[EXT_POWER] && lSimVarsValue[GPU_BLEED_PRESSURE] <= gpu_max_pressure && lSimVarsValue[GPU_BLEED_TEMPERATURE] <= gpu_max_temperature) {
            if (gpu_valve_open_pct < 1) {
                gpu_valve_open_pct += (currentAbsTime - lastAbsTime) * 0.001 * valveOpenRate;
            }
            if (gpu_valve_open_pct >= 1) {
                lSimVarsValue[GPU_BLEED_VALVE] = 1;
            }
            
        } else {
            lSimVarsValue[GPU_BLEED_VALVE] = 0;
        }
        //ANTIICE
        if (aSimVarsValue[STRUCT_ANTI_ICE] && (lSimVarsValue[DUCT1] || lSimVarsValue[DUCT2])) {
            if (anti_ice_valve_open_pct < 1) {
                anti_ice_valve_open_pct += (currentAbsTime - lastAbsTime) * 0.001 * valveOpenRate;
            }
            if (anti_ice_valve_open_pct >= 1) {
                lSimVarsValue[WING_ANTIICE] = 1;
                execute_calculator_code("1 (&gt;K:TOGGLE_STRUCTURAL_DEICE)", nullptr, nullptr, nullptr);
            }
        } else {
            lSimVarsValue[WING_ANTIICE] = 0;
            execute_calculator_code("0 (&gt;K:TOGGLE_STRUCTURAL_DEICE)", nullptr, nullptr, nullptr);
        }
        //XBLEED AUTO OR ON
        if ((lSimVarsValue[X_BLEED] == 1 && lSimVarsValue[APU_BLEED_VALVE] >= 1) || lSimVarsValue[X_BLEED] == 2) {
            if (xbleed_valve_open_pct < 1) {
                xbleed_valve_open_pct += (currentAbsTime - lastAbsTime) * 0.001 * valveOpenRate;
            }
            if (xbleed_valve_open_pct >= 1) {
                lSimVarsValue[X_BLEED_VALVE] = 1;
            }
        } else {
            lSimVarsValue[X_BLEED_VALVE] = 0;
        }
    }
    void updateSimVars() {
        for (int i = ENG1_IP_VALVE; i <= X_BLEED_VALVE; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};

class Ducts {
private:
    double duct1_temperature = 0;
    double duct2_temperature = 0;
    double duct1_pressure = 0;
    double duct2_pressure = 0;

    PIDClass duct1_temp_PID;
    PIDClass duct2_temp_PID;
    PIDClass duct1_press_PID;
    PIDClass duct2_press_PID;
    
    ENUM updateDuct1() {
        if (lSimVarsValue[GPU_BLEED_VALVE]) {
            duct1_temperature = lSimVarsValue[GPU_BLEED_TEMPERATURE];
            duct1_pressure = lSimVarsValue[GPU_BLEED_PRESSURE];
            return GPU_BLEED;
        }
        if (lSimVarsValue[APU_BLEED_VALVE]) {
            duct1_temperature = lSimVarsValue[APU_BLEED_TEMPERATURE];
            duct1_pressure = lSimVarsValue[APU_BLEED_PRESSURE];
            return APU_BLEED;
        }
        if (lSimVarsValue[ENG1_BLEED_VALVE]) {
            duct1_temperature = lSimVarsValue[ENG1_BLEED_TEMPERATURE];
            duct1_pressure = lSimVarsValue[ENG1_BLEED_PRESSURE];
            return ENG1_BLEED;
        }
        if (lSimVarsValue[X_BLEED] && lSimVarsValue[ENG1_BLEED_VALVE]) {
            duct1_temperature = lSimVarsValue[ENG2_BLEED_TEMPERATURE];
            duct1_pressure = lSimVarsValue[ENG2_BLEED_PRESSURE];
            return ENG2_BLEED;
        }
        duct1_temperature = aSimVarsValue[AMB_TEMP];
        duct1_pressure = convert_inHgToPSI(aSimVarsValue[AMB_PRESS]);       //AMB_PRESS is in inHg, 0.5times gives PSI
        return 0;
    }
    ENUM updateDuct2() {
        if (lSimVarsValue[ENG1_BLEED_VALVE]) {
            duct2_temperature = lSimVarsValue[ENG2_BLEED_TEMPERATURE];
            duct2_pressure = lSimVarsValue[ENG2_BLEED_PRESSURE];
            return ENG2_BLEED;
        }
        if (lSimVarsValue[X_BLEED]) {
            if (lSimVarsValue[GPU_BLEED_VALVE]) {
                duct2_temperature = lSimVarsValue[GPU_BLEED_TEMPERATURE];
                duct2_pressure = lSimVarsValue[GPU_BLEED_PRESSURE];
                return GPU_BLEED;
            }
            if (lSimVarsValue[APU_BLEED_VALVE]) {
                duct2_temperature = lSimVarsValue[APU_BLEED_TEMPERATURE];
                duct2_pressure = lSimVarsValue[APU_BLEED_PRESSURE];
                return APU_BLEED;
            }
            if (lSimVarsValue[ENG1_BLEED_VALVE]) {
                duct2_temperature = lSimVarsValue[ENG1_BLEED_TEMPERATURE];
                duct2_pressure = lSimVarsValue[ENG1_BLEED_PRESSURE];
                return ENG1_BLEED;
            }
        }
        duct2_temperature = aSimVarsValue[AMB_TEMP];
        duct2_pressure = convert_inHgToPSI(aSimVarsValue[AMB_PRESS]);
        return 0;
    }
    void initPID() {
        duct1_temp_PID.init(0.4, 0.01, 0.6, lastAbsTime, 20, -10);
        duct2_temp_PID.init(0.4, 0.01, 0.6, lastAbsTime, 20, -10);
        duct1_press_PID.init(0.6, 0.02, 0.7, lastAbsTime, 30, -30);
        duct2_press_PID.init(0.6, 0.02, 0.7, lastAbsTime, 30, -30);
    }
    void updatePID(const double currentAbsTime) {
        const double duct1_temperature_error = duct1_temperature - lSimVarsValue[DUCT1_TEMPERATURE];
        const double duct2_temperature_error = duct2_temperature - lSimVarsValue[DUCT2_TEMPERATURE];
        const double duct1_pressure_error = duct1_pressure - lSimVarsValue[DUCT1_PRESSURE];
        const double duct2_pressure_error = duct2_pressure - lSimVarsValue[DUCT2_PRESSURE];
        
        lSimVarsValue[DUCT1_TEMPERATURE] += duct1_temp_PID.control(duct1_temperature_error, currentAbsTime);
        lSimVarsValue[DUCT2_TEMPERATURE] += duct2_temp_PID.control(duct2_temperature_error, currentAbsTime);
        lSimVarsValue[DUCT1_PRESSURE] += duct1_press_PID.control(duct1_pressure_error, currentAbsTime);
        lSimVarsValue[DUCT2_PRESSURE] += duct2_press_PID.control(duct2_pressure_error, currentAbsTime);
    }
public:
    void init() {
        for (int i = DUCT1; i <= DUCT2_PRESSURE; i++) {
            lSimVarsValue[i] = 0;
        }
        lSimVarsValue[DUCT1_TEMPERATURE] = aSimVarsValue[AMB_TEMP];
        lSimVarsValue[DUCT2_TEMPERATURE] = aSimVarsValue[AMB_TEMP];
        lSimVarsValue[DUCT1_PRESSURE] = convert_inHgToPSI(aSimVarsValue[AMB_PRESS]);
        lSimVarsValue[DUCT2_PRESSURE] = convert_inHgToPSI(aSimVarsValue[AMB_PRESS]);
        initPID();
    }
    void update(const double currentAbsTime) {
        lSimVarsValue[DUCT1] = updateDuct1();
        lSimVarsValue[DUCT2] = updateDuct2();
        updatePID(currentAbsTime);
        if (lSimVarsValue[DUCT1] || (lSimVarsValue[DUCT2] && lSimVarsValue[X_BLEED])) {
            execute_calculator_code("1 (&gt;K:APU_BLEED_AIR_SOURCE_TOGGLE)", nullptr, nullptr, nullptr);            //all bleed starter engines require APU_BLEED SOURCE internally
        }
        else {
            execute_calculator_code("0 (&gt;K:APU_BLEED_AIR_SOURCE_TOGGLE)", nullptr, nullptr, nullptr);            //all bleed starter engines require APU_BLEED SOURCE internally
        }
    }
    void updateSimVars() {
        for (int i = DUCT1; i <= DUCT2_PRESSURE; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};
 
class BleedSys {
private:
    EngineBleed engUnit;
    APUBleed apuUnit;
    GPUBleed gpuUnit;
    Valves valveUnit;
    Ducts ductUnit;
public:
    void init() {
        engUnit.init();
        apuUnit.init();
        gpuUnit.init();
        valveUnit.init();
        ductUnit.init();
    }
    void update(const double currentAbsTime) {
        engUnit.update(currentAbsTime);
        apuUnit.update(currentAbsTime);
        gpuUnit.update(currentAbsTime);
        valveUnit.update(currentAbsTime);
        ductUnit.update(currentAbsTime);

    }
    void updateSimVars() {
        engUnit.updateSimVars();
        apuUnit.updateSimVars();
        gpuUnit.updateSimVars();
        valveUnit.updateSimVars();
        ductUnit.updateSimVars();
    }
};

#endif // !BLEEDSYS
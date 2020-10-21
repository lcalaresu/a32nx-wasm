#pragma once
#ifndef BLEEDSYS
#define BLEEDSYS

#include "../common_sys.h"

class EngineBleed {
private:
    const double ip_n2_press_const = -12408.498070943;
    const double ip_n2_press_x = 403.075348871;
    const double ip_n2_press_x2 = -4.365898861;
    const double ip_n2_press_x3 = 0.015805319;
    const double hp_n2_press_const = -7344.145533374;
    const double hp_n2_press_x = 277.614276003;
    const double hp_n2_press_x2 = -3.488104815;
    const double hp_n2_press_x3 = 0.014659383;
    const double ip_n2_temp_const = -6934.071330009;
    const double ip_n2_temp_x = 354.046322806;
    const double ip_n2_temp_x2 = -6.481339053;
    const double ip_n2_temp_x3 = 0.050869800;
    const double hp_n2_temp_const = 228.758383738;
    const double hp_n2_temp_x = -12.257304394;
    const double hp_n2_temp_x2 = 0.392117580;
    const double hp_n2_temp_x3 = -0.004708863;

    int eng_ID;

    double engN2ToIPPressure(const double eng_N2) {
        if (eng_N2 < 87) {
            return aSimVarsValue[AMB_PRESS];
        }
        return (ip_n2_press_const + (ip_n2_press_x * eng_N2) + (ip_n2_press_x2 * pow(eng_N2, 2)) + (ip_n2_press_x3 * pow(eng_N2, 3)));
    }
    
    double engN2ToHPPressure(const double eng_N2) {
        if (eng_N2 <= 70) {
            const double hp_press = (50/70) * eng_N2;
            return max(aSimVarsValue[AMB_PRESS], hp_press);
        }
        return (hp_n2_press_const + (hp_n2_press_x * eng_N2) + (hp_n2_press_x2 * pow(eng_N2, 2)) + (hp_n2_press_x3 * pow(eng_N2, 3)));
    }
    
    double engN2ToIPTemp(const double eng_N2) {
        if (eng_N2 < 30) {
            const double ip_temp = eng_N2 * 2;
            return max(aSimVarsValue[AMB_TEMP], ip_temp);
        } 
        else if (eng_N2 < 70) {
            return eng_N2 + 30;
        }
        else if (eng_N2 < 80) {
            return 100;
        }
        return (ip_n2_temp_const + (ip_n2_temp_x * eng_N2) + (ip_n2_temp_x2 * pow(eng_N2, 2)) + (ip_n2_temp_x3 * pow(eng_N2, 3)));
    }

    double engN2ToHPTemp(const double eng_N2) {
        if (eng_N2 < 30) {
            const double bleed_temp = 80 / 30 * eng_N2;
            return max(aSimVarsValue[AMB_TEMP], bleed_temp);
        }
        return (hp_n2_temp_const + (hp_n2_temp_x * eng_N2) + (hp_n2_temp_x2 * pow(eng_N2, 2)) + (hp_n2_temp_x3 * pow(eng_N2, 3)));
    }

public:
    void init(const int ENG_ID) {
        //0 = ENG1, 1 = ENG2
        eng_ID = ENG_ID - 1;
        lSimVarsValue[ENG1_BLEED_TEMPERATURE + eng_ID] = aSimVarsValue[AMB_TEMP];
        lSimVarsValue[ENG1_BLEED_PRESSURE + eng_ID] = aSimVarsValue[AMB_PRESS];
    }
    void update(const double currentAbsTime) {
        const double eng_n2 = aSimVarsValue[ENG1_N2 + eng_ID];

        if (lSimVarsValue[ENG1_HP_VALVE + eng_ID]) {
            lSimVarsValue[ENG1_BLEED_PRESSURE + eng_ID] = engN2ToHPPressure(eng_n2);
            lSimVarsValue[ENG1_BLEED_TEMPERATURE + eng_ID] = engN2ToHPTemp(eng_n2);
        } else if (lSimVarsValue[ENG1_IP_VALVE + eng_ID]) {
            lSimVarsValue[ENG1_BLEED_PRESSURE + eng_ID] = engN2ToIPPressure(eng_n2);
            lSimVarsValue[ENG1_BLEED_TEMPERATURE + eng_ID] = engN2ToIPTemp(eng_n2);
        } else {
            lSimVarsValue[ENG1_BLEED_PRESSURE + eng_ID] = aSimVarsValue[AMB_PRESS];
            lSimVarsValue[ENG1_BLEED_TEMPERATURE + eng_ID] = aSimVarsValue[AMB_TEMP];
        }
    }
};

class APUBleed {
private:
    const double press_min = 34;
    const double temp_min = 165;
public:
    void init() {
        for (int i = APU_BLEED_PRESSURE; i <= APU_BLEED_TEMPERATURE; i++) {
            lSimVarsValue[i] = 0;
        }
    }
    void update(const double currentAbsTime) {
        if (lSimVarsValue[APU_N1] >= 95) {
            lSimVarsValue[APU_BLEED_PRESSURE] = press_min + rand() % 4;
            lSimVarsValue[APU_BLEED_TEMPERATURE] = temp_min + rand() %2;
        } else {
            lSimVarsValue[APU_BLEED_PRESSURE] = aSimVarsValue[AMB_PRESS];
            lSimVarsValue[APU_BLEED_TEMPERATURE] = aSimVarsValue[AMB_TEMP];
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
        if (aSimVarsValue[EXT_POWER]) {
            lSimVarsValue[GPU_BLEED_PRESSURE] = 33 + rand() % 4;
            lSimVarsValue[GPU_BLEED_TEMPERATURE] = 150;
        } else {
            lSimVarsValue[GPU_BLEED_PRESSURE] = aSimVarsValue[AMB_PRESS];
            lSimVarsValue[GPU_BLEED_TEMPERATURE] = aSimVarsValue[AMB_TEMP];
        }
    }
};

class Valves {
private:
    const double valveDelay = 2 + rand()%1-0.5;            //2-3s
    const double valveOpenRate = 1/valveDelay;
    const double apu_max_pressure = 40;
    const double apu_max_temperature = 200;
    const double gpu_max_pressure = 40;
    const double gpu_max_temperature = 200;
    const double eng_bleed_max_pressure = 63;
    const double preCooler_max_cooling = 250;
    const double temperature_set_point = 200;
    const double pressure_set_point = 40 + rand() % 2;
    const double max_regulated_pressure = 180;

    double required_pressure = 40 + rand()%2;         //PSI
    double temp_safety_timer[2] = {0, 0};
    double ip_valve_open_pct[2] = {0, 0};
    double hp_valve_open_pct[2] = {0, 0};
    double eng_prv_open_pct[2] = {0, 0};
    double fan_air_valve_pct[2] = {0,0};
    double apu_valve_open_pct = 0;
    double gpu_valve_open_pct = 0;
    double xbleed_valve_open_pct = 0;
    double anti_ice_valve_open_pct = 0;

    bool apu_bleed_toggle_off = false;

    PIDClass *fan_valve[2];
 
    void updateEngValve(const int engine_ID, const double currentAbsTime) {
        const int eng_ID = engine_ID - 1;
        if (aSimVarsValue[ENG1_STARTER + eng_ID] && aSimVarsValue[ENG1_N2 + eng_ID] <= 20) {
            lSimVarsValue[ENG1_BLEED_STARTER_VALVE + eng_ID] = 1;
            lSimVarsValue[ENG1_IP_VALVE + eng_ID] = 0;
            lSimVarsValue[ENG1_HP_VALVE + eng_ID] = 0;
        }
        else if (aSimVarsValue[ENG1_N1 + eng_ID] < 84) {
            lSimVarsValue[ENG1_BLEED_STARTER_VALVE + eng_ID] = 0;
            if (hp_valve_open_pct[eng_ID] < 1) {
                hp_valve_open_pct[eng_ID] += deltaT * 0.001 * valveOpenRate;
            }
            if (hp_valve_open_pct[eng_ID] >= 1) {
                lSimVarsValue[ENG1_HP_VALVE + eng_ID] = 1;
            }
            lSimVarsValue[ENG1_IP_VALVE + eng_ID] = 0;
        }
        else {
            lSimVarsValue[ENG1_BLEED_STARTER_VALVE + eng_ID] = 0;
            if (ip_valve_open_pct[eng_ID] < 1) {
                ip_valve_open_pct[eng_ID] += deltaT * 0.001 * valveOpenRate;
            }
            if (ip_valve_open_pct[eng_ID] >= 1) {
                lSimVarsValue[ENG1_IP_VALVE + eng_ID] = 1;
            }
            lSimVarsValue[ENG1_HP_VALVE + eng_ID] = 0;
        }
    }

    void pressureRegulatorValve(const int eng_ID) {
        if (lSimVarsValue[DUCT1] == NOBLEED || lSimVarsValue[DUCT2] == NOBLEED) {
            required_pressure = 50 + rand() % 2;         //PSI
        } else {
            required_pressure = 40 + rand() % 2;         //PSI
        }
        const double pressRegulate = ((max_regulated_pressure - lSimVarsValue[ENG1_BLEED_PRESSURE + eng_ID] + required_pressure) * 100 / max_regulated_pressure);

        if (eng_prv_open_pct[eng_ID] <= pressRegulate && eng_prv_open_pct[eng_ID] < 100) {
            eng_prv_open_pct[eng_ID] += deltaT * 0.001 * valveOpenRate;
        }
        if (eng_prv_open_pct[eng_ID] > pressRegulate && eng_prv_open_pct[eng_ID] > 0) {
            eng_prv_open_pct[eng_ID] -= deltaT * 0.001 * valveOpenRate;
        }
        lSimVarsValue[ENG1_BLEED_PRESSURE + eng_ID] = eng_prv_open_pct[eng_ID] * lSimVarsValue[ENG1_BLEED_PRESSURE + eng_ID];
    }

    void preCooler(const int fan_ID, const double currentAbsTime) {
        double cooling_coef;
        const double temp_error = temperature_set_point - lSimVarsValue[ENG1_BLEED_TEMPERATURE];

        fan_air_valve_pct[fan_ID] += fan_valve[fan_ID]->control(temp_error, currentAbsTime);
        if (fan_air_valve_pct[fan_ID] < 0) {
            fan_air_valve_pct[fan_ID] = 0;
        }
        if (fan_air_valve_pct[fan_ID] > 100) {
            fan_air_valve_pct[fan_ID] = 100;
        }
        //cooling coefficient change based on amb_temp, linear scale from +2 at -60 to 0.35 at +60
        cooling_coef = (-0.01375 * lSimVarsValue[TAT_TEMP] + 1.175);
        lSimVarsValue[ENG1_BLEED_TEMPERATURE] -= cooling_coef * fan_air_valve_pct[fan_ID] * 0.01 * preCooler_max_cooling;
    }

    void engBleedMonitor(const int engine_ID, const double currentAbsTime) {
        const int eng_ID = engine_ID - 1;
        
        bool over_temp = false;
        bool over_press = false;

        pressureRegulatorValve(eng_ID);
        preCooler(eng_ID, currentAbsTime);
        //temp
        if (lSimVarsValue[ENG1_BLEED_TEMPERATURE + eng_ID] > 257 && lSimVarsValue[ENG1_BLEED_TEMPERATURE + eng_ID] < 270) {
            if (temp_safety_timer[eng_ID] == -1) {
                temp_safety_timer[eng_ID] = 55;
            } 
            if (temp_safety_timer[eng_ID] > 0) {
                temp_safety_timer[eng_ID] -= deltaT * 0.001;
            } else {
                over_temp = true;
            }
        } else if (lSimVarsValue[ENG1_BLEED_TEMPERATURE + eng_ID] >= 270 && lSimVarsValue[ENG1_BLEED_TEMPERATURE + eng_ID] < 290) {
            if (temp_safety_timer[eng_ID] == -1) {
                temp_safety_timer[eng_ID] = 15;
            } 
            if (temp_safety_timer[eng_ID] > 0) {
                temp_safety_timer[eng_ID] -= deltaT * 0.001;
            } else {
                over_temp = true;
            }
        } else if (lSimVarsValue[ENG1_BLEED_TEMPERATURE + eng_ID] >= 270 && lSimVarsValue[ENG1_BLEED_TEMPERATURE + eng_ID] < 290) {
            if (temp_safety_timer[eng_ID] == -1) {
                temp_safety_timer[eng_ID] = 15;
            } 
            if (temp_safety_timer[eng_ID] > 0) {
                temp_safety_timer[eng_ID] -= deltaT * 0.001;
            } else {
                over_temp = true;
            }
        } else {
            temp_safety_timer[eng_ID] = -1;
            over_temp = false;
        }
        //press
        if (lSimVarsValue[ENG1_BLEED_PRESSURE + eng_ID] >= eng_bleed_max_pressure) {
            over_press = true;
        } else {
             over_press = false;
        }
        //BMC_PRV_SAFETY_ISOLATION
        if (aSimVarsValue[ENG1_BLEED_SW + eng_ID] && !(over_press || over_temp || (lSimVarsValue[DUCT1 + eng_ID] == APU_BLEED) || lSimVarsValue[ENG1_FIRE_PUSH + eng_ID])) {
            lSimVarsValue[ENG1_BLEED_VALVE + eng_ID] = 1;
            if (eng_ID == 1 && lSimVarsValue[DUCT1] == APU_BLEED) {
                lSimVarsValue[ENG1_BLEED_VALVE + eng_ID] = 0;
            }
        } else {
            lSimVarsValue[ENG1_BLEED_VALVE + eng_ID] = 0;
        }
    }
    void bleedMonitoringComputer(const double currentAbsTime) {
        //ENG
        engBleedMonitor(1, currentAbsTime);
        engBleedMonitor(2, currentAbsTime);
        //APU
        if (aSimVarsValue[APU_BLEED_SW] && lSimVarsValue[APU_BLEED_PRESSURE] <= apu_max_pressure && lSimVarsValue[APU_BLEED_TEMPERATURE] <= apu_max_temperature && !(lSimVarsValue[FIRE_TEST_APU])) {
            lSimVarsValue[APU_BLEED_TOGGLE_OFF] = 0;
            if (apu_valve_open_pct < 100) {
                apu_valve_open_pct += deltaT * 0.001 * valveOpenRate;
            }
            if (apu_valve_open_pct >= 50) {
                lSimVarsValue[APU_BLEED_VALVE] = 1;
            }
        } else {
            if (apu_valve_open_pct > 0) {
                apu_valve_open_pct -= deltaT * 0.001 * valveOpenRate;
                lSimVarsValue[APU_BLEED_VALVE] = 0.5;
            }
            if (apu_valve_open_pct <= 0) {
                lSimVarsValue[APU_BLEED_VALVE] = 0;
            }
        }
        if (aSimVarsValue[APU_BLEED_SW] == 0 && lSimVarsValue[APU_BLEED_TOGGLE_OFF] == 0) {
            lSimVarsValue[APU_BLEED_TOGGLE_OFF] = 1;
        }
        //GPU
        if (aSimVarsValue[EXT_POWER] && lSimVarsValue[GPU_BLEED_PRESSURE] <= gpu_max_pressure && lSimVarsValue[GPU_BLEED_TEMPERATURE] <= gpu_max_temperature) {
            if (gpu_valve_open_pct < 100) {
                gpu_valve_open_pct += deltaT * 0.001 * valveOpenRate;
            }
            if (gpu_valve_open_pct >= 50) {
                lSimVarsValue[GPU_BLEED_VALVE] = 1;
            }
        } else {
            if (gpu_valve_open_pct > 0) {
                gpu_valve_open_pct -= deltaT * 0.001 * valveOpenRate;
                lSimVarsValue[GPU_BLEED_VALVE] = 0.5;
            }
            lSimVarsValue[GPU_BLEED_VALVE] = 0;
        }
    }
public:
    void init() {
        for (int i = ENG1_IP_VALVE; i <= X_BLEED_VALVE; i++) {
            lSimVarsValue[i] = 0;
        }
        fan_valve[0] = (PIDClass *)malloc(sizeof(PIDClass));
        fan_valve[1] = (PIDClass *)malloc(sizeof(PIDClass));
        fan_valve[0]->init(0.05, 0.01, 0.1, lastAbsTime, 15, -15);
        fan_valve[1]->init(0.05, 0.01, 0.1, lastAbsTime, 15, -15);
    }
    void update(const double currentAbsTime) {
        //ENG
        updateEngValve(1, currentAbsTime);
        updateEngValve(2, currentAbsTime);

        //BMC
        bleedMonitoringComputer(currentAbsTime);

        //AUTO PITOT HEAT
        if (aSimVarsValue[ENG1_N2] >= 30 || aSimVarsValue[ENG2_N2] >= 30) {
            execute_calculator_code("1 (&gt;K:PITOT_HEAT_TOGGLE)", nullptr, nullptr, nullptr);
        }
        else {
            execute_calculator_code("0 (&gt;K:PITOT_HEAT_TOGGLE)", nullptr, nullptr, nullptr);
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
        
        lSimVarsValue[DUCT1_TEMPERATURE] += duct1_temp_PID.control(duct1_temperature_error, currentAbsTime) * deltaT * 0.001;
        lSimVarsValue[DUCT2_TEMPERATURE] += duct2_temp_PID.control(duct2_temperature_error, currentAbsTime) * deltaT * 0.001;
        lSimVarsValue[DUCT1_PRESSURE] += duct1_press_PID.control(duct1_pressure_error, currentAbsTime) * deltaT * 0.001;
        lSimVarsValue[DUCT2_PRESSURE] += duct2_press_PID.control(duct2_pressure_error, currentAbsTime) * deltaT * 0.001;
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
};
 
class BleedSys {
private:
    EngineBleed eng1Unit;
    EngineBleed eng2Unit;
    APUBleed apuUnit;
    GPUBleed gpuUnit;
    Valves valveUnit;
    Ducts ductUnit;
public:
    void init() {
        eng1Unit.init(1);
        eng2Unit.init(2);
        apuUnit.init();
        gpuUnit.init();
        valveUnit.init();
        ductUnit.init();
    }
    void update(const double currentAbsTime) {
        eng1Unit.update(currentAbsTime);
        eng2Unit.update(currentAbsTime);
        apuUnit.update(currentAbsTime);
        gpuUnit.update(currentAbsTime);
        valveUnit.update(currentAbsTime);
        ductUnit.update(currentAbsTime);
    }
};

#endif // !BLEEDSYS
#pragma once
#ifndef PRESSSYS
#define PRESSSYS


#include "../common_sys.h"

class PressSys {
private:
    const int max_normal_cabin_alt = 8000;          //feet
    const int max_operating_alt = 39800;            //feet
    const int cab_alt_warn = 9200;
    const int outflow_max_alt = 15000;
    const int ldg_max_cabin_vs = 750;
    const int norm_max_cabin_vs = 2000;
    const double ofv_inc_rate = 3.33;               //3.33%/s
    const double ofv_dec_rate = -3.33;
    const double atm_air_moles_per_mass = 34.518;   //moles/kg
    const double toga_ldg_delta = 0.1;              //+0.1 deltaP
    const double fuselage_volume = 192;             //CKPT = 9, CABIN = 139, CARGO = 15.5, 28 cubic meters
    const double landing_depres_time = 55;          //55s of depressurisation post landing
    const double manpress_sys_switch_timer = 10;    //10s of man press on and then back to auto switches the active CPC system

    double landing_timer;
    double man_auto_timer;
    double cabin_vs_target;
    double packin_flow;
    double packout_flow;
    double out_flow;
    double max_cabin_vs;
    int active_CPC;                                 //0 = SYS1, 1 = SYS2
    double cabin_air_mass;
    double cabin_pressure;
    double ventilation_pulse_timer;

    PIDClass outflow_controller;

    void calculatePackFlow() {
        if(lSimVarsValue[DUCT1] && lSimVarsValue[DUCT2]){
            if (lSimVarsValue[PACK1_VALVE] && lSimVarsValue[PACK2_VALVE]) {
                const int pack_controller = lSimVarsValue[PACK_FLOW_CONTROLLER];
                switch (pack_controller)
                {
                    case 0: {
                        //CKPT  0.069+0.065     kg/s
                        //CABIN 0.582+0.548
                        //CARGO 0.088+0.132
                        const double fresh_flow = 0.761;
                        const double recirc_flow = 0.723;
                        packin_flow = fresh_flow;
                        if (lSimVarsValue[CAB_FANS]) {
                            packout_flow = recirc_flow;
                            packin_flow += recirc_flow;
                        } else {
                            packout_flow = 0;
                        }
                        break;
                    }
                    case 1: {
                        //CKPT  0.086+0.062     kg/s
                        //CABIN 0.722+0.522
                        //CARGO 0.088+0.132
                        const double fresh_flow = 0.918;
                        const double recirc_flow = 0.694;
                        packin_flow = fresh_flow;
                        if (lSimVarsValue[CAB_FANS]) {
                            packout_flow = recirc_flow;
                            packin_flow += recirc_flow;
                        } else {
                            packout_flow = 0;
                        }
                        break;
                    }
                    case 2: {
                        //CKPT  0.103+0.059     kg/s
                        //CABIN 0.872+1.501
                        //CARGO 0.088+0.132
                        const double fresh_flow = 1.085;
                        const double recirc_flow = 1.67;
                        packin_flow = fresh_flow;
                        if (lSimVarsValue[CAB_FANS]) {
                            packout_flow = recirc_flow;
                            packin_flow += recirc_flow;
                        } else {
                            packout_flow = 0;
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        } else if(lSimVarsValue[DUCT1] || lSimVarsValue[DUCT2]){
            if(lSimVarsValue[PACK1_VALVE] || lSimVarsValue[PACK2_VALVE]){
                //CKPT  0.050+0.067     kg/s
                //CABIN 0.437+0.577
                //CARGO 0.088+0.132
                const double fresh_flow = 0.597;
                const double recirc_flow = 0.754;
                packin_flow = fresh_flow;
                if (lSimVarsValue[CAB_FANS]) {
                    packout_flow = recirc_flow;
                } else {
                    packout_flow = 0;
                }
            }
        }
    }

    double calculateOutFlow() {
        const double ditching = !lSimVarsValue[DITCH];
        const double max_packflow = 2.755;
        const double max_outFlow = lSimVarsValue[DELTA_PRESSURE] * max_packflow;
        const double max_safety_outflow = lSimVarsValue[DELTA_PRESSURE] * max_packflow;
        const double max_leak_outflow = lSimVarsValue[DELTA_PRESSURE] * max_packflow;
        const double cabin_door = aSimVarsValue[CABIN_DOOR] * 0.1 * max_leak_outflow;
        const double cater_door = aSimVarsValue[CATER_DOOR] * 0.1 * max_leak_outflow;
        const double fwd_cargo = aSimVarsValue[FWD_CARGO] * 0.1 * max_leak_outflow;
        const double leak_outflow = cabin_door + cater_door + fwd_cargo;
        double safety_outflow = 0;

        if (lSimVarsValue[SAFETY_1] || lSimVarsValue[SAFETY_2]) {
            safety_outflow = max_safety_outflow;
        }
        else {
            safety_outflow = 0;
        }
        return (lSimVarsValue[OUTFLOW_VALVE] * 0.01 * ditching * max_outFlow) + safety_outflow + leak_outflow;
    }

    void safetyCheck() {
        if (lSimVarsValue[DELTA_PRESSURE] >= 8.6 || lSimVarsValue[DELTA_PRESSURE] <= -0.5) {
            lSimVarsValue[SAFETY_1] = 1;
            lSimVarsValue[SAFETY_2] = 1;
        } else {
            lSimVarsValue[SAFETY_1] = 0;
            lSimVarsValue[SAFETY_2] = 0;
        }
    }

    void setLDGELEV() {
        if (lSimVarsValue[MAN_LDG_ELEV_PCT] != 0) {
            lSimVarsValue[LDG_ELEV] = lSimVarsValue[MAN_LAND_ELEV];
        } else {
            lSimVarsValue[LDG_ELEV] = aSimVarsValue[DEST_ALT];
        }
    }

    void incCabinVS() {
        //to increase cabin VS the outflow valve closes
        lSimVarsValue[OUTFLOW_VALVE] += ofv_inc_rate * deltaT * 0.001;
    }

    void decCabinVS(){
        //to decrease cabin VS the outflow valve opens
        lSimVarsValue[OUTFLOW_VALVE] -= ofv_dec_rate * deltaT * 0.001;
    }

    void setMaxVS() {
        if ((aSimVarsValue[GEAR_POS] == 2 || aSimVarsValue[ALTITUDE_ABV_GND] <= 6000) && aSimVarsValue[CURRENT_VSPEED] < 0) {
            max_cabin_vs = ldg_max_cabin_vs;
        } else {
            max_cabin_vs = norm_max_cabin_vs;
        }
    }

    void autoSetCabinAltTarget(int flightPhase) {
        switch (flightPhase)
        {
            case 0: {
                lSimVarsValue[CABIN_ALTITUDE_GOAL] = aSimVarsValue[ALTITUDE];
                break;
            }
            case 1: {
                const double takeoff_pressure = pressure_AtAltitude(aSimVarsValue[ALTITUDE]) + toga_ldg_delta;
                const double takeoff_cab_alt = altitude_AtPressure(takeoff_pressure);
                lSimVarsValue[CABIN_ALTITUDE_GOAL] = takeoff_cab_alt;
                break;
            }
            case 2:
            case 3:
            case 4: {
                lSimVarsValue[CABIN_ALTITUDE_GOAL] = ((aSimVarsValue[ALTITUDE] + aSimVarsValue[CURRENT_VSPEED]) * max_normal_cabin_alt / max_operating_alt);
                break;
            }
            case 5: {
                const double ldg_pressure = pressure_AtAltitude(lSimVarsValue[LDG_ELEV]) + toga_ldg_delta;
                const double landing_cab_alt = altitude_AtPressure(ldg_pressure);
                lSimVarsValue[CABIN_ALTITUDE_GOAL] = landing_cab_alt;
                break;
            }
            default:
                break;
        }
    }

    void autoSetCabinVS() {
        setMaxVS();
        cabin_vs_target = max_cabin_vs * (lSimVarsValue[CABIN_ALTITUDE_GOAL] - lSimVarsValue[CABIN_ALTITUDE]) / max_normal_cabin_alt;
    }

    double cabVSToDeltaP(const double cabVS) {
        return pressure_AtAltitude((lSimVarsValue[CABIN_ALTITUDE] + cabVS)) - aSimVarsValue[AMB_PRESS];
    }

    double calculatedeltaP(const double cab_altitude) {
        return pressure_AtAltitude(cab_altitude) - aSimVarsValue[AMB_PRESS];
    }

    double outflow_control_Error() {
        //safety check
        if (lSimVarsValue[DELTA_PRESSURE] >= 8) {
            return (7.9 - lSimVarsValue[DELTA_PRESSURE]);
        }
        if (lSimVarsValue[DELTA_PRESSURE] <= -0.5) {
            return (-0.4 - lSimVarsValue[DELTA_PRESSURE]);
        }

        //inflight with non 0 V/S
        if (aSimVarsValue[CURRENT_VSPEED] > 50 || aSimVarsValue[CURRENT_VSPEED] < 50) {
            return cabVSToDeltaP(cabin_vs_target) - cabVSToDeltaP(lSimVarsValue[CABIN_ALTITUDE_RATE]);
        }

        //post landing depress
        if (lSimVarsValue[LANDED]) {
            landing_timer = landing_depres_time;
        }
        if (landing_timer > 0) {
            landing_timer -= deltaT * 0.001;
            return lSimVarsValue[DELTA_PRESSURE];
        }
        return calculatedeltaP(lSimVarsValue[CABIN_ALTITUDE_GOAL]) - lSimVarsValue[DELTA_PRESSURE];
    }

    void openOutFlow() {
        if (lSimVarsValue[OUTFLOW_VALVE] < 100) {
            lSimVarsValue[OUTFLOW_VALVE] += ofv_dec_rate * deltaT * 0.001;
        }
    }

    void closeOutFlow() {
        if (lSimVarsValue[OUTFLOW_VALVE] > 0) {
            lSimVarsValue[OUTFLOW_VALVE] -= ofv_dec_rate * deltaT * 0.001;
        }
    }

    void updateCabinPressure() {
        calculatePackFlow();
        calculateOutFlow();
        cabin_air_mass += (packin_flow - packout_flow - out_flow) * deltaT * 0.001;
        const double cabin_air_moles = cabin_air_mass * atm_air_moles_per_mass;
        const double cabin_temp = lSimVarsValue[FWD_TEMP] + 273.15;
        cabin_pressure = idealGasPressure(fuselage_volume, cabin_temp, cabin_air_moles);
    }

    void updateAvionicsVentilation() {
        const int flight_phase = flightPhase();
        if (flight_phase == 0) {
            if (aSimVarsValue[AMB_TEMP] < 10) {
                lSimVarsValue[VENT_INLET] = 0;
                lSimVarsValue[VENT_OUTLET] = 0;
            } else {
                if (ventilation_pulse_timer > 5) {
                    ventilation_pulse_timer = 0;
                    lSimVarsValue[VENT_INLET] = (int)(0 + (rand() % 20) / 10);
                    lSimVarsValue[VENT_OUTLET] = (int)(0 + (rand() % 20) / 10);
                }
            }
        } else {
            if (aSimVarsValue[TAT_TEMP] > 34) {
                lSimVarsValue[VENT_INLET] = 0;
                lSimVarsValue[VENT_OUTLET] = 0.5;
            } else {
                lSimVarsValue[VENT_INLET] = 0;
                lSimVarsValue[VENT_OUTLET] = 0;
            }
        }
        if (lSimVarsValue[VENT_BLOWER] && lSimVarsValue[VENT_EXTRACT]) {
            lSimVarsValue[VENT_INLET] = 0;
            lSimVarsValue[VENT_OUTLET] = 0.5;
        }
        else if (lSimVarsValue[VENT_BLOWER] && lSimVarsValue[VENT_EXTRACT]) {
            lSimVarsValue[VENT_INLET] = 0;
            lSimVarsValue[VENT_OUTLET] = 0;
        }
    }
public:
    void init() {
        for (int i = OUTFLOW_VALVE; i <= CPC_SYS2; i++) {
            lSimVarsValue[i] = 0;
        }
        ventilation_pulse_timer = 0;
        lSimVarsValue[OUTFLOW_VALVE] = 100;
        lSimVarsValue[CPC_SYS1] = 0;
        lSimVarsValue[CPC_SYS2] = 0;
        lSimVarsValue[CABIN_ALTITUDE] = aSimVarsValue[ALTITUDE];
        lSimVarsValue[CABIN_ALTITUDE_GOAL] = aSimVarsValue[ALTITUDE];
        cabin_pressure = pressure_AtAltitude(lSimVarsValue[CABIN_ALTITUDE]);
        cabin_air_mass = atm_air_moles_per_mass * idealGasMoles(cabin_pressure, fuselage_volume, lSimVarsValue[FWD_TEMP]+273.15);
        outflow_controller.init(0.01, 0.001, 0.05, lastAbsTime, ofv_inc_rate, ofv_dec_rate);
        man_auto_timer = manpress_sys_switch_timer;
    }

    void update(const double currentAbsTime) {

        setLDGELEV();
        //switching of the CPC_SYS
        if (lSimVarsValue[MAN_CAB_PRESS]) {
            if (man_auto_timer > 10) {
                man_auto_timer -= deltaT * 0.001;
            }
            lSimVarsValue[CPC_SYS1] = 0;
            lSimVarsValue[CPC_SYS2] = 0;
            if (lSimVarsValue[MAN_VS_CTRL] == 0) {
                incCabinVS();
            }
            else if (lSimVarsValue[MAN_VS_CTRL] == 2) {
                decCabinVS();
            }
        } else if(!(lSimVarsValue[MAN_CAB_PRESS])){
            if (man_auto_timer < 0) {
                man_auto_timer = manpress_sys_switch_timer;
                active_CPC ^= 1;
            }
            switch (active_CPC)
            {
                case 0:
                    lSimVarsValue[CPC_SYS1] = 1;
                    break;
                case 1:
                    lSimVarsValue[CPC_SYS2] = 1;
                    break;
                default:
                    break;
            }
            if (lSimVarsValue[MAN_LDG_ELEV_PCT] == 0) {
                const int flight_phase = lSimVarsValue[FLIGHT_PHASE];
                autoSetCabinAltTarget(flight_phase);
            }
            autoSetCabinVS();
        }

        lSimVarsValue[DELTA_PRESSURE] = calculatedeltaP(lSimVarsValue[CABIN_ALTITUDE]);
        
        safetyCheck();
        
        if (lSimVarsValue[CPC_SYS1] || lSimVarsValue[CPC_SYS2]) {
            const double error = outflow_control_Error();
            const double outflow_control = lSimVarsValue[OUTFLOW_VALVE] + outflow_controller.control(error, currentAbsTime);
            if (outflow_control >= 0 && outflow_control <= 100) {
                lSimVarsValue[OUTFLOW_VALVE] = outflow_control;
            }
        }
        if (lSimVarsValue[CABIN_ALTITUDE] >= outflow_max_alt) {
            closeOutFlow();
        }
        
        updateCabinPressure();
        updateAvionicsVentilation();
        
        lSimVarsValue[CABIN_ALTITUDE] = altitude_AtPressure(cabin_pressure);
    }
};

#endif // !PRESSSYS

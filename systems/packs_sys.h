#pragma once
#include "../common_sys.h"

const double CockpitTotalAirFlow[5]{ /* LO */ 0.134, /* NORM */ 0.148, /* HI */ 0.162, /* 1 PACK */ 0.117, /* NO PACK */ 0.07 };
const double CabinTotalAirFlow[5]{ /* LO */ 1.13, /* NORM */ 1.249, /* HI */ 1.373, /* 1 PACK */ 1.014, /* NO PACK */ 0.59 };

// recirculation flow rate depending on pack status
const double RecircFlowValue[5]{ /* LO */ 0.485, /* NORM */ 0.4184, /* HI */ 0.3645, /* 1 PACK */ 0.5708, /* NO PACK */ 1. };

// flow per pack depending on pack status (calculated from flow rates)
const double FlowPerPack[5]{ /* LO */ 0.326, /* NORM */ 0.407, /* HI */ 0.488, /* 1 PACK */ 0.487, /* NO PACK */ 0. };

/* Packs 1 and 2 */
class Pack {
private:
    const double COEF_TAT = 0.5;
    const double COEF_BLEED = 0.85;
    const double COEF_TURB = 1.7;
    const double COEF_BYPASS = 1.4;
    const double BYPASS_RATIO = 0.8;
    const double CELSIUS_KELVIN = 273.15;
    const double MAX_FLOW = 0.49;       // max air flow (HI position for FCV)
    const double EXP_BLEED_PRESS = 35.;
    const double VALVE_OPEN_TIME = 2000 + rand() % 1000;    // 2000-3000ms
    const double FLOW_CHANGE_RATE = 0.1 / VALVE_OPEN_TIME;
    const double BYPASS_CHANGE_RATE = 0.5 / VALVE_OPEN_TIME;
    const double INLET_PRESS_CHANGE_RATE = 100. / VALVE_OPEN_TIME;
    const double INLET_TEMP_CHANGE_RATE = 500. / VALVE_OPEN_TIME;
    const double COMPR_TEMP_CHANGE_RATE = 450. / VALVE_OPEN_TIME;

    int pack_idx = 0;           // index of the pack (0 or 1)

    double inlet_press = 0.;    // pack inlet pressure (bleed press)
    double inlet_temp = 0.;     // pack inlet temperature (bleed temp)
    bool fcv_toggle = false;    // FCV (Flow Control Valve) position
    double fcv_flow = 0.;       // FCV flow
    double fcv_flow_rate = 0.;  // FCV flow rate (% : 0.=LO,  1.=HI)
    double bypass_pos = 0.;     // BPV (ByPass Valve) position (% open : 0.=closed, 1.=open)
    double cdts_temp = 0.;      // CDTS (Compressor Discharge Temp. Sensor) temperature
    double output_temp = 0.;    // pack outlet temperature

    double target_fcv_flow = 0.;
    double target_max_flow = MAX_FLOW;

    double cabin_press = 0.;

    double calculateOutputTemperature(const double temp_turb, const double temp_bypass, double bypass_valve_pos) {
        // valve position is between 0. and 1.
        if (bypass_valve_pos > 1.) {
            bypass_valve_pos = 1.;
        } else if (bypass_valve_pos < 0.) {
            bypass_valve_pos = 0.;
        }
        // calculate the air flow ratios frow the valve position
        double ratio_bypass = bypass_valve_pos * BYPASS_RATIO;
        double ratio_turb = 1. - ratio_bypass;
        // then returns the temperature of the mixed air
        return temp_turb * ratio_turb + temp_bypass * ratio_bypass;
    }

    void updateBypassPosition(const double temp_turb, const double temp_bypass, const double target_temp) {
        double bypass_delta = BYPASS_CHANGE_RATE * deltaT;

        if (fcv_flow_rate < 0.02) {
            // if pack flow < 2% : close the bypass
            bypass_pos = max(bypass_pos - bypass_delta, 0.);
        }
        else {
            double current_temp = calculateOutputTemperature(temp_turb, temp_bypass, bypass_pos);
            double output_temp_delta = fabs(target_temp - current_temp);
            if (output_temp_delta > 0.5) {
                // if the current output temperature is too far from the expected temperature, we try to change the bypass flow
                double output_bypass_open = calculateOutputTemperature(temp_turb, temp_bypass, bypass_pos + bypass_delta);
                double output_bypass_close = calculateOutputTemperature(temp_turb, temp_bypass, bypass_pos - bypass_delta);

                if (output_temp_delta > fabs(target_temp - output_bypass_open)) {
                    // if by opening the valve we get closer to the expected temperature, we do so
                    bypass_pos = min(bypass_pos + bypass_delta, 1.);
                }
                else if (output_temp_delta > fabs(target_temp - output_bypass_close)) {
                    // if by closing the valve we get closer to the expected temperature, we do so
                    bypass_pos = max(bypass_pos - bypass_delta, 0.);
                }
            }
        }
    }

    double updatePhysicalValue(double value, const double target_value, const double change_rate) {
        if (value != target_value) {
            value += change_rate * deltaT;
            if (((change_rate >= 0) && (value > target_value)) || ((change_rate < 0) && (value < target_value))) {
                value = target_value;
            }
        }
        return value;
    }

    double getCabinPressure() {
        if (isnan(lSimVarsValue[CABIN_ALTITUDE])) {
            return convert_inHgToPSI(lSimVarsValue[AMB_PRESS]);
        }
        return convert_PaToPSI(101325 * exp(-0.0003937 * lSimVarsValue[CABIN_ALTITUDE]));
    }

public:
    void init(const int PACK_ID) {
        pack_idx = PACK_ID - 1;

        for (int i = PACK1_OUTLET_TEMP; i <= PACK2_FLOW_RATE; i++) {
            lSimVarsValue[i] = 0.;
        }
    }

    /*
     * Update the pack temperatures.
     * in  : target_temp (target output temperature)
     */
    void update(const double target_temp, const double currentAbsTime) {
        cabin_press = getCabinPressure();

        // update the pack input temperature and pressure
        const double input_press = lSimVarsValue[DUCT1_PRESSURE + pack_idx];
        const double input_temp = lSimVarsValue[DUCT1_TEMPERATURE + pack_idx];
        fcv_toggle = lSimVarsValue[PACK1_VALVE + pack_idx];

        double press_rate = max(min(1, ((input_press - cabin_press) / (EXP_BLEED_PRESS - cabin_press))), 0);
        // if FCV is opening, the flow will go to its max. if closing, the flow will go to a stop
        target_fcv_flow = isRunning() ? (target_max_flow * press_rate) : 0.;
        // update FCV flow
        if (fcv_flow != target_fcv_flow) {
            const double change_rate = (fcv_toggle ? FLOW_CHANGE_RATE : -FLOW_CHANGE_RATE);
            fcv_flow += change_rate * deltaT;
            if (((change_rate >= 0) && (fcv_flow > target_fcv_flow)) || ((change_rate < 0) && (fcv_flow < target_fcv_flow))) {
                fcv_flow = target_fcv_flow;
            }
        }
        // fcv_flow_rate: flow rate from the max possible rate (LO = 0%, HI = 100% = MAX_FLOW)
        fcv_flow_rate = fcv_flow / MAX_FLOW;
        
        // update the inlet pressure and temp depending on FCV position
        inlet_press = updatePhysicalValue(inlet_press, fcv_toggle ? input_press : cabin_press, fcv_toggle ? INLET_PRESS_CHANGE_RATE : -INLET_PRESS_CHANGE_RATE);
        inlet_temp = updatePhysicalValue(inlet_temp, fcv_toggle ? input_temp : aSimVarsValue[FWD_TEMP], fcv_toggle ? INLET_TEMP_CHANGE_RATE : -INLET_TEMP_CHANGE_RATE);

        double cabin_temp = (1. - fcv_flow_rate) * aSimVarsValue[FWD_TEMP];

        // calculate the expected Compressor Discharge Temperature
        double cdts_target_temp = fcv_flow_rate * (COEF_TAT * aSimVarsValue[AMB_TEMP] + COEF_BLEED * input_temp) + cabin_temp;
        // update the current temperature towards that value
        cdts_temp = updatePhysicalValue(cdts_temp, cdts_target_temp, COMPR_TEMP_CHANGE_RATE);
        // calculate turbine discharge temperature and bypass temperature
        double temp_turb = fcv_flow_rate *(((cdts_temp + CELSIUS_KELVIN) / COEF_TURB) - CELSIUS_KELVIN) + cabin_temp;
        double temp_bypass = fcv_flow_rate * (((input_temp + CELSIUS_KELVIN) / COEF_BYPASS) - CELSIUS_KELVIN) + cabin_temp;

        // calculate the appropriate bypass valve new position in order to reach the target output temperature
        updateBypassPosition(temp_turb, temp_bypass, target_temp);

        // calculate the output temperature (mix ou ByPass + Turbine air)
        output_temp = calculateOutputTemperature(temp_turb, temp_bypass, bypass_pos);

        // update the simvars
        lSimVarsValue[PACK1_OUTLET_TEMP + pack_idx] = output_temp;
        lSimVarsValue[PACK1_CMPRSO_TEMP + pack_idx] = cdts_temp;
        lSimVarsValue[PACK1_TURB_BYPASS_POS + pack_idx] = bypass_pos;
        lSimVarsValue[PACK1_FLOW_RATE + pack_idx] = fcv_flow_rate;
#ifdef DEBUG2
        printf("Pack %d : inlet press = %.2f, inlet temp = %.2f, FCV toggle = %d\n", pack_idx, input_press, input_temp, fcv_toggle);
        printf("Pack %d : outlet temp = %.2f, compr. temp = %.2f, bypass pos. = %.2f, flow rate = %.2f\n", pack_idx, output_temp, cdts_temp, bypass_pos, fcv_flow_rate);
#endif
    }

    /*
     * Returns true if the pack is running (if air is going through it).
     * (the current pack is running if the pack Flow Control Valve is open, ai pressure is sufficient and no overheat)
     */
    bool isRunning() { 
        // force not running if:
        //    - same side engine start
        //    - opposite engine start provided the crossbleed valve is open
        //    - engine fire pushbutton released out
        //    - ditching pushbutton set to ON
        bool inletFlowPositive = lSimVarsValue[DUCT1 + pack_idx] && inlet_press > cabin_press;
        float sameSideEngineN2 = aSimVarsValue[ENG1_N2 + pack_idx];
        float otherSideEngineN2 = aSimVarsValue[ENG1_N2 + ((pack_idx + 1) % 2)];
        bool sameSideEngineStarting = sameSideEngineN2 > 1 && sameSideEngineN2 < 58;
        bool otherSideEngineStarting = otherSideEngineN2 > 1 && otherSideEngineN2 < 58;
        bool xBleedOpen = lSimVarsValue[X_BLEED_VALVE];
        bool ditchButtonPushed = lSimVarsValue[DITCH];
        bool sameEngineFireButtonPulled = lSimVarsValue[FIRE_ENG1_PUSH + pack_idx];
        return fcv_toggle && inletFlowPositive && !isOverheating() && !sameSideEngineStarting && !(otherSideEngineStarting && xBleedOpen) && !ditchButtonPushed && !sameEngineFireButtonPulled;
    }

    /* Returns true if the pack is overheating  (pack outlet > 88°C or pack compressor outlet temp > 260°C). */
    bool isOverheating() {
        return  (cdts_temp > 260.) || (output_temp > 88.);
    }

    /* Returns the pack inlet air pressure (bleed air P°). */
    double getInletPressure() { return isnan(inlet_press) ? getCabinPressure() : inlet_press;}

    /* Returns the pack inlet air temperature (bleed air T°). */
    double getInletTemp() { return isnan(inlet_temp) ? aSimVarsValue[AMB_TEMP] : inlet_temp; }

    /* Returns the pack outlet air temperature. */
    double getOuletTemperature() { return isnan(output_temp) ? aSimVarsValue[AMB_TEMP] : output_temp; }

    /* Returns the pack outlet air temperature. */
    double getFlow() { return fcv_flow; }

    /* Update the air flow through the Flow Control Valve. */
    void setTargetMaxFlow(const double flow) {
        if (flow != target_max_flow) {
            target_max_flow = flow;
            // update the FCV flow to reflects the new maximum flow
            target_fcv_flow = isRunning() ? target_max_flow : 0.;
        }
    }
};

/* Cabin (3 zones: cockpit, fwd and aft) */
class Cabin {
private:
    const double CkptVolume = 9;    // m3
    const double FwdVolume = 69.5;  // m3
    const double AftVolume = 69.5;  // m3
    const double Cpa = 1.006;        // Specific heat of air at constant pressure (between -100°C and 100°C, can be considered 1.006 kJ/kg.°C)
    const double Cpv = 1.89;         // specific heat of water vapor at constant pressure (1.89 kJ/kg.°C)
    const double Hwe = 2501.;        // evaporation heat (water at 0°C, 2501 kJ/kg)
    const double Rh = 0.20;          // Relative humidity on the plane (%)
    const double Rd = 287.058;       // Specific gas constant for dry air (J/kg.K)
    const double Rw = 461.495;       // Specific gas constant for water vapor (J/kg.K)

    Pack* pack1Unit;
    Pack* pack2Unit;

    /*
     * Update the current zone temperature.
     */
    double updateZoneTemperature(const double cabin_pressure, const double zone_volume, const double duct_temp_c, const double zone_temp_c, const double target_temp_c, const double zone_flow, const double delta_time) {
        const double zone_temp_k = zone_temp_c + 273.15;      // temperature of zone in K

        // calculate the enthalpy of the air bleed into the zone
        double Pws = exp(34.494 - (4924.99 / (duct_temp_c + 237.1))) / pow(duct_temp_c + 105., 1.57); // Saturated water vapor pressure simplified formula  (Pa)
        double Pw = Rh * Pws;             // Water vapor pressure  (Pa)
        double Pd = cabin_pressure - Pw;  // Dry air pressure  (Pa)
        double W = (0.62197 * Pw) / (cabin_pressure - Pw);  // mixing ratio (kg/kg)
        double duct_enthalpy = Cpa * duct_temp_c + Cpv * W * duct_temp_c + Hwe * W;       // enthalpy of zone duct air (kJ/kg)
        double zone_enthalpy = Cpa * zone_temp_c + Cpv * W * zone_temp_c + Hwe * W;       // enthalpy of zone air (kJ/kg)

         // calculate energy added to the zone during the specified time
        double added_energy = zone_flow * delta_time/1000. * duct_enthalpy;               // energy added since the last update (kJ)
        double lost_energy = zone_flow * delta_time / 1000. * zone_enthalpy;              // energy lost since the last update (kJ)

        double air_density = (Pd / (Rd * zone_temp_k)) + (Pw / (Rw * zone_temp_k));   // density of humid air (kg/m3)
        double latent_energy = zone_temp_c * Cpa * air_density * zone_volume;         // latent energy in the given zone (kJ)

        // return the new temperature of the zone
        return (latent_energy + added_energy - lost_energy) / (Cpa * air_density * zone_volume);
    }


public:
    void init(Pack* pack1, Pack* pack2) {
        pack1Unit = pack1;
        pack2Unit = pack2;
        
        // Initialisation: cabin temperature is equal to ambiant temperature (not always true but close enough)
        for (int i = CKPT_TEMP; i <= AFT_TEMP; i++) {
            lSimVarsValue[i] = aSimVarsValue[AMB_TEMP];
        }
    }

    /*
     * Update the cabin temperatures.
     *  in:  CKPT_TEMP_KNOB, CKPT_TEMP, CKPT_DUCT_TEMP
     *       FWD_TEMP_KNOB, FWD_TEMP, FWD_DUCT_TEMP
     *       AFT_TEMP_KNOB, AFT_TEMP, AFT_DUCT_TEMP
     *  out: CKPT_TEMP, FWD_TEMP, AFT_TEMP
     */
    void update(const double currentAbsTime) {
        // get knobs position in order to get the target temperatures (XXX_TEMP_KNOB know values between 0 and 100)
        const int ckpt_target_temp = 18 + (0.05 * lSimVarsValue[CKPT_TEMP_KNOB]);
        const int fwd_target_temp = 18 + (0.05 * lSimVarsValue[FWD_TEMP_KNOB]);
        const int aft_target_temp = 18 + (0.05 * lSimVarsValue[AFT_TEMP_KNOB]);

        // get pack flow controller position in order to calculate cabin air flows
        // first get the knob position (0: LOW, 1: NORM, 2: HI)
        ENUM flow_status = lSimVarsValue[PACK_FLOW_CONTROLLER];
        if (!pack1Unit->isRunning() && !pack2Unit->isRunning()) {
            // if both packs are not running : NO PACK
            flow_status = FLOW_NO_PACK;
        }
        else if (!pack1Unit->isRunning() || !pack2Unit->isRunning()) {
            // if only 1 pack is not running, we are running on 1 pack: 1 PACK
            flow_status = FLOW_1PACK;
        }
        const double ckpt_flow = CockpitTotalAirFlow[flow_status];      // associated total air flow for each zone (kg/s)
        const double fwd_flow = CabinTotalAirFlow[flow_status] / 2.;
        const double aft_flow = fwd_flow;

        // get current cabin pressure and temperature
        const double cabin_pressure = isnan(lSimVarsValue[CABIN_ALTITUDE]) ? (3386. * lSimVarsValue[AMB_PRESS]) : (101325 * exp(-0.0003937 * lSimVarsValue[CABIN_ALTITUDE])); // pressure in Pa
        const double ckpt_temp = isnan(lSimVarsValue[CKPT_TEMP]) ? lSimVarsValue[AMB_TEMP] : lSimVarsValue[CKPT_TEMP];                  // temperature of cockpit in °C
        const double ckpt_duct_temp_c = isnan(lSimVarsValue[CKPT_DUCT_TEMP]) ? lSimVarsValue[AMB_TEMP] : lSimVarsValue[CKPT_DUCT_TEMP]; // temperature of cockpit duct in °C
        const double fwd_temp = isnan(lSimVarsValue[FWD_TEMP]) ? lSimVarsValue[AMB_TEMP] : lSimVarsValue[FWD_TEMP];                     // temperature of fwd in °C
        const double fwd_duct_temp_c = isnan(lSimVarsValue[FWD_DUCT_TEMP]) ? lSimVarsValue[AMB_TEMP] : lSimVarsValue[FWD_DUCT_TEMP];    // temperature of fwd duct in °C
        const double aft_temp = isnan(lSimVarsValue[AFT_TEMP]) ? lSimVarsValue[AMB_TEMP] : lSimVarsValue[AFT_TEMP];                     // temperature of aft in °C
        const double aft_duct_temp_c = isnan(lSimVarsValue[AFT_DUCT_TEMP]) ? lSimVarsValue[AMB_TEMP] : lSimVarsValue[AFT_DUCT_TEMP];    // temperature of aft duct in °C
#ifdef DEBUG
        printf("Zone current temperatures (ckpt = %.1f, fwd = %.1f, aft = %.1f)\n", ckpt_temp, fwd_temp, aft_temp);
#endif
        double delta_time = (currentAbsTime - lastAbsTime);

        lSimVarsValue[CKPT_TEMP] = updateZoneTemperature(cabin_pressure, CkptVolume, ckpt_duct_temp_c, ckpt_temp, ckpt_target_temp, ckpt_flow, delta_time);
        lSimVarsValue[FWD_TEMP] = updateZoneTemperature(cabin_pressure, FwdVolume, fwd_duct_temp_c, fwd_temp, fwd_target_temp, fwd_flow, delta_time);
        lSimVarsValue[AFT_TEMP] = updateZoneTemperature(cabin_pressure, AftVolume, aft_duct_temp_c, aft_temp, aft_target_temp, aft_flow, delta_time);
    }
};


/* Pack system : 
 *   - Air Conditioning System Controllers (ACSCs) automatically control the air conditioning system
 *   - Cabin
 */
class PacksSys {
private:
    const double VALVE_CHANGE_RATE = 1. / (2000 + rand() % 1000);  // 2000-3000ms
    const double TEMP_DELTA = 0.01;
    const double HOT_RATIO = 0.5;
    const double PERCENT_DELTA = 0.01; // percentages are different if they have a 1% difference at least
    const double MAX_OUT_FLOW = 1.535;

    Pack packUnit1;   // Pack 1
    Pack packUnit2;   // Pack 2
    Cabin cabin;

    ENUM flow_status;       // pack flow status (FLOW_2PACKS_LO, FLOW_2PACKS_NORM, FLOW_2PACKS_HI, FLOW_1PACK, FLOW_NO_PACK)
    double recirc_ratio;    // recirculation ratio within the mixer unit
    double ckpt_flow;       // total air flow in the cockpit (kg/s)
    double cabin_flow;      // total air flow in the cabin (kg/s)

    double target_recirc_ratio;    // recirculation ratio within the mixer unit
    double target_ckpt_flow;       // total air flow in the cockpit (kg/s)
    double target_cabin_flow;      // total air flow in the cabin (kg/s)

    double recirc_ratio_change_rate = VALVE_CHANGE_RATE;
    double ckpt_flow_change_rate = VALVE_CHANGE_RATE;
    double cabin_flow_change_rate = VALVE_CHANGE_RATE;

    bool cabin_fan = true;

    /*
     * Return the pack flow index.
     *  in:  PACK_FLOW_CONTROLLER, 
     *       packUnit1, packUnit2
     *  out: (FLOW_2PACKS_LO, FLOW_2PACKS_NORM, FLOW_2PACKS_HI, FLOW_1PACK, FLOW_NO_PACK)
     */
    ENUM getPackFlowStatus() {
        // first get the knob position (0: LOW, 1: NORM, 2: HI)
        int flow_status = lSimVarsValue[PACK_FLOW_CONTROLLER];
        if (!packUnit1.isRunning() && !packUnit2.isRunning()) {
            // if both packs are not running : NO PACK
            flow_status = FLOW_NO_PACK;
        }
        else if (!packUnit1.isRunning() || !packUnit2.isRunning()) {
            // if only 1 pack is not running, we are running on 1 pack: 1 PACK
            flow_status = FLOW_1PACK;
        }
        else if ((lSimVarsValue[DUCT1] == GPU_BLEED || lSimVarsValue[DUCT1] == APU_BLEED) 
            && (lSimVarsValue[DUCT2] == GPU_BLEED || lSimVarsValue[DUCT2] == APU_BLEED)) {
            // force high flow on APU & GPU bleed air supply
            flow_status = FLOW_2PACKS_HI;
        }
        else if (flightPhase() == 1 || lSimVarsValue[LANDED]) {
            flow_status = FLOW_2PACKS_LO;
        }
        return flow_status;
    }

    double updateValue(double value, const double target_value, const double change_rate, const uint64_t deltaT) {
        // update the recirculation ratio until we reech the new expected value
        if (value != target_value) {
            value += change_rate * deltaT;
            if (((change_rate >= 0) && (value > target_value)) || ((change_rate < 0) && (value < target_value))) {
                value = target_value;
            }
        }
        return value;
    }

    double calculateDuctTemperature(const double mixer_temp, const double hot_manifold_temp, double valve_pos) {
        // duct temp is calculated by mixing mixer unit temp and hot manifold temperature
        return valve_pos * HOT_RATIO * hot_manifold_temp + (1. - valve_pos * HOT_RATIO) * mixer_temp;
    }

    double chooseValvePosition(const double duct_target_temp, const double mixer_temp, const double hot_manifold_temp, double valve_pos) {
        double valve_open_pos = min(valve_pos + VALVE_CHANGE_RATE * deltaT, 1.);  // next open position
        double valve_close_pos = max(valve_pos - VALVE_CHANGE_RATE * deltaT, 0.); // next close position

        // when the HOT_AIR valve is closed or if both packs are OFF, we close all the trim valves
        if (!lSimVarsValue[HOT_AIR_VALVE_POS] || flow_status == FLOW_NO_PACK) {
            return valve_close_pos;
        }
        double duct_temp = calculateDuctTemperature(mixer_temp, hot_manifold_temp, valve_pos);
        double duct_temp_delta = fabs(duct_target_temp - duct_temp);
        if (duct_temp_delta > TEMP_DELTA) {
            // if the current duct temperature is different from the expected temperature, we try to change the trim valve position
            double duct_trim_open = calculateDuctTemperature(mixer_temp, hot_manifold_temp, valve_open_pos);
            if (duct_temp_delta > fabs(duct_target_temp - duct_trim_open)) {
                // if by opening the valve we get closer to the expected temperature, we do so
                return valve_open_pos;
            }

            double duct_trim_close = calculateDuctTemperature(mixer_temp, hot_manifold_temp, valve_close_pos);
            if (duct_temp_delta > fabs(duct_target_temp - duct_trim_close)) {
                // if by closing the valve we get closer to the expected temperature, we do so
                return valve_close_pos;
            }
        }
        // if temperature is close enough, keep the current trim position
        return valve_pos;
    }

public:
    void init() {
        packUnit1.init(1);
        packUnit2.init(2);
        cabin.init(&packUnit1, &packUnit2);

        // initialisation : consider no packs running for now
        flow_status = FLOW_NO_PACK;
        recirc_ratio = RecircFlowValue[FLOW_NO_PACK]; 
        ckpt_flow = CockpitTotalAirFlow[FLOW_NO_PACK];
        cabin_flow = CabinTotalAirFlow[FLOW_NO_PACK];

        for (int i = CKPT_DUCT_TEMP; i <= FWD_TRIM_VALVE_POS; i++) {
            lSimVarsValue[i] = 0;
        }
    }

    /*
     * Update the ACSC.
     *  in:  CKPT_TEMP, CKPT_DUCT_TEMP
     *       FWD_TEMP, FWD_DUCT_TEMP
     *       AFT_TEMP, AFT_DUCT_TEMP
     *  out: CKPT_TEMP, FWD_TEMP, AFT_TEMP
     */
    void update(const double currentAbsTime) {
        const double ckpt_temp = isnan(lSimVarsValue[CKPT_TEMP]) ? (aSimVarsValue[AMB_TEMP]) : (lSimVarsValue[CKPT_TEMP]);  // temperature of cockpit in °C
        const double fwd_temp = isnan(lSimVarsValue[FWD_TEMP]) ? (aSimVarsValue[AMB_TEMP]) : lSimVarsValue[FWD_TEMP];    // temperature of fwd in °C
        const double aft_temp = isnan(lSimVarsValue[AFT_TEMP]) ? (aSimVarsValue[AMB_TEMP]) : lSimVarsValue[AFT_TEMP];    // temperature of aft in °C

        // get pack flow controller position in order to get recirculation flow ratio
        ENUM new_flow_status = getPackFlowStatus();
        if (flow_status != new_flow_status) {
            // if the flow status has changed, we have to update the flow to reach the new value
            flow_status = new_flow_status;

            // update the target ratio/flow from the new pack flow status
            target_recirc_ratio = cabin_fan ? RecircFlowValue[flow_status] : 0.;   // recirculation ratio within the mixer unit
            target_ckpt_flow = CockpitTotalAirFlow[flow_status];  // associated total air flow in the cockpit (kg/s)
            target_cabin_flow = CabinTotalAirFlow[flow_status];   // associated total air flow in the cabin (kg/s)

            recirc_ratio_change_rate = (target_recirc_ratio - recirc_ratio) * VALVE_CHANGE_RATE;
            ckpt_flow_change_rate = (target_ckpt_flow - ckpt_flow) * VALVE_CHANGE_RATE;
            cabin_flow_change_rate = (target_cabin_flow - cabin_flow) * VALVE_CHANGE_RATE;
        }

        // if fan off : no recirculation 
        bool new_cabin_fan = lSimVarsValue[CAB_FANS];
        if (cabin_fan != new_cabin_fan) {
            cabin_fan = new_cabin_fan;

            target_recirc_ratio = cabin_fan ? RecircFlowValue[flow_status] : 0.;   // recirculation ratio within the mixer unit
        }

        // update the recirculation ratio and flow until we reech the new expected value
        recirc_ratio = updateValue(recirc_ratio, target_recirc_ratio, recirc_ratio_change_rate, deltaT);
        ckpt_flow = updateValue(ckpt_flow, target_ckpt_flow, ckpt_flow_change_rate, deltaT);
        cabin_flow = updateValue(cabin_flow, target_cabin_flow, cabin_flow_change_rate, deltaT);

        // calculate all air recirculation flows in each zone
        double recirc_flow_ckpt = recirc_ratio * ckpt_flow;
        double recirc_flow_fwd = recirc_ratio * cabin_flow / 2.;
        double recirc_flow_aft = recirc_flow_fwd;
        double recirc_flow_total = recirc_flow_ckpt + recirc_flow_fwd + recirc_flow_aft;

        // then calculate the recirculation temperature coming to the mixer unit if cabin fan ON
        double recirc_temp = aSimVarsValue[AMB_TEMP];
        if (recirc_flow_total > 0.) {
            recirc_temp = ((recirc_flow_ckpt / recirc_flow_total) * ckpt_temp)
                + ((recirc_flow_fwd / recirc_flow_total) * fwd_temp)
                + ((recirc_flow_aft / recirc_flow_total) * aft_temp);
        }

        // update air going through the packs
        packUnit1.setTargetMaxFlow(FlowPerPack[flow_status]);
        packUnit2.setTargetMaxFlow(FlowPerPack[flow_status]);

        // get fresh air flow coming from the packs
        double pack1_flow = packUnit1.getFlow();
        double pack2_flow = packUnit2.getFlow();
        double pack_flow_total = pack1_flow + pack2_flow;

        // mixer unit input flow = recirculation flow + pack flow
        double mixer_unit_input_flow = recirc_flow_total + pack_flow_total;

        // calculate the temperature inside the mixer unit
        double pack1_ratio = pack1_flow / mixer_unit_input_flow;
        double pack2_ratio = pack2_flow / mixer_unit_input_flow;
        double recirc_ratio = recirc_flow_total / mixer_unit_input_flow;

        // CKPT_TEMP_KNOB (percent) : represent target temp from 18 to 23 degrees
        const double ckpt_target_temp = 18 + (0.05 * lSimVarsValue[CKPT_TEMP_KNOB]);
        const double fwd_target_temp = 18 + (0.05 * lSimVarsValue[FWD_TEMP_KNOB]);
        const double aft_target_temp = 18 + (0.05 * lSimVarsValue[AFT_TEMP_KNOB]);
        // we target the minimum temperature inside the mixer unit
        const double min_request_temp = min(min(ckpt_target_temp, fwd_target_temp), aft_target_temp);

        // calculate the packs target temperatures in order to reach (min_request_temp) inside the mixer unit
        const double pack_target_temp = (min_request_temp - recirc_ratio * recirc_temp) / (pack1_ratio + pack2_ratio);

        // update the packs
        packUnit1.update(pack_target_temp, currentAbsTime);
        packUnit2.update(pack_target_temp, currentAbsTime);

        double mixer_unit_temp = recirc_ratio * recirc_temp
            + pack1_ratio * packUnit1.getOuletTemperature()
            + pack2_ratio * packUnit2.getOuletTemperature();

        // hot manifold temperature is the mixing between hot air from duct1 and duct2
        const double pack_total_pressure = packUnit1.getInletPressure() + packUnit2.getInletPressure();
        double hot_manifold_temp = aSimVarsValue[AMB_TEMP];
        if (pack_total_pressure > 0.) {
            hot_manifold_temp = (packUnit1.getInletPressure() / pack_total_pressure) * packUnit1.getInletTemp()
                + (packUnit2.getInletPressure() / pack_total_pressure) * packUnit2.getInletTemp();
        }

        if (lSimVarsValue[HOT_AIR_VALVE_POS] > lSimVarsValue[HOT_AIR] + PERCENT_DELTA) {
            lSimVarsValue[HOT_AIR_VALVE_POS] = max(0., lSimVarsValue[HOT_AIR_VALVE_POS] - VALVE_CHANGE_RATE*deltaT);
        } else if (lSimVarsValue[HOT_AIR_VALVE_POS] < lSimVarsValue[HOT_AIR] - PERCENT_DELTA) {
            lSimVarsValue[HOT_AIR_VALVE_POS] = min(1., lSimVarsValue[HOT_AIR_VALVE_POS] + VALVE_CHANGE_RATE*deltaT);
        }

        // choose the new trim valve positions (0.=closed to 1.=open)
        lSimVarsValue[CKPT_TRIM_VALVE_POS] = chooseValvePosition(ckpt_target_temp, mixer_unit_temp, hot_manifold_temp, lSimVarsValue[CKPT_TRIM_VALVE_POS]);
        lSimVarsValue[FWD_TRIM_VALVE_POS] = chooseValvePosition(fwd_target_temp, mixer_unit_temp, hot_manifold_temp, lSimVarsValue[FWD_TRIM_VALVE_POS]);
        lSimVarsValue[AFT_TRIM_VALVE_POS] = chooseValvePosition(aft_target_temp, mixer_unit_temp, hot_manifold_temp, lSimVarsValue[AFT_TRIM_VALVE_POS]);

        // then update the pack duct temperature
        lSimVarsValue[CKPT_DUCT_TEMP] = calculateDuctTemperature(mixer_unit_temp, hot_manifold_temp, lSimVarsValue[CKPT_TRIM_VALVE_POS]);
        lSimVarsValue[FWD_DUCT_TEMP] = calculateDuctTemperature(mixer_unit_temp, hot_manifold_temp, lSimVarsValue[FWD_TRIM_VALVE_POS]);
        lSimVarsValue[AFT_DUCT_TEMP] = calculateDuctTemperature(mixer_unit_temp, hot_manifold_temp, lSimVarsValue[AFT_TRIM_VALVE_POS]);

        // update the air cond output flow rate
        double mixer_out_flow_rate = min(1., mixer_unit_input_flow / MAX_OUT_FLOW);
        lSimVarsValue[MIXER_UNIT_OUT_FLOW_RATE] = mixer_out_flow_rate;
#ifdef DEBUG
        printf("Pack flow status = %d\n", flow_status);
        printf("Mixer Unit : temperature = %.2f, out flow = %.2f\n", mixer_unit_temp, mixer_out_flow_rate);
        printf("Trim valve pos (ckpt = %.1f, fwd = %.1f, aft = %.1f)\n", lSimVarsValue[CKPT_TRIM_VALVE_POS], lSimVarsValue[FWD_TRIM_VALVE_POS], lSimVarsValue[AFT_TRIM_VALVE_POS]);
        printf("Trim duct temp (ckpt = %.1f, fwd = %.1f, aft = %.1f)\n", lSimVarsValue[CKPT_DUCT_TEMP], lSimVarsValue[FWD_DUCT_TEMP], lSimVarsValue[AFT_DUCT_TEMP]);
#endif

        // then, update the cabin temperature
        cabin.update(currentAbsTime);
    }
};

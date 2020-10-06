#pragma once

#ifndef SYS_DEF
#include <stdio.h>
#include <MSFS/MSFS.h>
#include <MSFS/MSFS_Render.h>
#include <MSFS/Legacy/gauges.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <vector>

#include "utils/utils.h"
#include "utils/PID.h"
#include "utils/bit_oper.h"

#define REFRESH_RATE 500        //update refresh rate in milliseconds
double lastAbsTime = 0;         //last time the update function was run

/*
* =============== *
* DATA CRUD STUFF *
* =============== *
*/
void initUnitEnums();
void initLocalSimVarsIDs();
void updateLocalSimVars();
void updateASimVars();

ENUM* ENUM_UNITS;
ID* ID_LSIMVAR;
ENUM* keyEventID;

FLOAT64 aSimVarsValue[aSimVarsCount];
FLOAT64 lSimVarsValue[totalLVarsCount];
std::vector<int> dirtylSimVars;

typedef enum unitsEnum {
    bool_units,
    percent_units,
    volts_units,
    amperes_units,
    hertz_units,
    PSI_units,
    number_units,
    enum_units,
    celcius_units,

    enumUnitsCount
}unitsEnum;

typedef enum aSimVars {
    EXT_AVAIL,              //"EXTERNAL POWER AVAILABLE","Bool"
    EXT_POWER,              //"EXTERNAL POWER ON", "Bool"
    APU_MASTER,             //"FUELSYSTEM VALVE SWITCH:8", "Bool"
    APU_RPM,                //"APU PCT RPM", "percent"
    ENG1_N2,                //"ENG N2 RPM:1","percent"
    ENG2_N2,                //"ENG N2 RPM:2","percent"
    ENG1_N1,                //"ENG N1 RPM:1","percent"
    ENG2_N1,                //"ENG N1 RPM:2","percent"
    APU_START_SWITCH,       //"A:APU SWITCH", "Bool"
    BATT1_SW,               //A:ELECTRICAL MASTER BATTERY:#ID#, Bool
    BATT2_SW,               //A:ELECTRICAL MASTER BATTERY:#ID#, Bool
    AMB_TEMP,               //"AMBIENT TEMPERATURE", "celsius"
    AMB_PRESS,              //AMBIENT PRESSURE, inHg
    TAS,                    //"AIRSPEED TRUE","Knots"
    IAS,                    //"AIRSPEED INDICATED","Knots"
    GEN1_SW,                //A:GENERAL ENG MASTER ALTERNATOR:1
    GEN2_SW,                //A:GENERAL ENG MASTER ALTERNATOR:2
    ENG1_BLEED,             //(A:BLEED AIR ENGINE:#ID#, Bool)
    ENG2_BLEED,             //(A:BLEED AIR ENGINE:#ID#, Bool)
    APU_BLEED,              //"BLEED AIR APU","Bool"
    ENG1_STARTER,           //(A:GENERAL ENG STARTER:1, Bool)
    ENG2_STARTER,           //(A:GENERAL ENG STARTER:2, Bool)
    STRUCT_ANTI_ICE,        //A:STRUCTURAL DEICE SWITCH, Bool
    ENG1_EGT,                //A:ENG EXHAUST GAS TEMPERATURE:1, Rankine
    ENG2_EGT,                //A:ENG EXHAUST GAS TEMPERATURE:2, Rankine
    

    aSimVarsCount
}aSimVars;


typedef enum keyEventTrigger {
    APU_START_TRIGGER,              //KEY_APU_STARTER

    keyEventTriggerCount
}keyEventTrigger;

typedef enum lSimVars {
    /*============================================================================*/
    /*                                   ELEC                                     */
    /*============================================================================*/
    /*
    * ========= *
    * Batteries *
    * ========= *
    */
    BATT1_ONLINE,           //"L:BATT1_ONLINE","Bool"
    BATT2_ONLINE,           //"L:BATT2_ONLINE","Bool"
    BATT1_CAPACITY,         //"L:BATT1_CAPACITY","Amperes"
    BATT2_CAPACITY,         //"L:BATT2_CAPACITY","Amperes"
    BATT1_VOLTAGE,          //"L:BATT1_VOLTAGE","Volts"
    BATT2_VOLTAGE,          //"L:BATT2_VOLTAGE","Volts"
    BATT1_AMPERAGE,         //"L:BATT1_AMPERAGE","Amperes"
    BATT2_AMPERAGE,         //"L:BATT2_AMPERAGE","Amperes"

    BATT_BUS_LOAD,          //"L:BATT_BUS_LOAD","Amperes"
    
    /*
    * === *
    * GPU *
    * === *
    */
    EXT_GEN_ONLINE,         //"L:EXT_GEN_ONLINE
    EXT_GEN_VOLTAGE,        //"L:EXT_GEN_VOLTAGE","Volts"
    EXT_GEN_AMPERAGE,       //"L:EXT_GEN_AMPERAGE","Amperes"
    EXT_GEN_FREQ,           //"L:EXT_GEN_FREQ","Hertz"

    /*
    * === *
    * APU *
    * === *
    */

    APU_GEN_ONLINE,         //"L:APU_GEN_ONLINE","Bool"
    APU_GEN_VOLTAGE,        //"L:APU_GEN_VOLTAGE", "Volts"
    APU_GEN_AMPERAGE,       //"L:APU_GEN_AMPERAGE","Amperes"
    APU_GEN_FREQ,           //"L:APU_GEN_FREQ","Hertz"
    APU_LOAD_PERCENT,       //"L:APU_LOAD_PERCENT","percent"

    /*
    * =========== *
    * ENGINE GENS *
    * =========== *
    */
    GEN1_ONLINE,            //"L:GEN1_ONLINE","Bool"
    GEN2_ONLINE,            //"L:GEN2_ONLINE","Bool"
    GEN1_VOLTAGE,           //"L:GEN1_VOLTAGE","Volts"
    GEN2_VOLTAGE,           //"L:GEN2_VOLTAGE","Volts"
    GEN1_AMPERAGE,          //"L:GEN1_AMPERAGE","Amperes"
    GEN2_AMPERAGE,          //"L:GEN2_AMPERAGE","Amperes"
    GEN1_FREQ,              //"L:GEN1_FREQ","Hertz"
    GEN2_FREQ,              //"L:GEN2_FREQ","Hertz"
    GEN1_IDG,               //"L:GEN1_IDG_TEMP","celcius"
    GEN2_IDG,               //"L:GEN2_IDG_TEMP","celcius"
    
    /*
    * === *
    * RAT *
    * === *
    */
    EMER_ONLINE,            //"L:EMER_ONLINE","Bool"
    EMER_VOLTAGE,           //"L:EMER_VOLTAGE","Volts"
    EMER_AMPERAGE,          //"L:EMER_AMPERAGE","Amperes"
    EMER_FREQ,              //"L:EMER_FREQ","Hertz"

    /*
    * ===== *
    * BUSES *
    * ===== *               //PRIORITY                  0               1               2               3               4                   5
    */
    AC_BUS1,                //"L:AC_BUS1","Enum"        0 = No power,   1 = gen1,       3 = ext,        4 = apu,        2 = gen 2
    AC_BUS2,                //"L:AC_BUS2","Enum"        0 = No power,   2 = gen2,       3 = ext,        4 = apu,        1 = gen 2
    AC_ESS,                 //"L:AC_ESS","Enum"         0 = No power,   AC_BUS1,        AC_BUS2,        5 = rat
    AC_SHED,                //"L:AC_SHED","Enum"        0 = No power,   AC_ESS
    GALLEY_SHED,            //"L:GALLEY_SHED","Enum"    0 = No power,   AC_SHED
    DC_BUS1,                //"L:DC_BUS1","Enum"        0 = No power,   7 = TR1,        8 = TR2
    DC_BUS2,                //"L:DC_BUS2","Enum"        0 = No power,   8 = TR2,        7 = TR1
    DC_BAT,                 //"L:DC_BATBUS","Enum"      0 = No power,   DC_BUS1,        DC_BUS2,        12 = BATT12,    10 = BATT1,         11 = BATT2
    DC_ESS,                 //"L:DC_ESS","Enum"         0 = No power,   DC_BAT,         9 = TRESS
    DC_SHED,                //"L:DC_SHED","Enum"        0 = No power,   DC_ESS
    HOT_BUS1,               //"L:HOT_BUS1","Enum"       0 = No power,   10 = BATT1
    HOT_BUS2,               //"L:HOT_BUS2","Enum"       0 = No power,   11 = BATT2

    /*
    * ================ *
    * AC/DC convertors *
    * ================ *
    */
    TR1_ONLINE,             //"L:TR1_ONLINE","Bool"
    TR2_ONLINE,             //"L:TR2_ONLINE","Bool"
    TRESS_ONLINE,           //"L:TRESS_ONLINE","Bool"
    TR1_VOLTAGE,            //"L:TR1_VOLTAGE","Volts"
    TR2_VOLTAGE,            //"L:TR2_VOLTAGE","Volts"
    TRESS_VOLTAGE,          //"L:TRESS_VOLTAGE","Volts"
    TR1_AMPERAGE,           //"L:TR1_AMPERAGE","Amperes"
    TR2_AMPERAGE,           //"L:TR2_AMPERAGE","Amperes"
    TRESS_AMPERAGE,         //"L:TRESS_AMPERAGE","Amperes"
    STATICINV_ONLINE,       //"L:STATICINV_ONLINE","Bool"
    STATICINV_VOLTAGE,      //"L:STATICINV_VOLTAGE","Volts"
    STATICINV_AMPERAGE,     //"L:STATICINV_AMPERAGE","Amperes"
    STATICINV_FREQ,         //"L:STATICINV_FREQ","Hertz"

    /*
    * =========== * 
    * AC/DC Power *
    * =========== *
    */
    AC_POWER_AVAIL,         //"L:ACPowerAvailable, Bool"
    DC_POWER_AVAIL,         //"L:DCPowerAvailable, Bool"
    
    /*
    * ======== *
    * CIRCUITS *
    * ======== *
    *
    * NOTE: ALL CIRCUITS AND CIRCUIT BREAKERS ARE BIT WISE ENCODED INTO UNSIGNED LONG INT  
    * L:CIRCUIT, FLOAT64
    * L:CIRCUIT_BREAKER, FLOAT64
    */
    CIRCUIT,

    /*
    * ================ *
    * CIRCUIT_BREAKERS *    //0 = circuit breaker pulled(disconnects power); 1 = circuit breaker pushed
    * ================ *
    */
    CIRCUIT_BREAKER,
    
    /*
    * ========== *
    * XML L VARS *
    * ========== *
    */
    APU_START,              //L : A32NX_APU_START_ACTIVATED , "Bool"
    IDG1_FAULT,             //L : A32NX_ELEC_IDG1_FAULT
    IDG1_DISC_SW,           //L : A32NX_ELEC_IDG1_TOGGLE
    IDG2_FAULT,             //L : A32NX_ELEC_IDG2_FAULT
    IDG2_DISC_SW,           //L : A32NX_ELEC_IDG2_TOGGLE
    BUSTIE_AUTO,            //L : A32NX_ELEC_BUSTIE_TOGGLE
    ACESS_FEED_FAULT,       //L : A32NX_ELEC_ACESSFEED_FAULT
    ACESS_FEED_AUTO,        //L : A32NX_ELEC_ACESSFEED_TOGGLE
    COMMERCIAL_SW,          //L : A32NX_ELEC_COMMERCIAL_TOGGLE
    COMMERCIAL_FAULT,       //L : A32NX_ELEC_COMMERCIAL_FAULT
    GALLY_CAB_SW,           //L : A32NX_ELEC_GALYCAB_TOGGLE
    GALLY_CAB_FAULT,        //L : A32NX_ELEC_GALYCAB_FAULT

/*============================================================================*/
/*                                   BLEED                                    */
/*============================================================================*/
    /*
    * ============ *
    * BLEED Valves *
    * ============ *
    */
    ENG1_IP_VALVE,              //L:ENG1_IP_VALVE, Bool
    ENG2_IP_VALVE,              //L:ENG2_IP_VALVE, Bool
    ENG1_HP_VALVE,              //L:ENG1_HP_VALVE, Bool
    ENG2_HP_VALVE,              //L:ENG2_HP_VALVE, Bool
    ENG1_BLEED_VALVE,           //L:ENG1_BLEED_VALVE, Bool
    ENG2_BLEED_VALVE,           //L:ENG2_BLEED_VALVE, Bool
    APU_BLEED_VALVE,            //L:APU_BLEED_VALVE, Bool
    GPU_BLEED_VALVE,            //L:GPU_BLEED_VALVE, Bool
    ENG1_BLEED_STARTER_VALVE,   //L:ENG1_BLEED_STARTER, Bool
    ENG2_BLEED_STARTER_VALVE,   //L:ENG2_BLEED_STARTER, Bool
    WING_ANTIICE,               //L:WING_ANTICE, Bool
    X_BLEED_VALVE,              //L:X_BLEED_VALVE, Bool

    /*
    * ============== *
    * BLEED PRESSURE *
    * ============== *
    */
    ENG1_BLEED_PRESSURE,    //L:ENG1_BLEED_PRESSURE, PSI
    ENG2_BLEED_PRESSURE,    //L:ENG2_BLEED_PRESSURE, PSI
    ENG1_BLEED_TEMPERATURE, //L:ENG1_BLEED_TEMPERATURE, Celcius
    ENG2_BLEED_TEMPERATURE, //L:ENG2_BLEED_TEMPERATURE, Celcius
    APU_BLEED_PRESSURE,     //L:APU_BLEED_PRESSURE, PSI
    APU_BLEED_TEMPERATURE,  //L:APU_BLEED_TEMPERATURE, Celcius
    GPU_BLEED_PRESSURE,     //L:GPU_BLEED_PRESSURE, PSI
    GPU_BLEED_TEMPERATURE,  //L:GPU_BLEED_TEMPERATURE, Celcius
    

    /*
    * ========== *
    * BLEED DUCT *
    * ========== *
    */                      //PIORITY       0       1       2       3       4       5
    DUCT1,                  //L:DUCT1   NOBLEED     GPU     APU     ENG1    DUCT2   RAT
    DUCT2,                  //L:DUCT2   NOBLEED     ENG2    DUCT1
    
    DUCT1_TEMPERATURE,      //L:DUCT1_TEMPERATURE, Celcius
    DUCT2_TEMPERATURE,      //L:DUCT2_TEMPERATURE, Celcius
    DUCT1_PRESSURE,         //L:DUCT1_PRESSURE, PSI
    DUCT2_PRESSURE,         //L:DUCT2_PRESSURE, PSI
    /*
    * ========== *
    * XML L VARS * 
    * ========== *
    */
    X_BLEED,            //TODO

/*============================================================================*/
/*                                   PACKS                                    */
/*============================================================================*/
    /*
    * ===== *
    * PACKS *
    * ===== *
    */
    PACK1_OUTLET_TEMP,      //L:PACK1_OUTLET_TEMP, Celcius
    PACK2_OUTLET_TEMP,      //L:PACK2_OUTLET_TEMP, Celcius
    PACK1_CMPRSO_TEMP,      //L:PACK1_CMPRSO_TEMP, Celcius
    PACK2_CMPRSO_TEMP,      //L:PACK2_CMPRSO_TEMP, Celcius

    /*
    * ========== *
    * CABIN TEMP *
    * ========== *
    */
    CKPT_TEMP,            //L:CKPT_TEMP, Celcius
    FWD_TEMP,             //L:FWD_TEMP, Celcius
    AFT_TEMP,             //L:AFT_TEMP, Celcius

    /*
    * ========== *
    * XML L VARS *
    * ========== *
    */
    CKPT_TEMP_KNOB,         //L : A320_Neo_AIRCOND_LVL_1
    FWD_TEMP_KNOB,          //L : A320_Neo_AIRCOND_LVL_2
    AFT_TEMP_KNOB,          //L : A320_Neo_AIRCOND_LVL_3
    HOT_AIR,                //TODO
    PACK1_VALVE,            //TODO
    PACK2_VALVE,            //TODO
    PACK_FLOW_CONTROLLER,   //TODO

/*============================================================================*/
/*                                   ENGINES                                  */
/*============================================================================*/

    APU_FLAP_OPEN,          //"L:APU_FLAP_OPEN", "Percent"
    APU_N1,                 //"L:APU_N1","Percent"
    APU_EGT,                //"L:APU_EGT","celcius"
    APU_EGT_WARN,           //"L:APU_EGT_WARN","celcius"

    totalLVarsCount
}lSimVars;


typedef enum powerSource{
    NOPOWER,                //0 = No Power

    //AC
    GEN1,                   //1 = AC_GEN1
    GEN2,                   //2 = AC_GEN2
    EXT,                    //3 = EXT_PWR
    APU,                    //4 = APU
    EMER,                   //5 = RAT
    STATINV,                //6 = STATINV

    //DC
    TR1,                    //7 = TR1
    TR2,                    //8 = TR2
    TRESS,                  //9 = TRESS
    BATT1,                  //10 = BATT1
    BATT2,                  //11 = BATT2
    BATT12                  //12 = BATT12

}powerSource;

typedef enum bleedSource {
    NOBLEED,

    GPU_BLEED,
    APU_BLEED,
    ENG1_BLEED,
    ENG2_BLEED,
    RAT_BLEED
};

typedef enum ElECConf {
    NORM_CONF,
    ONE_GEN_INOP,
    PRE_RAT_EMER_CONF,
    RAT100_EMER_CONF,
    RAT50_EMER_CONF,
    RAT0_EMER_CONF,
    TR1_FAULT,
    TR2_FAULT,
    TR12_FAULT
}ELECConf;

typedef enum ELECCircuit {
    //AC_BUS1
    CIRCUIT_1XP,    //0
    CIRCUIT_1IWXP,  //1
    CIRCUIT_101XP,  //2
    CIRCUIT_103XP,  //3
    CIRCUIT_131XP,  //4
    CIRCUIT_110XP,  //5
    //AC_BUS2
    CIRCUIT_2XP,    //6
    CIRCUIT_2IWXP,  //7
    CIRCUIT_202XP,  //8
    CIRCUIT_204XP,  //9
    CIRCUIT_210XP,  //10
    CIRCUIT_212XP,  //11
    CIRCUIT_214XP,  //12
    CIRCUIT_216XP,  //13
    CIRCUIT_231XP,  //14
    //AC_ESS
    CIRCUIT_4IWXP,  //15
    CIRCUIT_401XP,  //16
    CIRCUIT_431XP,  //17
    //AC_SHED
    CIRCUIT_801XP,  //18
    //DC_BAT
    CIRCUIT_3PP,    //19
    CIRCUIT_301PP,  //20
    //DC_BUS1
    CIRCUIT_1PP,    //21
    CIRCUIT_101PP,  //22
    CIRCUIT_103PP,  //23
    CIRCUIT_1IWPP,  //24
    //DC_BUS2
    CIRCUIT_2PP,    //25
    CIRCUIT_202PP,  //26
    CIRCUIT_204PP,  //27
    CIRCUIT_206PP,  //28
    CIRCUIT_208PP,  //29
    CIRCUIT_210PP,  //30
    CIRCUIT_2IWPP,  //31
    CIRCUIT_602PP,  //32
    //DC_ESS
    CIRCUIT_401PP,  //33
    CIRCUIT_4IWPP,  //34
    //DC_SHED
    CIRCUIT_8PP,    //35
    CIRCUIT_801PP,  //36
    CIRCUIT_802PP,  //37
    //HOT_BUS1
    CIRCUIT_701PP,  //38
    CIRCUIT_703PP,  //39
    CIRCUIT_501PP,  //40
    CIRCUIT_502PP,  //41
    //HOT_BUS2
    CIRCUIT_702PP,  //42
    CIRCUIT_704PP,  //43
}eleccircuits;

/*pcstring_<type> holds string inputs required by simconnect function
Ensure that their indices match their corresponding enum definitions.*/

const PCSTRINGZ pcstring_units[enumUnitsCount] = {
    "Bool",
    "Percent",
    "Volts",
    "Hertz",
    "PSI",
    "Number",
    "Enum"
};

const PCSTRINGZ pcstring_aSimVars[aSimVarsCount] = {
    "EXTERNAL POWER AVAILABLE, Bool",
    "EXTERNAL POWER ON, Bool",
    "FUELSYSTEM VALVE SWITCH : 8, Bool",
    "APU PCT RPM, Percent",
    "ENG N2 RPM:1, Percent",
    "ENG N2 RPM:2, Percent",
    "ENG N1 RPM:1, Percent",
    "ENG N1 RPM:2, Percent",
    "APU SWITCH, Bool",
    "ELECTRICAL MASTER BATTERY : 1 , Bool",
    "ELECTRICAL MASTER BATTERY : 2 , Bool",
    "AMBIENT TEMPERATURE, celsius",
    "AMBIENT PRESSURE, inHg",
    "AIRSPEED TRUE, Knots",
    "AIRSPEED INDICATED, Knots",
    "GENERAL ENG MASTER ALTERNATOR : 1, Bool",
    "GENERAL ENG MASTER ALTERNATOR : 2, Bool",
    "BLEED AIR ENGINE : 1, Bool",
    "BLEED AIR ENGINE : 2, Bool"
    "BLEED AIR APU, Bool",
    "GENERAL ENG STARTER:1, Bool",
    "GENERAL ENG STARTER:2, Bool",
    "STRUCTURAL DEICE SWITCH, Bool",
    "ENG EXHAUST GAS TEMPERATURE:1, Rankine",
    "ENG EXHAUST GAS TEMPERATURE:2, Rankine"
};

const PCSTRINGZ pcstring_lSimVars[totalLVarsCount] = {  
//===============================ELEC======================
    "BATT1_ONLINE",
    "BATT2_ONLINE",
    "BATT1_CAPACITY",
    "BATT2_CAPACITY",
    "BATT1_VOLTAGE",
    "BATT2_VOLTAGE",
    "BATT1_AMPERAGE",
    "BATT2_AMPERAGE",
    "BATT_BUS_LOAD",
    "EXT_GEN_ONLINE",
    "EXT_GEN_VOLTAGE",
    "EXT_GEN_AMPERAGE",
    "EXT_GEN_FREQ",
    "APU_GEN_ONLINE",
    "APU_GEN_VOLTAGE",
    "APU_GEN_AMPERAGE",
    "APU_GEN_FREQ",
    "APU_LOAD_PERCENT",
    "GEN1_ONLINE",
    "GEN2_ONLINE",
    "GEN1_VOLTAGE",
    "GEN2_VOLTAGE",
    "GEN1_AMPERAGE",
    "GEN2_AMPERAGE",
    "GEN1_FREQ",
    "GEN2_FREQ",
    "GEN1_IDG_TEMP",
    "GEN2_IDG_TEMP",
    "EMER_ONLINE",
    "EMER_VOLTAGE",
    "EMER_AMPERAGE",
    "EMER_FREQ",
    "AC_BUS1",
    "AC_BUS2",
    "AC_ESS",
    "AC_SHED",
    "GALLEY_SHED",
    "DC_BUS1",
    "DC_BUS2",
    "DC_BATBUS",
    "DC_ESS",
    "DC_SHED",
    "HOT_BUS1",
    "HOT_BUS2",
    "TR1_ONLINE",
    "TR2_ONLINE",
    "TRESS_ONLINE",
    "TR1_VOLTAGE",
    "TR2_VOLTAGE",
    "TRESS_VOLTAGE",
    "TR1_AMPERAGE",
    "TR2_AMPERAGE",
    "TRESS_AMPERAGE",
    "STATIC_INV",
    "STATIC_INV_VOLTAGE",
    "STATICINV_AMPERAGE",
    "STATIC_INV_FREQ",
    "ACPowerAvailable",
    "DCPowerAvailable",
    "CIRCUIT_1XP",
    "CIRCUIT_1IWXP",
    "CIRCUIT_101XP",
    "CIRCUIT_103XP",
    "CIRCUIT_131XP",
    "CIRCUIT_110XP",
    "CIRCUIT_2XP",
    "CIRCUIT_2IWXP",
    "CIRCUIT_202XP",
    "CIRCUIT_204XP",
    "CIRCUIT_210XP",
    "CIRCUIT_212XP",
    "CIRCUIT_214XP",
    "CIRCUIT_216XP",
    "CIRCUIT_231XP",
    "CIRCUIT_4IWXP",
    "CIRCUIT_401XP",
    "CIRCUIT_431XP",
    "CIRCUIT_801XP",
    "CIRCUIT_3PP",
    "CIRCUIT_301PP",
    "CIRCUIT_1PP",
    "CIRCUIT_101PP",
    "CIRCUIT_103PP",
    "CIRCUIT_1IWPP",
    "CIRCUIT_2PP",
    "CIRCUIT_202PP",
    "CIRCUIT_204PP",
    "CIRCUIT_206PP",
    "CIRCUIT_208PP",
    "CIRCUIT_210PP",
    "CIRCUIT_2IWPP",
    "CIRCUIT_602PP",
    "CIRCUIT_401PP",
    "CIRCUIT_4IWPP",
    "CIRCUIT_8PP",
    "CIRCUIT_801PP",
    "CIRCUIT_802PP",
    "CIRCUIT_701PP",
    "CIRCUIT_703PP",
    "CIRCUIT_501PP",
    "CIRCUIT_502PP",
    "CIRCUIT_702PP",
    "CIRCUIT_704PP",
    "CIRCUIT_BREAKER_1XP",
    "CIRCUIT_BREAKER_1IWXP",
    "CIRCUIT_BREAKER_101XP",
    "CIRCUIT_BREAKER_103XP",
    "CIRCUIT_BREAKER_131XP",
    "CIRCUIT_BREAKER_110XP",
    "CIRCUIT_BREAKER_2XP",
    "CIRCUIT_BREAKER_2IWXP",
    "CIRCUIT_BREAKER_202XP",
    "CIRCUIT_BREAKER_204XP",
    "CIRCUIT_BREAKER_210XP",
    "CIRCUIT_BREAKER_212XP",
    "CIRCUIT_BREAKER_214XP",
    "CIRCUIT_BREAKER_216XP",
    "CIRCUIT_BREAKER_231XP",
    "CIRCUIT_BREAKER_4IWXP",
    "CIRCUIT_BREAKER_401XP",
    "CIRCUIT_BREAKER_431XP",
    "CIRCUIT_BREAKER_801XP",
    "CIRCUIT_BREAKER_3PP",
    "CIRCUIT_BREAKER_301PP",
    "CIRCUIT_BREAKER_1PP",
    "CIRCUIT_BREAKER_101PP",
    "CIRCUIT_BREAKER_103PP",
    "CIRCUIT_BREAKER_1IWPP",
    "CIRCUIT_BREAKER_2PP",
    "CIRCUIT_BREAKER_202PP",
    "CIRCUIT_BREAKER_204PP",
    "CIRCUIT_BREAKER_206PP",
    "CIRCUIT_BREAKER_208PP",
    "CIRCUIT_BREAKER_210PP",
    "CIRCUIT_BREAKER_2IWPP",
    "CIRCUIT_BREAKER_602PP",
    "CIRCUIT_BREAKER_401PP",
    "CIRCUIT_BREAKER_4IWPP",
    "CIRCUIT_BREAKER_8PP",
    "CIRCUIT_BREAKER_801PP",
    "CIRCUIT_BREAKER_802PP",
    "CIRCUIT_BREAKER_701PP",
    "CIRCUIT_BREAKER_703PP",
    "CIRCUIT_BREAKER_501PP",
    "CIRCUIT_BREAKER_502PP",
    "CIRCUIT_BREAKER_702PP",
    "CIRCUIT_BREAKER_704PP",
    "A32NX_APU_START_ACTIVATED",
    "A32NX_ELEC_IDG1_FAULT",
    "A32NX_ELEC_IDG1_TOGGLE",
    "A32NX_ELEC_IDG2_FAULT",
    "A32NX_ELEC_IDG2_TOGGLE",
    "A32NX_ELEC_BUSTIE_TOGGLE",
    "A32NX_ELEC_ACESSFEED_FAULT",
    "A32NX_ELEC_ACESSFEED_TOGGLE",
    "A32NX_ELEC_COMMERCIAL_TOGGLE",
    "A32NX_ELEC_COMMERCIAL_FAULT",
    "A32NX_ELEC_GALYCAB_TOGGLE",
    "A32NX_ELEC_GALYCAB_FAULT",
    //===============================BLEED=========================
    "ENG1_IP_VALVE",
    "ENG2_IP_VALVE",
    "ENG1_HP_VALVE",
    "ENG2_HP_VALVE",
    "ENG1_BLEED_VALVE",
    "ENG2_BLEED_VALVE",
    "APU_BLEED_VALVE",
    "GPU_BLEED_VALVE",
    "ENG1_BLEED_STARTER_VALVE",
    "ENG2_BLEED_STARTER_VALVE",
    "WING_ANTICE_VALVE",
    "X_BLEED_VALVE",
    "ENG1_BLEED_PRESSURE",
    "ENG2_BLEED_PRESSURE",
    "ENG1_BLEED_TEMPERATURE",
    "ENG2_BLEED_TEMPERATURE",
    "APU_BLEED_PRESSURE",
    "APU_BLEED_TEMPERATURE",
    "GPU_BLEED_PRESSURE",
    "GPU_BLEED_TEMPERATURE",
    "DUCT1",
    "DUCT2",
    "DUCT1_TEMPERATURE",
    "DUCT2_TEMPERATURE",
    "DUCT1_PRESSURE",
    "DUCT2_PRESSURE",
    "TODO",
    //==============================PACKS============================
    "PACK1_OUTLET_TEMP",
    "PACK2_OUTLET_TEMP",
    "PACK1_CMPRSO_TEMP",
    "PACK2_CMPRSO_TEMP",
    "CKPT_TEMP",
    "FWD_TEMP",
    "AFT_TEMP",
    "A320_Neo_AIRCOND_LVL_1",
    "A320_Neo_AIRCOND_LVL_2",
    "A320_Neo_AIRCOND_LVL_3",
    "TODO",
    "TODO",
    "TODO",
    "TODO",
    //=============================ENGINES============================
    "APU_FLAP_OPEN",
    "APU_N1",
    "APU_EGT",
    "APU_EGT_WARN"
};

void initUnitEnums() {
    ENUM_UNITS = (ENUM*)malloc(sizeof(ENUM) * enumUnitsCount);
    for (int i = 0; i < enumUnitsCount; i++) {
        ENUM_UNITS[i] = get_units_enum(pcstring_units[i]);
    }
}

void initLocalSimVarsIDs() {
    ID_LSIMVAR = (ID*)malloc(sizeof(ID) * totalLVarsCount);
    for (int i = 0; i < totalLVarsCount; i++) {
        ID_LSIMVAR[i] = register_named_variable(pcstring_lSimVars[i]);
    }
}
void updateLocalSimVars() {
    for (int i = BATT1_ONLINE; i < totalLVarsCount; i++) {
        set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
    }
}

void updateASimVars() {
    for (int i = 0; i < aSimVarsCount; i++) {
        execute_calculator_code(pcstring_aSimVars[i], &aSimVarsValue[i], nullptr, nullptr);
    }
}

#define SYS_DEF
#endif

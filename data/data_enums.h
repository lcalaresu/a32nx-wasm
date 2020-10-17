#ifndef DATAENUMS
#define DATAENUMS

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


enum aSimVars {
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
    ENG1_BLEED_SW,          //(A:BLEED AIR ENGINE:#ID#, Bool)
    ENG2_BLEED_SW,          //(A:BLEED AIR ENGINE:#ID#, Bool)
    APU_BLEED_SW,           //"BLEED AIR APU","Bool"
    ENG1_STARTER,           //(A:GENERAL ENG STARTER:1, Bool)
    ENG2_STARTER,           //(A:GENERAL ENG STARTER:2, Bool)
    STRUCT_ANTI_ICE,        //A:STRUCTURAL DEICE SWITCH, Bool
    ENG1_EGT,               //A:ENG EXHAUST GAS TEMPERATURE:1, Rankine
    ENG2_EGT,               //A:ENG EXHAUST GAS TEMPERATURE:2, Rankine
    ON_GROUND,              //A:SIM ON GROUND, Bool
    SURFACE_ALTITUDE,       //A:GROUND ALTITUDE, Meters
    ALTITUDE,               //A:PLANE ALTITUDE, Feet
    ALTITUDE_ABV_GND,       //A:PLANE ALT ABOVE GROUND, Feet
    ENG1_THROTTLE,          //A:GENERAL ENG THROTTLE LEVER POSITION: 1, Percent //CLIMB = 89, FLEX = 95, TOGA = 100
    ENG2_THROTTLE,          //A:GENERAL ENG THROTTLE LEVER POSITION: 2, Percent //CLIMB = 89, FLEX = 95, TOGA = 100
    CURRENT_VSPEED,         //A:VERTICAL SPEED, Feet per second
    GEAR_POS,               //A:GEAR POSITION, enum             unknown = 0, up = 1, dn = 2
    MAN_LDG_ELEV_PCT,        //I:XMLVAR_KNOB_OVHD_CABINPRESS_LDGELEV
    
    aSimVarsCount
};

enum notificationGroupID {
    SDK_CONTROL
};

enum lSimVars {
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
    * L:CIRCUIT, FLOAT64 (Note that this should be typecasted to UINT_64)
    * at bit location(0-63), 0 = circuit offline(no power); 1 = circuit breaker pushed
    */
    CIRCUIT,

    /*
    * ================ *
    * CIRCUIT_BREAKERS *
    * ================ *
    * at bit location(0-63), 0 = circuit breaker pulled(disconnects power); 1 = circuit breaker pushed
    * L:CIRCUIT_BREAKER, FLOAT64 (Note that this should be typecasted to UINT_64)
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
    X_BLEED,                //L:A32NX_KNOB_OVHD_AIRCOND_XBLEED_Position  SHUT = 0, AUTO = 1, OPEN = 2

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
    HOT_AIR,                //L:A32NX_AIRCOND_HOTAIR_TOGGLE, Bool
    PACK1_VALVE,            //L:A32NX_AIRCOND_PACK1_TOGGLE, Bool
    PACK2_VALVE,            //L:A32NX_AIRCOND_PACK2_TOGGLE, Bool
    PACK_FLOW_CONTROLLER,   //L:A32NX_KNOB_OVHD_AIRCOND_PACKFLOW_POSITION       LO = 0, NORM = 1, HI = 2

/*============================================================================*/
/*                                   ENGINES                                  */
/*============================================================================*/

    APU_FLAP_OPEN,          //"L:APU_FLAP_OPEN", "Percent"
    APU_N1,                 //"L:APU_N1","Percent"
    APU_EGT,                //"L:APU_EGT","celcius"
    APU_EGT_WARN,           //"L:APU_EGT_WARN","celcius"

/*============================================================================*/
/*                                     PRESS                                  */
/*============================================================================*/
    /*
    * ====== *
    * Valves *
    * ====== *
    */
    OUTFLOW_VALVE,          //"L:OUTFLOW_VALVE_PCT", "Percent"
    SAFETY_1,               //"L:SAFETY_VALVE_1", "Bool"
    SAFETY_2,               //"L:SAFETY_VALVE_2", "Bool"

    CABIN_ALTITUDE,         //"L:CABIN_ALTITUDE", "Feet"
    CABIN_ALTITUDE_GOAL,    //"L:CABIN_ALTITUDE_GOAL", "Feet"
    CABIN_ALTITUDE_RATE,    //"L:CABIN_ALTITUDE_RATE", "Feet per second"
    DELTA_PRESSURE,         //"L:DELTA_PRESSURE", "PSI"
    LDG_ELEV_MODE,          //"L:LANDING_ELEV_MODE", "Bool"
    LDG_ELEV,               //"L:LANDING_ELEV", "Feet"

    CPC_SYS1,               //"L:CPC_SYS1","Bool"
    CPC_SYS2,               //"L:CPC_SYS2","Bool"
    /*
    * ======== *
    * XML VARS *
    * ======== *
    */
    MAN_LAND_ELEV,          //L:A32NX_LANDING_ELEVATION, feet
    MAN_CAB_PRESS,          //L:A32NX_CAB_PRESS_MODE_MAN, bool
    MAN_VS_CTRL,            //L:A32NX_MAN_VS_CONTROL            up = 0, off = 1, dn = 2
    DITCH,                  //L:A32NX_DITCHING, Bool    

    totalLVarsCount
};


enum powerSource {
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

};

enum bleedSource {
    NOBLEED,

    GPU_BLEED,
    APU_BLEED,
    ENG1_BLEED,
    ENG2_BLEED,
    RAT_BLEED
};

enum ElECConf {
    NORM_CONF,
    ONE_GEN_INOP,
    PRE_RAT_EMER_CONF,
    RAT100_EMER_CONF,
    RAT50_EMER_CONF,
    RAT0_EMER_CONF,
    TR1_FAULT,
    TR2_FAULT,
    TR12_FAULT
};

enum ELECCircuit {
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
    CIRCUIT_704PP  //43
};

#endif // !DATAENUMS
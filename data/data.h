#ifndef DATA
#define DATA

#include <MSFS/Legacy/gauges.h>
#include <stdlib.h>
#include <vector>

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

#endif // !DATA
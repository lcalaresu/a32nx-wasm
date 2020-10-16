#ifndef DATA
#define DATA

#include "../common_sys.h"

#include "data_enums.h"
#include "data_strings.h"

/*
* =============== *
* DATA CRUD STUFF *
* =============== *
*/
void initUnitEnums();
void initLocalSimVarsIDs();
void updateGetLSimVars();
void updateSetLSimVars();
void updateASimVars();

ENUM* ENUM_UNITS;
ID* ID_LSIMVAR;

FLOAT64 aSimVarsValue[aSimVarsCount];
FLOAT64 lSimVarsValue[totalLVarsCount];
FLOAT64 lastLVarsValue[totalLVarsCount];

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
        lastLVarsValue[i] = -1;
        ID_LSIMVAR[i] = register_named_variable(pcstring_lSimVars[i]);
    }
}

void updateGetLSimVars() {
    for (int i = 0; i < totalLVarsCount; i++) {
        lSimVarsValue[i] = get_named_variable_value(ID_LSIMVAR[i]);
    }
}

void updateSetLSimVars() {
    //check for dirtyLVars
    for (int i = 0; i < totalLVarsCount; i++) {
        if (lastLVarsValue[i] != lSimVarsValue[i]) {
            dirtylSimVars.push_back(i);
        }
        lastLVarsValue[i] = lSimVarsValue[i];
    }
    //update only dirtyLVars
    for (auto i = dirtylSimVars.end(); i != dirtylSimVars.begin(); --i) {
        set_named_variable_value(ID_LSIMVAR[*i], lSimVarsValue[*i]);
        dirtylSimVars.pop_back();
    }
}

void updateASimVars() {
    for (int i = 0; i < aSimVarsCount; i++) {
        execute_calculator_code(pcstring_aSimVars[i], &aSimVarsValue[i], nullptr, nullptr);
    }
}

#endif // !DATA
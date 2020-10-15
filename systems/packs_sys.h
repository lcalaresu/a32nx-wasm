#pragma once
#include "../common_sys.h"


class Packs {
private:
public:
    void init() {

    }
    void update() {

    }
};

class Cabin {
private:
public:
    void init() {

    }
    void update() {

    }
};


class PacksSys {
private:
    Packs packsUnit;
    Cabin cabinUnit;
public:
    void init() {
        packsUnit.init();
        cabinUnit.init();
    }
    void update() {
        packsUnit.update();
        cabinUnit.update();
    }
};
#pragma once

#ifndef SYS_DEF
#define SYS_DEF

#include <MSFS/MSFS.h>
#include <MSFS/MSFS_Render.h>
#include <MSFS/Legacy/gauges.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <ratio>
#include <chrono>
#include <vector>

#include "utils/utils.h"

#include "data/data.h"

#define REFRESH_RATE 500        //update refresh rate in milliseconds
uint64_t lastAbsTime = 0;         //last time the update function was run
uint64_t deltaT = 0;

#define DEBUG

#endif

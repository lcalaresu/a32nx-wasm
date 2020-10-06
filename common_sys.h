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
#include <time.h>

#include "utils/utils.h"
#include "utils/PID.h"
#include "utils/bit_oper.h"

#include "data/data_enums.h"
#include "data/data_strings.h"
#include "data/data.h"

#define REFRESH_RATE 500        //update refresh rate in milliseconds
double lastAbsTime = 0;         //last time the update function was run

#endif

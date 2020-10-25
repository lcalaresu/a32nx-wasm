#include "common_sys.h"
#include "wasm_sys.h"
#include "simconnect/ServiceDef.h"

WasmSys WASM_SYS;
ServiceDef service;

bool initialized = false;
bool kill = false;

// Callbacks
extern "C" {
    MSFS_CALLBACK bool wasm_sys_gauge_callback(FsContext ctx, int service_id, void* pData)
    {
        debug_print("MSFS_CALLBACK initiated...");

        uint64_t currentAbsTime = timeSinceEpoch();
        if (!(initialized)) {
            if (service.handleSimConnect(service_id)) {
                debug_print("Service handled simconnect open with status: SUCCESS");
                lastAbsTime = currentAbsTime;
                WASM_SYS.init();
                //service.registerToEvents();
                initialized = true;
                debug_print("WASM_SYS initialized");
            }
        } else if (!kill){
            uint64_t lastRefresh = currentAbsTime - lastAbsTime;
            #ifdef DEBUG
            printf("WASM_SYS waiting till %lldms to update\n", REFRESH_RATE - lastRefresh);
            #endif
            if (lastRefresh >= REFRESH_RATE) {
                deltaT = currentAbsTime - lastAbsTime;
                debug_print("WASM_SYS now updating...");
                //service.handleSimDispatch();
                WASM_SYS.update(currentAbsTime);
                debug_print("WASM_SYS update completed.");
            }
        }
        kill = service.simStopCheck(service_id);
        if (kill) {
            debug_print("Service handle received KILL trigger...");
            WASM_SYS.destroy();
            service.handleSimDisconnect();
            debug_print("Service handled simconnect with status: FAILED/EXIT");
        }
        return !kill;
    }
}
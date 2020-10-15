#include "common_sys.h"
#include "wasm_sys.h"
#include "simconnect/ServiceDef.h"

WasmSys WASM_SYS;
ServiceDef service;

bool initialized = false;
bool kill = false;
struct timeval timestruct;

// Callbacks
extern "C" {
    MSFS_CALLBACK bool wasm_sys_gauge_callback(FsContext ctx, int service_id, void* pData)
    {
        debug_print("MSFS_CALLBACK initiated...");
        gettimeofday(&timestruct, 0);
        const double currentAbsTime = timestruct.tv_usec * 1000;
        if (!(initialized)) {
            if (service.handleSimConnect(ctx, service_id, pData)) {
                debug_print("Service handled simconnect open with status: SUCCESS");
                lastAbsTime = currentAbsTime;
                WASM_SYS.init();
                //service.registerToEvents();
                initialized = true;
                debug_print("WASM_SYS initialized");
            }
        } else {
            const double lastRefresh = currentAbsTime - lastAbsTime;
            #ifdef DEBUG
            printf("WASM_SYS waiting till %fms to update\n", REFRESH_RATE - lastRefresh);
            #endif
            if (lastRefresh >= REFRESH_RATE) {

                debug_print("WASM_SYS now updating...");

                //service.handleSimDispatch();
                WASM_SYS.update(currentAbsTime);

                debug_print("WASM_SYS update completed.");

            }
        }
        kill = service.simStopCheck();
        if (kill) {
            debug_print("Service handle received KILL trigger...");
            WASM_SYS.destroy();
            service.handleSimDisconnect();
        }
        debug_print("Service handled simconnect with status: FAILED/EXIT");
        return false;
    }
}
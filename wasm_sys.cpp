#include "common_sys.h"
#include "wasm_sys.h"
#include "simconnect/ServiceDef.h"

#include <ratio>
#include <chrono>
#include <sys/time.h>

WasmSys WASM_SYS;
ServiceDef service;

// Callbacks
extern "C" {
    MSFS_CALLBACK bool wasm_sys_gauge_callback(FsContext ctx, int service_id, void* pData)
    {
        printf("MSFS_CALLBACK initiated...\n wasm_sys fn call successful\n");

        if(service.handleSimConnect(ctx, service_id, pData)){
            printf("Service handled simconnect open with status: SUCCESS\n");
            bool initialized = false;
            bool kill = false;
            struct timeval time;

            while (!(kill)) {
                gettimeofday(&time, 0);
                const double currentAbsTime = time.tv_usec * 1000;
                
                if (!(initialized)) {
                    printf("WASM_SYS initialized\n");
                    lastAbsTime = currentAbsTime;
                    WASM_SYS.init();
                    //service.registerToEvents();
                    initialized = true;
                } else {
                    const double lastRefresh = currentAbsTime - lastAbsTime;
                    printf("WASM_SYS waiting till %fms to update\n", REFRESH_RATE - lastRefresh);
                    
                    if (lastRefresh >= REFRESH_RATE) {
                        printf("WASM_SYS now updating...\n");
                        //service.handleSimDispatch();
                        WASM_SYS.update(currentAbsTime);
                        printf("WASM_SYS update completed.\n");
                    }
                }
                kill = service.simStopCheck();
                
                if (kill) {
                    printf("Service handle received KILL trigger...\n");
                    WASM_SYS.destroy();
                    service.handleSimDisconnect();
                }
            }
        }
        printf("Service handled simconnect with status: FAILED/EXIT");
        return false;
    }
}
#include "systems/elec_sys.h"
#include "systems/packs_sys.h"
#include "systems/bleed_sys.h"
#include "systems/press_sys.h"
#include "systems/eng_sys.h"

#include "common_sys.h"
#include <SimConnect.h>
#include <ratio>
#include <chrono>
#include <sys/time.h>

class WasmSys {
    private:
        ElecSys ELEC_SYSTEM;
        PacksSys PACK_SYSTEM;
        BleedSys BLEED_SYSTEM;
        PressSys PRESS_SYSTEM;
        EngSys ENG_SYSTEM;
    public:
        void init() {
            initUnitEnums();
            initLocalSimVarsIDs();
            srand(time(nullptr));
            ELEC_SYSTEM.init();
            PACK_SYSTEM.init();
            BLEED_SYSTEM.init();
            PRESS_SYSTEM.init();
            ENG_SYSTEM.init();
        }
        void update(double const currentAbsTime) {
            updateASimVars();

            ELEC_SYSTEM.update(currentAbsTime);
            PACK_SYSTEM.update();
            BLEED_SYSTEM.update(currentAbsTime);
            PRESS_SYSTEM.update();
            ENG_SYSTEM.update(currentAbsTime);

            ELEC_SYSTEM.updateSimVars();
            PACK_SYSTEM.updateSimVars();
            BLEED_SYSTEM.updateSimVars();
            PRESS_SYSTEM.updateSimVars();
            ENG_SYSTEM.updateSimVars();

            lastAbsTime = currentAbsTime;
        }
        void destroy() {
            unregister_all_named_vars();
            free(ENUM_UNITS);
            free(ID_LSIMVAR);
        }
}WASM_SYS;

class ServiceDef {
private:
    HANDLE hSimConnect = 0;
    const char* stopState = "SimStop";
    SIMCONNECT_DATA_REQUEST_ID  RequestID;
public:
    bool handleSimConnect(FsContext ctx, int service_id, void* pData) {
        switch (service_id)
        {
        case PANEL_SERVICE_PRE_INSTALL: {
            HRESULT hr = SimConnect_Open(&hSimConnect, "A32NX_wasm_sys", nullptr, 0, 0, 0);
            if (SUCCEEDED(hr)) {
                return true;
            }
            return false;
        }
        default:
            return true;
        }
    }
    bool simStopCheck() {
        HRESULT kill = SimConnect_RequestSystemState(hSimConnect, RequestID, stopState);
        if (SUCCEEDED(kill)) {
            return true;
        }
        return false;
    }
    bool handleSimDisconnect() {
        WASM_SYS.destroy();
        HRESULT hr = SimConnect_Close(hSimConnect);
        if (SUCCEEDED(hr)) {
            return true;
        }
        return false;
    }
}service;

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
                    initialized = true;
                } else {
                    const double lastRefresh = currentAbsTime - lastAbsTime;
                    printf("WASM_SYS waiting till %dms to update\n", REFRESH_RATE - lastRefresh);
                    if (lastRefresh >= REFRESH_RATE) {
                        printf("WASM_SYS now updating...\n");
                        WASM_SYS.update(currentAbsTime);
                        printf("WASM_SYS updated completed.\n");
                    }
                }
                kill = service.simStopCheck();
                if (kill) {
                    printf("Service handle received KILL trigger...\n");
                    service.handleSimDisconnect();
                }
            }
        }
        printf("Service handled simconnect with status: FAILED/EXIT");
        return false;
    }
}
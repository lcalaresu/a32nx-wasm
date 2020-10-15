#pragma once
#ifndef SERVICEDEF
#define SERVICEDEF

#include "../common_sys.h"
#include <SimConnect.h>

class ServiceDef {
private:
    HANDLE hSimConnect = 0;
    const char* stopState = "SimStop";
    SIMCONNECT_DATA_REQUEST_ID  RequestID;

    static void executeEventRequest(ENUM eventID, DWORD data) {
        lSimVarsValue[eventID] = data;
    }
    static void CALLBACK DispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext) {
        switch (pData->dwID) {
            case SIMCONNECT_RECV_ID_EVENT: {
                SIMCONNECT_RECV_EVENT* evt = (SIMCONNECT_RECV_EVENT*)pData;
                executeEventRequest((ENUM)evt->uEventID, evt->dwData);
            }
            default: {
                break;
            }
        }
    }
public:
    bool handleSimConnect(FsContext ctx, int service_id, void* pData) {
        switch (service_id) {
            case PANEL_SERVICE_PRE_INSTALL: {
                HRESULT hr = SimConnect_Open(&hSimConnect, "A32NX_wasm_sys", nullptr, 0, 0, 0);
                if (SUCCEEDED(hr)) {
                    return true;
                }
                return false;
            }
            default: {
                return true;
            }
        }
    }

    bool registerToEvents() {
        //this listens to the event, masks the event after receiving it
        //a single notification group can at max hold 1000events
        //all third party applications should refer to events by adding THIRD_PARTY_EVENT_ID_MIN to the enum definitions provided under data.h for lVars
        HRESULT Result = 1;
        for (int i = BATT1_ONLINE; i < totalLVarsCount; i++) {
            HRESULT hr = SUCCEEDED(SimConnect_AddClientEventToNotificationGroup(hSimConnect, SDK_CONTROL, THIRD_PARTY_EVENT_ID_MIN + i, 1));
            
            Result &= hr;
            if (!(Result)) {
                return false;
            }
        }
        HRESULT hr = SUCCEEDED(SimConnect_SetNotificationGroupPriority(hSimConnect, SDK_CONTROL, SIMCONNECT_GROUP_PRIORITY_HIGHEST));
        if(hr){
            return true;
        }
        return false;
    }

    void handleSimDispatch() {
        SimConnect_CallDispatch(hSimConnect, DispatchProc, nullptr);
    }

    bool simStopCheck() {
        HRESULT kill = SimConnect_RequestSystemState(hSimConnect, RequestID, stopState);
        if (SUCCEEDED(kill)) {
            return true;
        }
        return false;
    }

    bool handleSimDisconnect() {
        HRESULT hr = SimConnect_Close(hSimConnect);
        if (SUCCEEDED(hr)) {
            return true;
        }
        return false;
    }
};
#endif
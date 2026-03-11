#include "SerialUtils.h"
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>

std::vector<SerialPortInfo> SerialUtils::GetSerialPorts() {
    std::vector<SerialPortInfo> ports;

    HDEVINFO hDevInfo = SetupDiGetClassDevs(
        &GUID_DEVCLASS_PORTS, nullptr, nullptr,
        DIGCF_PRESENT);

    if (hDevInfo == INVALID_HANDLE_VALUE)
        return ports;

    SP_DEVINFO_DATA devInfoData = {};
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++) {
        // デバイス名取得
        char desc[256] = {};
        SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData,
            SPDRP_FRIENDLYNAME, nullptr, (PBYTE)desc, sizeof(desc), nullptr);

        // レジストリからCOMポート名取得
        HKEY hKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData,
            DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);

        if (hKey != INVALID_HANDLE_VALUE) {
            char port[32] = {};
            DWORD size = sizeof(port);
            RegQueryValueExA(hKey, "PortName", nullptr, nullptr, (LPBYTE)port, &size);
            RegCloseKey(hKey);

            if (strncmp(port, "COM", 3) == 0) {
                ports.push_back({ port, desc });
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return ports;
}
#include <rtthread.h>

#include "SerialPort.h"
#include "SerialPort_log.h"

static void initAtClientDevice(const char * atDeviceName);

static void initAtClientDevice(const char * atDeviceName) {
    struct rt_device* atClientDevice = RT_NULL;
    atClientDevice = rt_device_find(atDeviceName);
    if (atClientDevice == RT_NULL) {
        LOG_I("Null device %s.", atDeviceName);
    } else {
        LOG_I("Find device %s.", atDeviceName);
        int ret = -1;
        switch(ret) {
            case 0:
                LOG_I("initialize at_client with %s success.", atDeviceName);
                break;
            case -1:
                LOG_I("initialize at_client with %s failed.", atDeviceName);
                break;
            case -5:
                LOG_I("initialize at_client with %s no memory.", atDeviceName);
                break;
            default:
                LOG_I("initialize at_client with %s failed :%d.", atDeviceName, ret);
                break;
        }
    }
}

int init_at_cli_dev(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        LOG_I("argv[%d] = %s", i, argv[i]);
    }

    if (argc > 1) {
        if (!strcmp("uart1", argv[1])) {
            initAtClientDevice("uart1");
        } else if (!strcmp("uart2", argv[1])) {
            initAtClientDevice("uart2");
        } else if (!strcmp("uart6", argv[1])) {
            initAtClientDevice("uart6");
        }
    }

    return 0;
}

MSH_CMD_EXPORT(init_at_cli_dev, init at client device)

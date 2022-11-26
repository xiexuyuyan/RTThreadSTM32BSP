#include "SerialPort.h"
#include "SerialPortService.h"
#include "SerialPortService_log.h"

#include <rtthread.h>

extern rt_device_t serialPort;

#define THREAD_NAME "SerialPortService"
#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 512
#define THREAD_TIMESLICE 5

static rt_thread_t serialPortService = RT_NULL;
static void (*serialPortRunnable)(void* parameter);

static void runEntry(void* parameter) {
    int count = 0;
    char buff[64];
    buff[63] = '\0';
    while (++count) {
        rt_device_write(serialPort, 0, (
            rt_sprintf(buff, "%s%d\r\n\0"
                , "count SerialPort: ", count)
            , buff), strlen(buff));

        rt_thread_mdelay(1000);
    }
}


static void runLoopBackTestEntry(void* parameter) {
    char buff[10];
    while (1) {
        for (int i = 0; i < 10; i++) {
            buff[i] = '\0';
        }

        rt_size_t readLen = readInner(buff, 0, 9);
        LOG_I("str: %s, readLen: %d", buff, readLen);
    }
}

int startSerialPortService() {
    LOG_I("testThread");

    serialPortRunnable = runLoopBackTestEntry;
    serialPortService = rt_thread_create(
            THREAD_NAME
            , serialPortRunnable, RT_NULL
            , THREAD_STACK_SIZE
            , THREAD_PRIORITY, THREAD_TIMESLICE);
    if (serialPortService != RT_NULL) {
        LOG_I("create SerialPortService success");
        rt_thread_startup(serialPortService);
    } else {
        LOG_I("create SerialPortService failed");
    }

    return -1;
}

int serial_start(int argc, char** argv) {
    startSerialPortService();
    return -1;
}

MSH_CMD_EXPORT(serial_start, start SerialPort thread);

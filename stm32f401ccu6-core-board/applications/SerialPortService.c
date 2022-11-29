#include "SerialPort.h"
#include "SerialPortService.h"
#include "SerialPortService_log.h"

#include <rtthread.h>

#define THREAD_NAME "SerialPortService"
#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 512
#define THREAD_TIMESLICE 5

static rt_thread_t serialPortService = RT_NULL;
static void (*serialPortRunnable)(void* parameter);

static void runLoopBackTestEntry(void* parameter) {
    rt_size_t MAX_READ = 128;
    char buff[MAX_READ];
    while (1) {
        for (int i = 0; i < MAX_READ; i++) {
            buff[i] = '\0';
        }

        rt_size_t readLen = readInner(buff, 0, MAX_READ-1, RT_WAITING_FOREVER);
        LOG_I("str: [\n%s\n], readLen: %d", buff, readLen);
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

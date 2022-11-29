#include <rtthread.h>
#include "Utils.h"
#include "SerialPort.h"
#include "SerialPort_log.h"

#ifdef DBG_TAG
#undef DBG_TAG
#define DBG_TAG "AT_CIFSR"
#endif

static int parseCIFSR(const char* src, const int lenSrc) {
    int crList[10];
    memset(crList, 0, 10);

    int crCount = split(src, lenSrc, crList);

    char buff[64];
    for (int i = 0; i < crCount-1; i++) {
        memset(buff, 0, 64);
        arraycopy(src, crList[i]+1, buff, 0, crList[i+1] - crList[i] - 2);
        // LOG_I("buff = [%s]\n", buff);
    }

    if (crCount == 0) {
        return -1;
    }

    memset(buff, 0, 64);
    arraycopy(src, 0, buff, 0, crList[0] - 2);
    if (0 != strcmp(buff, "AT+CIFSR")) {
        return -2;
    }

    memset(buff, 0, 64);
    arraycopy(src, crList[crCount-2]+1, buff, 0, crList[crCount-1] - crList[crCount-2] - 2);
    if (0 != strcmp(buff, "OK")) {
        return -3;
    }

    memset(buff, 0, 64);
    arraycopy(src, crList[0]+1, buff, 0, crList[1] - crList[0] - 2);
    if (0 == strcmp(buff, "+CIFSR:STAIP,\"0.0.0.0\"")) {
        return -4;
    }

    return 0;
}


int atCIFSR() {
    rt_device_t atClient = rt_device_find("uart1");

    rt_size_t MAX_READ = 128;
    char buff[MAX_READ];

    char buffCIFSR[64] = "AT+CIFSR\r\n";
    rt_device_write(atClient, -1, buffCIFSR, strlen(buffCIFSR));

    memset(buff, 0, MAX_READ);
    rt_size_t readLen = 0;
    while (1) {
        rt_size_t tReadLen = readInner(
                buff, readLen, MAX_READ - 1 - readLen, 500);
        if (tReadLen == 0) {
            break;
        } else {
            readLen += tReadLen;
        }
    }

    LOG_I("read len = %d, str = [\n%s\n]", readLen, buff);

    int ret = parseCIFSR(buff, strlen(buff));
    LOG_I("atCIFSR parse ret = %d", ret);

    return ret;
}

MSH_CMD_EXPORT(atCIFSR, at CIFSR);

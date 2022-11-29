#include <rtthread.h>
#include "Utils.h"
#include "SerialPort.h"
#include "SerialPort_log.h"

#ifdef DBG_TAG
#undef DBG_TAG
#define DBG_TAG "AT_SUBSCRIBE"
#endif


static void exitCIPSEND() {
    rt_device_t atClient = rt_device_find("uart1");
    char buffCIPSEND_FINISH[4] = "+++";
    rt_device_write(atClient, -1,
                    buffCIPSEND_FINISH, strlen(buffCIPSEND_FINISH));
    rt_thread_mdelay(100);
}

int atSubscribe() {
    int err = 0;

    rt_device_t atClient = rt_device_find("uart1");

    rt_size_t MAX_READ = 128;
    char buff[MAX_READ];

    char buffCIPSEND[64] = "AT+CIPSEND\r\n";
    rt_device_write(atClient, -1, buffCIPSEND, strlen(buffCIPSEND));

    {
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

        if (0 != strcmp("AT+CIPSEND\r\r\n\r\nOK\r\n\r\n>", buff)) {
            err = -1;
            if (buff[readLen-1] == '>') {
                exitCIPSEND();
            }
            LOG_E("atSubscribe CIPSEND not OK");
            return err;
        }
    }



    {
        char buffSubscribe[128] =
                "cmd=1&uid=f56c42717ac6d4ed32da64de41d6fd9a"
                "&topic=NightLed002\r\n";
        rt_device_write(atClient, -1,
                        buffSubscribe, strlen(buffSubscribe));
        memset(buff, 0, MAX_READ);
        rt_size_t readLen = 0;
        while (1) {
            rt_size_t tReadLen = readInner(
                    buff, readLen, MAX_READ - 1 - readLen, 5000);
            if (tReadLen == 0) {
                break;
            } else {
                readLen += tReadLen;
            }
        }
        LOG_I("read len = %d, str = [\n%s\n]", readLen, buff);

        if (0 != strcmp("cmd=1&res=1\r\n", buff)) {
            err = -1;
            LOG_E("Subscribe %s failed", buffSubscribe);
        } else {
            LOG_I("Subscribe %s Success", buffSubscribe);
        }
    }

    exitCIPSEND();

    return err;
}

MSH_CMD_EXPORT(atSubscribe, at subscribe);

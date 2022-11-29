#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "Utils.h"
#include "SerialPort.h"
#include "SerialPort_log.h"

#ifdef DBG_TAG
#undef DBG_TAG
#define DBG_TAG "AT_BOOTUP"
#endif


extern int atCIFSR();
extern int atCIPMODE();
extern int atCIPSTART();
extern int atSubscribe();

static void exitCIPSEND() {
    rt_device_t atClient = rt_device_find("uart1");
    char buffCIPSEND_FINISH[4] = "+++";
    rt_device_write(atClient, -1,
                    buffCIPSEND_FINISH, strlen(buffCIPSEND_FINISH));
    rt_thread_mdelay(100);
}

int at_ping() {
    int err = 0;
    rt_device_t atClient = rt_device_find("uart1");

    rt_size_t MAX_READ = 128;
    char buff[MAX_READ];

    if (0 != atCIPMODE()) {
        err = -1;
        return err;
    }

    {
        char buffCIPSEND[64] = "AT+CIPSEND\r\n";
        rt_device_write(atClient, -1, buffCIPSEND, strlen(buffCIPSEND));

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
            LOG_E("ping CIPSEND not OK");
            return err;
        }
    }

    {
        char buffCIPSEND[64] = "cmd=0&msg=ping\r\n";
        rt_device_write(atClient, -1, buffCIPSEND, strlen(buffCIPSEND));

        memset(buff, 0, MAX_READ);
        rt_size_t readLen = 0;
        while (1) {
            LOG_I("Blocking before read ping max 5000ms");
            rt_size_t tReadLen = readInner(
                    buff, readLen, MAX_READ - 1 - readLen, 5000);
            if (tReadLen == 0) {
                break;
            } else {
                readLen += tReadLen;
            }
        }
        LOG_I("at_ping read len = %d, str = [\n%s\n]", readLen, buff);
    }

    exitCIPSEND();

    return 0;
}

#define LED0_PIN    GET_PIN(C, 13)
#define LED1_PIN    GET_PIN(A, 1)
void handleMSG(char* buffRead) {
    LOG_I("handle msg %s", buffRead);

    char buffRead_T_Subscribe[128];
    memset(buffRead_T_Subscribe, '\0', 128);
    // cmd=2&uid=f56c42717ac6d4ed32da64de41d6fd9a&topic=TestLed002&msg=12
    const int MAGIC = 65;
    arraycopy(buffRead, 0, buffRead_T_Subscribe, 0, MAGIC);
    if (0 == strcmp("cmd=2&uid=f56c42717ac6d4ed32da64de41d6fd9a&topic=NightLed002&msg=", buffRead_T_Subscribe)) {
        memset(buffRead_T_Subscribe, '\0', 128);
        arraycopy(buffRead, MAGIC, buffRead_T_Subscribe, 0, 3);
        if (0 == strcmp("on\r", buffRead_T_Subscribe)) {
            LOG_I("The light on\n");
            rt_pin_write(LED1_PIN, PIN_HIGH);
            rt_pin_write(LED0_PIN, PIN_LOW);
        } else if (0 == strcmp("off", buffRead_T_Subscribe)) {
            LOG_I("The light off\n");
            rt_pin_write(LED1_PIN, PIN_LOW);
            rt_pin_write(LED0_PIN, PIN_HIGH);
        } else if (0 == strcmp("on#", buffRead_T_Subscribe)) {
            int degree = 0;
            memset(buffRead_T_Subscribe, '\0', 128);
            arraycopy(buffRead, MAGIC+3, buffRead_T_Subscribe, 0, 3);
            if ('\r' == buffRead_T_Subscribe[1]) {
                degree = buffRead_T_Subscribe[0] - '0';
            } else if (0 == strcmp("100", buffRead_T_Subscribe)) {
                degree = 100;
            } else if ('\r' == buffRead_T_Subscribe[2]) {
                int dec_10 = buffRead_T_Subscribe[0] - '0';
                int dec_1 = buffRead_T_Subscribe[1] - '0';
                degree = dec_10 * 10 + dec_1;
            }

            LOG_I("The light on, level = %d\n", degree);
        }
    }
}

static void atStartUp(void* parameter) {
    if (0 != atCIFSR()) {
        return;
    }

    LOG_I("------------------------------------------------------------");

    if (0 != atCIPMODE()) {
        return;
    }

    LOG_I("------------------------------------------------------------");

    if (0 != atCIPSTART()) {
        return;
    }

    LOG_I("------------------------------------------------------------");

    if (0 != atSubscribe()) {
        return;
    }


    LOG_I("------------------------------------------------------------");

    {
        rt_size_t MAX_READ = 128;
        char buff[MAX_READ];

        while (1) {
            if (0 != at_ping()) return;
            LOG_I("------------------------------------------------------------");

            memset(buff, 0, MAX_READ);
            rt_size_t readLen = 0;
            rt_size_t tReadLen = readInner(
                    buff, readLen, MAX_READ - 1, 45 * 1000);
            readLen += tReadLen;
            if (tReadLen == 0) continue;
            while (1) {
                tReadLen = readInner(
                        buff, readLen, MAX_READ - 1 - readLen, 100);

                if (tReadLen == 0) {
                    break;
                } else {
                    readLen += tReadLen;
                }
            }

            LOG_I("read len = %d, str = [\n%s\n]", readLen, buff);
            handleMSG(buff);
        }
    }
}

#define THREAD_NAME "ATBootup"
#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 2048
#define THREAD_TIMESLICE 5

static rt_thread_t atSubscribeService = RT_NULL;
static void (*atSubScribeRunnable)(void* parameter);

int startAtSubscribeService() {
    LOG_I("at subscribe thread");

    atSubScribeRunnable = atStartUp;
    atSubscribeService = rt_thread_create(
            THREAD_NAME
            , atSubScribeRunnable, RT_NULL
            , THREAD_STACK_SIZE
            , THREAD_PRIORITY, THREAD_TIMESLICE);
    if (atSubscribeService != RT_NULL) {
        LOG_I("create ATSubscribeService success");
        rt_thread_startup(atSubscribeService);
    } else {
        LOG_I("create ATSubscribeService failed");
    }

    return -1;
}

int serial_at_bootup(int argc, char** argv) {
    startAtSubscribeService();
    return -1;
}

MSH_CMD_EXPORT(serial_at_bootup, at start up);

void serial_ping() {
    at_ping();
}

MSH_CMD_EXPORT(serial_ping, serial at ping to bemfa);

#include <rtthread.h>
#include <rthw.h>

#include "SerialPort.h"
#include "SerialPort_log.h"

static rt_device_t serialPort = RT_NULL;
static struct rt_semaphore rx_sem;

/**
 * Reads the next byte of data from input stream.
 * The value byte is returned as an int in the range
 * 0 to 255. If no byte is available because the end
 * of the stream has reached, the value -1 is returned.
 * This method blocks until input data is available,
 * the end of the stream is detected {@link #END_OF_STREAM},
 * or an exception is thrown {@link #EXCEPTION_THROWN}.
 * Copy from jdk 8.
 * @param last the last status of read_byte
 * @return int the range 0 to 255
 */
static int read_byte(enum LAST_READ_STAT last, rt_int32_t timeout) {
    char ch = '\0';
    if (last == END_OF_STREAM) {
        rt_sem_take(&rx_sem, timeout);
    } else {
        if (rt_sem_trytake(&rx_sem) != RT_EOK) {
            return END_OF_STREAM;
        }
    }

    if (rt_device_read(serialPort, -1, &ch, 1) == 0) {
        return EXCEPTION_THROWN;
    }

    return ch;
}

static rt_size_t read_inner(char* buff, rt_size_t off, rt_size_t size, rt_int32_t timeout) {
    int last = END_OF_STREAM;
    rt_size_t count = 0;

    while (1) {
        last = read_byte(last, timeout);

        if (last == END_OF_STREAM || last == EXCEPTION_THROWN) {
            return count;
        }

        buff[off + count++] = last;

        if (count > size) break;
    }

    return count;
}

rt_size_t readInner(char* buff, rt_size_t off, rt_size_t size, rt_int32_t timeout) {
    if (buff == NULL) {
        return 0;
    }

    return read_inner(buff, off, size, timeout);
}

static rt_err_t onSerialPortRXIndicate(rt_device_t dev, rt_size_t len) {
    rt_sem_release(&rx_sem);
    return RT_EOK;
}

int initSerialPort(const char* name) {
    serialPort = rt_device_find(name);

    if (rt_device_open(
            serialPort, RT_DEVICE_FLAG_INT_RX) != RT_EOK) {
        return -1;
    }

    rt_sem_init(&rx_sem, "srx_sem", 0, RT_IPC_FLAG_FIFO);

    if (rt_device_set_rx_indicate(
            serialPort, onSerialPortRXIndicate) != RT_EOK) {
        return -1;
    }

    return 0;
}

int serial_init(int argc, char** argv) {
    int ret = -1;

    if (argc < 2) {
        LOG_E("init serial port failed: Please set device name.");
        return ret;
    }

    if (!strcmp("uart1", argv[1])) {
        ret = initSerialPort("uart1");
    } else if (!strcmp("uart2", argv[1])) {
        ret = initSerialPort("uart2");
    } else if (!strcmp("uart6", argv[1])) {
        ret = initSerialPort("uart6");
    } else {
        LOG_E("init serial port failed: name(%s).", argv[1]);
        return ret;
    }


    if (ret != 0) {
        LOG_E("init serial port failed: %d.", ret);
    } else {
        LOG_I("init serial port success.");
    }

    return ret;
}

MSH_CMD_EXPORT(serial_init, init serial port with uart2)

int serial_send(int argc, char** argv) {
    if (argc < 2) {
        return -1;
    }

    char buff[20];
    for (int i = 0; i < 20; i++) {
        buff[i] = '\0';
    }

    rt_sprintf(buff, "%s", argv[1]);

    rt_device_write(serialPort, -1, buff, strlen(buff));

    LOG_I("write(len = %d): %s.", strlen(buff), buff);

    return 0;
}

MSH_CMD_EXPORT(serial_send, send cmd to serial port);

int serial_sendln(int argc, char** argv) {
    if (argc < 2) {
        return -1;
    }

    char buff[20];
    for (int i = 0; i < 20; i++) {
        buff[i] = '\0';
    }

    rt_sprintf(buff, "%s\r\n", argv[1]);

    rt_device_write(serialPort, -1, buff, strlen(buff));

    LOG_I("write(len = %d): %s.", strlen(buff), buff);

    return 0;
}

MSH_CMD_EXPORT(serial_sendln, send cmd to serial port with ln);

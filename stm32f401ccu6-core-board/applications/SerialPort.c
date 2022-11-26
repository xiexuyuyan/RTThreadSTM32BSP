#include <rtthread.h>

#include "SerialPort.h"
#include "SerialPort_log.h"

rt_device_t serialPort = RT_NULL;
struct rt_semaphore rx_sem;

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
static int read_byte(enum LAST_READ_STAT last) {
    char ch = '\0';
    if (last == END_OF_STREAM) {
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
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

static rt_size_t read_inner(char* buff, rt_size_t off, rt_size_t size) {
    int last = END_OF_STREAM;
    rt_size_t count = 0;

    while (1) {
        last = read_byte(last);

        if (last == END_OF_STREAM || last == EXCEPTION_THROWN) {
            return count;
        }

        buff[off + count++] = last;

        if (count > size) break;
    }

    return count;
}

rt_size_t readInner(char* buff, rt_size_t off, rt_size_t size) {
    if (buff == NULL) {
        return 0;
    }

    return read_inner(buff, off, size);
}

static rt_err_t onSerialPortTXComplete(rt_device_t dev, void* buffer) {
    char* name = dev->parent.name;
    LOG_I("onSerialPortTXComplete: dev %s", name);
    return RT_EOK;
}

static rt_err_t onSerialPortRXIndicate(rt_device_t dev, rt_size_t len) {
    char* name = dev->parent.name;
    // LOG_I("onSerialPortRXIndicate: dev %s, len %d", name, len);
    rt_sem_release(&rx_sem);
    return RT_EOK;
}

int initSerialPort(const char* name) {
    serialPort = rt_device_find(name);

    if (rt_device_open(
            serialPort, RT_DEVICE_FLAG_INT_RX) != RT_EOK) {
        return -1;
    }

    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);

    if (rt_device_set_tx_complete(
            serialPort, onSerialPortTXComplete) != RT_EOK) {
        return -1;
    }

    if (rt_device_set_rx_indicate(
            serialPort, onSerialPortRXIndicate) != RT_EOK) {
        return -1;
    }

    return 0;
}
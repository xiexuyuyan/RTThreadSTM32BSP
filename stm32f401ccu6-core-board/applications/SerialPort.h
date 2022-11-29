#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <rtdef.h>

enum LAST_READ_STAT{
    END_OF_STREAM = -1,
    EXCEPTION_THROWN = -2
};

int initSerialPort(const char* name);
rt_size_t readInner(char* buff, rt_size_t off, rt_size_t size, rt_int32_t timeout);

#endif // SERIAL_PORT_H
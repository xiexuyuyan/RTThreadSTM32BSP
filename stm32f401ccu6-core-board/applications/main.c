/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define LOG_TAG "main"
#include "main_log.h"

/* defined the LED0 pin: PC13 */
#define LED0_PIN    GET_PIN(C, 13)
/* defined the KEY0 pin: PA0 */
#define KEY0_PIN    GET_PIN(A, 0)


static void keyEventCallback(void* args) {
    int before = rt_pin_read(LED0_PIN);
    rt_pin_write(LED0_PIN, (before == PIN_LOW ? PIN_HIGH : PIN_LOW));
}

int main(void)
{
    int count = 1;
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED0_PIN, PIN_HIGH);

    rt_pin_mode(KEY0_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(
            KEY0_PIN, PIN_IRQ_MODE_FALLING
            , keyEventCallback, (void*)RT_NULL);
    rt_pin_irq_enable(KEY0_PIN, PIN_IRQ_ENABLE);

    rt_device_t vcom = rt_device_find("vcom");
    char buff[15];
    if (rt_device_open(
            vcom, RT_DEVICE_OFLAG_RDWR
            | RT_DEVICE_FLAG_INT_RX
            | RT_DEVICE_FLAG_STREAM) != RT_EOK) {
        rt_pin_write(LED0_PIN, PIN_LOW);
        return -1;
    }

    char* const waiting = "waiting...";
    char* s = waiting;
    char* d = buff;
    buff[14] = '\0';
    while (*d++ = *s++);

    while (count++)
    {
        rt_thread_mdelay(1000);

        if (count < 4) {
            rt_device_write(vcom, 0, (rt_sprintf(buff, "%s%d%c", waiting, count, '\n'), buff), strlen(buff));
        } else if (count == 4) {
            rt_device_write(vcom, 0, (rt_sprintf(buff, "%s%c", "last second", '\n'), buff), strlen(buff));
            rt_device_close(vcom);
        } else if (count == 5) {
            rt_console_set_device("vcom");
            finsh_set_device("vcom");
        }
    }

    return RT_EOK;
}


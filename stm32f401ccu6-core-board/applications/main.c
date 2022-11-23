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

    while (count++)
    {
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}


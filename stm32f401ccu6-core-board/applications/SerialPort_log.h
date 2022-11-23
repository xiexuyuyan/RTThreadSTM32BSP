/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-10-12     armink       first version
 * 2022-11-22     Coder Wang   first version
 */

/*
 * NOTE: DO NOT include this file on the header file.
 */

#ifndef LOG_TAG
#define DBG_TAG               "SerialPort"
#else
#define DBG_TAG               LOG_TAG
#endif /* LOG_TAG */

#ifdef MAIN_DEBUG
#define DBG_LVL               DBG_LOG
#else
#define DBG_LVL               DBG_INFO
#endif /* MAIN_DEBUG */

#include <rtdbg.h>

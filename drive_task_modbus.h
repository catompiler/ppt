/**
 * @file drive_task_modbus.h Задача Modbus.
 */

#ifndef DRIVE_TASK_MODBUS_H
#define DRIVE_TASK_MODBUS_H

#include "errors/errors.h"
#include <stdint.h>
#include <stdbool.h>
#include "modbus/modbus_rtu.h"
#include <FreeRTOS.h>


//! Тип функции установки интерфейса rs485 на выход.
typedef void (*rs485_set_output_proc_t)(void);
//! Тип функции установки интерфейса rs485 на вход.
typedef void (*rs485_set_input_proc_t)(void);


/**
 * Инициалиазирует задачу Modbus.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_modbus_init(uint32_t priority);

/**
 * Устанавливает данные задачи Modbus.
 * @param rs485_set_out Функция установки rs485 на выход.
 * @param rs485_set_in Функция установки rs485 на вход.
 */
extern void drive_task_modbus_setup(rs485_set_output_proc_t rs485_set_out,
                                    rs485_set_input_proc_t rs485_set_in);

/**
 * Выполняет обработку полученного сообщения.
 * @param modbus Интерфейс Modbus.
 * @param is485 Флаг 485го интерфейса.
 * @param pxHigherPriorityTaskWoken Флаг необходимости переключения контекста.
 * @return Флаг необходимости переключения контекста.
 */
extern err_t drive_task_modbus_process_isr(modbus_rtu_t* modbus, bool is485, BaseType_t* pxHigherPriorityTaskWoken);


#endif /* DRIVE_TASK_MODBUS_H */


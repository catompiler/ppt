/**
 * @file drive_task_ui.h Задача интерфейса.
 */

#ifndef DRIVE_TASK_UI_H
#define DRIVE_TASK_UI_H

#include "errors/errors.h"
#include <stdint.h>


/**
 * Инициалиазирует задачу интерфейса.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_ui_init(uint32_t priority);

#endif /* DRIVE_TASK_UI_H */


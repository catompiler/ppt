/**
 * @file drive_task_selftune.h Задача обработки данных самонастройки.
 */

#ifndef DRIVE_TASK_SELFTUNE_H
#define DRIVE_TASK_SELFTUNE_H

#include "errors/errors.h"
#include <stdint.h>
#include <stdbool.h>
#include "future/future.h"


/**
 * Инициалиазирует задачу обработки данных самонастройки.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_selftune_init(uint32_t priority);

/**
 * Обрабатывает текущие собранные данных самонастройки.
 * Результат будущего - флаг обсчёта текущих данных.
 * @param future Будущее.
 * @return Флаг отправки команды для расчёта.
 */
extern bool drive_task_selftune_calc_data(future_t* future);

/**
 * Вычисляет R и L по всем собранным данным.
 * Результат будущего - флаг обсчёта данных.
 * @param future Будущее.
 * @return Флаг отправки команды для расчёта.
 */
extern bool drive_task_selftune_calc(future_t* future);



#endif /* DRIVE_TASK_SELFTUNE_H */


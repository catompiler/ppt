/**
 * @file drive_tasks.h Библиотека задач привода.
 */

#ifndef DRIVE_TASKS_H
#define DRIVE_TASKS_H

#include "errors/errors.h"
#include "future/future.h"
#include "drive_events.h"
#include "drive_task_i2c_watchdog.h"
#include "drive_task_buzz.h"
#include "drive_task_temp.h"
#include "drive_task_ui.h"
#include "drive_task_utils.h"
#include "drive_task_storage.h"
#include "drive_task_main.h"
#include "drive_task_triacs.h"
#include "drive_task_sync.h"
#include "drive_task_adc.h"
#include "drive_task_modbus.h"
#include "drive_task_selftune.h"


/**
 * Инициализирует задачи привода.
 * @return Код ошибки.
 */
extern err_t drive_tasks_init(void);

/**
 * Обновляет настройки.
 * @return Код ошибки.
 */
extern err_t drive_tasks_apply_settings(void);

/**
 * Сохраняет настройки.
 * @return Код ошибки.
 */
extern err_t drive_tasks_save_settings(void);

/**
 * Записывает событие.
 * ошибки привода и осциллограмму.
 * @return Код ошибки.
 */
extern err_t drive_tasks_write_error_event(void);

/**
 * Записывает событие.
 * состояния привода и осциллограмму.
 * @return Код ошибки.
 */
extern err_t drive_tasks_write_status_event(void);

/**
 * Записывает событие.
 * предупреждения привода без осциллограммы.
 * @return Код ошибки.
 */
extern err_t drive_tasks_write_warning_event(void);

/**
 * Читает событие из eeprom.
 * @param future Будущее.
 * @param event_index Индекс события.
 * @param event Событие.
 * @return Код ошибки.
 */
extern err_t drive_tasks_read_event(future_t* future, drive_event_index_t event_index, drive_event_t* event);

/**
 * Читает канал осциллограммы из eeprom.
 * @param future Будущее.
 * @param osc_index Индекс осциллограммы.
 * @param osc_channel Канал осциллограммы.
 * @return Код ошибки.
 */
extern err_t drive_tasks_read_osc_channel(future_t* future, drive_osc_index_t osc_index, size_t osc_channel);

/**
 * Очищает события.
 * @return Код ошибки.
 */
extern err_t drive_tasks_clear_events(void);

#endif /* DRIVE_TASKS_H */


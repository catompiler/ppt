/**
 * @file drive_task_storage.h Задача хранилища.
 */
#ifndef DRIVE_TASK_STORAGE_H
#define DRIVE_TASK_STORAGE_H

#include "errors/errors.h"
#include <stdint.h>
#include <stddef.h>
#include "future/future.h"
#include "drive_events.h"


/**
 * Создаёт задачу хранилища.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_storage_init(uint32_t priority);

/**
 * Запускает запись события.
 * @return Код ошибки.
 */
extern err_t drive_task_storage_write_event(drive_event_type_t type, bool need_osc);

/**
 * Записывает событие.
 * ошибки привода и осциллограмму.
 * @return Код ошибки.
 */
extern err_t drive_task_storage_write_error_event(void);

/**
 * Записывает событие.
 * состояния привода и осциллограмму.
 * @return Код ошибки.
 */
extern err_t drive_task_storage_write_status_event(void);

/**
 * Записывает событие.
 * предупреждения привода без осциллограммы.
 * @return Код ошибки.
 */
extern err_t drive_task_storage_write_warning_event(void);

/**
 * Читает событие.
 * @param future Будущее.
 * @param event_index Индекс события.
 * @param event Событие.
 * @return Код ошибки.
 */
extern err_t drive_task_storage_read_event(future_t* future, drive_event_index_t event_index, drive_event_t* event);

/**
 * Читает канал осциллограммы.
 * @param future Будущее.
 * @param osc_index Индекс осциллограммы.
 * @param osc_channel Канал осциллограммы.
 * @return Код ошибки.
 */
extern err_t drive_task_storage_read_osc_channel(future_t* future, drive_osc_index_t osc_index, size_t osc_channel);

/**
 * Очищает события.
 */
extern err_t drive_task_storage_clear_events(void);

/**
 * Сохраняет настройки.
 */
extern err_t drive_task_storage_save_settings(void);

/**
 * Сохраняет энергонезависимые данные.
 */
extern err_t drive_task_storage_save_nvdata(void);

#endif /* DRIVE_TASK_STORAGE_H */

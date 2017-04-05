/**
 * @file drive_phase_sync.h Библиотека синхронизации с сетевым напряжением.
 */

#ifndef DRIVE_PHASE_SYNC_H
#define DRIVE_PHASE_SYNC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "errors/errors.h"
#include "fixed/fixed32.h"
#include "drive_phase_state.h"




//! Тип каллбэка получения начального угла.
typedef fixed32_t (*drive_phase_sync_angle_callback_t)(void);


/**
 * Инициализирует синхронизацию с фазами.
 * @return Код ошибки.
 */
extern err_t drive_phase_sync_init(void);

/**
 * Сбрасывает состояние синхронизации с фазами.
 */
extern void drive_phase_sync_reset(void);

/**
 * Устанавливает каллбэк получения угла текущего значения АЦП.
 * @param callback Каллбэк.
 */
extern void drive_phase_sync_set_angle_callback(drive_phase_sync_angle_callback_t callback);

/**
 * Добавляет текущие мгновенные значения питания в буфер FFT.
 */
extern void drive_phase_sync_append_data(void);

/**
 * Переключает буферы чтения и записи.
 */
extern void drive_phase_sync_swap_buffers(void);

/**
 * Производит вычисления для заданной фазы.
 * @param phase Фаза.
 * @return Код ошибки.
 */
extern err_t drive_phase_sync_calc(phase_t phase);

/**
 * Получает угол заданной фазы на момент начала записи буфера FFT.
 * Угол возвращается в интервале
 * от -180 (фаза опережает буфер)
 * до +180 (фаза отстаёт от буфера).
 * @param phase Фаза.
 * @param angle Угол от -180 до +180 градусов.
 * @return Код ошибки.
 */
extern err_t drive_phase_sync_angle(phase_t phase, fixed32_t* angle);

/**
 * Получает смещение (опережение) фазы, мкс.
 * @param phase Фаза.
 * @return Смещение (опережение) фазы.
 */
extern int16_t drive_phase_sync_offset(phase_t phase);

/**
 * Получает угол смещения (опережения) фазы, мкс.
 * @param phase Фаза.
 * @return Угол смещения (опережения) фазы.
 */
extern fixed32_t drive_phase_sync_offset_angle(phase_t phase);

/**
 * Получает угол фазы относительно предыдущей фазы
 * согласно направлению чередования фаз.
 * @param phase Фаза.
 * @return Угол фазы относительно предыдущей фазы.
 */
extern fixed32_t drive_phase_sync_delta_angle(phase_t phase);

/**
 * Получает отклонение угла между фазой и предыдущей фазой
 * согласно направлению чередования фаз по сравнению
 * с нормальным углом между фазами.
 * @param phase Фаза.
 * @return Отклонение угла.
 */
extern fixed32_t drive_phase_sync_diff_delta_angle(phase_t phase);

/**
 * Вычисляет угол очередной фазы.
 * @return Код ошибки.
 */
extern err_t drive_phase_sync_process_calc(void);

/**
 * Получает наличие данных углов фаз.
 * @return Флаг доступности данных углов фаз.
 */
extern bool drive_phase_sync_data_avail(void);

/**
 * Устанавливает коэффициенты ПИД-регулятора синхронизации с фазами.
 * @param kp Коэффициент пропорционального звена.
 * @param ki Коэффициент интегрального звена.
 * @param kd Коэффициент дифференциального звена.
 */
extern void drive_phase_sync_set_pll_pid(fixed32_t kp, fixed32_t ki, fixed32_t kd);

/**
 * Устанавливает пределы значений ПИД-регулятора синхронизации с фазами.
 * @param pid_min Минимальное значение ПИД.
 * @param pid_max Максимальное значение ПИД.
 */
extern void drive_phase_sync_pll_pid_clamp(fixed32_t pid_min, fixed32_t pid_max);

/**
 * Получает значение ПИД-регулятора синхронизации с фазами.
 * @return Значение ПИД-регулятора синхронизации с фазами.
 */
extern fixed32_t drive_phase_sync_pll_pid_value(void);

/**
 * Выполняет ПИД-регулирование синхронизации с фазами.
 * @return Флаг регулировки.
 */
extern bool drive_phase_sync_pll_regulate(void);

/**
 * Устанавливает точность синхронизации.
 * @param angle Допустимый угол отклонения от фазы.
 */
extern void drive_phase_sync_set_accuracy(fixed32_t angle);

/**
 * Получает флаг синхронизированности с сетью.
 * @return Флаг синхронизированности с сетью.
 */
extern bool drive_phase_sync_synchronized(void);

/**
 * Получает текущее направление чередования фаз.
 * @return Текущее направление чередования фаз.
 */
extern drive_dir_t drive_phase_sync_current_dir(void);

/**
 * Получает текущую фазу.
 * @return Текущая фаза.
 */
extern phase_t drive_phase_sync_current_phase(void);

/**
 * Получает следующую фазу.
 * @return следующую фаза.
 */
extern phase_t drive_phase_sync_next_phase(void);

/**
 * Вычисляет смещение фаз относительно синхронизации.
 * @return Флаг расчёта.
 */
extern bool drive_phase_sync_calc_offsets(void);

/**
 * Выполняет вычисления и регулирование
 * синхронизации с фазами.
 * @return Флаг разрешения регулирования.
 */
extern bool drive_phase_sync_process(void);

#endif /* DRIVE_PHASE_SYNC_H */

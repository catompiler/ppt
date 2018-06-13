/**
 * @file drive_dip.h Библиотека DIP-переключателя привода.
 */

#ifndef DRIVE_DIP_H_
#define DRIVE_DIP_H_

#include <stdint.h>
#include <stdbool.h>


//! Перечисление пинов переключателя.
typedef enum _Drive_Dip_Pin {
    DRIVE_DIP_UI_PAD = 1,
    DRIVE_DIP_HS_TEMP = 2
} drive_dip_pin_t;

/**
 * Инициализирует переключатель.
 */
extern void drive_dip_init(void);

/**
 * Обновляет состояние пинов переключателя.
 */
extern void drive_dip_update(void);

/**
 * Получает состояние пина переключателя.
 * @param pin Пин переключателя.
 * @return Состояние пина переключателя.
 */
extern bool drive_dip_pin_state(drive_dip_pin_t pin);

#endif /* DRIVE_DIP_H_ */

/**
 * @file drive_task_adc.h Задача АЦП.
 */
#ifndef DRIVE_TASK_ADC_H
#define DRIVE_TASK_ADC_H

#include "errors/errors.h"
#include <stdint.h>
#include <FreeRTOS.h>


/**
 * Создаёт задачу АЦП.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_adc_init(uint32_t priority);

/**
 * Запускает применение настроек.
 * Число данных в массиве должно быть равно
 * DRIVE_POWER_ADC_CHANNELS_COUNT.
 * @param adc_data Данные АЦП.
 * @param pxHigherPriorityTaskWoken Флаг необходимости переключения контекста.
 * @return Код ошибки.
 */
extern err_t drive_task_adc_process_data_isr(uint16_t* adc_data, BaseType_t* pxHigherPriorityTaskWoken);


#endif /* DRIVE_TASK_ADC_H */

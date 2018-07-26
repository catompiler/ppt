/**
 * Сгененировано в LCD Vision HP Infotech 
 * (формат-контроллер Grayscale bit/pixel, 
 * из С-файла в массив скопированы 
 * Image data for monochrome displays organized
   as rows of vertical bytes)
 */
#ifndef ICONS_STATUSBAR_H
#define ICONS_STATUSBAR_H

#include <stdint.h>

#define ICONS_STATUSBAR_WIDTH (696)
#define ICONS_STATUSBAR_HEIGHT (24)
#define ICONS_STATUSBAR_FORMAT (GRAPHICS_FORMAT_BW_1_V)
#define ICONS_STATUSBAR_COUNT (29)
// реальные иконки
#define ICONS_STATUSBAR_VAL_CNTRL_BUS 0
#define ICONS_STATUSBAR_VAL_CNTRL_LOC 1
#define ICONS_STATUSBAR_VAL_CNTRL_REM 2
#define ICONS_STATUSBAR_VAL_FAULT_F 3
#define ICONS_STATUSBAR_VAL_FAULT_U 4
#define ICONS_STATUSBAR_VAL_FAULT_I 5
#define ICONS_STATUSBAR_VAL_LOOP_ON 6
#define ICONS_STATUSBAR_VAL_TARGET 7
#define ICONS_STATUSBAR_VAL_MODBUS_IDLE 8
#define ICONS_STATUSBAR_VAL_MODBUS_TX 9
#define ICONS_STATUSBAR_VAL_MODBUS_RX 10
#define ICONS_STATUSBAR_VAL_OVERHEAT 11
#define ICONS_STATUSBAR_VAL_USB 12
#define ICONS_STATUSBAR_VAL_SD 13
#define ICONS_STATUSBAR_VAL_BT 14
#define ICONS_STATUSBAR_VAL_TRIAC 15
#define ICONS_STATUSBAR_VAL_BEEP_OFF 16
#define ICONS_STATUSBAR_VAL_WARNING 17
#define ICONS_STATUSBAR_VAL_MAINTENANCE 18
#define ICONS_STATUSBAR_VAL_EMERGENCY 19
#define ICONS_STATUSBAR_VAL_LOOP_BREAK 20
#define ICONS_STATUSBAR_VAL_ROOT 21
#define ICONS_STATUSBAR_VAL_WORK_1 22
#define ICONS_STATUSBAR_VAL_WORK_2 23
#define ICONS_STATUSBAR_VAL_WORK_3 24
#define ICONS_STATUSBAR_VAL_WORK_4 25
#define ICONS_STATUSBAR_VAL_KEY 26
#define ICONS_STATUSBAR_VAL_FAN 27
#define ICONS_STATUSBAR_VAL_READY 28

// виртуальные иконки
#define ICONS_STATUSBAR_VAL_NOTHING 255 // отсутствие иконки

// анимированные иконки (последовательность иконок)
// работа привода
#define ICONS_STATUSBAR_ANIM_WORK_CNT 4
static const uint8_t icons_statusbar_anim_work[ICONS_STATUSBAR_ANIM_WORK_CNT] =
{
    ICONS_STATUSBAR_VAL_WORK_1,
    ICONS_STATUSBAR_VAL_WORK_2,
    ICONS_STATUSBAR_VAL_WORK_3,
    ICONS_STATUSBAR_VAL_WORK_4,
};
// готовность привода
#define ICONS_STATUSBAR_ANIM_READY_CNT 5
static const uint8_t icons_statusbar_anim_ready[ICONS_STATUSBAR_ANIM_READY_CNT] =
{
    ICONS_STATUSBAR_VAL_NOTHING,
    ICONS_STATUSBAR_VAL_READY,
    ICONS_STATUSBAR_VAL_READY,
    ICONS_STATUSBAR_VAL_READY,
    ICONS_STATUSBAR_VAL_READY,
};
// прием/передача сообщений по Modbus
#define ICONS_STATUSBAR_ANIM_MODBUS_WORK 2
static const uint8_t icons_statusbar_anim_modbus_work[ICONS_STATUSBAR_ANIM_MODBUS_WORK] =
{
    ICONS_STATUSBAR_VAL_MODBUS_RX,
    ICONS_STATUSBAR_VAL_MODBUS_TX,
};
// прием/передача сообщений по Bluetoth Modbus
#define ICONS_STATUSBAR_ANIM_MODBUS_BT_WORK 2
static const uint8_t icons_statusbar_anim_modbus_bt_work[ICONS_STATUSBAR_ANIM_MODBUS_BT_WORK] =
{
    ICONS_STATUSBAR_VAL_NOTHING,
    ICONS_STATUSBAR_VAL_BT,
};


static const uint8_t icons_statusbar_data[2088] =
{
/* Image data for monochrome displays organized
   as rows of vertical bytes */
0x00, 0x00, 0xE0, 0xE0, 0x60, 0x60, 0xE0, 0xC0, 
0x00, 0xE0, 0xE0, 0x00, 0x00, 0xE0, 0xE0, 0x00, 
0xC0, 0xE0, 0x60, 0x60, 0xE0, 0x80, 0x00, 0x00, 
0x00, 0x00, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xC0, 0xE0, 0x60, 0xE0, 0xC0, 0x00, 0x00, 
0x00, 0xC0, 0xE0, 0x60, 0xE0, 0xC0, 0x00, 0x00, 
0x00, 0xE0, 0xE0, 0x60, 0x60, 0xE0, 0xC0, 0x00, 
0xE0, 0xE0, 0x60, 0x60, 0x60, 0x00, 0xE0, 0xE0, 
0x80, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xE0, 0x00, 
0x00, 0x00, 0x80, 0xE0, 0x78, 0x1E, 0x06, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0xFC, 
0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x00, 
0x00, 0x00, 0x80, 0xE0, 0x78, 0x1E, 0x06, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0xFC, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0xFC, 0x00, 
0x00, 0x00, 0x80, 0xE0, 0x78, 0x1E, 0x06, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 
0xFC, 0xFC, 0xFC, 0x04, 0x04, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 
0x3C, 0x0C, 0x0E, 0x06, 0x06, 0x06, 0x0E, 0x0C, 
0x3C, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x80, 0x00, 0x00, 0xFC, 0xFC, 0xFC, 0x00, 
0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x60, 
0x30, 0xF8, 0xFC, 0xF8, 0x30, 0x60, 0xC0, 0x00, 
0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFC, 
0xFC, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 
0x8C, 0x0C, 0x0C, 0x0C, 0xFC, 0xFC, 0x00, 0x00, 
0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFC, 
0xFC, 0xAC, 0xDC, 0xAC, 0xDC, 0xAC, 0xDC, 0xAC, 
0xDC, 0xAC, 0x5C, 0xAC, 0xFC, 0xFC, 0x00, 0x00, 
0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFC, 
0xFC, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 
0x8C, 0x0C, 0x0C, 0x0C, 0xFC, 0xFC, 0x00, 0x00, 
0x00, 0xC0, 0xC0, 0xF8, 0xF8, 0xC0, 0xC0, 0xC0, 
0x00, 0x78, 0x84, 0x84, 0x84, 0x78, 0x00, 0xFC, 
0x02, 0xC2, 0xC2, 0x02, 0xFC, 0x00, 0x00, 0x00, 
0x00, 0xF8, 0xFC, 0x04, 0x04, 0xFC, 0x04, 0x04, 
0xFC, 0x8C, 0x84, 0xA4, 0x24, 0x24, 0xFC, 0x04, 
0x04, 0xB4, 0x04, 0x4C, 0xFC, 0xF8, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x0C, 
0x0C, 0xFC, 0x0C, 0x0C, 0xFC, 0x0C, 0x0C, 0xFC, 
0x0C, 0x0C, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 
0x00, 0x00, 0x00, 0xFE, 0x0C, 0x18, 0x30, 0xE0, 
0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 
0xC0, 0xC0, 0xFC, 0xFC, 0xC0, 0xC0, 0xC0, 0xC0, 
0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0C, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 
0xC0, 0xE0, 0xF0, 0xF8, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x80, 0xC0, 0x70, 0x1C, 0x1C, 0x70, 0xC0, 0x80, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x80, 0xE0, 0xF0, 0xF0, 0xF8, 0xFC, 
0x38, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
0x38, 0xFC, 0xF8, 0xF0, 0xF0, 0xE0, 0x80, 0x00, 
0x00, 0x00, 0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0C, 
0x0E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0E, 
0x0C, 0x1C, 0x38, 0x70, 0xE0, 0xC0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 
0x3C, 0x0C, 0x08, 0x00, 0x00, 0x00, 0x08, 0x0C, 
0x3C, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x7C, 
0x0C, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0C, 
0xFC, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x84, 0xEC, 0xFC, 0xFC, 0x7C, 
0xFC, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x60, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0x78, 
0x38, 0x38, 0x1C, 0x1C, 0x1C, 0x1C, 0x3C, 0x38, 
0x78, 0xF0, 0xF0, 0xE0, 0xF0, 0xF8, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0xF0, 0x78, 
0x38, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x38, 
0x78, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x40, 0xE0, 0xF0, 0x78, 
0x38, 0x3C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x38, 
0x78, 0xF0, 0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xC0, 0x60, 0x10, 0x08, 0xCC, 0xC4, 
0x04, 0x04, 0x0C, 0x08, 0x18, 0x30, 0xE0, 0x80, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xF8, 
0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xFC, 0xFC, 0x78, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 
0xF0, 0xF0, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF0, 
0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xFF, 0xFF, 0x18, 0x18, 0xFF, 0xE7, 
0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 
0x87, 0x8F, 0x18, 0x18, 0xF1, 0xE1, 0x00, 0x00, 
0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 
0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 
0xFF, 0xFF, 0x00, 0x00, 0x00, 0xC3, 0xC3, 0x00, 
0x00, 0xFF, 0xFF, 0x78, 0xF8, 0x9F, 0x0F, 0x00, 
0xFF, 0xFF, 0x18, 0x18, 0x18, 0x00, 0xFF, 0xFF, 
0x07, 0x1E, 0x78, 0x1E, 0x07, 0xFF, 0xFF, 0x00, 
0x00, 0x0E, 0x0F, 0x8D, 0x0C, 0x0C, 0x0C, 0xCC, 
0xFC, 0x3C, 0x0C, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
0x00, 0x0E, 0x0F, 0x8D, 0x0C, 0x0C, 0x0C, 0xCC, 
0xFC, 0x3C, 0x0C, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 
0x00, 0x0E, 0x0F, 0x8D, 0x0C, 0x0C, 0x0C, 0xCC, 
0xFC, 0x3C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x04, 0x0C, 0x18, 0x38, 0xFF, 0xFF, 
0x38, 0x18, 0x0C, 0x04, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x01, 0x03, 0x06, 0x0F, 0x1F, 0x0F, 0x06, 
0x03, 0x01, 0x00, 0x80, 0xC0, 0xE0, 0x60, 0x00, 
0x00, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 
0xFF, 0x60, 0x60, 0x60, 0x7F, 0x7F, 0x00, 0x00, 
0x00, 0x00, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 
0xFF, 0x6A, 0x75, 0x6A, 0x7F, 0x7F, 0x00, 0x00, 
0x00, 0x00, 0xFF, 0xFF, 0x55, 0xAB, 0x55, 0xAB, 
0x55, 0xAB, 0x55, 0xAB, 0x55, 0xAB, 0x55, 0xFF, 
0xFF, 0x60, 0x60, 0x60, 0x7F, 0x7F, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x7F, 0xFF, 0x80, 0x80, 0xC0, 
0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 
0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x80, 0x00, 0x00, 
0x00, 0x03, 0x07, 0x86, 0x04, 0x04, 0x04, 0x06, 
0x87, 0x44, 0x24, 0x24, 0x74, 0x76, 0x27, 0x04, 
0x04, 0x05, 0x04, 0x86, 0x07, 0x03, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xF8, 0xFC, 0xFF, 0x3F, 
0x1F, 0xDF, 0xDF, 0xDF, 0xFF, 0xDF, 0xDF, 0x9F, 
0x1F, 0x3F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 
0xC3, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0xC3, 0x81, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0x70, 0x1C, 
0x0E, 0x03, 0xFF, 0xFF, 0x03, 0x0E, 0x1C, 0x70, 
0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x83, 0xC6, 
0x6C, 0x38, 0x38, 0x6C, 0xC6, 0x83, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0x38, 0x0E, 
0x03, 0x00, 0x1C, 0xFF, 0xFF, 0x1C, 0x00, 0x03, 
0x0E, 0x38, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x01, 0x03, 0x07, 0x03, 0x01, 0x01, 0x03, 
0x07, 0x8E, 0xDC, 0xF8, 0x70, 0xF8, 0xDC, 0x8E, 
0x07, 0x03, 0x01, 0x01, 0x03, 0x07, 0x07, 0x00, 
0x00, 0x7E, 0xFF, 0xE3, 0x00, 0x01, 0x03, 0x0F, 
0x3F, 0xF3, 0xC3, 0x03, 0x83, 0xE3, 0x7B, 0x1F, 
0x07, 0x03, 0x00, 0x00, 0xC1, 0xFF, 0x7F, 0x00, 
0x00, 0x00, 0x81, 0xC3, 0x66, 0x3C, 0xFF, 0xFF, 
0x3C, 0x66, 0xC3, 0x81, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 
0xF0, 0xF0, 0x70, 0x30, 0x30, 0x70, 0xF0, 0xF0, 
0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x3E, 0xFF, 0xFF, 0xC3, 0x00, 0x00, 
0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x83, 0xFF, 0xFF, 0x7E, 0x00, 0x00, 
0x00, 0x00, 0x7E, 0xFF, 0xFF, 0x81, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 
0x00, 0x00, 0x7E, 0xFF, 0xFF, 0xC1, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 
0x00, 0x00, 0xC3, 0xFF, 0xFF, 0x7C, 0x00, 0x00, 
0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 
0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x81, 0xFF, 0xFF, 0x7E, 0x00, 0x00, 
0x00, 0x00, 0x0F, 0x18, 0x30, 0x60, 0x40, 0xC0, 
0x80, 0x80, 0xC0, 0x40, 0xC0, 0x00, 0x0C, 0x1F, 
0x30, 0x60, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE1, 
0xF7, 0xFF, 0xFF, 0xCF, 0xCF, 0xFF, 0xFD, 0xF8, 
0xF8, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x3E, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x07, 0x07, 0x06, 0x06, 0x07, 0x03, 
0x00, 0x01, 0x07, 0x06, 0x06, 0x07, 0x01, 0x00, 
0x01, 0x07, 0x06, 0x06, 0x07, 0x03, 0x00, 0x00, 
0x00, 0x00, 0x07, 0x07, 0x06, 0x06, 0x06, 0x06, 
0x00, 0x03, 0x07, 0x06, 0x07, 0x03, 0x00, 0x00, 
0x00, 0x03, 0x07, 0x06, 0x07, 0x03, 0x01, 0x00, 
0x00, 0x07, 0x07, 0x00, 0x01, 0x07, 0x06, 0x00, 
0x07, 0x07, 0x06, 0x06, 0x06, 0x00, 0x07, 0x07, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x00, 
0x00, 0x00, 0x00, 0x0F, 0x7E, 0x3C, 0x3F, 0x1B, 
0x18, 0x08, 0x08, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x0F, 0x7E, 0x3C, 0x3F, 0x1B, 
0x18, 0x08, 0x08, 0x00, 0x07, 0x0F, 0x1F, 0x3C, 
0x38, 0x30, 0x38, 0x3C, 0x1F, 0x0F, 0x07, 0x00, 
0x00, 0x00, 0x00, 0x0F, 0x7E, 0x3C, 0x3F, 0x1B, 
0x18, 0x08, 0x08, 0x00, 0x00, 0x00, 0x20, 0x20, 
0x3F, 0x3F, 0x3F, 0x20, 0x20, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x1F, 
0x3C, 0x30, 0x70, 0x60, 0x60, 0x60, 0x70, 0x30, 
0x3C, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 
0x1C, 0x0E, 0x07, 0x03, 0x01, 0x00, 0x00, 0x30, 
0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 
0x00, 0x00, 0x0F, 0x0F, 0x0C, 0x2C, 0x2C, 0x2C, 
0x3C, 0x3C, 0x3C, 0x2C, 0x2C, 0x2C, 0x0C, 0x0F, 
0x0F, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x0F, 0x0F, 0x0C, 0x2C, 0x2C, 0x2C, 
0x3C, 0x3C, 0x3C, 0x2C, 0x2C, 0x2C, 0x0C, 0x0F, 
0x0F, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x0F, 0x0F, 0x0D, 0x2E, 0x2D, 0x2E, 
0x3D, 0x3E, 0x3D, 0x2E, 0x2D, 0x2E, 0x0D, 0x0F, 
0x0F, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x30, 
0x66, 0x6F, 0x6F, 0x66, 0x30, 0x3F, 0x1F, 0x00, 
0x00, 0x02, 0x07, 0x0F, 0x07, 0x02, 0x02, 0x03, 
0x02, 0x02, 0x02, 0x06, 0x0A, 0x12, 0x3A, 0x3A, 
0x3A, 0x02, 0x02, 0x0F, 0x07, 0x02, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x1F, 0x37, 0x33, 0x33, 
0x32, 0x30, 0x39, 0x37, 0x33, 0x33, 0x33, 0x31, 
0x38, 0x3C, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 
0x00, 0x00, 0x00, 0x7F, 0x30, 0x18, 0x0C, 0x07, 
0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 
0x03, 0x03, 0x3F, 0x3F, 0x03, 0x03, 0x03, 0x03, 
0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
0x03, 0x07, 0x0F, 0x1F, 0x00, 0x00, 0x01, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 
0x00, 0x30, 0x3C, 0x37, 0x31, 0x30, 0x30, 0x30, 
0x30, 0x30, 0x30, 0x36, 0x36, 0x30, 0x30, 0x30, 
0x30, 0x30, 0x30, 0x31, 0x37, 0x3C, 0x30, 0x00, 
0x00, 0x00, 0x00, 0x10, 0x38, 0x7C, 0x3E, 0x1F, 
0x0F, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 
0x0F, 0x1F, 0x3E, 0x7C, 0x38, 0x10, 0x00, 0x00, 
0x00, 0x00, 0x01, 0x07, 0x0F, 0x0C, 0x1C, 0x38, 
0x30, 0x30, 0x71, 0x77, 0x73, 0x71, 0x30, 0x30, 
0x38, 0x18, 0x1C, 0x0F, 0x07, 0x01, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x1F, 
0x3C, 0x30, 0x70, 0x60, 0x60, 0x60, 0x70, 0x30, 
0x3C, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 
0x3F, 0x3F, 0x3E, 0x30, 0x30, 0x3E, 0x3F, 0x3F, 
0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1E, 
0x1C, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x1C, 
0x1E, 0x0F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x0F, 0x1E, 
0x1C, 0x38, 0x38, 0x38, 0x38, 0x38, 0x3C, 0x1C, 
0x1E, 0x0F, 0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x06, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 
0x3E, 0x3F, 0x3F, 0x37, 0x21, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x1F, 0x0F, 0x07, 0x0F, 0x0F, 0x1E, 
0x1C, 0x3C, 0x38, 0x38, 0x38, 0x38, 0x1C, 0x1C, 
0x1E, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x07, 
0x04, 0x3C, 0x20, 0x21, 0x23, 0x3E, 0x00, 0x00, 
0x00, 0x03, 0x0F, 0x1F, 0x3F, 0x3F, 0x3F, 0x1F, 
0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x01, 0x07, 
0x0F, 0x1F, 0x1F, 0x1F, 0x0F, 0x07, 0x03, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 
0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 
0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#endif /* ICONS_STATUSBAR_H */


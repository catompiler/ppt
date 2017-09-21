#include "drive_gui.h"
#include "drive.h"
#include "drive_power.h"
#include "utils/utils.h"
#include "utils/delay.h"
#include "drive_keypad.h"
#include "tft9341/tft9341_cache.h"
#include "tft9341/tft9341_cache_vbuf.h"
#include "graphics/graphics.h"
#include "graphics/painter.h"
#include "gui/gui_metro.h"
#include "gui/gui_object.h"
#include "gui/gui_widget.h"
#include "drive_events.h"
#include "fixed/fixed.h"
#include "gui/widgets/gui_time.h"
#include "gui/widgets/gui_statusbar.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui/bitmaps/icons_statusbar.h"
#include "gui/fonts/arialnarrow12.h"
#include "gui/fonts/arialnarrow18.h"
#include "gui/fonts/arialbold42.h"
#include "gui/widgets/gui_tile.h"
#include "gui/widgets/gui_statusbar.h"
#include "gui/widgets/gui_menu.h"
#include "gui/widgets/gui_home.h"
#include <stdlib.h>
#include <locale.h>
#include "list/list.h"
#include "gui/resources/resources_menu.h"
#include "gui/resources/resources_colors.h"
#include "gui/resources/resources_params.h"
#include "gui/menu/menu_explorer.h"
#include "translations.h"

#define DRIVE_MODBUS_GUI_ICON_WORK_SEC 4
#define DRIVE_MODBUS_GUI_ICON_IDLE_SEC 30

// позиция иконки статусбара
#define gui_statusbar_icon_pos(pos) pos * ICONS_STATUSBAR_HEIGHT

// список доступных языков интерфейса
GUI_TILE_LANGUAGES(gui_languages, GUI_TILE_LANGUAGES_COUNT) {
    GUI_TILE_LANGUAGE(TR_LANG_ID_RU),
    GUI_TILE_LANGUAGE(TR_LANG_ID_EN),    
};

// список вариантов отображения значений на плитках
GUI_TILE_TYPES(gui_tile_types, GUI_TILE_TYPES_COUNT) {
    GUI_TILE_TYPE(PARAM_ID_POWER_U_A, TEXT(TR_ID_HOME_PHASE_A_VOLTAGE), PARAM_ID_POWER_U_ALARM_MIN, PARAM_ID_POWER_U_WARN_MIN, PARAM_ID_POWER_U_WARN_MAX, PARAM_ID_POWER_U_ALARM_MAX, NULL),
    GUI_TILE_TYPE(PARAM_ID_POWER_U_B, TEXT(TR_ID_HOME_PHASE_B_VOLTAGE), PARAM_ID_POWER_U_ALARM_MIN, PARAM_ID_POWER_U_WARN_MIN, PARAM_ID_POWER_U_WARN_MAX, PARAM_ID_POWER_U_ALARM_MAX, NULL),
    GUI_TILE_TYPE(PARAM_ID_POWER_U_C, TEXT(TR_ID_HOME_PHASE_C_VOLTAGE), PARAM_ID_POWER_U_ALARM_MIN, PARAM_ID_POWER_U_WARN_MIN, PARAM_ID_POWER_U_WARN_MAX, PARAM_ID_POWER_U_ALARM_MAX, NULL),
    GUI_TILE_TYPE(PARAM_ID_POWER_U_ROT, TEXT(TR_ID_HOME_ANCHOR_VOLTAGE), PARAM_ID_POWER_U_ROT_ALARM_MIN, PARAM_ID_POWER_U_ROT_WARN_MIN, PARAM_ID_POWER_U_ROT_WARN_MAX, PARAM_ID_POWER_U_ROT_ALARM_MAX, NULL),
    GUI_TILE_TYPE(PARAM_ID_MOTOR_E, TEXT(TR_ID_HOME_MOTOR_E), PARAM_ID_POWER_U_ROT_ALARM_MIN, PARAM_ID_POWER_U_ROT_WARN_MIN, PARAM_ID_POWER_U_ROT_WARN_MAX, PARAM_ID_POWER_U_ROT_ALARM_MAX, NULL),
    GUI_TILE_TYPE(PARAM_ID_POWER_I_A, TEXT(TR_ID_HOME_PHASE_A_CURRENT), PARAM_ID_POWER_I_ALARM_MIN, PARAM_ID_POWER_I_WARN_MIN, PARAM_ID_POWER_I_WARN_MAX, PARAM_ID_POWER_I_ALARM_MAX, NULL),
    GUI_TILE_TYPE(PARAM_ID_POWER_I_B, TEXT(TR_ID_HOME_PHASE_B_CURRENT), PARAM_ID_POWER_I_ALARM_MIN, PARAM_ID_POWER_I_WARN_MIN, PARAM_ID_POWER_I_WARN_MAX, PARAM_ID_POWER_I_ALARM_MAX, NULL),
    GUI_TILE_TYPE(PARAM_ID_POWER_I_C, TEXT(TR_ID_HOME_PHASE_C_CURRENT), PARAM_ID_POWER_I_ALARM_MIN, PARAM_ID_POWER_I_WARN_MIN, PARAM_ID_POWER_I_WARN_MAX, PARAM_ID_POWER_I_ALARM_MAX, NULL),
    GUI_TILE_TYPE(PARAM_ID_POWER_I_ROT, TEXT(TR_ID_HOME_ANCHOR_CURRENT), PARAM_ID_POWER_I_ROT_ALARM_MIN, PARAM_ID_POWER_I_ROT_WARN_MIN, PARAM_ID_POWER_I_ROT_WARN_MAX, PARAM_ID_POWER_I_ROT_ALARM_MAX, NULL),
    GUI_TILE_TYPE(PARAM_ID_POWER_I_EXC, TEXT(TR_ID_HOME_EXCITATION_CURRENT), PARAM_ID_POWER_I_EXC_ALARM_MIN, PARAM_ID_POWER_I_EXC_WARN_MIN, PARAM_ID_POWER_I_EXC_WARN_MAX, PARAM_ID_POWER_I_EXC_ALARM_MAX, NULL),
            
    GUI_TILE_TYPE(PARAM_ID_POWER_I_FAN, TEXT(TR_ID_HOME_POWER_I_FAN), PARAM_ID_POWER_I_FAN_ALARM_MIN, PARAM_ID_POWER_I_FAN_WARN_MIN, PARAM_ID_POWER_I_FAN_WARN_MAX, PARAM_ID_POWER_I_FAN_ALARM_MAX, NULL),
    GUI_TILE_TYPE(PARAM_ID_POWER_I_REF, TEXT(TR_ID_HOME_POWER_I_REF), PARAM_ID_POWER_I_REF_ALARM_MIN, PARAM_ID_POWER_I_REF_WARN_MIN, PARAM_ID_POWER_I_REF_WARN_MAX, PARAM_ID_POWER_I_REF_ALARM_MAX, NULL),
      
    GUI_TILE_TYPE(PARAM_ID_HEATSINK_TEMP, TEXT(TR_ID_HOME_HEATSINK_TEMP), PARAM_ID_HEATSINK_TEMP_ALARM_MIN, PARAM_ID_HEATSINK_TEMP_WARN_MIN, PARAM_ID_HEATSINK_TEMP_WARN_MAX, PARAM_ID_HEATSINK_TEMP_ALARM_MAX, NULL),
            
    GUI_TILE_TYPE(PARAM_ID_HEATSINK_FAN_RPM, TEXT(TR_ID_HOME_HEATSINK_FAN_RPM), PARAM_ID_HEATSINK_FAN_RPM_ALARM_MIN, PARAM_ID_HEATSINK_FAN_RPM_WARN_MIN, PARAM_ID_HEATSINK_FAN_RPM_WARN_MAX, PARAM_ID_HEATSINK_FAN_RPM_ALARM_MAX, TEXT(TR_ID_UNITS_RPM_SHORT)),   
    GUI_TILE_TYPE(PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE, TEXT(TR_ID_HOME_TRIACS_PAIRS_OPEN_ANGLE), PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MIN, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_WARN_MIN, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_WARN_MAX, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MAX, NULL),   
    GUI_TILE_TYPE(PARAM_ID_TRIAC_EXC_OPEN_ANGLE, TEXT(TR_ID_HOME_TRIAC_EXC_OPEN_ANGLE), PARAM_ID_TRIAC_EXC_OPEN_ANGLE_ALARM_MIN, PARAM_ID_TRIAC_EXC_OPEN_ANGLE_WARN_MIN, PARAM_ID_TRIAC_EXC_OPEN_ANGLE_WARN_MAX, PARAM_ID_TRIAC_EXC_OPEN_ANGLE_ALARM_MAX, NULL),   

    GUI_TILE_TYPE(PARAM_ID_MOTOR_EFF, TEXT(TR_ID_HOME_MOTOR_EFF), PARAM_ID_MOTOR_EFF_ALARM_MIN, PARAM_ID_MOTOR_EFF_WARN_MIN, PARAM_ID_MOTOR_EFF_WARN_MAX, PARAM_ID_MOTOR_EFF_ALARM_MAX, NULL),    
    GUI_TILE_TYPE(PARAM_ID_MOTOR_R_ROT, TEXT(TR_ID_HOME_MOTOR_R_ROT), PARAM_ID_MOTOR_R_ROT_ALARM_MIN, PARAM_ID_MOTOR_R_ROT_WARN_MIN, PARAM_ID_MOTOR_R_ROT_WARN_MAX, PARAM_ID_MOTOR_R_ROT_ALARM_MAX, TEXT(TR_ID_UNITS_OHM_SHORT)),   
    GUI_TILE_TYPE(PARAM_ID_MOTOR_RPM, TEXT(TR_ID_HOME_MOTOR_RPM), PARAM_ID_MOTOR_RPM_ALARM_MIN, PARAM_ID_MOTOR_RPM_WARN_MIN, PARAM_ID_MOTOR_RPM_WARN_MAX, PARAM_ID_MOTOR_RPM_ALARM_MAX, TEXT(TR_ID_UNITS_RPM_SHORT)),   
    GUI_TILE_TYPE(PARAM_ID_MOTOR_TORQUE, TEXT(TR_ID_HOME_MOTOR_TORQUE), PARAM_ID_MOTOR_TORQUE_ALARM_MIN, PARAM_ID_MOTOR_TORQUE_WARN_MIN, PARAM_ID_MOTOR_TORQUE_WARN_MAX, PARAM_ID_MOTOR_TORQUE_ALARM_MAX, TEXT(TR_ID_UNITS_TORQUE_SHORT)),   
};

// соответствие плитки идентификатору параметра
HOME_TILES_VALUES(gui_tiles_values)

// последнее время обработки callback_modbus
static uint32_t drive_modbus_last_time; 

//! Первый буфер кэша TFT.
static uint8_t tft_cache_buf_data0[TFT_CACHE_BUF0_SIZE];
//! Второй буфер кэша TFT.
static uint8_t tft_cache_buf_data1[TFT_CACHE_BUF1_SIZE];

//! Буферы кэша TFT.
static tft9341_cache_buffer_t tft_cache_bufs[TFT_CACHE_BUFS_COUNT] = {
    make_tft9341_cache_buffer(tft_cache_buf_data0, TFT_CACHE_BUF0_SIZE),
    make_tft9341_cache_buffer(tft_cache_buf_data1, TFT_CACHE_BUF1_SIZE)
};
//! Кэш TFT.
static tft9341_cache_t tft_cache = make_tft9341_cache(NULL, TFT_PIXEL_SIZE, tft_cache_bufs, TFT_CACHE_BUFS_COUNT, TFT9341_ROW_COL_REVERSE_MODE);
//! Виртуальный буфер изображения..
static graphics_vbuf_t graph_vbuf = make_tft9341_cache_vbuf();
//! Изображение на экране.
static graphics_t graphics = make_tft9341_cache_graphics(&tft_cache, &graph_vbuf, TFT_WIDTH, TFT_HEIGHT, GRAPHICS_FORMAT_RGB_565);
//static painter_t painter = make_painter(&graphics);

// графика иконок статусбара
static graphics_t icon_statusbar_graphics = make_graphics(icons_statusbar_data, ICONS_STATUSBAR_WIDTH, ICONS_STATUSBAR_HEIGHT, ICONS_STATUSBAR_FORMAT);

// маленький шрифт
static const font_bitmap_t font_arialnarrow12_bitmaps[] = {
	make_font_bitmap_descrs(ARIALNARROW12_PART0_FIRST_CHAR,ARIALNARROW12_PART0_LAST_CHAR, arialnarrow12_part0_data, ARIALNARROW12_PART0_WIDTH, ARIALNARROW12_PART0_HEIGHT, ARIALNARROW12_PART0_GRAPHICS_FORMAT, arialnarrow12_part0_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART1_FIRST_CHAR,ARIALNARROW12_PART1_LAST_CHAR, arialnarrow12_part1_data, ARIALNARROW12_PART1_WIDTH, ARIALNARROW12_PART1_HEIGHT, ARIALNARROW12_PART1_GRAPHICS_FORMAT, arialnarrow12_part1_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART2_FIRST_CHAR,ARIALNARROW12_PART2_LAST_CHAR, arialnarrow12_part2_data, ARIALNARROW12_PART2_WIDTH, ARIALNARROW12_PART2_HEIGHT, ARIALNARROW12_PART2_GRAPHICS_FORMAT, arialnarrow12_part2_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART3_FIRST_CHAR,ARIALNARROW12_PART3_LAST_CHAR, arialnarrow12_part3_data, ARIALNARROW12_PART3_WIDTH, ARIALNARROW12_PART3_HEIGHT, ARIALNARROW12_PART3_GRAPHICS_FORMAT, arialnarrow12_part3_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART4_FIRST_CHAR,ARIALNARROW12_PART4_LAST_CHAR, arialnarrow12_part4_data, ARIALNARROW12_PART4_WIDTH, ARIALNARROW12_PART4_HEIGHT, ARIALNARROW12_PART4_GRAPHICS_FORMAT, arialnarrow12_part4_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART5_FIRST_CHAR,ARIALNARROW12_PART5_LAST_CHAR, arialnarrow12_part5_data, ARIALNARROW12_PART5_WIDTH, ARIALNARROW12_PART5_HEIGHT, ARIALNARROW12_PART5_GRAPHICS_FORMAT, arialnarrow12_part5_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART6_FIRST_CHAR,ARIALNARROW12_PART6_LAST_CHAR, arialnarrow12_part6_data, ARIALNARROW12_PART6_WIDTH, ARIALNARROW12_PART6_HEIGHT, ARIALNARROW12_PART6_GRAPHICS_FORMAT, arialnarrow12_part6_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART7_FIRST_CHAR,ARIALNARROW12_PART7_LAST_CHAR, arialnarrow12_part7_data, ARIALNARROW12_PART7_WIDTH, ARIALNARROW12_PART7_HEIGHT, ARIALNARROW12_PART7_GRAPHICS_FORMAT, arialnarrow12_part7_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART8_FIRST_CHAR,ARIALNARROW12_PART8_LAST_CHAR, arialnarrow12_part8_data, ARIALNARROW12_PART8_WIDTH, ARIALNARROW12_PART8_HEIGHT, ARIALNARROW12_PART8_GRAPHICS_FORMAT, arialnarrow12_part8_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART9_FIRST_CHAR,ARIALNARROW12_PART9_LAST_CHAR, arialnarrow12_part9_data, ARIALNARROW12_PART9_WIDTH, ARIALNARROW12_PART9_HEIGHT, ARIALNARROW12_PART9_GRAPHICS_FORMAT, arialnarrow12_part9_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART10_FIRST_CHAR,ARIALNARROW12_PART10_LAST_CHAR, arialnarrow12_part10_data, ARIALNARROW12_PART10_WIDTH, ARIALNARROW12_PART10_HEIGHT, ARIALNARROW12_PART10_GRAPHICS_FORMAT, arialnarrow12_part10_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART11_FIRST_CHAR,ARIALNARROW12_PART11_LAST_CHAR, arialnarrow12_part11_data, ARIALNARROW12_PART11_WIDTH, ARIALNARROW12_PART11_HEIGHT, ARIALNARROW12_PART11_GRAPHICS_FORMAT, arialnarrow12_part11_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART12_FIRST_CHAR,ARIALNARROW12_PART12_LAST_CHAR, arialnarrow12_part12_data, ARIALNARROW12_PART12_WIDTH, ARIALNARROW12_PART12_HEIGHT, ARIALNARROW12_PART12_GRAPHICS_FORMAT, arialnarrow12_part12_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART13_FIRST_CHAR,ARIALNARROW12_PART13_LAST_CHAR, arialnarrow12_part13_data, ARIALNARROW12_PART13_WIDTH, ARIALNARROW12_PART13_HEIGHT, ARIALNARROW12_PART13_GRAPHICS_FORMAT, arialnarrow12_part13_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART14_FIRST_CHAR,ARIALNARROW12_PART14_LAST_CHAR, arialnarrow12_part14_data, ARIALNARROW12_PART14_WIDTH, ARIALNARROW12_PART14_HEIGHT, ARIALNARROW12_PART14_GRAPHICS_FORMAT, arialnarrow12_part14_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART15_FIRST_CHAR,ARIALNARROW12_PART15_LAST_CHAR, arialnarrow12_part15_data, ARIALNARROW12_PART15_WIDTH, ARIALNARROW12_PART15_HEIGHT, ARIALNARROW12_PART15_GRAPHICS_FORMAT, arialnarrow12_part15_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART16_FIRST_CHAR,ARIALNARROW12_PART16_LAST_CHAR, arialnarrow12_part16_data, ARIALNARROW12_PART16_WIDTH, ARIALNARROW12_PART16_HEIGHT, ARIALNARROW12_PART16_GRAPHICS_FORMAT, arialnarrow12_part16_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART17_FIRST_CHAR,ARIALNARROW12_PART17_LAST_CHAR, arialnarrow12_part17_data, ARIALNARROW12_PART17_WIDTH, ARIALNARROW12_PART17_HEIGHT, ARIALNARROW12_PART17_GRAPHICS_FORMAT, arialnarrow12_part17_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART18_FIRST_CHAR,ARIALNARROW12_PART18_LAST_CHAR, arialnarrow12_part18_data, ARIALNARROW12_PART18_WIDTH, ARIALNARROW12_PART18_HEIGHT, ARIALNARROW12_PART18_GRAPHICS_FORMAT, arialnarrow12_part18_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART19_FIRST_CHAR,ARIALNARROW12_PART19_LAST_CHAR, arialnarrow12_part19_data, ARIALNARROW12_PART19_WIDTH, ARIALNARROW12_PART19_HEIGHT, ARIALNARROW12_PART19_GRAPHICS_FORMAT, arialnarrow12_part19_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART20_FIRST_CHAR,ARIALNARROW12_PART20_LAST_CHAR, arialnarrow12_part20_data, ARIALNARROW12_PART20_WIDTH, ARIALNARROW12_PART20_HEIGHT, ARIALNARROW12_PART20_GRAPHICS_FORMAT, arialnarrow12_part20_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART21_FIRST_CHAR,ARIALNARROW12_PART21_LAST_CHAR, arialnarrow12_part21_data, ARIALNARROW12_PART21_WIDTH, ARIALNARROW12_PART21_HEIGHT, ARIALNARROW12_PART21_GRAPHICS_FORMAT, arialnarrow12_part21_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART22_FIRST_CHAR,ARIALNARROW12_PART22_LAST_CHAR, arialnarrow12_part22_data, ARIALNARROW12_PART22_WIDTH, ARIALNARROW12_PART22_HEIGHT, ARIALNARROW12_PART22_GRAPHICS_FORMAT, arialnarrow12_part22_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART23_FIRST_CHAR,ARIALNARROW12_PART23_LAST_CHAR, arialnarrow12_part23_data, ARIALNARROW12_PART23_WIDTH, ARIALNARROW12_PART23_HEIGHT, ARIALNARROW12_PART23_GRAPHICS_FORMAT, arialnarrow12_part23_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART24_FIRST_CHAR,ARIALNARROW12_PART24_LAST_CHAR, arialnarrow12_part24_data, ARIALNARROW12_PART24_WIDTH, ARIALNARROW12_PART24_HEIGHT, ARIALNARROW12_PART24_GRAPHICS_FORMAT, arialnarrow12_part24_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART25_FIRST_CHAR,ARIALNARROW12_PART25_LAST_CHAR, arialnarrow12_part25_data, ARIALNARROW12_PART25_WIDTH, ARIALNARROW12_PART25_HEIGHT, ARIALNARROW12_PART25_GRAPHICS_FORMAT, arialnarrow12_part25_descrs),
	make_font_bitmap_descrs(ARIALNARROW12_PART26_FIRST_CHAR,ARIALNARROW12_PART26_LAST_CHAR, arialnarrow12_part26_data, ARIALNARROW12_PART26_WIDTH, ARIALNARROW12_PART26_HEIGHT, ARIALNARROW12_PART26_GRAPHICS_FORMAT, arialnarrow12_part26_descrs),
};

static font_t font_arialnarrow12 = make_font_defchar(font_arialnarrow12_bitmaps, ARIALNARROW12_BITMAPS_COUNT, ARIALNARROW12_CHAR_WIDTH, ARIALNARROW12_CHAR_HEIGHT, 0, ARIALNARROW12_DEF_VSPACE, ARIALNARROW12_DEF_CHAR);

// средний шрифт
static const font_bitmap_t font_arialnarrow18_bitmaps[] = {
	make_font_bitmap_descrs(ARIALNARROW18_PART0_FIRST_CHAR,ARIALNARROW18_PART0_LAST_CHAR, arialnarrow18_part0_data, ARIALNARROW18_PART0_WIDTH, ARIALNARROW18_PART0_HEIGHT, ARIALNARROW18_PART0_GRAPHICS_FORMAT, arialnarrow18_part0_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART1_FIRST_CHAR,ARIALNARROW18_PART1_LAST_CHAR, arialnarrow18_part1_data, ARIALNARROW18_PART1_WIDTH, ARIALNARROW18_PART1_HEIGHT, ARIALNARROW18_PART1_GRAPHICS_FORMAT, arialnarrow18_part1_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART2_FIRST_CHAR,ARIALNARROW18_PART2_LAST_CHAR, arialnarrow18_part2_data, ARIALNARROW18_PART2_WIDTH, ARIALNARROW18_PART2_HEIGHT, ARIALNARROW18_PART2_GRAPHICS_FORMAT, arialnarrow18_part2_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART3_FIRST_CHAR,ARIALNARROW18_PART3_LAST_CHAR, arialnarrow18_part3_data, ARIALNARROW18_PART3_WIDTH, ARIALNARROW18_PART3_HEIGHT, ARIALNARROW18_PART3_GRAPHICS_FORMAT, arialnarrow18_part3_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART4_FIRST_CHAR,ARIALNARROW18_PART4_LAST_CHAR, arialnarrow18_part4_data, ARIALNARROW18_PART4_WIDTH, ARIALNARROW18_PART4_HEIGHT, ARIALNARROW18_PART4_GRAPHICS_FORMAT, arialnarrow18_part4_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART5_FIRST_CHAR,ARIALNARROW18_PART5_LAST_CHAR, arialnarrow18_part5_data, ARIALNARROW18_PART5_WIDTH, ARIALNARROW18_PART5_HEIGHT, ARIALNARROW18_PART5_GRAPHICS_FORMAT, arialnarrow18_part5_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART6_FIRST_CHAR,ARIALNARROW18_PART6_LAST_CHAR, arialnarrow18_part6_data, ARIALNARROW18_PART6_WIDTH, ARIALNARROW18_PART6_HEIGHT, ARIALNARROW18_PART6_GRAPHICS_FORMAT, arialnarrow18_part6_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART7_FIRST_CHAR,ARIALNARROW18_PART7_LAST_CHAR, arialnarrow18_part7_data, ARIALNARROW18_PART7_WIDTH, ARIALNARROW18_PART7_HEIGHT, ARIALNARROW18_PART7_GRAPHICS_FORMAT, arialnarrow18_part7_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART8_FIRST_CHAR,ARIALNARROW18_PART8_LAST_CHAR, arialnarrow18_part8_data, ARIALNARROW18_PART8_WIDTH, ARIALNARROW18_PART8_HEIGHT, ARIALNARROW18_PART8_GRAPHICS_FORMAT, arialnarrow18_part8_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART9_FIRST_CHAR,ARIALNARROW18_PART9_LAST_CHAR, arialnarrow18_part9_data, ARIALNARROW18_PART9_WIDTH, ARIALNARROW18_PART9_HEIGHT, ARIALNARROW18_PART9_GRAPHICS_FORMAT, arialnarrow18_part9_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART10_FIRST_CHAR,ARIALNARROW18_PART10_LAST_CHAR, arialnarrow18_part10_data, ARIALNARROW18_PART10_WIDTH, ARIALNARROW18_PART10_HEIGHT, ARIALNARROW18_PART10_GRAPHICS_FORMAT, arialnarrow18_part10_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART11_FIRST_CHAR,ARIALNARROW18_PART11_LAST_CHAR, arialnarrow18_part11_data, ARIALNARROW18_PART11_WIDTH, ARIALNARROW18_PART11_HEIGHT, ARIALNARROW18_PART11_GRAPHICS_FORMAT, arialnarrow18_part11_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART12_FIRST_CHAR,ARIALNARROW18_PART12_LAST_CHAR, arialnarrow18_part12_data, ARIALNARROW18_PART12_WIDTH, ARIALNARROW18_PART12_HEIGHT, ARIALNARROW18_PART12_GRAPHICS_FORMAT, arialnarrow18_part12_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART13_FIRST_CHAR,ARIALNARROW18_PART13_LAST_CHAR, arialnarrow18_part13_data, ARIALNARROW18_PART13_WIDTH, ARIALNARROW18_PART13_HEIGHT, ARIALNARROW18_PART13_GRAPHICS_FORMAT, arialnarrow18_part13_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART14_FIRST_CHAR,ARIALNARROW18_PART14_LAST_CHAR, arialnarrow18_part14_data, ARIALNARROW18_PART14_WIDTH, ARIALNARROW18_PART14_HEIGHT, ARIALNARROW18_PART14_GRAPHICS_FORMAT, arialnarrow18_part14_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART15_FIRST_CHAR,ARIALNARROW18_PART15_LAST_CHAR, arialnarrow18_part15_data, ARIALNARROW18_PART15_WIDTH, ARIALNARROW18_PART15_HEIGHT, ARIALNARROW18_PART15_GRAPHICS_FORMAT, arialnarrow18_part15_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART16_FIRST_CHAR,ARIALNARROW18_PART16_LAST_CHAR, arialnarrow18_part16_data, ARIALNARROW18_PART16_WIDTH, ARIALNARROW18_PART16_HEIGHT, ARIALNARROW18_PART16_GRAPHICS_FORMAT, arialnarrow18_part16_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART17_FIRST_CHAR,ARIALNARROW18_PART17_LAST_CHAR, arialnarrow18_part17_data, ARIALNARROW18_PART17_WIDTH, ARIALNARROW18_PART17_HEIGHT, ARIALNARROW18_PART17_GRAPHICS_FORMAT, arialnarrow18_part17_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART18_FIRST_CHAR,ARIALNARROW18_PART18_LAST_CHAR, arialnarrow18_part18_data, ARIALNARROW18_PART18_WIDTH, ARIALNARROW18_PART18_HEIGHT, ARIALNARROW18_PART18_GRAPHICS_FORMAT, arialnarrow18_part18_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART19_FIRST_CHAR,ARIALNARROW18_PART19_LAST_CHAR, arialnarrow18_part19_data, ARIALNARROW18_PART19_WIDTH, ARIALNARROW18_PART19_HEIGHT, ARIALNARROW18_PART19_GRAPHICS_FORMAT, arialnarrow18_part19_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART20_FIRST_CHAR,ARIALNARROW18_PART20_LAST_CHAR, arialnarrow18_part20_data, ARIALNARROW18_PART20_WIDTH, ARIALNARROW18_PART20_HEIGHT, ARIALNARROW18_PART20_GRAPHICS_FORMAT, arialnarrow18_part20_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART21_FIRST_CHAR,ARIALNARROW18_PART21_LAST_CHAR, arialnarrow18_part21_data, ARIALNARROW18_PART21_WIDTH, ARIALNARROW18_PART21_HEIGHT, ARIALNARROW18_PART21_GRAPHICS_FORMAT, arialnarrow18_part21_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART22_FIRST_CHAR,ARIALNARROW18_PART22_LAST_CHAR, arialnarrow18_part22_data, ARIALNARROW18_PART22_WIDTH, ARIALNARROW18_PART22_HEIGHT, ARIALNARROW18_PART22_GRAPHICS_FORMAT, arialnarrow18_part22_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART23_FIRST_CHAR,ARIALNARROW18_PART23_LAST_CHAR, arialnarrow18_part23_data, ARIALNARROW18_PART23_WIDTH, ARIALNARROW18_PART23_HEIGHT, ARIALNARROW18_PART23_GRAPHICS_FORMAT, arialnarrow18_part23_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART24_FIRST_CHAR,ARIALNARROW18_PART24_LAST_CHAR, arialnarrow18_part24_data, ARIALNARROW18_PART24_WIDTH, ARIALNARROW18_PART24_HEIGHT, ARIALNARROW18_PART24_GRAPHICS_FORMAT, arialnarrow18_part24_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART25_FIRST_CHAR,ARIALNARROW18_PART25_LAST_CHAR, arialnarrow18_part25_data, ARIALNARROW18_PART25_WIDTH, ARIALNARROW18_PART25_HEIGHT, ARIALNARROW18_PART25_GRAPHICS_FORMAT, arialnarrow18_part25_descrs),
	make_font_bitmap_descrs(ARIALNARROW18_PART26_FIRST_CHAR,ARIALNARROW18_PART26_LAST_CHAR, arialnarrow18_part26_data, ARIALNARROW18_PART26_WIDTH, ARIALNARROW18_PART26_HEIGHT, ARIALNARROW18_PART26_GRAPHICS_FORMAT, arialnarrow18_part26_descrs),
};

static font_t font_arialnarrow18 = make_font_defchar(font_arialnarrow18_bitmaps, ARIALNARROW18_BITMAPS_COUNT, ARIALNARROW18_CHAR_WIDTH, ARIALNARROW18_CHAR_HEIGHT, ARIALNARROW18_DEF_HSPACE, ARIALNARROW18_DEF_VSPACE, ARIALNARROW18_DEF_CHAR);

// крупный шрифт
static const font_bitmap_t font_arialbold42_bitmaps[] = {
	make_font_bitmap_descrs(ARIALBOLD42_PART0_FIRST_CHAR,ARIALBOLD42_PART0_LAST_CHAR, arialbold42_part0_data, ARIALBOLD42_PART0_WIDTH, ARIALBOLD42_PART0_HEIGHT, ARIALBOLD42_PART0_GRAPHICS_FORMAT, arialbold42_part0_descrs),
	make_font_bitmap_descrs(ARIALBOLD42_PART1_FIRST_CHAR,ARIALBOLD42_PART1_LAST_CHAR, arialbold42_part1_data, ARIALBOLD42_PART1_WIDTH, ARIALBOLD42_PART1_HEIGHT, ARIALBOLD42_PART1_GRAPHICS_FORMAT, arialbold42_part1_descrs),
	make_font_bitmap_descrs(ARIALBOLD42_PART2_FIRST_CHAR,ARIALBOLD42_PART2_LAST_CHAR, arialbold42_part2_data, ARIALBOLD42_PART2_WIDTH, ARIALBOLD42_PART2_HEIGHT, ARIALBOLD42_PART2_GRAPHICS_FORMAT, arialbold42_part2_descrs),
	make_font_bitmap_descrs(ARIALBOLD42_PART3_FIRST_CHAR,ARIALBOLD42_PART3_LAST_CHAR, arialbold42_part3_data, ARIALBOLD42_PART3_WIDTH, ARIALBOLD42_PART3_HEIGHT, ARIALBOLD42_PART3_GRAPHICS_FORMAT, arialbold42_part3_descrs),
	make_font_bitmap_descrs(ARIALBOLD42_PART4_FIRST_CHAR,ARIALBOLD42_PART4_LAST_CHAR, arialbold42_part4_data, ARIALBOLD42_PART4_WIDTH, ARIALBOLD42_PART4_HEIGHT, ARIALBOLD42_PART4_GRAPHICS_FORMAT, arialbold42_part4_descrs),
	make_font_bitmap_descrs(ARIALBOLD42_PART5_FIRST_CHAR,ARIALBOLD42_PART5_LAST_CHAR, arialbold42_part5_data, ARIALBOLD42_PART5_WIDTH, ARIALBOLD42_PART5_HEIGHT, ARIALBOLD42_PART5_GRAPHICS_FORMAT, arialbold42_part5_descrs),
	make_font_bitmap_descrs(ARIALBOLD42_PART6_FIRST_CHAR,ARIALBOLD42_PART6_LAST_CHAR, arialbold42_part6_data, ARIALBOLD42_PART6_WIDTH, ARIALBOLD42_PART6_HEIGHT, ARIALBOLD42_PART6_GRAPHICS_FORMAT, arialbold42_part6_descrs),
};

static font_t font_arialbold42 = make_font_defchar(font_arialbold42_bitmaps, ARIALBOLD42_BITMAPS_COUNT, ARIALBOLD42_CHAR_WIDTH, ARIALBOLD42_CHAR_HEIGHT, ARIALBOLD42_DEF_HSPACE, ARIALBOLD42_DEF_VSPACE, ARIALBOLD42_DEF_CHAR);

// тема графического интерфейса
static gui_metro_theme_t theme = MAKE_GUI_METRO_THEME(
        THEME_COLOR_GRAY,   //!< Цвет фона.
        THEME_COLOR_BLACK,  //!< Цвет строки состояния.
        THEME_COLOR_WHITE,  //!< Цвет шрифта строки состояния.
        THEME_COLOR_BLUE_D, //!< Цвет плитки.
        THEME_COLOR_ORANGE, //!< Цвет плитки (предупреждение).
        THEME_COLOR_RED_L,  //!< Цвет плитки (авария).
        THEME_COLOR_WHITE,  //!< Цвет шрифта плитки.
        THEME_COLOR_GRAY_L, //!< Цвет фона меню.
        THEME_COLOR_GRAY_D, //!< Цвет фона заголовка меню.
        THEME_COLOR_WHITE,  //!< Цвет шрифта заголовка меню.
        THEME_COLOR_BLACK,  //!< Цвет шрифта меню.
        THEME_COLOR_BLUE_D, //!< Цвет фона выбранного элемента меню.
        THEME_COLOR_WHITE,  //!< Цвет шрифта выбранного элемента меню.
        THEME_COLOR_BLACK,  //!< Цвет фона редактируемого элемента меню.
        THEME_COLOR_WHITE,  //!< Цвет символа редактируемого элемента меню.
        THEME_COLOR_GRAY_D,  //!< Цвет полосы прокрутки.
        &font_arialbold42,  //!< Крупный шрифт виджета.
        &font_arialnarrow18,//!< Средний шрифт меню.
        &font_arialnarrow12 //!< Мелкий шрифт меню.
);

//! Тип структуры графического интерфейса привода.
typedef struct _Drive_Gui {
    lang_id_t language; // язык интерфейса
    
    tft9341_t* tft;
    // GUI.
    gui_metro_t gui;
    gui_widget_t root_widget;
    
    gui_home_t home;
    
    // cтрока состояния
    gui_statusbar_t statusbar;
    // иконки строки состояния
    gui_icon_t icons[GUI_STATUSBAR_ICONS_COUNT];
    
    // главный экран
    gui_tile_t tiles[GUI_HOME_TILES_COUNT];
    
    // меню 
    gui_menu_t menu;
} drive_gui_t;

//! Графический интерфейс привода.
static drive_gui_t gui;

static err_t drive_gui_tft_reset(void)
{
    RETURN_ERR_IF_FAIL(drive_keypad_pins_off(DRIVE_KPD_PIN_TFT_RST));
    RETURN_ERR_IF_FAIL(drive_keypad_wait());
    delay_ms(10);
    
    RETURN_ERR_IF_FAIL(drive_keypad_pins_on(DRIVE_KPD_PIN_TFT_RST));
    RETURN_ERR_IF_FAIL(drive_keypad_wait());
    delay_ms(150);
    
    return E_NO_ERROR;
}

static err_t drive_gui_init_tft(void)
{
    RETURN_ERR_IF_FAIL(drive_gui_tft_reset());
    
    tft9341_madctl_t madctl;
    madctl.row_address_order = TFT9341_ROW_BOTTOM_TO_TOP;//TFT9341_ROW_TOP_TO_BOTTOM;
    madctl.col_address_order = TFT9341_COL_RIGHT_TO_LEFT;//TFT9341_COL_LEFT_TO_RIGHT;
    madctl.row_col_exchange = TFT9341_ROW_COL_REVERSE_MODE;
    madctl.vertical_refresh = TFT9341_REFRESH_TOP_TO_BOTTOM;
    madctl.color_order = TFT9341_COLOR_ORDER_BGR;
    madctl.horizontal_refresh = TFT9341_REFRESH_LEFT_TO_RIGHT;
    
    RETURN_ERR_IF_FAIL(tft9341_set_madctl(gui.tft, &madctl));
    RETURN_ERR_IF_FAIL(tft9341_set_pixel_format(gui.tft, TFT9341_PIXEL_16BIT, TFT9341_PIXEL_16BIT));
    RETURN_ERR_IF_FAIL(tft9341_sleep_out(gui.tft));
    RETURN_ERR_IF_FAIL(tft9341_display_on(gui.tft));
    
    tft_cache.tft = gui.tft;
    
    return E_NO_ERROR;
}

static void make_gui_struct(void) {
            
    gui_widget_init(&gui.root_widget, &gui.gui);
    gui_widget_move(&gui.root_widget, 0, 0);
    gui_widget_resize(&gui.root_widget, TFT_WIDTH, TFT_HEIGHT);
        
    gui_statusbar_init_parent(&gui.statusbar, &gui.gui, &gui.root_widget);
    gui_statusbar_set_graphics(&gui.statusbar, &icon_statusbar_graphics, ICONS_STATUSBAR_COUNT);
    gui_widget_move(GUI_WIDGET(&gui.statusbar), 0, 0);
    gui_widget_resize(GUI_WIDGET(&gui.statusbar), TFT_WIDTH, GUI_STATUSBAR_HEIGHT);
    gui_widget_set_back_color(GUI_WIDGET(&gui.statusbar), theme.color_statusbar);
    gui_widget_set_visible(GUI_WIDGET(&gui.statusbar), true);
    gui_statusbar_set_icons(&gui.statusbar, &gui.icons[0]);
    
    gui_home_init_parent(&gui.home, &gui.gui, &gui.root_widget);
    gui_widget_move(GUI_WIDGET(&gui.home), 0, GUI_STATUSBAR_HEIGHT);
    gui_widget_resize(GUI_WIDGET(&gui.home), TFT_WIDTH, TFT_HEIGHT - GUI_STATUSBAR_HEIGHT);
    gui_widget_set_back_color(GUI_WIDGET(&gui.home), theme.color_back);
    gui_widget_set_visible(GUI_WIDGET(&gui.home), true);
    gui_home_set_on_enter(&gui.home, GUI_HOME_ON_ENTER_PROC(drive_gui_home_on_enter));
    
    drive_gui_tiles_init();
    drive_gui_update_tiles();
    
    gui_menu_init_parent(&gui.menu, &gui.gui, &gui.root_widget);
    gui_menu_set_graphics(&gui.menu, &icon_statusbar_graphics, ICONS_STATUSBAR_COUNT);
    gui_widget_move(GUI_WIDGET(&gui.menu), 0, GUI_STATUSBAR_HEIGHT);
    gui_widget_resize(GUI_WIDGET(&gui.menu), TFT_WIDTH, TFT_HEIGHT - GUI_STATUSBAR_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.menu), GUI_BORDER_NONE);
    gui_widget_set_back_color(GUI_WIDGET(&gui.menu), THEME_COLOR_GRAY_L);
    gui_widget_set_visible(GUI_WIDGET(&gui.menu), false);
    gui_menu_set_on_home(&gui.menu, GUI_MENU_ON_HOME_PROC(drive_gui_menu_on_home));
}

void drive_gui_tiles_init() 
{
    int i;
    graphics_pos_t x = GUI_TILE_SEP;
    graphics_pos_t y = GUI_TILE_SEP;
    for (i = 0; i < GUI_HOME_TILES_HEIGHT; i++) {
        int j;
        x = GUI_TILE_SEP;
        for (j = 0; j < GUI_HOME_TILES_WIDTH; j++) {
            int k = i * GUI_HOME_TILES_HEIGHT + j;
            gui_tile_t* tile = &gui.tiles[k];
            tile->id = k;
            gui_tile_init_parent(tile, &gui.gui, GUI_WIDGET(&gui.home));
            gui_widget_move(GUI_WIDGET(tile), x, y);
            gui_widget_resize(GUI_WIDGET(tile), GUI_TILE_WIDTH, GUI_TILE_HEIGHT);
            gui_widget_set_border(GUI_WIDGET(tile), GUI_BORDER_NONE);
            gui_widget_set_back_color(GUI_WIDGET(tile), THEME_COLOR_BLUE_D);
            gui_widget_set_visible(GUI_WIDGET(tile), true);
            x += (GUI_TILE_WIDTH + GUI_TILE_SEP);
        }
        y += (GUI_TILE_HEIGHT + GUI_TILE_SEP);
    }
}

static void make_gui(void)
{    
    // инициализация локализации
    localization_init(trs_main, TRANSLATIONS_COUNT(trs_main));
    localization_set_default_lang(TR_LANG_ID_RU);
    localization_set_default_text("No translation");
    
    gui.language = localization_lang();
    
    drive_gui_check_language();
        
    // инициализация графического интерфейcа
    make_gui_struct();
    gui_metro_set_root_widget(&gui.gui, &gui.root_widget);
    gui_metro_set_focus_widget(&(gui.gui), GUI_WIDGET(&gui.home));
    
    gui_widget_set_visible(&gui.root_widget, true);
}

bool drive_gui_check_language(void) {
    param_t* param = settings_param_by_id(PARAM_ID_GUI_LANGUAGE);
    if (param) {
        uint32_t val = settings_param_valueu(param);
        if (gui.language != gui_languages[val]) {
            gui.language = gui_languages[val];
            localization_set_lang(gui.language);
            return true; // язык изменился
        }
    }
    return false; // язык не изменился
}

// перерисовка графич. интерфейса
void drive_gui_repaint(void)
{
    gui_metro_repaint(&gui.gui, NULL);
}

void drive_gui_update(void)
{    
    gui_statusbar_update_icons(&gui.statusbar, false);
    gui_statusbar_update(&gui.statusbar, NULL);
    
    // проверка языка интерфейса
    if (drive_gui_check_language()) drive_gui_repaint();
    
    drive_gui_update_tiles();

    gui_menu_on_timer_home_action(&gui.menu);
}

err_t drive_gui_init(drive_gui_init_t* gui_is)
{
    if(gui_is == NULL) return E_NULL_POINTER;
    if(gui_is->tft == NULL) return E_NULL_POINTER;
    
    gui.tft = gui_is->tft;
    
    drive_modbus_last_time = (uint32_t)time(NULL) - DRIVE_MODBUS_GUI_ICON_IDLE_SEC;
    
    RETURN_ERR_IF_FAIL(drive_gui_init_tft());
    RETURN_ERR_IF_FAIL(gui_metro_init(&gui.gui, &graphics, &theme));
    
    make_gui();
    
    return E_NO_ERROR;
}

void drive_gui_on_key_pressed(keycode_t key)
{
    gui_metro_key_pressed(&gui.gui, key);
}

void drive_gui_on_key_released(keycode_t key)
{
    gui_metro_key_released(&gui.gui, key);
}

void drive_gui_home_on_enter(gui_home_t* home)
{
    gui_home_tiles_set_visible(&gui.tiles[0], false);

    menu_explorer_init(&gui.menu.explorer, &gui.menu.menu);
    gui_widget_set_visible(GUI_WIDGET(&gui.menu), true);
    gui_metro_set_focus_widget(&(gui.gui), GUI_WIDGET(&gui.menu));
    gui_menu_init_counters(&gui.menu, &(gui.gui));
}

void drive_gui_menu_on_home(gui_menu_t* menu)
{
    gui_widget_set_visible(GUI_WIDGET(&gui.menu), false);
    gui_metro_set_focus_widget(&(gui.gui), GUI_WIDGET(&gui.home));
    
    gui_home_tiles_set_visible(&gui.tiles[0], true);
}

void drive_gui_update_tiles() 
{
    int i;
    for (i = 0; i < GUI_HOME_TILES_COUNT; i++) {
        gui_tile_t* tile = &gui.tiles[i];
        param_t* param = settings_param_by_id(gui_tiles_values[i]);
        uint32_t val = settings_param_valueu(param);
        if (val < 0 || val > GUI_TILE_TYPES_COUNT) val = 0;
        
        // изменения показания
        gui_tile_set_type(tile, gui_tile_types[val]);
        // обновление значения
        gui_tile_repaint_value(&gui.tiles[i], NULL);
    }
}

void drive_gui_modbus_set_last_time()
{
    drive_modbus_last_time = (uint32_t)time(NULL);
}

bool drive_gui_modbus_status_work()
{
    uint32_t cur = (uint32_t)time(NULL);
    // Если были прием/передача N секунд - статус WORK
    return ((cur - drive_modbus_last_time) < DRIVE_MODBUS_GUI_ICON_WORK_SEC);
}

bool drive_gui_modbus_status_idle()
{
    uint32_t cur = (uint32_t)time(NULL);
    // Если не было приема/передачи N секунд 
    // и при этом последний прием/передача была N2 сек. назад - статус IDLE
    return ((cur - drive_modbus_last_time) >= DRIVE_MODBUS_GUI_ICON_WORK_SEC)\
            && ((cur - drive_modbus_last_time) < DRIVE_MODBUS_GUI_ICON_IDLE_SEC);
}

bool drive_gui_menu_user_is(menu_user_t user)
{
    return gui.menu.explorer.user == user;
}

counter_t drive_gui_get_touch_menu_explorer()
{
    return gui.menu.explorer.touch;
}
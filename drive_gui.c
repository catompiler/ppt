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

// позиция иконки статусбара
#define gui_statusbar_icon_pos(pos) pos * ICONS_STATUSBAR_HEIGHT

// наименования и единицы измерения плиток
HOME_TILES_TEXT(home_tiles_text)

// отображаемые значения плиток
HOME_TILES_VALUES(home_tiles_values)


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
    
    gui_home_init_parent(&gui.home, &gui.gui, &gui.root_widget);
    gui_widget_move(GUI_WIDGET(&gui.home), 0, GUI_STATUSBAR_HEIGHT);
    gui_widget_resize(GUI_WIDGET(&gui.home), TFT_WIDTH, TFT_HEIGHT - GUI_STATUSBAR_HEIGHT);
    gui_widget_set_back_color(GUI_WIDGET(&gui.home), theme.color_back);
    gui_widget_set_visible(GUI_WIDGET(&gui.home), true);
    gui_home_set_on_enter(&gui.home, GUI_HOME_ON_ENTER_PROC(drive_gui_home_on_enter));
    
    int i;
    graphics_pos_t x = GUI_TILE_SEP;
    graphics_pos_t y = 0;
    for (i = 0; i < GUI_HOME_TILES_WIDTH; i++) {
        int j;
        y = GUI_TILE_SEP;
        for (j = 0; j < GUI_HOME_TILES_HEIGHT; j++) {
            int k = i * GUI_HOME_TILES_WIDTH + j;
            gui_tile_t* tile = &gui.tiles[k];
            gui_tile_init_parent(tile, &gui.gui, GUI_WIDGET(&gui.home));
            gui_tile_set_caption(tile, home_tiles_text[k][HOME_TILE_CAPTION]);
            gui_tile_set_unit(tile, home_tiles_text[k][HOME_TILE_UNIT]);
            gui_widget_move(GUI_WIDGET(tile), x, y);
            gui_widget_resize(GUI_WIDGET(tile), GUI_TILE_WIDTH, GUI_TILE_HEIGHT);
            gui_widget_set_border(GUI_WIDGET(tile), GUI_BORDER_NONE);
            gui_widget_set_back_color(GUI_WIDGET(tile), THEME_COLOR_BLUE_D);
            gui_widget_set_visible(GUI_WIDGET(tile), true);
            y += (GUI_TILE_HEIGHT + GUI_TILE_SEP);
        }
        x += (GUI_TILE_WIDTH + GUI_TILE_SEP);
    }

    /*
    for (i = 0; i < GUI_STATUSBAR_ICONS_COUNT; i++) {
        gui_icon_t* icon = &gui.icons[i];
        icon->value = rand() % ICONS_STATUSBAR_VAL_READY + ICONS_STATUSBAR_VAL_CNTRL_BUS;
        icon->count = 0;
        icon->current = ICONS_STATUSBAR_VAL_NOTHING;
        icon->list = NULL;
        icon->color = rand() % 65535;
    }
    */
    
    gui_statusbar_set_icons(&gui.statusbar, &gui.icons[0]);
    
    gui_menu_init_parent(&gui.menu, &gui.gui, &gui.root_widget);
    gui_widget_move(GUI_WIDGET(&gui.menu), 0, GUI_STATUSBAR_HEIGHT);
    gui_widget_resize(GUI_WIDGET(&gui.menu), TFT_WIDTH, TFT_HEIGHT - GUI_STATUSBAR_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.menu), GUI_BORDER_NONE);
    gui_widget_set_back_color(GUI_WIDGET(&gui.menu), THEME_COLOR_GRAY_L);
    gui_widget_set_visible(GUI_WIDGET(&gui.menu), false);
    gui_menu_set_on_home(&gui.menu, GUI_MENU_ON_HOME_PROC(drive_gui_menu_on_home));
    
    /*
    gui_label_init_parent(&gui.label_time, &gui.gui, &gui.parent_widget);
    gui_widget_move(GUI_WIDGET(&gui.label_time), 4, 28);
    gui_widget_resize(GUI_WIDGET(&gui.label_time), 154, 102);
    gui_widget_set_border(GUI_WIDGET(&gui.label_time), GUI_BORDER_NONE);
    gui_widget_set_back_color(GUI_WIDGET(&gui.label_time), THEME_COLOR_BLUE_D);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_time), true);
    */
}

static void make_gui(void)
{    
    // инициализация локализации
    localization_init(trs_main, TRANSLATIONS_COUNT(trs_main));
    localization_set_default_lang(TR_LANG_ID_RU);
    localization_set_default_text("No translation");
    localization_set_lang(TR_LANG_ID_EN);
    
    // инициализация графического интерфейча
    make_gui_struct();
    gui_metro_set_root_widget(&gui.gui, &gui.root_widget);
    gui_metro_set_focus_widget(&(gui.gui), GUI_WIDGET(&gui.home));
    
    gui_widget_set_visible(&gui.root_widget, true);
}

// перерисовка графич. интерфейса
void drive_gui_repaint(void)
{
    gui_metro_repaint(&gui.gui, NULL);
}

void drive_gui_update(void)
{
    static char pos = 0;
    
    gui_statusbar_update_icons(&gui.statusbar, false);
    gui_statusbar_update(&gui.statusbar, NULL);
    
    int i;
    for (i = 0; i < GUI_HOME_TILES_COUNT; i++) {
        drive_gui_update_tile(&gui.tiles[i], home_tiles_values[i]);
    }
        
    gui_menu_on_timer_home_action(&gui.menu);
    
    pos++;
    // Не, круче rev = (bool)(rand() % 2); // :)
    static bool rev;
    if (pos > ICONS_STATUSBAR_COUNT) {
        pos = 0;
        if (rev) {
            //gui_widget_set_visible(GUI_WIDGET(&gui.home), true);
            //localization_set_lang(TR_LANG_ID_EN);
            //gui_widget_set_visible(GUI_WIDGET(&gui.menu), true);
            //gui_widget_repaint(GUI_WIDGET(&gui.root_widget), NULL);
            rev = false;
        }
        else {
            //gui_widget_set_visible(GUI_WIDGET(&gui.menu), false);
            //localization_set_lang(TR_LANG_ID_RU);
            //gui_widget_set_visible(GUI_WIDGET(&gui.home), false);
            //gui_widget_repaint(GUI_WIDGET(&gui.root_widget), NULL);
            rev = true;
        }
    }
}

err_t drive_gui_init(drive_gui_init_t* gui_is)
{
    if(gui_is == NULL) return E_NULL_POINTER;
    if(gui_is->tft == NULL) return E_NULL_POINTER;
    
    gui.tft = gui_is->tft;
    
    RETURN_ERR_IF_FAIL(drive_gui_init_tft());
    RETURN_ERR_IF_FAIL(gui_metro_init(&gui.gui, &graphics, &theme));
    
    make_gui();
    
    return E_NO_ERROR;
}

void drive_gui_on_key_pressed(keycode_t key)
{
    switch (key) {
        //case KEY_LEFT:
        //case KEY_MINUS:
        case KEY_DOWN:
            //gui_focus_prev_widget(&gui.gui);
            //menu_explorer_next(&explorer);
            break;
        //case KEY_RIGHT:
        //case KEY_PLUS:
        case KEY_UP:
            //gui_focus_next_widget(&gui.gui);
            //menu_explorer_prev(&explorer);
            break;
        case KEY_ENTER:
            //
            break;
        case KEY_ESC:
            //menu_explorer_out(&explorer);
            break;
        default:
            break;
    }
    gui_metro_key_pressed(&gui.gui, key);
}

void drive_gui_on_key_released(keycode_t key)
{
    /**
    switch(key){
        case KEY_LEFT:
        case KEY_MINUS:
        case KEY_RIGHT:
        case KEY_PLUS:
            return;
        default:
            break;
    }
     */
    gui_metro_key_released(&gui.gui, key);
}

void drive_gui_home_on_enter(gui_home_t* home)
{
    gui_home_tiles_set_visible(&gui.tiles[0], false);

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

void drive_gui_update_tile(gui_tile_t* tile, size_t value) 
{
    if(drive_power_data_avail(DRIVE_POWER_CHANNELS)) {
        char str[9];
        uint32_t int_part = fixed32_get_int(drive_power_channel_real_value(value));
        //fract_part = fixed32_get_fract_by_denom((int64_t)fixed_abs(drive_power_channel_real_value(DRIVE_POWER_Urot)), 10);
        snprintf(str, 9, "% 4d", (int)int_part);
        gui_tile_set_value(tile, str);
        /**
        gui_tile_status_t new_tile_status = GUI_TILE_STATUS_OK;
        
        if (cur3 > 150) {
            new_tile_status = GUI_TILE_STATUS_ALARM;
        } else if (cur3 > 70) {
            new_tile_status = GUI_TILE_STATUS_WARNING;
        }
        
        if (new_tile_status != gui_tile_status(&gui.tiles[3])) {
            gui_widget_set_repaint_enable(GUI_WIDGET(&gui.tiles[3]), false);
            gui_tile_set_value(&gui.tiles[3], time_str);
            gui_widget_set_repaint_enable(GUI_WIDGET(&gui.tiles[3]), true);
            gui_tile_set_status(&gui.tiles[3], new_tile_status);
        }
        else {
            gui_tile_set_value(&gui.tiles[3], time_str);
        }
        */
    }
}

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
#include "graphics/font_5x8_utf8.h"
#include "graphics/font_10x16_utf8.h"
#include "gui/gui.h"
#include "gui/gui_object.h"
#include "gui/gui_widget.h"
#include "gui/gui_label.h"
#include "gui/gui_number_label.h"
#include "gui/gui_checkbox.h"
#include "gui/gui_spinbox.h"
#include "gui/gui_button.h"
#include "drive_events.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "drive_phase_sync.h"
//#include "input/key_layout_ru.h"
//#include "input/key_layout_en.h"



//! Размер пиксела - 2 байта (16 бит).
#define TFT_PIXEL_SIZE 2
//! Ширина экрана.
#define TFT_WIDTH 320
//! Высота экрана.
#define TFT_HEIGHT 240
//! Число буферов кэша TFT.
#define TFT_CACHE_BUFS_COUNT 2
//! Размер первого буфера.
#define TFT_CACHE_BUF0_PIXELS 240
#define TFT_CACHE_BUF0_SIZE (TFT_CACHE_BUF0_PIXELS * TFT_PIXEL_SIZE)
//! Размер второго буфера.
#define TFT_CACHE_BUF1_PIXELS 80
#define TFT_CACHE_BUF1_SIZE (TFT_CACHE_BUF1_PIXELS * TFT_PIXEL_SIZE)
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
//! Битмапы шрифта 5x8.
static const font_bitmap_t font_5x8_utf8_bitmaps[] = {
    make_font_bitmap(32, 127, font_5x8_utf8_part0_data, FONT_5X8_UTF8_PART0_WIDTH, FONT_5X8_UTF8_PART0_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0xb0, 0xb0, font_5x8_utf8_part1_data, FONT_5X8_UTF8_PART1_WIDTH, FONT_5X8_UTF8_PART1_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0x400, 0x451, font_5x8_utf8_part2_data, FONT_5X8_UTF8_PART2_WIDTH, FONT_5X8_UTF8_PART2_HEIGHT, GRAPHICS_FORMAT_BW_1_V)
};
//! Шрифт 5x8.
static font_t font5x8 = make_font(font_5x8_utf8_bitmaps, 3, 5, 8, 1, 0);
//! Битмапы шрифта 10x16.
const font_bitmap_t font_10x16_utf8_bitmaps[] = {
    make_font_bitmap(32, 127, font_10x16_utf8_part0_data, FONT_10X16_UTF8_PART0_WIDTH, FONT_10X16_UTF8_PART0_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0xb0, 0xb0, font_10x16_utf8_part1_data, FONT_10X16_UTF8_PART1_WIDTH, FONT_10X16_UTF8_PART1_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0x400, 0x451, font_10x16_utf8_part2_data, FONT_10X16_UTF8_PART2_WIDTH, FONT_10X16_UTF8_PART2_HEIGHT, GRAPHICS_FORMAT_BW_1_V)
};
//! Шрифт 10x16.
static font_t font10x16 = make_font(font_10x16_utf8_bitmaps, 3, 10, 16, 1, 0);

//! Раскладки клавиатуры.
/*#define KEY_LAYOUTS_COUNT 2
const key_layout_t* key_layouts[KEY_LAYOUTS_COUNT] = {
    &key_layout_en, &key_layout_ru
};*/

#define MAKE_RGB(r, g, b) TFT9341_MAKE_RGB565(r, g, b)
#define THEME_COLOR_BACK MAKE_RGB(0xff, 0xff, 0xff)
#define THEME_COLOR_FRONT MAKE_RGB(0x00, 0x00, 0x00)
#define THEME_COLOR_PANEL MAKE_RGB(0xf8, 0xf8, 0xf8)//0xf0
#define THEME_COLOR_WIDGET MAKE_RGB(0xe1, 0xe1, 0xe1)
#define THEME_COLOR_BORDER MAKE_RGB(0xb1, 0xb1, 0xb1)
#define THEME_COLOR_FONT MAKE_RGB(0x00, 0x00, 0x00)
#define THEME_COLOR_FOCUS MAKE_RGB(0x00, 0x78, 0xd7)
#define THEME_COLOR_PRESSED MAKE_RGB(0xcd, 0xe3, 0xf8)

static gui_theme_t theme = MAKE_GUI_THEME(
                    THEME_COLOR_BACK, THEME_COLOR_FRONT,
                    THEME_COLOR_PANEL, THEME_COLOR_WIDGET,
                    THEME_COLOR_BORDER, THEME_COLOR_FONT,
                    THEME_COLOR_FOCUS, THEME_COLOR_PRESSED,
                    &font5x8, &font10x16);





//! Тип структуры графического интерфейса привода.
typedef struct _Drive_Gui {
    tft9341_t* tft;
    // GUI.
    gui_t gui;
    gui_widget_t root_widget;
    gui_widget_t parent_widget;
    gui_label_t label_time;
    gui_label_t label_time_val;
    gui_label_t label_val;
    gui_label_t label_errs;
    gui_label_t label_pwr_errs;
    gui_label_t label_state;
    gui_label_t label_ref;
    gui_label_t label_last_pwr_errs;
    gui_spinbox_t spinbox_ref;
    gui_button_t button_start;
    gui_button_t button_stop;
    gui_label_t lbl_adc1_in1;
    gui_label_t lbl_adc1_in2;
    gui_label_t lbl_adc1_in3;
    gui_label_t lbl_adc1_in4;
    gui_label_t lbl_adc2_in1;
    gui_label_t lbl_adc2_in2;
    gui_label_t lbl_adc2_in3;
    gui_label_t lbl_adc2_in4;
    gui_label_t lbl_adc3_in1;
    gui_label_t lbl_adc3_in2;
    gui_label_t lbl_adc3_in3;
    gui_number_label_t label_num;
    gui_number_label_t label_num_errs;
    gui_number_label_t label_num_pwr_errs;
    gui_number_label_t label_num_state;
    gui_number_label_t label_num_last_pwr_errs;
    gui_number_label_t lbl_num_adc1_in1;
    gui_number_label_t lbl_num_adc1_in2;
    gui_number_label_t lbl_num_adc1_in3;
    gui_number_label_t lbl_num_adc1_in4;
    gui_number_label_t lbl_num_adc2_in1;
    gui_number_label_t lbl_num_adc2_in2;
    gui_number_label_t lbl_num_adc2_in3;
    gui_number_label_t lbl_num_adc2_in4;
    gui_number_label_t lbl_num_adc3_in1;
    gui_number_label_t lbl_num_adc3_in2;
    gui_number_label_t lbl_num_adc3_in3;
    //
    drive_event_t last_event;
} drive_gui_t;

//! Графический интерфейс привода.
static drive_gui_t gui;


#define GUI_NUMBER_LABEL_DECIMALS 2

#define GUI_LABEL_HEIGHT 15
#define GUI_LABEL_TOP(N) (5 + GUI_LABEL_HEIGHT * N)




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


static void spinbox_reference_on_value_changed(gui_spinbox_t* spinbox, int value)
{
    drive_set_reference(CLAMP(value, 0, 100));
}

static void button_start_on_clicked(gui_button_t* button)
{
    drive_start();
}

static void button_stop_on_clicked(gui_button_t* button)
{
    drive_stop();
}

static void make_gui_adc(void)
{
    gui_label_init_parent(&gui.lbl_adc1_in1, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.lbl_adc1_in1, "Ua:");
    gui_widget_move(GUI_WIDGET(&gui.lbl_adc1_in1), 120, GUI_LABEL_TOP(0));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_adc1_in1), 60, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_adc1_in1), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_adc1_in1), true);
    
    gui_label_init_parent(&gui.lbl_adc1_in2, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.lbl_adc1_in2, "Ub:");
    gui_widget_move(GUI_WIDGET(&gui.lbl_adc1_in2), 120, GUI_LABEL_TOP(1));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_adc1_in2), 60, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_adc1_in2), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_adc1_in2), true); 
    
    gui_label_init_parent(&gui.lbl_adc1_in3, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.lbl_adc1_in3, "Uc:");
    gui_widget_move(GUI_WIDGET(&gui.lbl_adc1_in3), 120, GUI_LABEL_TOP(2));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_adc1_in3), 60, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_adc1_in3), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_adc1_in3), true);

    gui_label_init_parent(&gui.lbl_adc1_in4, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.lbl_adc1_in4, "Urot:");
    gui_widget_move(GUI_WIDGET(&gui.lbl_adc1_in4), 120, GUI_LABEL_TOP(3));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_adc1_in4), 60, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_adc1_in4), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_adc1_in4), true);
    
    gui_label_init_parent(&gui.lbl_adc2_in1, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.lbl_adc2_in1, "Ia:");
    gui_widget_move(GUI_WIDGET(&gui.lbl_adc2_in1), 120, GUI_LABEL_TOP(4));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_adc2_in1), 60, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_adc2_in1), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_adc2_in1), true);
    
    gui_label_init_parent(&gui.lbl_adc2_in2, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.lbl_adc2_in2, "Ib:");
    gui_widget_move(GUI_WIDGET(&gui.lbl_adc2_in2), 120, GUI_LABEL_TOP(5));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_adc2_in2), 60, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_adc2_in2), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_adc2_in2), true);
    
    gui_label_init_parent(&gui.lbl_adc2_in3, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.lbl_adc2_in3, "Ic:");
    gui_widget_move(GUI_WIDGET(&gui.lbl_adc2_in3), 120, GUI_LABEL_TOP(6));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_adc2_in3), 60, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_adc2_in3), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_adc2_in3), true);

    gui_label_init_parent(&gui.lbl_adc2_in4, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.lbl_adc2_in4, "Irot:");
    gui_widget_move(GUI_WIDGET(&gui.lbl_adc2_in4), 120, GUI_LABEL_TOP(7));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_adc2_in4), 60, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_adc2_in4), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_adc2_in4), true);
    
    gui_label_init_parent(&gui.lbl_adc3_in1, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.lbl_adc3_in1, "Iexc:");
    gui_widget_move(GUI_WIDGET(&gui.lbl_adc3_in1), 120, GUI_LABEL_TOP(8));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_adc3_in1), 60, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_adc3_in1), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_adc3_in1), true);
    
    gui_label_init_parent(&gui.lbl_adc3_in2, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.lbl_adc3_in2, "Iref:");
    gui_widget_move(GUI_WIDGET(&gui.lbl_adc3_in2), 120, GUI_LABEL_TOP(9));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_adc3_in2), 60, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_adc3_in2), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_adc3_in2), true); 
    
    gui_label_init_parent(&gui.lbl_adc3_in3, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.lbl_adc3_in3, "Ifan:");
    gui_widget_move(GUI_WIDGET(&gui.lbl_adc3_in3), 120, GUI_LABEL_TOP(10));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_adc3_in3), 60, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_adc3_in3), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_adc3_in3), true);
    
    gui_number_label_init_parent(&gui.lbl_num_adc1_in1, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.lbl_num_adc1_in1, 1);//0x1234
    gui_number_label_set_format(&gui.lbl_num_adc1_in1, GUI_NUMBER_LABEL_FIX);
    gui_number_label_set_decimals(&gui.lbl_num_adc1_in1, GUI_NUMBER_LABEL_DECIMALS);
    gui_widget_move(GUI_WIDGET(&gui.lbl_num_adc1_in1), 180, GUI_LABEL_TOP(0));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_num_adc1_in1), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_num_adc1_in1), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_num_adc1_in1), true);
    
    gui_number_label_init_parent(&gui.lbl_num_adc1_in2, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.lbl_num_adc1_in2, 2);//0x1234
    gui_number_label_set_format(&gui.lbl_num_adc1_in2, GUI_NUMBER_LABEL_FIX);
    gui_number_label_set_decimals(&gui.lbl_num_adc1_in2, GUI_NUMBER_LABEL_DECIMALS);
    gui_widget_move(GUI_WIDGET(&gui.lbl_num_adc1_in2), 180, GUI_LABEL_TOP(1));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_num_adc1_in2), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_num_adc1_in2), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_num_adc1_in2), true);
    
    gui_number_label_init_parent(&gui.lbl_num_adc1_in3, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.lbl_num_adc1_in3, 3);//0x1234
    gui_number_label_set_format(&gui.lbl_num_adc1_in3, GUI_NUMBER_LABEL_FIX);
    gui_number_label_set_decimals(&gui.lbl_num_adc1_in3, GUI_NUMBER_LABEL_DECIMALS);
    gui_widget_move(GUI_WIDGET(&gui.lbl_num_adc1_in3), 180, GUI_LABEL_TOP(2));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_num_adc1_in3), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_num_adc1_in3), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_num_adc1_in3), true);
    
    gui_number_label_init_parent(&gui.lbl_num_adc1_in4, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.lbl_num_adc1_in4, 3);//0x1234
    //gui_number_label_set_format(&gui.lbl_num_adc1_in4, GUI_NUMBER_LABEL_DEC);
    gui_number_label_set_format(&gui.lbl_num_adc1_in4, GUI_NUMBER_LABEL_FIX);
    gui_number_label_set_decimals(&gui.lbl_num_adc1_in4, GUI_NUMBER_LABEL_DECIMALS);
    gui_widget_move(GUI_WIDGET(&gui.lbl_num_adc1_in4), 180, GUI_LABEL_TOP(3));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_num_adc1_in4), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_num_adc1_in4), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_num_adc1_in4), true);
    
    gui_number_label_init_parent(&gui.lbl_num_adc2_in1, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.lbl_num_adc2_in1, 4);//0x1234
    //gui_number_label_set_format(&gui.lbl_num_adc2_in1, GUI_NUMBER_LABEL_DEC);
    gui_number_label_set_format(&gui.lbl_num_adc2_in1, GUI_NUMBER_LABEL_FIX);
    gui_number_label_set_decimals(&gui.lbl_num_adc2_in1, GUI_NUMBER_LABEL_DECIMALS);
    gui_widget_move(GUI_WIDGET(&gui.lbl_num_adc2_in1), 180, GUI_LABEL_TOP(4));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_num_adc2_in1), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_num_adc2_in1), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_num_adc2_in1), true);
    
    gui_number_label_init_parent(&gui.lbl_num_adc2_in2, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.lbl_num_adc2_in2, 5);//0x1234
    //gui_number_label_set_format(&gui.lbl_num_adc2_in2, GUI_NUMBER_LABEL_DEC);
    gui_number_label_set_format(&gui.lbl_num_adc2_in2, GUI_NUMBER_LABEL_FIX);
    gui_number_label_set_decimals(&gui.lbl_num_adc2_in2, GUI_NUMBER_LABEL_DECIMALS);
    gui_widget_move(GUI_WIDGET(&gui.lbl_num_adc2_in2), 180, GUI_LABEL_TOP(5));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_num_adc2_in2), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_num_adc2_in2), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_num_adc2_in2), true);
    
    gui_number_label_init_parent(&gui.lbl_num_adc2_in3, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.lbl_num_adc2_in3, 6);//0x1234
    //gui_number_label_set_format(&gui.lbl_num_adc2_in3, GUI_NUMBER_LABEL_DEC);
    gui_number_label_set_format(&gui.lbl_num_adc2_in3, GUI_NUMBER_LABEL_FIX);
    gui_number_label_set_decimals(&gui.lbl_num_adc2_in3, GUI_NUMBER_LABEL_DECIMALS);
    gui_widget_move(GUI_WIDGET(&gui.lbl_num_adc2_in3), 180, GUI_LABEL_TOP(6));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_num_adc2_in3), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_num_adc2_in3), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_num_adc2_in3), true);
 
    gui_number_label_init_parent(&gui.lbl_num_adc2_in4, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.lbl_num_adc2_in4, 6);//0x1234
    //gui_number_label_set_format(&gui.lbl_num_adc2_in4, GUI_NUMBER_LABEL_DEC);
    gui_number_label_set_format(&gui.lbl_num_adc2_in4, GUI_NUMBER_LABEL_FIX);
    gui_number_label_set_decimals(&gui.lbl_num_adc2_in4, GUI_NUMBER_LABEL_DECIMALS);
    gui_widget_move(GUI_WIDGET(&gui.lbl_num_adc2_in4), 180, GUI_LABEL_TOP(7));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_num_adc2_in4), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_num_adc2_in4), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_num_adc2_in4), true);
    
    gui_number_label_init_parent(&gui.lbl_num_adc3_in1, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.lbl_num_adc3_in1, 1);//0x1234
    gui_number_label_set_format(&gui.lbl_num_adc3_in1, GUI_NUMBER_LABEL_FIX);
    gui_number_label_set_decimals(&gui.lbl_num_adc3_in1, GUI_NUMBER_LABEL_DECIMALS);
    gui_widget_move(GUI_WIDGET(&gui.lbl_num_adc3_in1), 180, GUI_LABEL_TOP(8));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_num_adc3_in1), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_num_adc3_in1), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_num_adc3_in1), true);
    
    gui_number_label_init_parent(&gui.lbl_num_adc3_in2, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.lbl_num_adc3_in2, 2);//0x1234
    gui_number_label_set_format(&gui.lbl_num_adc3_in2, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&gui.lbl_num_adc3_in2), 180, GUI_LABEL_TOP(9));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_num_adc3_in2), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_num_adc3_in2), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_num_adc3_in2), true);
    
    gui_number_label_init_parent(&gui.lbl_num_adc3_in3, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.lbl_num_adc3_in3, 3);//0x1234
    gui_number_label_set_format(&gui.lbl_num_adc3_in3, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&gui.lbl_num_adc3_in3), 180, GUI_LABEL_TOP(10));
    gui_widget_resize(GUI_WIDGET(&gui.lbl_num_adc3_in3), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.lbl_num_adc3_in3), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.lbl_num_adc3_in3), true);
}

static void make_err_gui(void)
{
    gui_label_init_parent(&gui.label_last_pwr_errs, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.label_last_pwr_errs, "Псл ошп:");
    gui_widget_move(GUI_WIDGET(&gui.label_last_pwr_errs), 5, GUI_LABEL_TOP(7));
    gui_widget_resize(GUI_WIDGET(&gui.label_last_pwr_errs), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_last_pwr_errs), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.label2), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_last_pwr_errs), true);
    
    gui_number_label_init_parent(&gui.label_num_last_pwr_errs, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.label_num_last_pwr_errs, 0);//0x1234
    gui_number_label_set_format(&gui.label_num_last_pwr_errs, GUI_NUMBER_LABEL_HEX);
    gui_widget_move(GUI_WIDGET(&gui.label_num_last_pwr_errs), 60, GUI_LABEL_TOP(7));
    gui_widget_resize(GUI_WIDGET(&gui.label_num_last_pwr_errs), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_num_last_pwr_errs), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.label3), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_num_last_pwr_errs), true);
}

static void make_gui(void)
{
    gui_widget_init(&gui.root_widget, &gui.gui);
    gui_widget_move(&gui.root_widget, 0, 0);
    gui_widget_resize(&gui.root_widget, TFT_WIDTH, TFT_HEIGHT);
    
    gui_widget_init_parent(&gui.parent_widget, &gui.gui, &gui.root_widget);
    gui_widget_move(&gui.parent_widget, 5, 5);
    gui_widget_resize(&gui.parent_widget, 310, 230);
    gui_widget_set_border(&gui.parent_widget, GUI_BORDER_SOLID);
    gui_widget_set_visible(&gui.parent_widget, true);
    
    gui_label_init_parent(&gui.label_time, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.label_time, "Время:");
    gui_widget_move(GUI_WIDGET(&gui.label_time), 5, GUI_LABEL_TOP(0));
    gui_widget_resize(GUI_WIDGET(&gui.label_time), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_time), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_time), true);
    
    gui_label_init_parent(&gui.label_time_val, &gui.gui, &gui.parent_widget);
    //gui_label_set_text(&gui.label_time_val, ":");
    gui_widget_move(GUI_WIDGET(&gui.label_time_val), 60, GUI_LABEL_TOP(0));
    gui_widget_resize(GUI_WIDGET(&gui.label_time_val), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_time_val), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_time_val), true);
    
    gui_label_init_parent(&gui.label_val, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.label_val, "Знач:");
    gui_widget_move(GUI_WIDGET(&gui.label_val), 5, GUI_LABEL_TOP(1));
    gui_widget_resize(GUI_WIDGET(&gui.label_val), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_val), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.label2), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_val), true);
    
    gui_number_label_init_parent(&gui.label_num, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.label_num, 0);//0x1234
    gui_number_label_set_format(&gui.label_num, GUI_NUMBER_LABEL_FIX);
    gui_number_label_set_decimals(&gui.label_num, 2);
    gui_widget_move(GUI_WIDGET(&gui.label_num), 60, GUI_LABEL_TOP(1));
    gui_widget_resize(GUI_WIDGET(&gui.label_num), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_num), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.label3), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_num), true);
    
    gui_label_init_parent(&gui.label_errs, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.label_errs, "Ошибки:");
    gui_widget_move(GUI_WIDGET(&gui.label_errs), 5, GUI_LABEL_TOP(2));
    gui_widget_resize(GUI_WIDGET(&gui.label_errs), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_errs), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.label2), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_errs), true);
    
    gui_number_label_init_parent(&gui.label_num_errs, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.label_num_errs, 0);//0x1234
    gui_number_label_set_format(&gui.label_num_errs, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&gui.label_num_errs), 60, GUI_LABEL_TOP(2));
    gui_widget_resize(GUI_WIDGET(&gui.label_num_errs), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_num_errs), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.label3), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_num_errs), true);
    
    gui_label_init_parent(&gui.label_pwr_errs, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.label_pwr_errs, "Ош пит:");
    gui_widget_move(GUI_WIDGET(&gui.label_pwr_errs), 5, GUI_LABEL_TOP(3));
    gui_widget_resize(GUI_WIDGET(&gui.label_pwr_errs), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_pwr_errs), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.label2), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_pwr_errs), true);
    
    gui_number_label_init_parent(&gui.label_num_pwr_errs, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.label_num_pwr_errs, 0);//0x1234
    gui_number_label_set_format(&gui.label_num_pwr_errs, GUI_NUMBER_LABEL_HEX);
    gui_widget_move(GUI_WIDGET(&gui.label_num_pwr_errs), 60, GUI_LABEL_TOP(3));
    gui_widget_resize(GUI_WIDGET(&gui.label_num_pwr_errs), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_num_pwr_errs), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.label3), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_num_pwr_errs), true);
    
    gui_label_init_parent(&gui.label_state, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.label_state, "Сост.:");
    gui_widget_move(GUI_WIDGET(&gui.label_state), 5, GUI_LABEL_TOP(4));
    gui_widget_resize(GUI_WIDGET(&gui.label_state), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_state), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.label2), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_state), true);
    
    gui_number_label_init_parent(&gui.label_num_state, &gui.gui, &gui.parent_widget);
    gui_number_label_set_number(&gui.label_num_state, 0);//0x1234
    gui_number_label_set_format(&gui.label_num_state, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&gui.label_num_state), 60, GUI_LABEL_TOP(4));
    gui_widget_resize(GUI_WIDGET(&gui.label_num_state), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_num_state), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.label3), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_num_state), true);
    
    gui_button_init_parent(&gui.button_start, &gui.gui, &gui.parent_widget);
    gui_button_set_text(&gui.button_start, "Старт");
    gui_button_set_on_clicked(&gui.button_start, button_start_on_clicked);
    gui_widget_move(GUI_WIDGET(&gui.button_start), 5, GUI_LABEL_TOP(5));
    gui_widget_resize(GUI_WIDGET(&gui.button_start), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.button_start), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.button2), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.button_start), true);
    
    gui_button_init_parent(&gui.button_stop, &gui.gui, &gui.parent_widget);
    gui_button_set_text(&gui.button_stop, "Стоп");
    gui_button_set_on_clicked(&gui.button_stop, button_stop_on_clicked);
    gui_widget_move(GUI_WIDGET(&gui.button_stop), 60, GUI_LABEL_TOP(5));
    gui_widget_resize(GUI_WIDGET(&gui.button_stop), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.button_stop), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.button2), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.button_stop), true);
    
    gui_label_init_parent(&gui.label_ref, &gui.gui, &gui.parent_widget);
    gui_label_set_text(&gui.label_ref, "Задание:");
    gui_widget_move(GUI_WIDGET(&gui.label_ref), 5, GUI_LABEL_TOP(6));
    gui_widget_resize(GUI_WIDGET(&gui.label_ref), 50, GUI_LABEL_HEIGHT);
    gui_widget_set_border(GUI_WIDGET(&gui.label_ref), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.label2), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&gui.label_ref), true);
    
    gui_spinbox_init_parent(&gui.spinbox_ref, &gui.gui, &gui.parent_widget);
    gui_spinbox_set_value(&gui.spinbox_ref, drive_reference());
    gui_spinbox_set_format(&gui.spinbox_ref, GUI_NUMBER_LABEL_DEC);
    gui_spinbox_set_range(&gui.spinbox_ref, 0, 100);
    gui_spinbox_set_on_value_changed(&gui.spinbox_ref, spinbox_reference_on_value_changed);
    gui_widget_move(GUI_WIDGET(&gui.spinbox_ref), 60, GUI_LABEL_TOP(6));
    gui_widget_resize(GUI_WIDGET(&gui.spinbox_ref), 50, 20);
    gui_widget_set_visible(GUI_WIDGET(&gui.spinbox_ref), true);
    
    make_gui_adc();
    make_err_gui();
    
    gui_set_root_widget(&gui.gui, &gui.root_widget);
    gui_set_focus_widget(&gui.gui, GUI_WIDGET(&gui.spinbox_ref));

    gui_widget_set_visible(&gui.root_widget, true);
}

void drive_gui_repaint(void)
{
    gui_repaint(&gui.gui, NULL);
}

static bool drive_gui_read_last_err(void)
{
    static drive_event_index_t last_index = 0;
    static bool readed = false;
    
    if(drive_events_count() == 0) return false;
    
    if(last_index == drive_events_last_index() && readed) return false;
    
    last_index = drive_events_last_index();
    readed = (drive_events_read_event(&gui.last_event, last_index) == E_NO_ERROR);
    
    return readed;
}

void drive_gui_update(void)
{
    //gui_number_label_set_number(&gui.label_num, drive_keypad_state());
    //gui_number_label_set_number(&gui.label_num, drive_init_state());
    fixed32_t angle = 0;
    drive_phase_sync_angle(PHASE_A, &angle);
    gui_number_label_set_number(&gui.label_num, angle);
    gui_number_label_set_number(&gui.label_num_state, drive_state());
    gui_number_label_set_number(&gui.label_num_errs, drive_errors());
    gui_number_label_set_number(&gui.label_num_pwr_errs, drive_power_errors());
    gui_spinbox_set_value(&gui.spinbox_ref, drive_reference());
    
    static char time_str[9];
    time_t t = time(NULL);
    struct tm* ts = localtime(&t);
    snprintf(time_str, 9, "%02d:%02d:%02d", ts->tm_hour, ts->tm_min, ts->tm_sec);
    gui_label_set_text(&gui.label_time_val, time_str);
    
    if(drive_gui_read_last_err()){
        gui_number_label_set_number(&gui.label_num_last_pwr_errs, gui.last_event.power_errors);
    }

    if(drive_power_data_avail(DRIVE_POWER_CHANNELS)){

        gui_number_label_set_number(&gui.lbl_num_adc1_in1, (drive_power_channel_real_value(0)));
        gui_number_label_set_number(&gui.lbl_num_adc1_in2, (drive_power_channel_real_value(2)));
        gui_number_label_set_number(&gui.lbl_num_adc1_in3, (drive_power_channel_real_value(4)));
        gui_number_label_set_number(&gui.lbl_num_adc1_in4, (drive_power_channel_real_value(6)));
        //gui_number_label_set_number(&gui.lbl_num_adc1_in4, (drive_power_channel_raw_value(6)));

        gui_number_label_set_number(&gui.lbl_num_adc2_in1, drive_power_channel_real_value(1));
        gui_number_label_set_number(&gui.lbl_num_adc2_in2, drive_power_channel_real_value(3));
        gui_number_label_set_number(&gui.lbl_num_adc2_in3, drive_power_channel_real_value(5));
        gui_number_label_set_number(&gui.lbl_num_adc2_in4, drive_power_channel_real_value(7));

        gui_number_label_set_number(&gui.lbl_num_adc3_in1, drive_power_channel_real_value(8));
        
        //gui_number_label_set_number(&gui.lbl_num_adc3_in1, fixed32_get_int(drive_power_channel_real_value(8)));
        gui_number_label_set_number(&gui.lbl_num_adc3_in2, fixed32_get_int(drive_power_channel_real_value(9)));
        gui_number_label_set_number(&gui.lbl_num_adc3_in3, fixed32_get_int(drive_power_channel_real_value(10)));
    }
}


err_t drive_gui_init(drive_gui_init_t* gui_is)
{
    if(gui_is == NULL) return E_NULL_POINTER;
    if(gui_is->tft == NULL) return E_NULL_POINTER;
    
    gui.tft = gui_is->tft;
    
    RETURN_ERR_IF_FAIL(drive_gui_init_tft());
    RETURN_ERR_IF_FAIL(gui_init(&gui.gui, &graphics, &theme));
    
    make_gui();
    
    memset(&gui.last_event, 0x0, sizeof(drive_event_t));
    
    return E_NO_ERROR;
}

void drive_gui_on_key_pressed(keycode_t key)
{
    switch(key){
        case KEY_LEFT:
        case KEY_MINUS:
            gui_focus_prev_widget(&gui.gui);
            return;
        case KEY_RIGHT:
        case KEY_PLUS:
            gui_focus_next_widget(&gui.gui);
            return;
        default:
            break;
    }
    gui_key_pressed(&gui.gui, key);
}

void drive_gui_on_key_released(keycode_t key)
{
    switch(key){
        case KEY_LEFT:
        case KEY_MINUS:
        case KEY_RIGHT:
        case KEY_PLUS:
            return;
        default:
            break;
    }
    gui_key_released(&gui.gui, key);
}


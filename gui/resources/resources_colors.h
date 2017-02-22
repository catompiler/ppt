
#ifndef RESOURCES_COLORS_H
#define RESOURCES_COLORS_H

#include "../../../lib/tft9341/tft9341.h"

#define MAKE_RGB(r, g, b) TFT9341_MAKE_RGB565(r, g, b)

// цветовая схема
#define THEME_COLOR_WHITE   MAKE_RGB(0xff, 0xff, 0xff)
#define THEME_COLOR_BLACK   MAKE_RGB(0x00, 0x00, 0x00)
#define THEME_COLOR_GRAY    MAKE_RGB(0x7f, 0x7f, 0x7f)
#define THEME_COLOR_GRAY_L  MAKE_RGB(0xf5, 0xf5, 0xf5)
#define THEME_COLOR_GRAY_D  MAKE_RGB(0x46, 0x46, 0x46)
#define THEME_COLOR_GREEN_L MAKE_RGB(0x00, 0xff, 0x00)
#define THEME_COLOR_GREEN_D MAKE_RGB(0x00, 0x7d, 0x00)
#define THEME_COLOR_RED_L   MAKE_RGB(0xff, 0x00, 0x00)
#define THEME_COLOR_RED_D   MAKE_RGB(0xee, 0x00, 0x00)
#define THEME_COLOR_BLUE_L  MAKE_RGB(0x00, 0x00, 0xff)
#define THEME_COLOR_BLUE_D  MAKE_RGB(0x00, 0x79, 0xdb)
#define THEME_COLOR_YELLOW  MAKE_RGB(0xff, 0xff, 0x00)
#define THEME_COLOR_ORANGE  MAKE_RGB(0xff, 0x79, 0x00)
#define THEME_COLOR_MAGENTA MAKE_RGB(0xff, 0x00, 0xff)
#define THEME_COLOR_CYAN    MAKE_RGB(0x00, 0xff, 0xff)

#endif /* RESOURCES_COLORS_H */


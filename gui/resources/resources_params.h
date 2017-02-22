#ifndef RESOURCES_PARAMS_H
#define RESOURCES_PARAMS_H

//! Размер пиксела - 2 байта (16 бит).
#define TFT_PIXEL_SIZE 2
//! Ширина экрана.
#define TFT_WIDTH 320
#define TFT_HEIGHT 240
//! Высота строки состояния
#define GUI_STATUSBAR_HEIGHT (TFT_HEIGHT / 10)
//! Разделитель между плитками
#define GUI_TILE_SEP 4
//! Ширина плитки
#define GUI_TILE_WIDTH ((TFT_WIDTH - 3 * GUI_TILE_SEP) / 2)
//! Высота плитки
#define GUI_TILE_HEIGHT ((TFT_HEIGHT - GUI_STATUSBAR_HEIGHT - 3 * GUI_TILE_SEP) / 2)

//! Число буферов кэша TFT.
#define TFT_CACHE_BUFS_COUNT 2
//! Размер первого буфера.
#define TFT_CACHE_BUF0_PIXELS 240
#define TFT_CACHE_BUF0_SIZE (TFT_CACHE_BUF0_PIXELS * TFT_PIXEL_SIZE)
//! Размер второго буфера.
#define TFT_CACHE_BUF1_PIXELS 80
#define TFT_CACHE_BUF1_SIZE (TFT_CACHE_BUF1_PIXELS * TFT_PIXEL_SIZE)

#endif /* RESOURCES_PARAMS_H */


/**
 * @file resources_menu.h Ресурсы: данные списка меню.
 */

#ifndef RESOURCES_MENU_H
#define	RESOURCES_MENU_H

//! Флаги элементов меню.
typedef enum _Menu_Flag {
    MENU_FLAG_NONE  = 0,
    MENU_FLAG_DATA  = 1,
    MENU_FLAG_CMD   = 2,
    MENU_FLAG_ADMIN = 4,
    MENU_FLAG_ROOT  = 8
} menu_flag_t;

#define MENU_DATA_PROP_COUNT 2
//! Тип свойства элемента меню.
typedef enum _Menu_Data_Prop {
    MDP_TITLE = 0,
    MDP_UNIT
} menu_data_prop_t;

#endif	/* RESOURCES_MENU_H */


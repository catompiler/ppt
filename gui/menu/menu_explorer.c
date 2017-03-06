#include "menu_explorer.h"
#include "utils/utils.h"
#include <stdlib.h>
#include "menu/menu.h"
#include "parameters_ids.h"

err_t menu_explorer_init(menu_explorer_t* explorer, menu_t* menu)
{
    explorer->menu = menu;
    menu_reset_current(menu);
    
    // Выбранный элемент (первый элемент главного меню)
    explorer->sel_object = menu_current(menu); 
    explorer->count = menu_item_count(explorer->sel_object);
    explorer->level = 0;
    explorer->sel_pos = 0;
    explorer->state = MENU_EXPLORER_STATE_NAVI;
    // отображаемые элементы меню
    int i;
    explorer->item_pos = menu_item_pos(explorer->sel_object);
    menu_item_t* obj = explorer->sel_object;
    for (i = 0; i < menu_explorer_displayed_cnt(explorer); i++) {
        if (obj != NULL) {
            explorer->displayed[i] = obj;
        }
        obj = menu_item_next(obj);
    }
    return E_NO_ERROR;
}

menu_item_t* menu_explorer_sel(menu_explorer_t* explorer)
{
    return explorer->sel_object;
}

menu_item_t* menu_explorer_in(menu_explorer_t* explorer)
{
    switch(explorer->state)   
    {  
        case MENU_EXPLORER_STATE_NAVI:  
            return menu_explorer_navi_in(explorer);
            break;  
        case MENU_EXPLORER_STATE_EDIT: 
            menu_explorer_edit_enter(explorer);
            break;  
        default: 
            break;  
    }
    return explorer->sel_object;
}

menu_item_t* menu_explorer_navi_in(menu_explorer_t* explorer)
{
    if (explorer->sel_object->child == NULL) {
        // переходим в режим редактирования
        explorer->state = MENU_EXPLORER_STATE_EDIT;
    }
    else {
        // Есть дочерние элементы, выбираем первый из списка
        if (explorer->level < MENU_EXPLORER_MAX_LEVEL) {
            explorer->history[explorer->level] = explorer->sel_object;
            explorer->history_pos[explorer->level] = explorer->sel_pos;
            explorer->history_item_pos[explorer->level] = explorer->item_pos;
            explorer->level++;
        }
        explorer->count = menu_item_childs_count(explorer->sel_object);
        explorer->sel_object = explorer->sel_object->child;
        explorer->sel_pos = 0;
        // отображаемые элементы
        explorer->displayed[0] = explorer->sel_object;
        
        explorer->item_pos = 0;
        int i;
        menu_item_t* obj = explorer->sel_object;
        for (i = 0; i < menu_explorer_displayed_cnt(explorer); i++) {
            explorer->displayed[i] = NULL;
        }
        for (i = 0; i < menu_explorer_displayed_cnt(explorer); i++) {
            explorer->displayed[i] = obj;
            obj = menu_item_next(obj);
            if (obj == NULL) break;
        }
    }
    return explorer->sel_object;
}

bool menu_explorer_edit_enter(menu_explorer_t* explorer)
{
    explorer->state = MENU_EXPLORER_STATE_NAVI;
    return true;
}

bool menu_explorer_edit_esc(menu_explorer_t* explorer)
{
    explorer->state = MENU_EXPLORER_STATE_NAVI;
    return true;
}

bool menu_explorer_out(menu_explorer_t* explorer) 
{
    switch(explorer->state)   
    {  
        case MENU_EXPLORER_STATE_NAVI:  
            return menu_explorer_navi_out(explorer);
            break;  
        case MENU_EXPLORER_STATE_EDIT: 
            return menu_explorer_edit_esc(explorer);
            break;  
        default: 
            break;  
    }
    return false;
}

bool menu_explorer_navi_out(menu_explorer_t* explorer) 
{
    bool is_not_root = !(explorer->sel_object->parent == NULL);
    if (is_not_root) {
        // родитель - не корневой элемент
        
        if (explorer->level > 0) explorer->level--;
        explorer->sel_object = explorer->history[explorer->level];
        explorer->count = menu_item_count(explorer->sel_object);
        explorer->sel_pos = explorer->history_pos[explorer->level];
        if (explorer->sel_pos >= menu_explorer_displayed_cnt(explorer)) {
            explorer->sel_pos = menu_explorer_displayed_cnt(explorer) - 1;
        }
        explorer->item_pos = explorer->history_item_pos[explorer->level];
        
        // отображаемые элементы
        int i;
        for (i = 0; i < menu_explorer_displayed_cnt(explorer); i++) {
            explorer->displayed[i] = NULL;
        }
         
        menu_item_t* obj = explorer->sel_object;
        
        //explorer->displayed[0] = obj;
        for (i = explorer->sel_pos; i >= 0; i--) {
            explorer->displayed[i] = obj;
            obj = menu_item_prev(obj);
            if (obj == NULL) break;
        }
        obj = explorer->sel_object;
        for (i = explorer->sel_pos; i < menu_explorer_displayed_cnt(explorer); i++) {
            explorer->displayed[i] = obj;
            obj = menu_item_next(obj);
            if (obj == NULL) break;
        }
    }
    return is_not_root;
}

/**
 * Выбрать следующий элемент меню
 * @return Выбранный элемент меню
 */
EXTERN menu_item_t* menu_explorer_next(menu_explorer_t* explorer)
{
    menu_item_t* obj = menu_item_next(explorer->sel_object);
    if (obj != NULL) {
        explorer->item_pos++;
        explorer->sel_object = obj;
        if (explorer->sel_pos < menu_explorer_center_pos(explorer) || explorer->displayed[menu_explorer_displayed_cnt(explorer) - 1] == NULL) {
            explorer->sel_pos++;
        }
        else {
            obj = menu_item_next(explorer->displayed[menu_explorer_displayed_cnt(explorer) - 1]);
            if (obj != NULL) {
                int i;
                for (i = 0; i < menu_explorer_displayed_cnt(explorer) - 1; i++) {
                    if (explorer->displayed[i + 1] != NULL) {
                        explorer->displayed[i] = explorer->displayed[i + 1];
                    }
                }
                explorer->displayed[menu_explorer_displayed_cnt(explorer) - 1] = obj;
            }
            else {
                explorer->sel_pos++;
            }
        }
    }
    return explorer->sel_object;
}

/**
 * Выбрать предыдущий элемент меню
 * @return Выбранный элемент меню
 */
EXTERN menu_item_t* menu_explorer_prev(menu_explorer_t* explorer)
{
    menu_item_t* obj = menu_item_prev(explorer->sel_object);
    if (obj != NULL) {
        explorer->item_pos--;
        explorer->sel_object = obj;
        if (explorer->sel_pos > menu_explorer_center_pos(explorer)) {
            explorer->sel_pos--;
        }
        else {
            obj = menu_item_prev(explorer->displayed[0]);
            if (obj != NULL) {
                int i;
                for (i = menu_explorer_displayed_cnt(explorer) - 1; i > 0; i--) {
                    if (explorer->displayed[i - 1] != NULL) {
                        explorer->displayed[i] = explorer->displayed[i - 1];
                    }
                }
                explorer->displayed[0] = obj;
            }
            else {
                explorer->sel_pos--;
            }
        }
    }
    return explorer->sel_object;
}

bool menu_explorer_help_visible(menu_explorer_t* explorer)
{
    return explorer->help;
}

uint8_t menu_explorer_displayed_cnt(menu_explorer_t* explorer)
{
    if (menu_explorer_help_visible(explorer)) {
        return (MENU_EXPLORER_DISPLAYED_COUNT - 2);
    }
    else {
        return MENU_EXPLORER_DISPLAYED_COUNT;
    }
}

void menu_explorer_set_help_visible(menu_explorer_t* explorer, bool visible)
{
    explorer->help = visible;
    if (explorer->sel_pos >= menu_explorer_displayed_cnt(explorer)) {
        explorer->sel_pos = menu_explorer_displayed_cnt(explorer) - 1;
    }
    
    // отображаемые элементы
    int i;
    for (i = 0; i < menu_explorer_displayed_cnt(explorer); i++) {
        explorer->displayed[i] = NULL;
    }

    menu_item_t* obj = explorer->sel_object;

    //explorer->displayed[0] = obj;
    for (i = explorer->sel_pos; i >= 0; i--) {
        explorer->displayed[i] = obj;
        obj = menu_item_prev(obj);
        if (obj == NULL) break;
    }
    obj = explorer->sel_object;
    for (i = explorer->sel_pos; i < menu_explorer_displayed_cnt(explorer); i++) {
        explorer->displayed[i] = obj;
        obj = menu_item_next(obj);
        if (obj == NULL) break;
    }
}

uint8_t menu_explorer_center_pos(menu_explorer_t* explorer)
{
    return menu_explorer_displayed_cnt(explorer) / 2;
}

bool menu_explorer_is_selected(menu_explorer_t* explorer, menu_item_t* item)
{
    return (explorer->sel_object == item);
}

void menu_explorer_help_toggle(menu_explorer_t* explorer) 
{
    menu_explorer_set_help_visible(explorer, !menu_explorer_help_visible(explorer));
}

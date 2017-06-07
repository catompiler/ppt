#include "menu_explorer.h"
#include "utils/utils.h"
#include <stdlib.h>
#include <time.h>
#include "menu/menu.h"
#include "parameters_ids.h"
#include "settings.h"
#include "gui/resources/resources_menu.h"
#include "commands.h"
#include "../resources/resources_params.h"
#include "drive_tasks.h"

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
        
    explorer->edit_param = 0;
    explorer->edit_param_decim = 0;
    explorer->edit_param_curdecim = 0;
    explorer->draw_mode = GUI_MENU_DRAW_MODE_ALL;
    
    menu_explorer_touch(explorer);
    //menu_explorer_set_user(explorer, MENU_USER_NONE);
            
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

void menu_explorer_touch(menu_explorer_t* explorer)
{
    counter_t cur = system_counter_ticks();
    counter_t ticks_per_sec = system_counter_ticks_per_sec();
    counter_t reset_time = explorer->touch + ticks_per_sec * MENU_EXPLORER_USER_RESET_SEC;
    if (cur >= reset_time) {
        // сброс предыдущих прав пользователя по таймауту
        explorer->user = MENU_USER_NONE;
    }
    explorer->touch = cur;
    explorer->autoupdate = cur;
}

counter_t menu_explorer_get_touch(menu_explorer_t* explorer)
{
    return explorer->touch;
}

void menu_explorer_set_user(menu_explorer_t* explorer, menu_user_t user)
{
    explorer->user = user;
}

bool menu_explorer_user_is(menu_explorer_t* explorer, menu_user_t user)
{
    return explorer->user == user;
}

bool menu_explorer_check_user(menu_explorer_t* explorer)
{
    bool check = true;
    if (explorer->sel_object->flags & MENU_FLAG_ROOT) {
        check = menu_explorer_user_is(explorer, MENU_USER_ROOT);
    }
    if (explorer->sel_object->flags & MENU_FLAG_ADMIN) {
        check = menu_explorer_user_is(explorer, MENU_USER_ADMIN)\
                ||  menu_explorer_user_is(explorer, MENU_USER_ROOT);
    }
    return check;
}

menu_item_t* menu_explorer_sel(menu_explorer_t* explorer)
{
    return explorer->sel_object;
}

menu_item_t* menu_explorer_in(menu_explorer_t* explorer)
{
    menu_explorer_touch(explorer);
    switch(explorer->state)   
    {
        case MENU_EXPLORER_STATE_PASSWORD_REQUEST: 
        {
            int i;
            int32_t pow = 1;
            for (i = 0; i < (explorer->edit_param_decim - explorer->edit_param_curdecim - 1); i++) {
                pow *= 10;
            }
            explorer->edit_param += explorer->edit_param_curdecim_val * pow;
            explorer->edit_param_curdecim_val = 0;
            explorer->edit_param_curdecim++;
            if (explorer->edit_param_curdecim >= explorer->edit_param_decim) {
                explorer->state = MENU_EXPLORER_STATE_NAVI;
                explorer->draw_mode = GUI_MENU_DRAW_MODE_ALL;
                if (menu_explorer_user_change(explorer, explorer->edit_param)) {
                    return menu_explorer_navi_in(explorer);
                }
            }
            else {
                explorer->draw_mode = GUI_MENU_DRAW_MODE_PASSWORD_VALUE;
            }
        }
            break; 
        case MENU_EXPLORER_STATE_NAVI: 
            if (menu_explorer_check_user(explorer)) {// есть права доступа
                return menu_explorer_navi_in(explorer);
            }
            else {// запрос прав доступа
                explorer->edit_param_decim = PASSWORD_LENGHT;
                explorer->edit_param = 0;
                explorer->edit_param_curdecim_val = 0;
                explorer->edit_param_curdecim = 0;
                explorer->state = MENU_EXPLORER_STATE_PASSWORD_REQUEST;
                explorer->draw_mode = GUI_MENU_DRAW_MODE_PASSWORD;
            }
            break;  
        case MENU_EXPLORER_STATE_EDIT: 
            menu_explorer_edit_enter(explorer);
            break;  
        default: 
            break;  
    }
    return explorer->sel_object;
}

param_t* menu_explorer_selelected_param(menu_explorer_t* explorer) {
    param_t* param = NULL;
    menu_item_t* menu_item = explorer->sel_object;
    bool item_not_null = (menu_item != NULL);
    //bool item_is_null = !item_not_null;
    //bool item_is_selected = false;
    bool item_is_submenu = false;
    bool item_is_param = false;
    //bool item_is_enum = false;
    if (item_not_null) {
        //item_is_selected = menu_explorer_is_selected(explorer, menu_item);
        item_is_submenu = menu_item_childs_count(menu_item) > 0;
        item_is_param = !item_is_submenu;
        if (item_is_param) {
            param = settings_param_by_id(menu_item->id);
            item_is_param = (param != NULL);
            //item_is_enum = (menu_item->value != NULL);
        } 
    }
    return param;
}

void menu_explorer_navi_in_param(menu_explorer_t* explorer)
{
    
        explorer->state = MENU_EXPLORER_STATE_EDIT;
        
        param_t* param = menu_explorer_selelected_param(explorer);
        param_type_t param_type = settings_param_type(param);

        fixed32_t value = 0;
        
        switch(param_type) {
            default:
            case PARAM_TYPE_INT:
                explorer->edit_param = (int32_t)settings_param_valuei(param);
                break;
            case PARAM_TYPE_UINT:
                explorer->edit_param = (int32_t)settings_param_valueu(param);
                break;
            case PARAM_TYPE_FRACT_10:
                value = settings_param_valuef(param);
                if(value >= 0)
                    value += fixed32_make_from_fract(5, 100);
                else
                    value -= fixed32_make_from_fract(5, 100);
                explorer->edit_param = fixed32_get_int(value) * 10 + fixed32_get_fract_by_denom(value, 10);
                break;
            case PARAM_TYPE_FRACT_100:
                value = settings_param_valuef(param);
                if(value >= 0)
                    value += fixed32_make_from_fract(5, 1000);
                else
                    value -= fixed32_make_from_fract(5, 1000);
                explorer->edit_param = fixed32_get_int(value) * 100 + fixed32_get_fract_by_denom(value, 100);
                break;
            case PARAM_TYPE_FRACT_1000:
                value = settings_param_valuef(param);
                if(value >= 0)
                    value += fixed32_make_from_fract(5, 10000);
                else
                    value -= fixed32_make_from_fract(5, 10000);
                explorer->edit_param = fixed32_get_int(value) * 1000 + fixed32_get_fract_by_denom(value, 1000); 
                break;
            case PARAM_TYPE_FRACT_10000:
                value = settings_param_valuef(param);
                if(value >= 0)
                    value += fixed32_make_from_fract(5, 100000);
                else
                    value -= fixed32_make_from_fract(5, 100000);
                explorer->edit_param = fixed32_get_int(value) * 10000 + fixed32_get_fract_by_denom(value, 10000);
                break;
        }
        
        explorer->edit_param_decim = 1;
        explorer->edit_param_curdecim = 0;
        if (param != NULL) {
            bool item_is_enum = (explorer->sel_object->value != NULL);
            
            // расчет кол-ва разрядов для поразрядного редактирования
            uint8_t maxdecim = 0;
            if (item_is_enum) {
                maxdecim = 1;
            }
            else {
                int32_t max;
                int32_t min;

                param_type_t param_type = settings_param_type(param);
                param_value_t param_max = settings_param_max(param);
                param_value_t param_min = settings_param_min(param);
                switch(param_type) {
                    default:
                    case PARAM_TYPE_INT:
                        max = param_max.int_value;
                        min = param_min.int_value;
                        break;
                    case PARAM_TYPE_UINT:
                        max = (int32_t)param_max.uint_value;
                        min = (int32_t)param_min.uint_value;
                        break;
                    case PARAM_TYPE_FRACT_10:
                    case PARAM_TYPE_FRACT_100:
                    case PARAM_TYPE_FRACT_1000:
                    case PARAM_TYPE_FRACT_10000:
                        max = (int32_t)settings_param_data_from_fixed32(param, param_max.fixed_value);
                        min = (int32_t)settings_param_data_from_fixed32(param, param_min.fixed_value);
                        break;
                }

                if (min < 0) min = -min;
                if (min > max) max = min;

                int32_t den = 1;
                while (den <= max) {
                    maxdecim++;
                    den *= 10;
                }
            }
            explorer->edit_param_decim = maxdecim - 1;
        }
        
        explorer->draw_mode = GUI_MENU_DRAW_MODE_EDIT;
}

void menu_explorer_navi_in_submenu(menu_explorer_t* explorer) 
{
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
        explorer->draw_mode = GUI_MENU_DRAW_MODE_ALL;
}

void menu_explorer_navi_in_command(menu_explorer_t* explorer) 
{
    command_menu_t* command = command_get_by_id(explorer->sel_object->id);
    if (command) {
        command->callback();
    }    
}

menu_item_t* menu_explorer_navi_in(menu_explorer_t* explorer)
{
    if (explorer->sel_object->child == NULL) {
        if (explorer->sel_object->flags & MENU_FLAG_CMD) { // команда
            // обработка команды
            menu_explorer_navi_in_command(explorer);
        }
        else if (explorer->sel_object->flags & MENU_FLAG_DATA) { // параметр
            // переходим в режим редактирования
            menu_explorer_navi_in_param(explorer);
        }
    }
    else { // есть дочерние элементы 
        // переходим в подменю
        menu_explorer_navi_in_submenu(explorer);
    }
    return explorer->sel_object;
}

bool menu_explorer_edit_enter(menu_explorer_t* explorer)
{
    // переход к редактированию следующего разряда
    explorer->edit_param_curdecim++;
    // последний разряд - сохраняем значение
    if (explorer->edit_param_curdecim > explorer->edit_param_decim) {
        param_t* param = menu_explorer_selelected_param(explorer);
        if (settings_param_check_minmax(param, &explorer->edit_param)) {
            param_type_t param_type = settings_param_type(param);
            switch(param_type) {
                default:
                case PARAM_TYPE_INT:
                case PARAM_TYPE_UINT:
                    settings_param_set_valuei(param, explorer->edit_param);
                    break;
                case PARAM_TYPE_FRACT_10:
                    {
                        fixed32_t fixvalue = fixed32_make_from_int(explorer->edit_param / 10);
                        fixvalue += fixed32_make_from_fract(explorer->edit_param % 10, 10);
                        settings_param_set_valuef(param, fixvalue);
                    }
                    break;
                case PARAM_TYPE_FRACT_100:
                    {
                        fixed32_t fixvalue = fixed32_make_from_int(explorer->edit_param / 100);
                        fixvalue += fixed32_make_from_fract(explorer->edit_param % 100, 100);
                        settings_param_set_valuef(param, fixvalue);
                    }
                    break;
                case PARAM_TYPE_FRACT_1000:
                    {
                        fixed32_t fixvalue = fixed32_make_from_int(explorer->edit_param / 1000);
                        fixvalue += fixed32_make_from_fract(explorer->edit_param % 1000, 1000);
                        settings_param_set_valuef(param, fixvalue);
                    }
                    break;
                case PARAM_TYPE_FRACT_10000:
                    {
                        fixed32_t fixvalue = fixed32_make_from_int(explorer->edit_param / 10000);
                        fixvalue += fixed32_make_from_fract(explorer->edit_param % 10000, 10000);
                        settings_param_set_valuef(param, fixvalue);
                    }
                    break;
            }
            drive_tasks_apply_settings(); // применяем настройки
        }
        explorer->state = MENU_EXPLORER_STATE_NAVI;
        explorer->draw_mode = GUI_MENU_DRAW_MODE_ALL;
    }
    return true;
}

bool menu_explorer_edit_esc(menu_explorer_t* explorer)
{
    // первый разряд - выход из режима редактирования без сохранения значения
    if (explorer->edit_param_curdecim <= 0) {
        explorer->state = MENU_EXPLORER_STATE_NAVI;
        explorer->draw_mode = GUI_MENU_DRAW_MODE_ALL;
    }
    else {
        // переход к редактированию предыдущего разряда
        explorer->edit_param_curdecim--;
    }
    return true;
}

bool menu_explorer_out(menu_explorer_t* explorer) 
{
    menu_explorer_touch(explorer);
    switch(explorer->state)   
    {  
        case MENU_EXPLORER_STATE_PASSWORD_REQUEST:
            explorer->state = MENU_EXPLORER_STATE_NAVI;
            explorer->draw_mode = GUI_MENU_DRAW_MODE_ALL;
            return true;
            break;
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
        explorer->draw_mode = GUI_MENU_DRAW_MODE_ALL;
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
        explorer->draw_mode = GUI_MENU_DRAW_MODE_ALL;
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
        explorer->draw_mode = GUI_MENU_DRAW_MODE_ALL;
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
    explorer->draw_mode = GUI_MENU_DRAW_MODE_ALL;
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

menu_explorer_state_t menu_explorer_state(menu_explorer_t* explorer)
{
    return explorer->state;
}

bool menu_explorer_long_esc(menu_explorer_t* explorer)
{
    menu_explorer_touch(explorer);
    if (explorer->state == MENU_EXPLORER_STATE_EDIT) {
        explorer->state = MENU_EXPLORER_STATE_NAVI;
        return true;
    }
    else if (explorer->state == MENU_EXPLORER_STATE_NAVI) {
        explorer->state = MENU_EXPLORER_STATE_HOME;
        return true;
    }
    return false;
}

menu_item_t* menu_explorer_up(menu_explorer_t* explorer)
{
    menu_explorer_touch(explorer);
    switch(explorer->state)   
    {  
        case MENU_EXPLORER_STATE_NAVI:  
            return menu_explorer_prev(explorer);
            break;  
        case MENU_EXPLORER_STATE_EDIT: 
            {
                param_t* param = menu_explorer_selelected_param(explorer);
                if (param != NULL) {
                    settings_param_inc_dec(param, &explorer->edit_param, explorer->edit_param_curdecim, true);
                }
                explorer->draw_mode = GUI_MENU_DRAW_MODE_EDIT;
            }
            break;
        case MENU_EXPLORER_STATE_PASSWORD_REQUEST: 
            {
                explorer->edit_param_curdecim_val++;
                if (explorer->edit_param_curdecim_val > 9) 
                    explorer->edit_param_curdecim_val = 0;
                explorer->draw_mode = GUI_MENU_DRAW_MODE_PASSWORD_VALUE;
            }
            break;  
        default: 
            break;  
    }
    return explorer->sel_object;
}

menu_item_t* menu_explorer_down(menu_explorer_t* explorer)
{
    menu_explorer_touch(explorer);
    switch(explorer->state)   
    {  
        case MENU_EXPLORER_STATE_NAVI:  
            return menu_explorer_next(explorer);
            break;  
        case MENU_EXPLORER_STATE_EDIT: 
            {
                param_t* param = menu_explorer_selelected_param(explorer);
                if (param != NULL) {
                    settings_param_inc_dec(param, &explorer->edit_param, explorer->edit_param_curdecim, false);
                }
                explorer->draw_mode = GUI_MENU_DRAW_MODE_EDIT;
            }
            break;  
        case MENU_EXPLORER_STATE_PASSWORD_REQUEST: 
            {
                if (explorer->edit_param_curdecim_val <= 0) {
                    explorer->edit_param_curdecim_val = 9;
                }
                else {
                    explorer->edit_param_curdecim_val--;
                }
                explorer->draw_mode = GUI_MENU_DRAW_MODE_PASSWORD_VALUE;
            }
            break;  
        default: 
            break;  
    }
    return explorer->sel_object;
}

bool menu_explorer_state_navi(menu_explorer_t* explorer) {
    return explorer->state == MENU_EXPLORER_STATE_NAVI;
}

bool menu_explorer_state_edit(menu_explorer_t* explorer) {
    return explorer->state == MENU_EXPLORER_STATE_EDIT;
}

bool menu_explorer_state_home(menu_explorer_t* explorer) {
    return explorer->state == MENU_EXPLORER_STATE_HOME;
}

bool menu_explorer_state_password_request(menu_explorer_t* explorer)
{
    return explorer->state == MENU_EXPLORER_STATE_PASSWORD_REQUEST;
}

bool menu_explorer_user_change(menu_explorer_t* explorer, int32_t password)
{
    param_t* root_pass_param = settings_param_by_id(PARAM_ID_GUI_PASSWORD_ROOT);
    param_t* admin_pass_param = settings_param_by_id(PARAM_ID_GUI_PASSWORD_ADMIN);
    int32_t root_pass = settings_param_valuei(root_pass_param);
    int32_t admin_pass = settings_param_valuei(admin_pass_param);
    if (root_pass == password) {
        explorer->user = MENU_USER_ROOT;
        return true;
    }
    else if (admin_pass == password) {
        explorer->user = MENU_USER_ADMIN;
        return true;
    }
    explorer->user = MENU_USER_NONE;
    return false;
}
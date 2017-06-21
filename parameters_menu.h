/**
 * @file parameters_menu.h Файл меню параметров.
 */

#ifndef PARAMETERS_MENU_H
#define PARAMETERS_MENU_H

#include "menu/menu.h"
#include "localization/localization.h"
#include "translations_ids.h"
#include "parameters_ids.h"
#include "commands_ids.h"
#include "gui/resources/resources_menu.h"

/*
 * Перечисления типов для выбора в меню.
 */
//! Перечисление фаз.
MENU_VALUES(menu_enum_phases,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_PHASE_UNK)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_PHASE_A)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_PHASE_B)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_PHASE_C))
);
//! Перечисление Да/Нет.
MENU_VALUES(menu_enum_bool,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_NO)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_YES))
);
//! Перечисление режима регулирования.
MENU_VALUES(menu_enum_regulator_mode,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_REGULATOR_MODE_SPEED)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_REGULATOR_MODE_TORQUE))
);
//! Перечисление режима возбуждения.
MENU_VALUES(menu_enum_exc_mode,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_EXC_MODE_FIXED)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_EXC_MODE_REGULATED)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_EXC_MODE_EXTERNAL))
);
//! Перечисление типа останова.
MENU_VALUES(menu_enum_stop_mode,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_STOP_MODE_NORMAL)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_STOP_MODE_FAST)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_STOP_MODE_COAST))
);
//! Перечисление действия защиты.
MENU_VALUES(menu_enum_prot_action,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_PROT_ACTION_IGNORE)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_PROT_ACTION_WARNING)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_PROT_ACTION_FAST_STOP)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_PROT_ACTION_COAST_STOP)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_PROT_ACTION_EMERGENCY_STOP))
);
//! Перечисление типа цифровых входов.
MENU_VALUES(menu_enum_dio_in_type,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_IN_NONE)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_IN_START_STOP)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_IN_EMERGENCY_STOP)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_IN_REFERENCE_INC)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_IN_REFERENCE_DEC)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_IN_CLEAR_ERRORS)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_IN_USER))
);
//! Перечисление типа цифровых выходов.
MENU_VALUES(menu_enum_dio_out_type,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_OUT_NONE)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_OUT_OK)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_OUT_READY)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_OUT_RUNNING)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_OUT_ERROR)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_OUT_WARNING)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIO_OUT_USER))
);
//! Перечисление типа работы звукового оповещения.
MENU_VALUES(menu_enum_gui_buzzer,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_BUZZER_ON)),
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_BUZZER_OFF)),
);
//! Перечисление выбора отображаемого измерения на плитке
MENU_VALUES(menu_enum_gui_tiles,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_DRIVE_POWER_Ua)),      // PARAM_ID_POWER_U_A
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_DRIVE_POWER_Ub)),      // PARAM_ID_POWER_U_B
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_DRIVE_POWER_Uc)),      // PARAM_ID_POWER_U_C
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_DRIVE_POWER_Urot)),    // PARAM_ID_POWER_U_ROT
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_DRIVE_POWER_Ia)),      // PARAM_ID_POWER_I_A
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_DRIVE_POWER_Ib)),      // PARAM_ID_POWER_I_B
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_DRIVE_POWER_Ic)),      // PARAM_ID_POWER_I_C
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_DRIVE_POWER_Irot)),    // PARAM_ID_POWER_I_ROT
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_DRIVE_POWER_Iexc)),    // PARAM_ID_POWER_I_EXC 
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_DRIVE_POWER_Ifan)),    // PARAM_ID_POWER_I_FAN
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_DRIVE_POWER_Iref)),    // PARAM_ID_POWER_I_REF
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_HEATSINK_TEMP)),             // PARAM_ID_HEATSINK_TEMP  
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_HEATSINK_FAN_RPM)),          // PARAM_ID_HEATSINK_FAN_RPM 
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_TRIACS_PAIRS_OPEN_ANGLE)),   // PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_TRIAC_EXC_OPEN_ANGLE)),      // PARAM_ID_TRIAC_EXC_OPEN_ANGLE    
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_MOTOR_EFF)),             // PARAM_ID_MOTOR_EFF  
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_MOTOR_R_ROT)),           // PARAM_ID_MOTOR_R_ROT 
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_MOTOR_RPM)),             // PARAM_ID_MOTOR_RPM
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_TILES_MOTOR_TORQUE)),          // PARAM_ID_MOTOR_TORQUE
);
//! Перечисление выбора языка интерфейса
MENU_VALUES(menu_enum_gui_languages,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_LANGUAGES_RU)),      // Русский
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_LANGUAGES_EN)),      // Английский
);
//! Перечисления состояний цифрового входа/выхода
MENU_VALUES(menu_enum_gui_digital_states,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIGITAL_STATES_OFF)),      // Включен
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_DIGITAL_STATES_ON)),      // Выключен
);
//! Перечисления плиток для выбора отображения текста ошибок и предупреждений
MENU_VALUES(menu_enum_gui_tiles_for_warnings,
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_GUI_TILE_1)),      // Плитка 1
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_GUI_TILE_2)),      // Плитка 2
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_GUI_TILE_3)),      // Плитка 3
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_GUI_TILE_4)),      // Плитка 4
    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_GUI_TILE_NONE)),   // Не отображать
);

//! Перечисление .
//MENU_VALUES(menu_enum_,
//    MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_)), MAKE_MENU_VALUE_STRING(TEXT(TR_ID_ENUM_))
//);

// Макрос для определения числа элементов перечисления.
#define MENU_ENUM_LEN(MVARR) (sizeof(MVARR)/sizeof(menu_value_t))

/*
 * Значения перечислений элементов меню.
 */
MENU_VALUE_ENUM(menu_val_phase, 0, MENU_ENUM_LEN(menu_enum_phases), menu_enum_phases);
MENU_VALUE_ENUM(menu_val_bool, 0, MENU_ENUM_LEN(menu_enum_bool), menu_enum_bool);
MENU_VALUE_ENUM(menu_val_exc_mode, 0, MENU_ENUM_LEN(menu_enum_exc_mode), menu_enum_exc_mode);
MENU_VALUE_ENUM(menu_val_regulator_mode, 0, MENU_ENUM_LEN(menu_enum_regulator_mode), menu_enum_regulator_mode);
MENU_VALUE_ENUM(menu_val_stop_mode, 0, MENU_ENUM_LEN(menu_enum_stop_mode), menu_enum_stop_mode);
MENU_VALUE_ENUM(menu_val_prot_action, 0, MENU_ENUM_LEN(menu_enum_prot_action), menu_enum_prot_action);
MENU_VALUE_ENUM(menu_val_dio_in_type, 0, MENU_ENUM_LEN(menu_enum_dio_in_type), menu_enum_dio_in_type);
MENU_VALUE_ENUM(menu_val_dio_out_type, 0, MENU_ENUM_LEN(menu_enum_dio_out_type), menu_enum_dio_out_type);
MENU_VALUE_ENUM(menu_val_gui_buzzer, 0, MENU_ENUM_LEN(menu_enum_gui_buzzer), menu_enum_gui_buzzer);
MENU_VALUE_ENUM(menu_val_gui_tiles, 0, MENU_ENUM_LEN(menu_enum_gui_tiles), menu_enum_gui_tiles);
MENU_VALUE_ENUM(menu_val_gui_languages, 0, MENU_ENUM_LEN(menu_enum_gui_languages), menu_enum_gui_languages);
MENU_VALUE_ENUM(menu_val_gui_digital_states, 0, MENU_ENUM_LEN(menu_enum_gui_digital_states), menu_enum_gui_digital_states);
MENU_VALUE_ENUM(menu_val_gui_tiles_for_warnings, 0, MENU_ENUM_LEN(menu_enum_gui_tiles_for_warnings), menu_enum_gui_tiles_for_warnings);

//MENU_VALUE_ENUM(menu_val_, 0, MENU_ENUM_LEN(menu_enum_), menu_enum_);

#undef MENU_ENUM_LEN

/**
 * Cтроковые значения меню
 */
MENU_VALUE_STRING(menu_val_firmware_version, __GIT_VERSION);
MENU_VALUE_STRING(menu_val_firmware_datetime, __GIT_DATETIME);

DECLARE_MENU_ITEMS(m_item1, m_item2, m_item3, m_item4, m_item5, m_item6, m_item7, m_item8, m_item9, m_item10, m_item11, m_item12, m_item13, m_item14, m_item15, m_item16, m_item17, m_item18, m_item19, m_item20, m_item21, m_item22, m_item23, m_item24, m_item25, m_item26, m_item27, m_item28, m_item29, m_item30, m_item31, m_item32, m_item33, m_item34, m_item35, m_item36, m_item37, m_item38, m_item39, m_item40, m_item41, m_item42, m_item43, m_item44, m_item45, m_item46, m_item47, m_item48, m_item49, m_item50, m_item51, m_item52, m_item53, m_item54, m_item55, m_item56, m_item57, m_item58, m_item59, m_item60, m_item61, m_item62, m_item63, m_item64, m_item65, m_item66, m_item67, m_item68, m_item69, m_item70, m_item71, m_item72, m_item73, m_item74, m_item75, m_item76, m_item77, m_item78, m_item79, m_item80, m_item81, m_item82, m_item83, m_item84, m_item85, m_item86, m_item87, m_item88, m_item89, m_item90, m_item91, m_item92, m_item93, m_item94, m_item95, m_item96, m_item97, m_item98, m_item99, m_item100, m_item101, m_item102, m_item103, m_item104, m_item105, m_item106, m_item107, m_item108, m_item109, m_item110, m_item111, m_item112, m_item113, m_item114, m_item115, m_item116, m_item117, m_item118, m_item119, m_item120, m_item121, m_item122, m_item123, m_item124, m_item125, m_item126, m_item127, m_item128, m_item129, m_item130, m_item131, m_item132, m_item133, m_item134, m_item135, m_item136, m_item137, m_item138, m_item139, m_item140, m_item141, m_item142, m_item143, m_item144, m_item145, m_item146, m_item147, m_item148, m_item149, m_item150, m_item151, m_item152, m_item153, m_item154, m_item155, m_item156, m_item157, m_item158, m_item159, m_item160, m_item161, m_item162, m_item163, m_item164, m_item165, m_item166, m_item167, m_item168, m_item169, m_item170, m_item171, m_item172, m_item173, m_item174, m_item175, m_item176, m_item177, m_item178, m_item179, m_item180, m_item181, m_item182, m_item183, m_item184, m_item185, m_item186, m_item187, m_item188, m_item189, m_item190, m_item191, m_item192, m_item193, m_item194, m_item195, m_item196, m_item197, m_item198, m_item199, m_item200, m_item201, m_item202, m_item203, m_item204, m_item205, m_item206, m_item207, m_item208, m_item209, m_item210, m_item211, m_item212, m_item213, m_item214, m_item215, m_item216, m_item217, m_item218, m_item219, m_item220, m_item221, m_item222, m_item223, m_item224, m_item225, m_item226, m_item227, m_item228, m_item229, m_item230, m_item231, m_item232, m_item233, m_item234, m_item235, m_item236, m_item237, m_item238, m_item239, m_item240, m_item241, m_item242, m_item243, m_item244, m_item245, m_item246, m_item247, m_item248, m_item249, m_item250, m_item251, m_item252, m_item253, m_item254, m_item255, m_item256, m_item257, m_item258, m_item259, m_item260, m_item261, m_item262, m_item263, m_item264, m_item265, m_item266, m_item267, m_item268, m_item269, m_item270, m_item271, m_item272, m_item273, m_item274, m_item275, m_item276, m_item277, m_item278, m_item279, m_item280, m_item281, m_item282, m_item283, m_item284, m_item285, m_item286, m_item287, m_item288, m_item289, m_item290, m_item291, m_item292, m_item293, m_item294, m_item295, m_item296, m_item297, m_item298, m_item299, m_item300, m_item301, m_item302, m_item303, m_item304, m_item305, m_item306, m_item307, m_item308, m_item309, m_item310, m_item311, m_item312, m_item313, m_item314, m_item315, m_item316, m_item317, m_item318, m_item319, m_item320, m_item321, m_item322, m_item323, m_item324, m_item325, m_item326, m_item327, m_item328, m_item329, m_item330, m_item331, m_item332, m_item333, m_item334, m_item335, m_item336, m_item337, m_item338, m_item339, m_item340, m_item341, m_item342, m_item343, m_item344, m_item345, m_item346, m_item347, m_item348, m_item349, m_item350, m_item351, m_item352, m_item353, m_item354, m_item355, m_item356, m_item357, m_item358, m_item359, m_item360, m_item361, m_item362, m_item363, m_item364, m_item365, m_item366, m_item367, m_item368, m_item369, m_item370, m_item371, m_item372, m_item373, m_item374, m_item375, m_item376, m_item377, m_item378, m_item379, m_item380, m_item381, m_item382, m_item383, m_item384, m_item385, m_item386, m_item387, m_item388, m_item389, m_item390, m_item391, m_item392, m_item393, m_item394, m_item395, m_item396, m_item397, m_item398, m_item399, m_item400, m_item401, m_item402, m_item403, m_item404, m_item405, m_item406, m_item407, m_item408, m_item409, m_item410, m_item411, m_item412, m_item413, m_item414, m_item415, m_item416, m_item417, m_item418, m_item419, m_item420, m_item421, m_item422, m_item423, m_item424, m_item425, m_item426, m_item427, m_item428, m_item429, m_item430, m_item431, m_item432, m_item433, m_item434, m_item435, m_item436, m_item437, m_item438, m_item439, m_item440, m_item441, m_item442, m_item443, m_item444, m_item445, m_item446, m_item447, m_item448, m_item449, m_item450, m_item451, m_item452, m_item453, m_item454, m_item455, m_item456, m_item457, m_item458, m_item459, m_item460, m_item461, m_item462, m_item463, m_item464, m_item465, m_item466, m_item467, m_item468, m_item469, m_item470, m_item471, m_item472, m_item473, m_item474, m_item475, m_item476, m_item477, m_item478, m_item479, m_item480, m_item481, m_item482, m_item483, m_item484, m_item485, m_item486, m_item487, m_item488, m_item489, m_item490, m_item491, m_item492, m_item493, m_item4110, m_item8_1);

SUBMENU(m_item1, 0, NULL, &m_item2, NULL, &m_item9, TEXT(TR_ID_MENU_COMMANDS), NULL, 0, 0, 0);
	MENU_ITEM(m_item2, CMD_ID_START_STOP, &m_item1, NULL, NULL, &m_item3, TEXT(TR_ID_MENU_CMD_START_STOP), NULL, 0, 0, MENU_FLAG_CMD | MENU_FLAG_ADMIN, 0);
	MENU_ITEM(m_item3, CMD_ID_ENABLE_DRIVE, &m_item1, NULL, &m_item2, &m_item4, TEXT(TR_ID_MENU_CMD_ENABLE_DRIVE), NULL, 0, 0, MENU_FLAG_CMD, 0);
	MENU_ITEM(m_item4, CMD_ID_SETTINGS_SAVE, &m_item1, NULL, &m_item3, &m_item5, TEXT(TR_ID_MENU_CMD_SETTINGS_SAVE), NULL, 0, 0, MENU_FLAG_CMD, 0);
	MENU_ITEM(m_item5, CMD_ID_STATE_SAVE, &m_item1, NULL, &m_item4, &m_item6, TEXT(TR_ID_MENU_CMD_STATE_SAVE), NULL, 0, 0, MENU_FLAG_CMD, 0);
	MENU_ITEM(m_item6, CMD_ID_CLEAR_ERRORS, &m_item1, NULL, &m_item5, &m_item7, TEXT(TR_ID_MENU_CMD_CLEAR_ERRORS), NULL, 0, 0, MENU_FLAG_CMD, 0);
	MENU_ITEM(m_item7, CMD_ID_SETTINGS_DEFAULT, &m_item1, NULL, &m_item6, &m_item8, TEXT(TR_ID_MENU_CMD_SETTINGS_DEFAULT), NULL, 0, 0, MENU_FLAG_CMD, 0);
	MENU_ITEM(m_item8, CMD_ID_TEST_LEDS, &m_item1, NULL, &m_item7, &m_item8_1, TEXT(TR_ID_MENU_CMD_TEST_LEDS), NULL, 0, 0, MENU_FLAG_CMD, 0);
        MENU_ITEM(m_item8_1, CMD_ID_CLEAR_EVENTS, &m_item1, NULL, &m_item8, NULL, TEXT(TR_ID_MENU_CMD_CLEAR_EVENTS), NULL, 0, 0, MENU_FLAG_CMD, 0);
SUBMENU(m_item9, 0, NULL, &m_item10, &m_item1, &m_item47, TEXT(TR_ID_MENU_STATUS), NULL, 0, 0, 0);
	MENU_ITEM(m_item10, 0, &m_item9, NULL, NULL, &m_item11, TEXT(TR_ID_MENU_EVENTS), NULL, 0, 0, MENU_FLAG_EVENTS, 0);
	SUBMENU(m_item11, 0, &m_item9, &m_item12, &m_item10, &m_item31, TEXT(TR_ID_MENU_MEASUREMENTS), NULL, 0, 0, 0);
		MENU_ITEM(m_item12, PARAM_ID_POWER_U_A, &m_item11, NULL, NULL, &m_item13, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_U_A), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item13, PARAM_ID_POWER_U_B, &m_item11, NULL, &m_item12, &m_item14, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_U_B), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item14, PARAM_ID_POWER_U_C, &m_item11, NULL, &m_item13, &m_item15, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_U_C), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item15, PARAM_ID_POWER_U_ROT, &m_item11, NULL, &m_item14, &m_item16, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_U_ROT), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item16, PARAM_ID_POWER_I_A, &m_item11, NULL, &m_item15, &m_item17, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_A), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item17, PARAM_ID_POWER_I_B, &m_item11, NULL, &m_item16, &m_item18, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_B), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item18, PARAM_ID_POWER_I_C, &m_item11, NULL, &m_item17, &m_item19, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_C), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item19, PARAM_ID_POWER_I_ROT, &m_item11, NULL, &m_item18, &m_item20, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_ROT), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item20, PARAM_ID_POWER_I_EXC, &m_item11, NULL, &m_item19, &m_item21, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_EXC), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item21, PARAM_ID_POWER_I_FAN, &m_item11, NULL, &m_item20, &m_item22, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_FAN), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item22, PARAM_ID_POWER_I_REF, &m_item11, NULL, &m_item21, &m_item23, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_REF), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item23, PARAM_ID_HEATSINK_TEMP, &m_item11, NULL, &m_item22, &m_item24, TEXT(TR_ID_MENU_MESS_PARAM_ID_HEATSINK_TEMP), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item24, PARAM_ID_HEATSINK_FAN_RPM, &m_item11, NULL, &m_item23, &m_item25, TEXT(TR_ID_MENU_MESS_PARAM_ID_HEATSINK_FAN_RPM), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item25, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE, &m_item11, NULL, &m_item24, &m_item26, TEXT(TR_ID_MENU_MESS_PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item26, PARAM_ID_TRIAC_EXC_OPEN_ANGLE, &m_item11, NULL, &m_item25, &m_item27, TEXT(TR_ID_MENU_MESS_PARAM_ID_TRIAC_EXC_OPEN_ANGLE), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item27, PARAM_ID_MOTOR_EFF, &m_item11, NULL, &m_item26, &m_item28, TEXT(TR_ID_MENU_MESS_PARAM_ID_MOTOR_EFF), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item28, PARAM_ID_MOTOR_R_ROT, &m_item11, NULL, &m_item27, &m_item29, TEXT(TR_ID_MENU_MESS_PARAM_ID_MOTOR_R_ROT), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item29, PARAM_ID_MOTOR_RPM, &m_item11, NULL, &m_item28, &m_item30, TEXT(TR_ID_MENU_MESS_PARAM_ID_MOTOR_RPM), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item30, PARAM_ID_MOTOR_TORQUE, &m_item11, NULL, &m_item29, NULL, TEXT(TR_ID_MENU_MESS_PARAM_ID_MOTOR_TORQUE), NULL, 0, 0, MENU_FLAG_VALUE, 0);
	SUBMENU(m_item31, 0, &m_item9, &m_item32, &m_item11, &m_item37, TEXT(TR_ID_MENU_DIGITAL_INPUTS_STATE), NULL, 0, 0, 0);
		MENU_ITEM(m_item32, PARAM_ID_DIGITAL_IN_1_STATE, &m_item31, NULL, NULL, &m_item33, TEXT(TR_ID_MENU_DIGITAL_IN_1_STATE), NULL, 0, &menu_val_gui_digital_states, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item33, PARAM_ID_DIGITAL_IN_2_STATE, &m_item31, NULL, &m_item32, &m_item34, TEXT(TR_ID_MENU_DIGITAL_IN_2_STATE), NULL, 0, &menu_val_gui_digital_states, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item34, PARAM_ID_DIGITAL_IN_3_STATE, &m_item31, NULL, &m_item33, &m_item35, TEXT(TR_ID_MENU_DIGITAL_IN_3_STATE), NULL, 0, &menu_val_gui_digital_states, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item35, PARAM_ID_DIGITAL_IN_4_STATE, &m_item31, NULL, &m_item34, &m_item36, TEXT(TR_ID_MENU_DIGITAL_IN_4_STATE), NULL, 0, &menu_val_gui_digital_states, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item36, PARAM_ID_DIGITAL_IN_5_STATE, &m_item31, NULL, &m_item35, NULL, TEXT(TR_ID_MENU_DIGITAL_IN_5_STATE), NULL, 0, &menu_val_gui_digital_states, MENU_FLAG_VALUE, 0);
	SUBMENU(m_item37, 0, &m_item9, &m_item38, &m_item31, &m_item42, TEXT(TR_ID_MENU_DIGITAL_OUTPUTS_STATE), NULL, 0, 0, 0);
		MENU_ITEM(m_item38, PARAM_ID_DIGITAL_OUT_1_STATE, &m_item37, NULL, NULL, &m_item39, TEXT(TR_ID_MENU_DIGITAL_OUT_1_STATE), NULL, 0, &menu_val_gui_digital_states, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item39, PARAM_ID_DIGITAL_OUT_2_STATE, &m_item37, NULL, &m_item38, &m_item40, TEXT(TR_ID_MENU_DIGITAL_OUT_2_STATE), NULL, 0, &menu_val_gui_digital_states, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item40, PARAM_ID_DIGITAL_OUT_3_STATE, &m_item37, NULL, &m_item39, &m_item41, TEXT(TR_ID_MENU_DIGITAL_OUT_3_STATE), NULL, 0, &menu_val_gui_digital_states, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item41, PARAM_ID_DIGITAL_OUT_4_STATE, &m_item37, NULL, &m_item40, NULL, TEXT(TR_ID_MENU_DIGITAL_OUT_4_STATE), NULL, 0, &menu_val_gui_digital_states, MENU_FLAG_VALUE, 0);
	SUBMENU(m_item42, 0, &m_item9, &m_item43, &m_item37, NULL, TEXT(TR_ID_MENU_RUNTIMES), NULL, 0, 0, 0);
		MENU_ITEM(m_item43, PARAM_ID_LAST_RUNTIME, &m_item42, NULL, NULL, &m_item44, TEXT(TR_ID_MENU_LAST_RUNTIME), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item44, PARAM_ID_RUNTIME, &m_item42, NULL, &m_item43, &m_item45, TEXT(TR_ID_MENU_RUNTIME), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item45, PARAM_ID_FAN_RUNTIME, &m_item42, NULL, &m_item44, &m_item46, TEXT(TR_ID_MENU_FAN_RUNTIME), NULL, 0, 0, MENU_FLAG_VALUE, 0);
		MENU_ITEM(m_item46, PARAM_ID_LIFETIME, &m_item42, NULL, &m_item45, NULL, TEXT(TR_ID_MENU_LIFETIME), NULL, 0, 0, MENU_FLAG_VALUE, 0);
SUBMENU(m_item47, 0, NULL, &m_item48, &m_item9, &m_item132, TEXT(TR_ID_MENU_GUI), NULL, 0, MENU_FLAG_ROOT, 0);
	MENU_ITEM(m_item48, PARAM_ID_GUI_PASSWORD_ADMIN, &m_item47, NULL, NULL, &m_item49, TEXT(TR_ID_MENU_GUI_PASSWORD_ADMIN), NULL, 0, 0, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0);
	MENU_ITEM(m_item49, PARAM_ID_GUI_PASSWORD_ROOT, &m_item47, NULL, &m_item48, &m_item50, TEXT(TR_ID_MENU_GUI_PASSWORD_ROOT), NULL, 0, 0, MENU_FLAG_DATA | MENU_FLAG_ROOT, 0);
	SUBMENU(m_item50, 0, &m_item47, &m_item51, &m_item49, &m_item55, TEXT(TR_ID_MENU_GUI_TILES), NULL, 0, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0);
		MENU_ITEM(m_item51, PARAM_ID_GUI_TILE_1, &m_item50, NULL, NULL, &m_item52, TEXT(TR_ID_MENU_GUI_TILE_1), NULL, 0, &menu_val_gui_tiles, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0);
		MENU_ITEM(m_item52, PARAM_ID_GUI_TILE_2, &m_item50, NULL, &m_item51, &m_item53, TEXT(TR_ID_MENU_GUI_TILE_2), NULL, 0, &menu_val_gui_tiles, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0);
		MENU_ITEM(m_item53, PARAM_ID_GUI_TILE_3, &m_item50, NULL, &m_item52, &m_item54, TEXT(TR_ID_MENU_GUI_TILE_3), NULL, 0, &menu_val_gui_tiles, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0);
		MENU_ITEM(m_item54, PARAM_ID_GUI_TILE_4, &m_item50, NULL, &m_item53, NULL, TEXT(TR_ID_MENU_GUI_TILE_4), NULL, 0, &menu_val_gui_tiles, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0);
	SUBMENU(m_item55, 0, &m_item47, &m_item56, &m_item50, &m_item131, TEXT(TR_ID_MENU_GUI_TILE_VALUES), NULL, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item56, 0, &m_item55, &m_item57, NULL, &m_item61, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item57, PARAM_ID_POWER_U_ALARM_MIN, &m_item56, NULL, NULL, &m_item58, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item58, PARAM_ID_POWER_U_WARN_MIN, &m_item56, NULL, &m_item57, &m_item59, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item59, PARAM_ID_POWER_U_WARN_MAX, &m_item56, NULL, &m_item58, &m_item60, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item60, PARAM_ID_POWER_U_ALARM_MAX, &m_item56, NULL, &m_item59, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item61, 0, &m_item55, &m_item62, &m_item56, &m_item66, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item62, PARAM_ID_POWER_U_ROT_ALARM_MIN, &m_item61, NULL, NULL, &m_item63, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item63, PARAM_ID_POWER_U_ROT_WARN_MIN, &m_item61, NULL, &m_item62, &m_item64, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item64, PARAM_ID_POWER_U_ROT_WARN_MAX, &m_item61, NULL, &m_item63, &m_item65, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item65, PARAM_ID_POWER_U_ROT_ALARM_MAX, &m_item61, NULL, &m_item64, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item66, 0, &m_item55, &m_item67, &m_item61, &m_item71, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item67, PARAM_ID_POWER_I_ALARM_MIN, &m_item66, NULL, NULL, &m_item68, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item68, PARAM_ID_POWER_I_WARN_MIN, &m_item66, NULL, &m_item67, &m_item69, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item69, PARAM_ID_POWER_I_WARN_MAX, &m_item66, NULL, &m_item68, &m_item70, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item70, PARAM_ID_POWER_I_ALARM_MAX, &m_item66, NULL, &m_item69, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item71, 0, &m_item55, &m_item72, &m_item66, &m_item76, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item72, PARAM_ID_POWER_I_ROT_ALARM_MIN, &m_item71, NULL, NULL, &m_item73, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item73, PARAM_ID_POWER_I_ROT_WARN_MIN, &m_item71, NULL, &m_item72, &m_item74, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item74, PARAM_ID_POWER_I_ROT_WARN_MAX, &m_item71, NULL, &m_item73, &m_item75, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item75, PARAM_ID_POWER_I_ROT_ALARM_MAX, &m_item71, NULL, &m_item74, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item76, 0, &m_item55, &m_item77, &m_item71, &m_item81, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item77, PARAM_ID_POWER_I_EXC_ALARM_MIN, &m_item76, NULL, NULL, &m_item78, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item78, PARAM_ID_POWER_I_EXC_WARN_MIN, &m_item76, NULL, &m_item77, &m_item79, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item79, PARAM_ID_POWER_I_EXC_WARN_MAX, &m_item76, NULL, &m_item78, &m_item80, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item80, PARAM_ID_POWER_I_EXC_ALARM_MAX, &m_item76, NULL, &m_item79, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item81, 0, &m_item55, &m_item82, &m_item76, &m_item86, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item82, PARAM_ID_HEATSINK_TEMP_ALARM_MIN, &m_item81, NULL, NULL, &m_item83, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item83, PARAM_ID_HEATSINK_TEMP_WARN_MIN, &m_item81, NULL, &m_item82, &m_item84, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item84, PARAM_ID_HEATSINK_TEMP_WARN_MAX, &m_item81, NULL, &m_item83, &m_item85, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item85, PARAM_ID_HEATSINK_TEMP_ALARM_MAX, &m_item81, NULL, &m_item84, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item86, 0, &m_item55, &m_item87, &m_item81, &m_item91, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item87, PARAM_ID_POWER_I_FAN_ALARM_MIN, &m_item86, NULL, NULL, &m_item88, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item88, PARAM_ID_POWER_I_FAN_WARN_MIN, &m_item86, NULL, &m_item87, &m_item89, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item89, PARAM_ID_POWER_I_FAN_WARN_MAX, &m_item86, NULL, &m_item88, &m_item90, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item90, PARAM_ID_POWER_I_FAN_ALARM_MAX, &m_item86, NULL, &m_item89, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item91, 0, &m_item55, &m_item92, &m_item86, &m_item96, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item92, PARAM_ID_POWER_I_REF_ALARM_MIN, &m_item91, NULL, NULL, &m_item93, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item93, PARAM_ID_POWER_I_REF_WARN_MIN, &m_item91, NULL, &m_item92, &m_item94, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item94, PARAM_ID_POWER_I_REF_WARN_MAX, &m_item91, NULL, &m_item93, &m_item95, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item95, PARAM_ID_POWER_I_REF_ALARM_MAX, &m_item91, NULL, &m_item94, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item96, 0, &m_item55, &m_item97, &m_item91, &m_item101, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item97, PARAM_ID_HEATSINK_FAN_RPM_ALARM_MIN, &m_item96, NULL, NULL, &m_item98, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item98, PARAM_ID_HEATSINK_FAN_RPM_WARN_MIN, &m_item96, NULL, &m_item97, &m_item99, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item99, PARAM_ID_HEATSINK_FAN_RPM_WARN_MAX, &m_item96, NULL, &m_item98, &m_item100, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item100, PARAM_ID_HEATSINK_FAN_RPM_ALARM_MAX, &m_item96, NULL, &m_item99, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item101, 0, &m_item55, &m_item102, &m_item96, &m_item106, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item102, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MIN, &m_item101, NULL, NULL, &m_item103, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item103, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_WARN_MIN, &m_item101, NULL, &m_item102, &m_item104, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item104, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_WARN_MAX, &m_item101, NULL, &m_item103, &m_item105, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item105, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MAX, &m_item101, NULL, &m_item104, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item106, 0, &m_item55, &m_item107, &m_item101, &m_item111, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item107, PARAM_ID_TRIAC_EXC_OPEN_ANGLE_ALARM_MIN, &m_item106, NULL, NULL, &m_item108, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item108, PARAM_ID_TRIAC_EXC_OPEN_ANGLE_WARN_MIN, &m_item106, NULL, &m_item107, &m_item109, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item109, PARAM_ID_TRIAC_EXC_OPEN_ANGLE_WARN_MAX, &m_item106, NULL, &m_item108, &m_item110, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item110, PARAM_ID_TRIAC_EXC_OPEN_ANGLE_ALARM_MAX, &m_item106, NULL, &m_item109, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item111, 0, &m_item55, &m_item112, &m_item106, &m_item116, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item112, PARAM_ID_MOTOR_EFF_ALARM_MIN, &m_item111, NULL, NULL, &m_item113, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item113, PARAM_ID_MOTOR_EFF_WARN_MIN, &m_item111, NULL, &m_item112, &m_item114, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item114, PARAM_ID_MOTOR_EFF_WARN_MAX, &m_item111, NULL, &m_item113, &m_item115, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item115, PARAM_ID_MOTOR_EFF_ALARM_MAX, &m_item111, NULL, &m_item114, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item116, 0, &m_item55, &m_item117, &m_item111, &m_item121, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item117, PARAM_ID_MOTOR_R_ROT_ALARM_MIN, &m_item116, NULL, NULL, &m_item118, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item118, PARAM_ID_MOTOR_R_ROT_WARN_MIN, &m_item116, NULL, &m_item117, &m_item119, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item119, PARAM_ID_MOTOR_R_ROT_WARN_MAX, &m_item116, NULL, &m_item118, &m_item120, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item120, PARAM_ID_MOTOR_R_ROT_ALARM_MAX, &m_item116, NULL, &m_item119, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item121, 0, &m_item55, &m_item122, &m_item116, &m_item126, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item122, PARAM_ID_MOTOR_RPM_ALARM_MIN, &m_item121, NULL, NULL, &m_item123, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item123, PARAM_ID_MOTOR_RPM_WARN_MIN, &m_item121, NULL, &m_item122, &m_item124, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item124, PARAM_ID_MOTOR_RPM_WARN_MAX, &m_item121, NULL, &m_item123, &m_item125, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item125, PARAM_ID_MOTOR_RPM_ALARM_MAX, &m_item121, NULL, &m_item124, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item126, 0, &m_item55, &m_item127, &m_item121, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE), NULL, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item127, PARAM_ID_MOTOR_TORQUE_ALARM_MIN, &m_item126, NULL, NULL, &m_item128, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_ALARM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item128, PARAM_ID_MOTOR_TORQUE_WARN_MIN, &m_item126, NULL, &m_item127, &m_item129, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_WARN_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item129, PARAM_ID_MOTOR_TORQUE_WARN_MAX, &m_item126, NULL, &m_item128, &m_item130, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_WARN_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item130, PARAM_ID_MOTOR_TORQUE_ALARM_MAX, &m_item126, NULL, &m_item129, NULL, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_ALARM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item131, PARAM_ID_MENU_GUI_TILE_WARNINGS, &m_item47, NULL, &m_item55, NULL, TEXT(TR_ID_MENU_GUI_TILE_WARNINGS), NULL, 0, &menu_val_gui_tiles_for_warnings, MENU_FLAG_DATA, 0);
SUBMENU(m_item132, 0, NULL, &m_item133, &m_item47, &m_item135, TEXT(TR_ID_MENU_SUPPLY_IN), TEXT(TR_ID_HELP_POWER_IN), 0, 0, 0);
	MENU_ITEM(m_item133, PARAM_ID_U_NOM, &m_item132, NULL, NULL, &m_item134, TEXT(TR_ID_MENU_U_NOM), TEXT(TR_ID_HELP_U_NOM), 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item134, PARAM_ID_I_NOM, &m_item132, NULL, &m_item133, NULL, TEXT(TR_ID_MENU_I_NOM), TEXT(TR_ID_HELP_I_NOM), 0, 0, MENU_FLAG_DATA, 0);
SUBMENU(m_item135, 0, NULL, &m_item136, &m_item132, &m_item145, TEXT(TR_ID_MENU_MOTOR), NULL, 0, 0, 0);
	MENU_ITEM(m_item136, PARAM_ID_MOTOR_P_NOM, &m_item135, NULL, NULL, &m_item137, TEXT(TR_ID_MENU_MOTOR_P_NOM), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item137, PARAM_ID_MOTOR_RPM_NOM, &m_item135, NULL, &m_item136, &m_item138, TEXT(TR_ID_MENU_MOTOR_RPM_NOM), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item138, PARAM_ID_MOTOR_RPM_MAX, &m_item135, NULL, &m_item137, &m_item139, TEXT(TR_ID_MENU_MOTOR_RPM_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item139, PARAM_ID_MOTOR_U_ROT_NOM, &m_item135, NULL, &m_item138, &m_item140, TEXT(TR_ID_MENU_MOTOR_U_ROT_NOM), TEXT(TR_ID_HELP_U_ROT_NOM), 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item140, PARAM_ID_MOTOR_I_ROT_NOM, &m_item135, NULL, &m_item139, &m_item141, TEXT(TR_ID_MENU_MOTOR_I_ROT_NOM), TEXT(TR_ID_HELP_I_ROT_NOM), 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item141, PARAM_ID_MOTOR_U_EXC_NOM, &m_item135, NULL, &m_item140, &m_item142, TEXT(TR_ID_MENU_MOTOR_U_EXC_NOM), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item142, PARAM_ID_MOTOR_I_EXC_NOM, &m_item135, NULL, &m_item141, &m_item143, TEXT(TR_ID_MENU_MOTOR_I_EXC_NOM), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item143, PARAM_ID_MOTOR_EFF_NOM, &m_item135, NULL, &m_item142, &m_item144, TEXT(TR_ID_MENU_MOTOR_EFF_NOM), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item144, PARAM_ID_MOTOR_R_ROT_NOM, &m_item135, NULL, &m_item143, NULL, TEXT(TR_ID_MENU_MOTOR_R_ROT_NOM), NULL, 0, 0, MENU_FLAG_DATA, 0);
SUBMENU(m_item145, 0, NULL, &m_item146, &m_item135, &m_item149, TEXT(TR_ID_MENU_EXCITATION), NULL, 0, 0, 0);
	MENU_ITEM(m_item146, PARAM_ID_I_EXC, &m_item145, NULL, NULL, &m_item147, TEXT(TR_ID_MENU_I_EXC), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item147, PARAM_ID_EXC_PHASE, &m_item145, NULL, &m_item146, &m_item148, TEXT(TR_ID_MENU_EXC_PHASE), NULL, 0, &menu_val_phase, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item148, PARAM_ID_EXC_MODE, &m_item145, NULL, &m_item147, NULL, TEXT(TR_ID_MENU_EXC_MODE), NULL, 0, &menu_val_exc_mode, MENU_FLAG_DATA, 0);
SUBMENU(m_item149, 0, NULL, &m_item150, &m_item145, &m_item155, TEXT(TR_ID_MENU_RAMP), NULL, 0, 0, 0);
	MENU_ITEM(m_item150, PARAM_ID_RAMP_START_TIME, &m_item149, NULL, NULL, &m_item151, TEXT(TR_ID_MENU_RAMP_START_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item151, PARAM_ID_RAMP_STOP_TIME, &m_item149, NULL, &m_item150, &m_item152, TEXT(TR_ID_MENU_RAMP_STOP_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item152, PARAM_ID_RAMP_FAST_STOP_TIME, &m_item149, NULL, &m_item151, &m_item153, TEXT(TR_ID_MENU_RAMP_FAST_STOP_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item153, PARAM_ID_RAMP_REFERENCE_TIME, &m_item149, NULL, &m_item152, &m_item154, TEXT(TR_ID_MENU_RAMP_REFERENCE_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item154, PARAM_ID_RAMP_STOP_MODE, &m_item149, NULL, &m_item153, NULL, TEXT(TR_ID_MENU_RAMP_STOP_MODE), NULL, 0, &menu_val_stop_mode, MENU_FLAG_DATA, 0);
SUBMENU(m_item155, 0, NULL, &m_item156, &m_item149, &m_item161, TEXT(TR_ID_MENU_PHASE_SYNC), NULL, 0, 0, 0);
	MENU_ITEM(m_item156, PARAM_ID_PHASE_SYNC_ACCURACY, &m_item155, NULL, NULL, &m_item157, TEXT(TR_ID_MENU_PHASE_SYNC_ACCURACY), NULL, 0, 0, MENU_FLAG_DATA, 0);
	SUBMENU(m_item157, 0, &m_item155, &m_item158, &m_item156, NULL, TEXT(TR_ID_MENU_PHASE_SYNC_PID), NULL, 0, 0, 0);
		MENU_ITEM(m_item158, PARAM_ID_PHASE_SYNC_PLL_PID_K_P, &m_item157, NULL, NULL, &m_item159, TEXT(TR_ID_MENU_PID_K_P), NULL, 0, 0, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item159, PARAM_ID_PHASE_SYNC_PLL_PID_K_I, &m_item157, NULL, &m_item158, &m_item160, TEXT(TR_ID_MENU_PID_K_I), NULL, 0, 0, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item160, PARAM_ID_PHASE_SYNC_PLL_PID_K_D, &m_item157, NULL, &m_item159, NULL, TEXT(TR_ID_MENU_PID_K_D), NULL, 0, 0, MENU_FLAG_DATA, 0);
SUBMENU(m_item161, 0, NULL, &m_item162, &m_item155, &m_item163, TEXT(TR_ID_MENU_REGULATOR), NULL, 0, 0, 0);
	MENU_ITEM(m_item162, PARAM_ID_REGULATOR_MODE, &m_item161, NULL, NULL, NULL, TEXT(TR_ID_MENU_REGULATOR_MODE), NULL, 0, &menu_val_regulator_mode, MENU_FLAG_DATA, 0);
SUBMENU(m_item163, 0, NULL, &m_item164, &m_item161, &m_item176, TEXT(TR_ID_MENU_PID), NULL, 0, 0, 0);
	SUBMENU(m_item164, 0, &m_item163, &m_item165, NULL, &m_item168, TEXT(TR_ID_MENU_PID_SPD), NULL, 0, 0, 0);
		MENU_ITEM(m_item165, PARAM_ID_SPD_PID_K_P, &m_item164, NULL, NULL, &m_item166, TEXT(TR_ID_MENU_PID_K_P), NULL, 0, 0, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item166, PARAM_ID_SPD_PID_K_I, &m_item164, NULL, &m_item165, &m_item167, TEXT(TR_ID_MENU_PID_K_I), NULL, 0, 0, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item167, PARAM_ID_SPD_PID_K_D, &m_item164, NULL, &m_item166, NULL, TEXT(TR_ID_MENU_PID_K_D), NULL, 0, 0, MENU_FLAG_DATA, 0);
	SUBMENU(m_item168, 0, &m_item163, &m_item169, &m_item164, &m_item172, TEXT(TR_ID_MENU_PID_ROT), NULL, 0, 0, 0);
		MENU_ITEM(m_item169, PARAM_ID_ROT_PID_K_P, &m_item168, NULL, NULL, &m_item170, TEXT(TR_ID_MENU_PID_K_P), NULL, 0, 0, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item170, PARAM_ID_ROT_PID_K_I, &m_item168, NULL, &m_item169, &m_item171, TEXT(TR_ID_MENU_PID_K_I), NULL, 0, 0, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item171, PARAM_ID_ROT_PID_K_D, &m_item168, NULL, &m_item170, NULL, TEXT(TR_ID_MENU_PID_K_D), NULL, 0, 0, MENU_FLAG_DATA, 0);
	SUBMENU(m_item172, 0, &m_item163, &m_item173, &m_item168, NULL, TEXT(TR_ID_MENU_PID_EXC), NULL, 0, 0, 0);
		MENU_ITEM(m_item173, PARAM_ID_EXC_PID_K_P, &m_item172, NULL, NULL, &m_item174, TEXT(TR_ID_MENU_PID_K_P), NULL, 0, 0, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item174, PARAM_ID_EXC_PID_K_I, &m_item172, NULL, &m_item173, &m_item175, TEXT(TR_ID_MENU_PID_K_I), NULL, 0, 0, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item175, PARAM_ID_EXC_PID_K_D, &m_item172, NULL, &m_item174, NULL, TEXT(TR_ID_MENU_PID_K_D), NULL, 0, 0, MENU_FLAG_DATA, 0);
SUBMENU(m_item176, 0, NULL, &m_item177, &m_item163, &m_item182, TEXT(TR_ID_MENU_START_STOP), NULL, 0, 0, 0);
	MENU_ITEM(m_item177, PARAM_ID_ROT_STOP_TIME, &m_item176, NULL, NULL, &m_item178, TEXT(TR_ID_MENU_ROT_STOP_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item178, PARAM_ID_EXC_STOP_TIME, &m_item176, NULL, &m_item177, &m_item179, TEXT(TR_ID_MENU_EXC_STOP_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item179, PARAM_ID_EXC_START_TIME, &m_item176, NULL, &m_item178, &m_item180, TEXT(TR_ID_MENU_EXC_START_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item180, PARAM_ID_PHASES_CHECK_TIME, &m_item176, NULL, &m_item179, &m_item181, TEXT(TR_ID_MENU_PHASES_CHECK_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item181, PARAM_ID_ZERO_SENSOR_TIME, &m_item176, NULL, &m_item180, NULL, TEXT(TR_ID_MENU_ZERO_SENSOR_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
SUBMENU(m_item182, 0, NULL, &m_item183, &m_item176, &m_item193, TEXT(TR_ID_MENU_TRIACS), NULL, 0, 0, 0);
	SUBMENU(m_item183, 0, &m_item182, &m_item184, NULL, &m_item186, TEXT(TR_ID_MENU_TRIACS_OPEN_TIME), NULL, 0, 0, 0);
		MENU_ITEM(m_item184, PARAM_ID_TRIACS_PAIRS_OPEN_TIME, &m_item183, NULL, NULL, &m_item185, TEXT(TR_ID_MENU_TRIACS_PAIRS_OPEN_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item185, PARAM_ID_TRIAC_EXC_OPEN_TIME, &m_item183, NULL, &m_item184, NULL, TEXT(TR_ID_MENU_TRIAC_EXC_OPEN_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
	SUBMENU(m_item186, 0, &m_item182, &m_item187, &m_item183, NULL, TEXT(TR_ID_MENU_TRIACS_OPEN_ANGLES), NULL, 0, 0, 0);
		SUBMENU(m_item187, 0, &m_item186, &m_item188, NULL, &m_item190, TEXT(TR_ID_MENU_ROTOR), NULL, 0, 0, 0);
			MENU_ITEM(m_item188, PARAM_ID_TRIACS_PAIRS_ANGLE_MIN, &m_item187, NULL, NULL, &m_item189, TEXT(TR_ID_MENU_TRIACS_OPEN_ANGLE_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item189, PARAM_ID_TRIACS_PAIRS_ANGLE_MAX, &m_item187, NULL, &m_item188, NULL, TEXT(TR_ID_MENU_TRIACS_OPEN_ANGLE_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item190, 0, &m_item186, &m_item191, &m_item187, NULL, TEXT(TR_ID_MENU_EXCITATION), NULL, 0, 0, 0);
			MENU_ITEM(m_item191, PARAM_ID_TRIAC_EXC_ANGLE_MIN, &m_item190, NULL, NULL, &m_item192, TEXT(TR_ID_MENU_TRIACS_OPEN_ANGLE_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item192, PARAM_ID_TRIAC_EXC_ANGLE_MAX, &m_item190, NULL, &m_item191, NULL, TEXT(TR_ID_MENU_TRIACS_OPEN_ANGLE_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
SUBMENU(m_item193, 0, NULL, &m_item194, &m_item182, &m_item405, TEXT(TR_ID_MENU_PROTECTION), NULL, 0, 0, 0);
	SUBMENU(m_item194, 0, &m_item193, &m_item195, NULL, &m_item198, TEXT(TR_ID_MENU_THERMAL_OVERLOAD_PROT), NULL, 0, 0, 0);
		MENU_ITEM(m_item195, PARAM_ID_THERMAL_OVERLOAD_PROT_ENABLE, &m_item194, NULL, NULL, &m_item196, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item196, PARAM_ID_THERMAL_OVERLOAD_PROT_TIME_6I, &m_item194, NULL, &m_item195, &m_item197, TEXT(TR_ID_MENU_THERMAL_OVERLOAD_PROT_TIME_6I), NULL, 0, 0, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item197, PARAM_ID_THERMAL_OVERLOAD_PROT_ACTION, &m_item194, NULL, &m_item196, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
	SUBMENU(m_item198, 0, &m_item193, &m_item199, &m_item194, &m_item200, TEXT(TR_ID_MENU_EMERGENCY_STOP), NULL, 0, 0, 0);
		MENU_ITEM(m_item199, PARAM_ID_EMERGENCY_STOP_ACTION, &m_item198, NULL, NULL, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
	SUBMENU(m_item200, 0, &m_item193, &m_item201, &m_item198, &m_item206, TEXT(TR_ID_MENU_ROT_BREAK_PROT), NULL, 0, 0, 0);
				MENU_ITEM(m_item201, PARAM_ID_PROT_ROT_BREAK_ENABLED, &m_item200, NULL, NULL, &m_item202, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item202, PARAM_ID_PROT_ROT_BREAK_VALUE, &m_item200, NULL, &m_item201, &m_item203, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item203, PARAM_ID_PROT_ROT_BREAK_TIME_MS, &m_item200, NULL, &m_item202, &m_item204, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item204, PARAM_ID_PROT_ROT_BREAK_LATCH_ENABLE, &m_item200, NULL, &m_item203, &m_item205, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item205, PARAM_ID_PROT_ROT_BREAK_ACTION, &m_item200, NULL, &m_item204, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
	SUBMENU(m_item206, 0, &m_item193, &m_item207, &m_item200, &m_item237, TEXT(TR_ID_MENU_PHASES_PROT), NULL, 0, 0, 0);
		SUBMENU(m_item207, 0, &m_item206, &m_item208, NULL, &m_item211, TEXT(TR_ID_MENU_PHASE_STATE_PROT), NULL, 0, 0, 0);
			MENU_ITEM(m_item208, PARAM_ID_PROT_PHASES_STATE_ENABLED, &m_item207, NULL, NULL, &m_item209, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item209, PARAM_ID_PROT_PHASES_STATE_TIME_MS, &m_item207, NULL, &m_item208, &m_item210, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item210, PARAM_ID_PROT_PHASES_STATE_ACTION, &m_item207, NULL, &m_item209, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
		SUBMENU(m_item211, 0, &m_item206, &m_item212, &m_item207, &m_item224, TEXT(TR_ID_MENU_PHASES_ANGLES_PROT), NULL, 0, 0, 0);
			SUBMENU(m_item212, 0, &m_item211, &m_item213, NULL, &m_item218, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item213, PARAM_ID_PROT_PHASES_ANGLES_FAULT_ENABLED, &m_item212, NULL, NULL, &m_item214, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item214, PARAM_ID_PROT_PHASES_ANGLES_FAULT_VALUE, &m_item212, NULL, &m_item213, &m_item215, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item215, PARAM_ID_PROT_PHASES_ANGLES_FAULT_TIME_MS, &m_item212, NULL, &m_item214, &m_item216, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item216, PARAM_ID_PROT_PHASES_ANGLES_FAULT_LATCH_ENABLE, &m_item212, NULL, &m_item215, &m_item217, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item217, PARAM_ID_PROT_PHASES_ANGLES_FAULT_ACTION, &m_item212, NULL, &m_item216, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item218, 0, &m_item211, &m_item219, &m_item212, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item219, PARAM_ID_PROT_PHASES_ANGLES_WARN_ENABLED, &m_item218, NULL, NULL, &m_item220, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item220, PARAM_ID_PROT_PHASES_ANGLES_WARN_VALUE, &m_item218, NULL, &m_item219, &m_item221, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item221, PARAM_ID_PROT_PHASES_ANGLES_WARN_TIME_MS, &m_item218, NULL, &m_item220, &m_item222, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item222, PARAM_ID_PROT_PHASES_ANGLES_WARN_LATCH_ENABLE, &m_item218, NULL, &m_item221, &m_item223, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item223, PARAM_ID_PROT_PHASES_ANGLES_WARN_ACTION, &m_item218, NULL, &m_item222, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
		SUBMENU(m_item224, 0, &m_item206, &m_item225, &m_item211, NULL, TEXT(TR_ID_MENU_PHASES_SYNC_PROT), NULL, 0, 0, 0);
			SUBMENU(m_item225, 0, &m_item224, &m_item226, NULL, &m_item231, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item226, PARAM_ID_PROT_PHASES_SYNC_FAULT_ENABLED, &m_item225, NULL, NULL, &m_item227, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item227, PARAM_ID_PROT_PHASES_SYNC_FAULT_VALUE, &m_item225, NULL, &m_item226, &m_item228, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item228, PARAM_ID_PROT_PHASES_SYNC_FAULT_TIME_MS, &m_item225, NULL, &m_item227, &m_item229, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item229, PARAM_ID_PROT_PHASES_SYNC_FAULT_LATCH_ENABLE, &m_item225, NULL, &m_item228, &m_item230, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item230, PARAM_ID_PROT_PHASES_SYNC_FAULT_ACTION, &m_item225, NULL, &m_item229, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item231, 0, &m_item224, &m_item232, &m_item225, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item232, PARAM_ID_PROT_PHASES_SYNC_WARN_ENABLED, &m_item231, NULL, NULL, &m_item233, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item233, PARAM_ID_PROT_PHASES_SYNC_WARN_VALUE, &m_item231, NULL, &m_item232, &m_item234, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item234, PARAM_ID_PROT_PHASES_SYNC_WARN_TIME_MS, &m_item231, NULL, &m_item233, &m_item235, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item235, PARAM_ID_PROT_PHASES_SYNC_WARN_LATCH_ENABLE, &m_item231, NULL, &m_item234, &m_item236, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item236, PARAM_ID_PROT_PHASES_SYNC_WARN_ACTION, &m_item231, NULL, &m_item235, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
	SUBMENU(m_item237, 0, &m_item193, &m_item238, &m_item206, &m_item246, TEXT(TR_ID_MENU_HEATSINK_TEMP_PROT), NULL, 0, 0, 0);
		SUBMENU(m_item238, 0, &m_item237, &m_item239, NULL, &m_item242, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
			MENU_ITEM(m_item239, PARAM_ID_PROT_HEATSINK_TEMP_FAULT_ENABLED, &m_item238, NULL, NULL, &m_item240, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item240, PARAM_ID_PROT_HEATSINK_TEMP_FAULT_VALUE, &m_item238, NULL, &m_item239, &m_item241, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item241, PARAM_ID_PROT_HEATSINK_TEMP_FAULT_ACTION, &m_item238, NULL, &m_item240, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
		SUBMENU(m_item242, 0, &m_item237, &m_item243, &m_item238, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
			MENU_ITEM(m_item243, PARAM_ID_PROT_HEATSINK_TEMP_WARN_ENABLED, &m_item242, NULL, NULL, &m_item244, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item244, PARAM_ID_PROT_HEATSINK_TEMP_WARN_VALUE, &m_item242, NULL, &m_item243, &m_item245, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item245, PARAM_ID_PROT_HEATSINK_TEMP_WARN_ACTION, &m_item242, NULL, &m_item244, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
	SUBMENU(m_item246, 0, &m_item193, &m_item247, &m_item237, &m_item275, TEXT(TR_ID_MENU_POWER_VOLTAGE), NULL, 0, 0, 0);
		SUBMENU(m_item247, 0, &m_item246, &m_item248, NULL, &m_item249, TEXT(TR_ID_MENU_CUTOFF), NULL, 0, 0, 0);
			MENU_ITEM(m_item248, PARAM_ID_PROT_U_IN_CUTOFF_LEVEL_VALUE, &m_item247, NULL, NULL, NULL, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item249, 0, &m_item246, &m_item250, &m_item247, &m_item262, TEXT(TR_ID_MENU_PROT_OVERFLOW), NULL, 0, 0, 0);
			SUBMENU(m_item250, 0, &m_item249, &m_item251, NULL, &m_item256, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item251, PARAM_ID_PROT_U_IN_OVF_FAULT_ENABLE, &m_item250, NULL, NULL, &m_item252, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item252, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_VALUE, &m_item250, NULL, &m_item251, &m_item253, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item253, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_TIME_MS, &m_item250, NULL, &m_item252, &m_item254, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item254, PARAM_ID_PROT_U_IN_OVF_FAULT_LATCH_ENABLE, &m_item250, NULL, &m_item253, &m_item255, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item255, PARAM_ID_PROT_U_IN_OVF_FAULT_ACTION, &m_item250, NULL, &m_item254, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item256, 0, &m_item249, &m_item257, &m_item250, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item257, PARAM_ID_PROT_U_IN_OVF_WARN_ENABLE, &m_item256, NULL, NULL, &m_item258, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item258, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_VALUE, &m_item256, NULL, &m_item257, &m_item259, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item259, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_TIME_MS, &m_item256, NULL, &m_item258, &m_item260, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item260, PARAM_ID_PROT_U_IN_OVF_WARN_LATCH_ENABLE, &m_item256, NULL, &m_item259, &m_item261, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item261, PARAM_ID_PROT_U_IN_OVF_WARN_ACTION, &m_item256, NULL, &m_item260, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
		SUBMENU(m_item262, 0, &m_item246, &m_item263, &m_item249, NULL, TEXT(TR_ID_MENU_PROT_UNDERFLOW), NULL, 0, 0, 0);
			SUBMENU(m_item263, 0, &m_item262, &m_item264, NULL, &m_item269, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item264, PARAM_ID_PROT_U_IN_UDF_FAULT_ENABLE, &m_item263, NULL, NULL, &m_item265, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item265, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_VALUE, &m_item263, NULL, &m_item264, &m_item266, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item266, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_TIME_MS, &m_item263, NULL, &m_item265, &m_item267, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item267, PARAM_ID_PROT_U_IN_UDF_FAULT_LATCH_ENABLE, &m_item263, NULL, &m_item266, &m_item268, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item268, PARAM_ID_PROT_U_IN_UDF_FAULT_ACTION, &m_item263, NULL, &m_item267, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item269, 0, &m_item262, &m_item270, &m_item263, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item270, PARAM_ID_PROT_U_IN_UDF_WARN_ENABLE, &m_item269, NULL, NULL, &m_item271, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item271, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_VALUE, &m_item269, NULL, &m_item270, &m_item272, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item272, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_TIME_MS, &m_item269, NULL, &m_item271, &m_item273, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item273, PARAM_ID_PROT_U_IN_UDF_WARN_LATCH_ENABLE, &m_item269, NULL, &m_item272, &m_item274, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item274, PARAM_ID_PROT_U_IN_UDF_WARN_ACTION, &m_item269, NULL, &m_item273, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
	SUBMENU(m_item275, 0, &m_item193, &m_item276, &m_item246, &m_item291, TEXT(TR_ID_MENU_POWER_CURRENT), NULL, 0, 0, 0);
		SUBMENU(m_item276, 0, &m_item275, &m_item277, NULL, &m_item278, TEXT(TR_ID_MENU_CUTOFF), NULL, 0, 0, 0);
			MENU_ITEM(m_item277, PARAM_ID_PROT_I_IN_CUTOFF_LEVEL_VALUE, &m_item276, NULL, NULL, NULL, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item278, 0, &m_item275, &m_item279, &m_item276, NULL, TEXT(TR_ID_MENU_PROT_OVERFLOW), NULL, 0, 0, 0);
			SUBMENU(m_item279, 0, &m_item278, &m_item280, NULL, &m_item285, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item280, PARAM_ID_PROT_I_IN_OVF_FAULT_ENABLE, &m_item279, NULL, NULL, &m_item281, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item281, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_VALUE, &m_item279, NULL, &m_item280, &m_item282, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item282, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_TIME_MS, &m_item279, NULL, &m_item281, &m_item283, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item283, PARAM_ID_PROT_I_IN_OVF_FAULT_LATCH_ENABLE, &m_item279, NULL, &m_item282, &m_item284, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item284, PARAM_ID_PROT_I_IN_OVF_FAULT_ACTION, &m_item279, NULL, &m_item283, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item285, 0, &m_item278, &m_item286, &m_item279, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item286, PARAM_ID_PROT_I_IN_OVF_WARN_ENABLE, &m_item285, NULL, NULL, &m_item287, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item287, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_VALUE, &m_item285, NULL, &m_item286, &m_item288, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item288, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_TIME_MS, &m_item285, NULL, &m_item287, &m_item289, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item289, PARAM_ID_PROT_I_IN_OVF_WARN_LATCH_ENABLE, &m_item285, NULL, &m_item288, &m_item290, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item290, PARAM_ID_PROT_I_IN_OVF_WARN_ACTION, &m_item285, NULL, &m_item289, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
	SUBMENU(m_item291, 0, &m_item193, &m_item292, &m_item275, &m_item307, TEXT(TR_ID_MENU_ROTOR_VOLTAGE), NULL, 0, 0, 0);
		SUBMENU(m_item292, 0, &m_item291, &m_item293, NULL, &m_item294, TEXT(TR_ID_MENU_CUTOFF), NULL, 0, 0, 0);
			MENU_ITEM(m_item293, PARAM_ID_PROT_U_ROT_CUTOFF_LEVEL_VALUE, &m_item292, NULL, NULL, NULL, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item294, 0, &m_item291, &m_item295, &m_item292, NULL, TEXT(TR_ID_MENU_PROT_OVERFLOW), NULL, 0, 0, 0);
			SUBMENU(m_item295, 0, &m_item294, &m_item296, NULL, &m_item301, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item296, PARAM_ID_PROT_U_ROT_OVF_FAULT_ENABLE, &m_item295, NULL, NULL, &m_item297, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item297, PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_VALUE, &m_item295, NULL, &m_item296, &m_item298, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item298, PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_TIME_MS, &m_item295, NULL, &m_item297, &m_item299, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item299, PARAM_ID_PROT_U_ROT_OVF_FAULT_LATCH_ENABLE, &m_item295, NULL, &m_item298, &m_item300, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item300, PARAM_ID_PROT_U_ROT_OVF_FAULT_ACTION, &m_item295, NULL, &m_item299, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item301, 0, &m_item294, &m_item302, &m_item295, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item302, PARAM_ID_PROT_U_ROT_OVF_WARN_ENABLE, &m_item301, NULL, NULL, &m_item303, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item303, PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_VALUE, &m_item301, NULL, &m_item302, &m_item304, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item304, PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_TIME_MS, &m_item301, NULL, &m_item303, &m_item305, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item305, PARAM_ID_PROT_U_ROT_OVF_WARN_LATCH_ENABLE, &m_item301, NULL, &m_item304, &m_item306, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item306, PARAM_ID_PROT_U_ROT_OVF_WARN_ACTION, &m_item301, NULL, &m_item305, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
	SUBMENU(m_item307, 0, &m_item193, &m_item308, &m_item291, &m_item323, TEXT(TR_ID_MENU_ROTOR_CURRENT), NULL, 0, 0, 0);
		SUBMENU(m_item308, 0, &m_item307, &m_item309, NULL, &m_item310, TEXT(TR_ID_MENU_CUTOFF), NULL, 0, 0, 0);
			MENU_ITEM(m_item309, PARAM_ID_PROT_I_ROT_CUTOFF_LEVEL_VALUE, &m_item308, NULL, NULL, NULL, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item310, 0, &m_item307, &m_item311, &m_item308, NULL, TEXT(TR_ID_MENU_PROT_OVERFLOW), NULL, 0, 0, 0);
			SUBMENU(m_item311, 0, &m_item310, &m_item312, NULL, &m_item317, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item312, PARAM_ID_PROT_I_ROT_OVF_FAULT_ENABLE, &m_item311, NULL, NULL, &m_item313, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item313, PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_VALUE, &m_item311, NULL, &m_item312, &m_item314, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item314, PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_TIME_MS, &m_item311, NULL, &m_item313, &m_item315, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item315, PARAM_ID_PROT_I_ROT_OVF_FAULT_LATCH_ENABLE, &m_item311, NULL, &m_item314, &m_item316, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item316, PARAM_ID_PROT_I_ROT_OVF_FAULT_ACTION, &m_item311, NULL, &m_item315, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item317, 0, &m_item310, &m_item318, &m_item311, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item318, PARAM_ID_PROT_I_ROT_OVF_WARN_ENABLE, &m_item317, NULL, NULL, &m_item319, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item319, PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_VALUE, &m_item317, NULL, &m_item318, &m_item320, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item320, PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_TIME_MS, &m_item317, NULL, &m_item319, &m_item321, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item321, PARAM_ID_PROT_I_ROT_OVF_WARN_LATCH_ENABLE, &m_item317, NULL, &m_item320, &m_item322, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item322, PARAM_ID_PROT_I_ROT_OVF_WARN_ACTION, &m_item317, NULL, &m_item321, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
	SUBMENU(m_item323, 0, &m_item193, &m_item324, &m_item307, &m_item352, TEXT(TR_ID_MENU_EXC_CURRENT), NULL, 0, 0, 0);
		SUBMENU(m_item324, 0, &m_item323, &m_item325, NULL, &m_item326, TEXT(TR_ID_MENU_CUTOFF), NULL, 0, 0, 0);
			MENU_ITEM(m_item325, PARAM_ID_PROT_I_EXC_CUTOFF_LEVEL_VALUE, &m_item324, NULL, NULL, NULL, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
		SUBMENU(m_item326, 0, &m_item323, &m_item327, &m_item324, &m_item339, TEXT(TR_ID_MENU_PROT_OVERFLOW), NULL, 0, 0, 0);
			SUBMENU(m_item327, 0, &m_item326, &m_item328, NULL, &m_item333, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item328, PARAM_ID_PROT_I_EXC_OVF_FAULT_ENABLE, &m_item327, NULL, NULL, &m_item329, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item329, PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_VALUE, &m_item327, NULL, &m_item328, &m_item330, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item330, PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_TIME_MS, &m_item327, NULL, &m_item329, &m_item331, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item331, PARAM_ID_PROT_I_EXC_OVF_FAULT_LATCH_ENABLE, &m_item327, NULL, &m_item330, &m_item332, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item332, PARAM_ID_PROT_I_EXC_OVF_FAULT_ACTION, &m_item327, NULL, &m_item331, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item333, 0, &m_item326, &m_item334, &m_item327, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item334, PARAM_ID_PROT_I_EXC_OVF_WARN_ENABLE, &m_item333, NULL, NULL, &m_item335, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item335, PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_VALUE, &m_item333, NULL, &m_item334, &m_item336, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item336, PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_TIME_MS, &m_item333, NULL, &m_item335, &m_item337, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item337, PARAM_ID_PROT_I_EXC_OVF_WARN_LATCH_ENABLE, &m_item333, NULL, &m_item336, &m_item338, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item338, PARAM_ID_PROT_I_EXC_OVF_WARN_ACTION, &m_item333, NULL, &m_item337, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
		SUBMENU(m_item339, 0, &m_item323, &m_item340, &m_item326, NULL, TEXT(TR_ID_MENU_PROT_UNDERFLOW), NULL, 0, 0, 0);
			SUBMENU(m_item340, 0, &m_item339, &m_item341, NULL, &m_item346, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item341, PARAM_ID_PROT_I_EXC_UDF_FAULT_ENABLE, &m_item340, NULL, NULL, &m_item342, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item342, PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_VALUE, &m_item340, NULL, &m_item341, &m_item343, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item343, PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_TIME_MS, &m_item340, NULL, &m_item342, &m_item344, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item344, PARAM_ID_PROT_I_EXC_UDF_FAULT_LATCH_ENABLE, &m_item340, NULL, &m_item343, &m_item345, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item345, PARAM_ID_PROT_I_EXC_UDF_FAULT_ACTION, &m_item340, NULL, &m_item344, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item346, 0, &m_item339, &m_item347, &m_item340, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item347, PARAM_ID_PROT_I_EXC_UDF_WARN_ENABLE, &m_item346, NULL, NULL, &m_item348, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item348, PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_VALUE, &m_item346, NULL, &m_item347, &m_item349, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item349, PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_TIME_MS, &m_item346, NULL, &m_item348, &m_item350, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item350, PARAM_ID_PROT_I_EXC_UDF_WARN_LATCH_ENABLE, &m_item346, NULL, &m_item349, &m_item351, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item351, PARAM_ID_PROT_I_EXC_UDF_WARN_ACTION, &m_item346, NULL, &m_item350, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
	SUBMENU(m_item352, 0, &m_item193, &m_item353, &m_item323, NULL, TEXT(TR_ID_MENU_ZERO_DRIFT), NULL, 0, 0, 0);
		SUBMENU(m_item353, 0, &m_item352, &m_item354, NULL, &m_item366, TEXT(TR_ID_MENU_POWER_CURRENT), NULL, 0, 0, 0);
			SUBMENU(m_item354, 0, &m_item353, &m_item355, NULL, &m_item360, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item355, PARAM_ID_PROT_I_IN_IDLE_FAULT_ENABLE, &m_item354, NULL, NULL, &m_item356, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item356, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE, &m_item354, NULL, &m_item355, &m_item357, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item357, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_TIME_MS, &m_item354, NULL, &m_item356, &m_item358, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item358, PARAM_ID_PROT_I_IN_IDLE_FAULT_LATCH_ENABLE, &m_item354, NULL, &m_item357, &m_item359, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item359, PARAM_ID_PROT_I_IN_IDLE_FAULT_ACTION, &m_item354, NULL, &m_item358, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item360, 0, &m_item353, &m_item361, &m_item354, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item361, PARAM_ID_PROT_I_IN_IDLE_WARN_ENABLE, &m_item360, NULL, NULL, &m_item362, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item362, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_VALUE, &m_item360, NULL, &m_item361, &m_item363, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item363, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_TIME_MS, &m_item360, NULL, &m_item362, &m_item364, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item364, PARAM_ID_PROT_I_IN_IDLE_WARN_LATCH_ENABLE, &m_item360, NULL, &m_item363, &m_item365, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item365, PARAM_ID_PROT_I_IN_IDLE_WARN_ACTION, &m_item360, NULL, &m_item364, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
		SUBMENU(m_item366, 0, &m_item352, &m_item367, &m_item353, &m_item379, TEXT(TR_ID_MENU_ROTOR_VOLTAGE), NULL, 0, 0, 0);
			SUBMENU(m_item367, 0, &m_item366, &m_item368, NULL, &m_item373, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item368, PARAM_ID_PROT_U_ROT_IDLE_FAULT_ENABLE, &m_item367, NULL, NULL, &m_item369, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item369, PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_VALUE, &m_item367, NULL, &m_item368, &m_item370, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item370, PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_TIME_MS, &m_item367, NULL, &m_item369, &m_item371, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item371, PARAM_ID_PROT_U_ROT_IDLE_FAULT_LATCH_ENABLE, &m_item367, NULL, &m_item370, &m_item372, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item372, PARAM_ID_PROT_U_ROT_IDLE_FAULT_ACTION, &m_item367, NULL, &m_item371, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item373, 0, &m_item366, &m_item374, &m_item367, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item374, PARAM_ID_PROT_U_ROT_IDLE_WARN_ENABLE, &m_item373, NULL, NULL, &m_item375, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item375, PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_VALUE, &m_item373, NULL, &m_item374, &m_item376, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item376, PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_TIME_MS, &m_item373, NULL, &m_item375, &m_item377, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item377, PARAM_ID_PROT_U_ROT_IDLE_WARN_LATCH_ENABLE, &m_item373, NULL, &m_item376, &m_item378, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item378, PARAM_ID_PROT_U_ROT_IDLE_WARN_ACTION, &m_item373, NULL, &m_item377, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
		SUBMENU(m_item379, 0, &m_item352, &m_item380, &m_item366, &m_item392, TEXT(TR_ID_MENU_ROTOR_CURRENT), NULL, 0, 0, 0);
			SUBMENU(m_item380, 0, &m_item379, &m_item381, NULL, &m_item386, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item381, PARAM_ID_PROT_I_ROT_IDLE_FAULT_ENABLE, &m_item380, NULL, NULL, &m_item382, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item382, PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_VALUE, &m_item380, NULL, &m_item381, &m_item383, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item383, PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_TIME_MS, &m_item380, NULL, &m_item382, &m_item384, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item384, PARAM_ID_PROT_I_ROT_IDLE_FAULT_LATCH_ENABLE, &m_item380, NULL, &m_item383, &m_item385, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item385, PARAM_ID_PROT_I_ROT_IDLE_FAULT_ACTION, &m_item380, NULL, &m_item384, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item386, 0, &m_item379, &m_item387, &m_item380, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item387, PARAM_ID_PROT_I_ROT_IDLE_WARN_ENABLE, &m_item386, NULL, NULL, &m_item388, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item388, PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_VALUE, &m_item386, NULL, &m_item387, &m_item389, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item389, PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_TIME_MS, &m_item386, NULL, &m_item388, &m_item390, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item390, PARAM_ID_PROT_I_ROT_IDLE_WARN_LATCH_ENABLE, &m_item386, NULL, &m_item389, &m_item391, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item391, PARAM_ID_PROT_I_ROT_IDLE_WARN_ACTION, &m_item386, NULL, &m_item390, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
		SUBMENU(m_item392, 0, &m_item352, &m_item393, &m_item379, NULL, TEXT(TR_ID_MENU_EXC_CURRENT), NULL, 0, 0, 0);
			SUBMENU(m_item393, 0, &m_item392, &m_item394, NULL, &m_item399, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item394, PARAM_ID_PROT_I_EXC_IDLE_FAULT_ENABLE, &m_item393, NULL, NULL, &m_item395, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item395, PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_VALUE, &m_item393, NULL, &m_item394, &m_item396, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item396, PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_TIME_MS, &m_item393, NULL, &m_item395, &m_item397, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item397, PARAM_ID_PROT_I_EXC_IDLE_FAULT_LATCH_ENABLE, &m_item393, NULL, &m_item396, &m_item398, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item398, PARAM_ID_PROT_I_EXC_IDLE_FAULT_ACTION, &m_item393, NULL, &m_item397, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
			SUBMENU(m_item399, 0, &m_item392, &m_item400, &m_item393, NULL, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0);
				MENU_ITEM(m_item400, PARAM_ID_PROT_I_EXC_IDLE_WARN_ENABLE, &m_item399, NULL, NULL, &m_item401, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item401, PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_VALUE, &m_item399, NULL, &m_item400, &m_item402, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item402, PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_TIME_MS, &m_item399, NULL, &m_item401, &m_item403, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item403, PARAM_ID_PROT_I_EXC_IDLE_WARN_LATCH_ENABLE, &m_item399, NULL, &m_item402, &m_item404, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
				MENU_ITEM(m_item404, PARAM_ID_PROT_I_EXC_IDLE_WARN_ACTION, &m_item399, NULL, &m_item403, NULL, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, &menu_val_prot_action, MENU_FLAG_DATA, 0);
SUBMENU(m_item405, 0, NULL, &m_item406, &m_item193, &m_item407, TEXT(TR_ID_MENU_CALCULATION), NULL, 0, 0, 0);
	MENU_ITEM(m_item406, PARAM_ID_CALC_PHASE_CURRENT, &m_item405, NULL, NULL, NULL, TEXT(TR_ID_MENU_CALC_PHASE_CURRENT), NULL, 0, &menu_val_phase, MENU_FLAG_DATA, 0);
SUBMENU(m_item407, 0, NULL, &m_item408, &m_item405, &m_item415, TEXT(TR_ID_MENU_CONFIGURATION), NULL, 0, 0, 0);
	SUBMENU(m_item408, 0, &m_item407, &m_item409, NULL, &m_item412, TEXT(TR_ID_MENU_MAIN_CONFIG), NULL, 0, 0, 0);
		MENU_ITEM(m_item409, PARAM_ID_GUI_LANGUAGE, &m_item408, NULL, NULL, &m_item410, TEXT(TR_ID_MENU_GUI_LANGUAGE), NULL, 0, &menu_val_gui_languages, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item410, PARAM_ID_GUI_BUZZER, &m_item408, NULL, &m_item409, &m_item411, TEXT(TR_ID_MENU_GUI_BUZZER), NULL, 0, &menu_val_gui_buzzer, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item411, 0, &m_item408, NULL, &m_item410, &m_item4110, TEXT(TR_ID_MENU_VERSION), TEXT(TR_ID_MENU_VERSION), 0, &menu_val_firmware_version, MENU_FLAG_VALUE, 0);
                MENU_ITEM(m_item4110, 0, &m_item408, NULL, &m_item411, NULL, TEXT(TR_ID_MENU_VERSION_DATE), TEXT(TR_ID_MENU_VERSION_DATE), 0, &menu_val_firmware_datetime, MENU_FLAG_VALUE, 0);
        SUBMENU(m_item412, 0, &m_item407, &m_item413, &m_item408, NULL, TEXT(TR_ID_MENU_COMMUNICATION), NULL, 0, 0, 0);
		MENU_ITEM(m_item413, PARAM_ID_MODBUS_BAUD, &m_item412, NULL, NULL, &m_item414, TEXT(TR_ID_MENU_MODBUS_BAUD), NULL, 0, 0, MENU_FLAG_DATA, 0);
		MENU_ITEM(m_item414, PARAM_ID_MODBUS_ADDRESS, &m_item412, NULL, &m_item413, NULL, TEXT(TR_ID_MENU_MODBUS_ADDRESS), NULL, 0, 0, MENU_FLAG_DATA, 0);
SUBMENU(m_item415, 0, NULL, &m_item416, &m_item407, &m_item426, TEXT(TR_ID_FAN_CONTROL), NULL, 0, 0, 0);
	MENU_ITEM(m_item416, PARAM_ID_FAN_CONTROL_ENABLE, &m_item415, NULL, NULL, &m_item417, TEXT(TR_ID_FAN_CONTROL_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item417, PARAM_ID_FAN_TEMP_MIN, &m_item415, NULL, &m_item416, &m_item418, TEXT(TR_ID_FAN_TEMP_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item418, PARAM_ID_FAN_TEMP_MAX, &m_item415, NULL, &m_item417, &m_item419, TEXT(TR_ID_FAN_TEMP_MAX), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item419, PARAM_ID_FAN_RPM_MIN, &m_item415, NULL, &m_item418, &m_item420, TEXT(TR_ID_FAN_RPM_MIN), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item420, PARAM_ID_FAN_ECO_MODE_ENABLE, &m_item415, NULL, &m_item419, &m_item421, TEXT(TR_ID_FAN_ECO_MODE_ENABLE), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item421, PARAM_ID_FAN_ECO_COOLING_TIME, &m_item415, NULL, &m_item420, &m_item422, TEXT(TR_ID_FAN_ECO_COOLING_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item422, PARAM_ID_FAN_I_NOM, &m_item415, NULL, &m_item421, &m_item423, TEXT(TR_ID_FAN_I_NOM), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item423, PARAM_ID_FAN_I_ZERO_NOISE, &m_item415, NULL, &m_item422, &m_item424, TEXT(TR_ID_FAN_I_ZERO_NOISE), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item424, PARAM_ID_FAN_PROT_TIME, &m_item415, NULL, &m_item423, &m_item425, TEXT(TR_ID_FAN_PROT_TIME), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item425, PARAM_ID_FAN_PROT_OVF_LEVEL, &m_item415, NULL, &m_item424, NULL, TEXT(TR_ID_FAN_PROT_OVF_LEVEL), NULL, 0, 0, MENU_FLAG_DATA, 0);
SUBMENU(m_item426, 0, NULL, &m_item427, &m_item415, &m_item458, TEXT(TR_ID_MENU_DIGITAL_IO), NULL, 0, 0, 0);
	SUBMENU(m_item427, 0, &m_item426, &m_item428, NULL, &m_item443, TEXT(TR_ID_MENU_DIGITAL_INPUTS), NULL, 0, 0, 0);
		SUBMENU(m_item428, 0, &m_item427, &m_item429, NULL, &m_item431, TEXT(TR_ID_MENU_DIO_INPUT1), NULL, 0, 0, 0);
			MENU_ITEM(m_item429, PARAM_ID_DIGITAL_IN_1_TYPE, &m_item428, NULL, NULL, &m_item430, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, &menu_val_dio_in_type, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item430, PARAM_ID_DIGITAL_IN_1_INVERSION, &m_item428, NULL, &m_item429, NULL, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
		SUBMENU(m_item431, 0, &m_item427, &m_item432, &m_item428, &m_item434, TEXT(TR_ID_MENU_DIO_INPUT2), NULL, 0, 0, 0);
			MENU_ITEM(m_item432, PARAM_ID_DIGITAL_IN_2_TYPE, &m_item431, NULL, NULL, &m_item433, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, &menu_val_dio_in_type, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item433, PARAM_ID_DIGITAL_IN_2_INVERSION, &m_item431, NULL, &m_item432, NULL, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
		SUBMENU(m_item434, 0, &m_item427, &m_item435, &m_item431, &m_item437, TEXT(TR_ID_MENU_DIO_INPUT3), NULL, 0, 0, 0);
			MENU_ITEM(m_item435, PARAM_ID_DIGITAL_IN_3_TYPE, &m_item434, NULL, NULL, &m_item436, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, &menu_val_dio_in_type, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item436, PARAM_ID_DIGITAL_IN_3_INVERSION, &m_item434, NULL, &m_item435, NULL, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
		SUBMENU(m_item437, 0, &m_item427, &m_item438, &m_item434, &m_item440, TEXT(TR_ID_MENU_DIO_INPUT4), NULL, 0, 0, 0);
			MENU_ITEM(m_item438, PARAM_ID_DIGITAL_IN_4_TYPE, &m_item437, NULL, NULL, &m_item439, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, &menu_val_dio_in_type, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item439, PARAM_ID_DIGITAL_IN_4_INVERSION, &m_item437, NULL, &m_item438, NULL, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
		SUBMENU(m_item440, 0, &m_item427, &m_item441, &m_item437, NULL, TEXT(TR_ID_MENU_DIO_INPUT5), NULL, 0, 0, 0);
			MENU_ITEM(m_item441, PARAM_ID_DIGITAL_IN_5_TYPE, &m_item440, NULL, NULL, &m_item442, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, &menu_val_dio_in_type, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item442, PARAM_ID_DIGITAL_IN_5_INVERSION, &m_item440, NULL, &m_item441, NULL, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
	SUBMENU(m_item443, 0, &m_item426, &m_item444, &m_item427, &m_item456, TEXT(TR_ID_MENU_DIGITAL_OUTPUTS), NULL, 0, 0, 0);
		SUBMENU(m_item444, 0, &m_item443, &m_item445, NULL, &m_item447, TEXT(TR_ID_MENU_DIO_OUTPUT1), NULL, 0, 0, 0);
			MENU_ITEM(m_item445, PARAM_ID_DIGITAL_OUT_1_TYPE, &m_item444, NULL, NULL, &m_item446, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, &menu_val_dio_out_type, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item446, PARAM_ID_DIGITAL_OUT_1_INVERSION, &m_item444, NULL, &m_item445, NULL, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
		SUBMENU(m_item447, 0, &m_item443, &m_item448, &m_item444, &m_item450, TEXT(TR_ID_MENU_DIO_OUTPUT2), NULL, 0, 0, 0);
			MENU_ITEM(m_item448, PARAM_ID_DIGITAL_OUT_2_TYPE, &m_item447, NULL, NULL, &m_item449, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, &menu_val_dio_out_type, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item449, PARAM_ID_DIGITAL_OUT_2_INVERSION, &m_item447, NULL, &m_item448, NULL, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
		SUBMENU(m_item450, 0, &m_item443, &m_item451, &m_item447, &m_item453, TEXT(TR_ID_MENU_DIO_OUTPUT3), NULL, 0, 0, 0);
			MENU_ITEM(m_item451, PARAM_ID_DIGITAL_OUT_3_TYPE, &m_item450, NULL, NULL, &m_item452, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, &menu_val_dio_out_type, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item452, PARAM_ID_DIGITAL_OUT_3_INVERSION, &m_item450, NULL, &m_item451, NULL, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
		SUBMENU(m_item453, 0, &m_item443, &m_item454, &m_item450, NULL, TEXT(TR_ID_MENU_DIO_OUTPUT4), NULL, 0, 0, 0);
			MENU_ITEM(m_item454, PARAM_ID_DIGITAL_OUT_4_TYPE, &m_item453, NULL, NULL, &m_item455, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, &menu_val_dio_out_type, MENU_FLAG_DATA, 0);
			MENU_ITEM(m_item455, PARAM_ID_DIGITAL_OUT_4_INVERSION, &m_item453, NULL, &m_item454, NULL, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, &menu_val_bool, MENU_FLAG_DATA, 0);
	SUBMENU(m_item456, 0, &m_item426, &m_item457, &m_item443, NULL, TEXT(TR_ID_MENU_DIGITAL_IO_SETTINGS), NULL, 0, 0, 0);
		MENU_ITEM(m_item457, PARAM_ID_DIGITAL_IO_DEADTIME_MS, &m_item456, NULL, NULL, NULL, TEXT(TR_ID_MENU_DIGITAL_IO_DEADTIME_MS), NULL, 0, 0, MENU_FLAG_DATA, 0);
SUBMENU(m_item458, 0, NULL, &m_item459, &m_item426, &m_item470, TEXT(TR_ID_MENU_ADC_VALUE_MULTS), NULL, 0, 0, 0);
	MENU_ITEM(m_item459, PARAM_ID_ADC_VALUE_MULTIPLIER_Ua, &m_item458, NULL, NULL, &m_item460, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ua), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item460, PARAM_ID_ADC_VALUE_MULTIPLIER_Ub, &m_item458, NULL, &m_item459, &m_item461, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ub), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item461, PARAM_ID_ADC_VALUE_MULTIPLIER_Uc, &m_item458, NULL, &m_item460, &m_item462, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Uc), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item462, PARAM_ID_ADC_VALUE_MULTIPLIER_Urot, &m_item458, NULL, &m_item461, &m_item463, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Urot), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item463, PARAM_ID_ADC_VALUE_MULTIPLIER_Ia, &m_item458, NULL, &m_item462, &m_item464, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ia), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item464, PARAM_ID_ADC_VALUE_MULTIPLIER_Ib, &m_item458, NULL, &m_item463, &m_item465, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ib), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item465, PARAM_ID_ADC_VALUE_MULTIPLIER_Ic, &m_item458, NULL, &m_item464, &m_item466, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ic), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item466, PARAM_ID_ADC_VALUE_MULTIPLIER_Irot, &m_item458, NULL, &m_item465, &m_item467, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Irot), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item467, PARAM_ID_ADC_VALUE_MULTIPLIER_Iexc, &m_item458, NULL, &m_item466, &m_item468, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Iexc), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item468, PARAM_ID_ADC_VALUE_MULTIPLIER_Iref, &m_item458, NULL, &m_item467, &m_item469, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Iref), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item469, PARAM_ID_ADC_VALUE_MULTIPLIER_Ifan, &m_item458, NULL, &m_item468, NULL, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ifan), NULL, 0, 0, MENU_FLAG_DATA, 0);
SUBMENU(m_item470, 0, NULL, &m_item471, &m_item458, &m_item482, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA), NULL, 0, 0, 0);
	MENU_ITEM(m_item471, PARAM_ID_ADC_CALIBRATION_DATA_Ua, &m_item470, NULL, NULL, &m_item472, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ua), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item472, PARAM_ID_ADC_CALIBRATION_DATA_Ub, &m_item470, NULL, &m_item471, &m_item473, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ub), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item473, PARAM_ID_ADC_CALIBRATION_DATA_Uc, &m_item470, NULL, &m_item472, &m_item474, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Uc), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item474, PARAM_ID_ADC_CALIBRATION_DATA_Urot, &m_item470, NULL, &m_item473, &m_item475, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Urot), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item475, PARAM_ID_ADC_CALIBRATION_DATA_Ia, &m_item470, NULL, &m_item474, &m_item476, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ia), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item476, PARAM_ID_ADC_CALIBRATION_DATA_Ib, &m_item470, NULL, &m_item475, &m_item477, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ib), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item477, PARAM_ID_ADC_CALIBRATION_DATA_Ic, &m_item470, NULL, &m_item476, &m_item478, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ic), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item478, PARAM_ID_ADC_CALIBRATION_DATA_Irot, &m_item470, NULL, &m_item477, &m_item479, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Irot), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item479, PARAM_ID_ADC_CALIBRATION_DATA_Iexc, &m_item470, NULL, &m_item478, &m_item480, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Iexc), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item480, PARAM_ID_ADC_CALIBRATION_DATA_Iref, &m_item470, NULL, &m_item479, &m_item481, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Iref), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item481, PARAM_ID_ADC_CALIBRATION_DATA_Ifan, &m_item470, NULL, &m_item480, NULL, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ifan), NULL, 0, 0, MENU_FLAG_DATA, 0);
SUBMENU(m_item482, 0, NULL, &m_item483, &m_item470, NULL, TEXT(TR_ID_MENU_VALUE_MULTS), NULL, 0, 0, 0);
	MENU_ITEM(m_item483, PARAM_ID_VALUE_MULTIPLIER_Ua, &m_item482, NULL, NULL, &m_item484, TEXT(TR_ID_MENU_VALUE_MULT_Ua), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item484, PARAM_ID_VALUE_MULTIPLIER_Ub, &m_item482, NULL, &m_item483, &m_item485, TEXT(TR_ID_MENU_VALUE_MULT_Ub), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item485, PARAM_ID_VALUE_MULTIPLIER_Uc, &m_item482, NULL, &m_item484, &m_item486, TEXT(TR_ID_MENU_VALUE_MULT_Uc), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item486, PARAM_ID_VALUE_MULTIPLIER_Urot, &m_item482, NULL, &m_item485, &m_item487, TEXT(TR_ID_MENU_VALUE_MULT_Urot), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item487, PARAM_ID_VALUE_MULTIPLIER_Ia, &m_item482, NULL, &m_item486, &m_item488, TEXT(TR_ID_MENU_VALUE_MULT_Ia), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item488, PARAM_ID_VALUE_MULTIPLIER_Ib, &m_item482, NULL, &m_item487, &m_item489, TEXT(TR_ID_MENU_VALUE_MULT_Ib), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item489, PARAM_ID_VALUE_MULTIPLIER_Ic, &m_item482, NULL, &m_item488, &m_item490, TEXT(TR_ID_MENU_VALUE_MULT_Ic), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item490, PARAM_ID_VALUE_MULTIPLIER_Irot, &m_item482, NULL, &m_item489, &m_item491, TEXT(TR_ID_MENU_VALUE_MULT_Irot), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item491, PARAM_ID_VALUE_MULTIPLIER_Iexc, &m_item482, NULL, &m_item490, &m_item492, TEXT(TR_ID_MENU_VALUE_MULT_Iexc), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item492, PARAM_ID_VALUE_MULTIPLIER_Iref, &m_item482, NULL, &m_item491, &m_item493, TEXT(TR_ID_MENU_VALUE_MULT_Iref), NULL, 0, 0, MENU_FLAG_DATA, 0);
	MENU_ITEM(m_item493, PARAM_ID_VALUE_MULTIPLIER_Ifan, &m_item482, NULL, &m_item492, NULL, TEXT(TR_ID_MENU_VALUE_MULT_Ifan), NULL, 0, 0, MENU_FLAG_DATA, 0);

/**
 * Дескрипторы элементов меню.
 * 
MENU_DESCRS(menu_descrs) {
    // Команды
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_COMMANDS), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, CMD_ID_START_STOP, TEXT(TR_ID_MENU_CMD_START_STOP), NULL, 0, MENU_FLAG_CMD | MENU_FLAG_ADMIN, 0, 0),
        MENU_DESCR(1, CMD_ID_ENABLE_DRIVE, TEXT(TR_ID_MENU_CMD_ENABLE_DRIVE), NULL, 0, MENU_FLAG_CMD, 0, 0),
        MENU_DESCR(1, CMD_ID_SETTINGS_SAVE, TEXT(TR_ID_MENU_CMD_SETTINGS_SAVE), NULL, 0, MENU_FLAG_CMD, 0, 0),
        MENU_DESCR(1, CMD_ID_STATE_SAVE, TEXT(TR_ID_MENU_CMD_STATE_SAVE), NULL, 0, MENU_FLAG_CMD, 0, 0),
        MENU_DESCR(1, CMD_ID_CLEAR_EVENTS, TEXT(TR_ID_MENU_CMD_CLEAR_EVENTS), NULL, 0, MENU_FLAG_CMD, 0, 0),   
        MENU_DESCR(1, CMD_ID_SETTINGS_DEFAULT, TEXT(TR_ID_MENU_CMD_SETTINGS_DEFAULT), NULL, 0, MENU_FLAG_CMD, 0, 0),
        MENU_DESCR(1, CMD_ID_TEST_LEDS, TEXT(TR_ID_MENU_CMD_TEST_LEDS), NULL, 0, MENU_FLAG_CMD, 0, 0),
    // Статус привода
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_STATUS), NULL, 0, 0, 0, 0), 
        // История событий
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_EVENTS), NULL, 0, MENU_FLAG_EVENTS, 0, 0),
        // Измерения
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_MEASUREMENTS), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, PARAM_ID_POWER_U_A, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_U_A), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_POWER_U_B, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_U_B), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_POWER_U_C, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_U_C), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_POWER_U_ROT, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_U_ROT), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_POWER_I_A, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_A), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_POWER_I_B, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_B), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_POWER_I_C, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_C), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_POWER_I_ROT, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_ROT), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_POWER_I_EXC, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_EXC), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_POWER_I_FAN, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_FAN), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_POWER_I_REF, TEXT(TR_ID_MENU_MESS_PARAM_ID_POWER_I_REF), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_HEATSINK_TEMP, TEXT(TR_ID_MENU_MESS_PARAM_ID_HEATSINK_TEMP), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_HEATSINK_FAN_RPM, TEXT(TR_ID_MENU_MESS_PARAM_ID_HEATSINK_FAN_RPM), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE, TEXT(TR_ID_MENU_MESS_PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_TRIAC_EXC_OPEN_ANGLE, TEXT(TR_ID_MENU_MESS_PARAM_ID_TRIAC_EXC_OPEN_ANGLE), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            // Мотор
            MENU_DESCR(2, PARAM_ID_MOTOR_EFF, TEXT(TR_ID_MENU_MESS_PARAM_ID_MOTOR_EFF), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_MOTOR_R_ROT, TEXT(TR_ID_MENU_MESS_PARAM_ID_MOTOR_R_ROT), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_MOTOR_RPM, TEXT(TR_ID_MENU_MESS_PARAM_ID_MOTOR_RPM), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            MENU_DESCR(2, PARAM_ID_MOTOR_TORQUE, TEXT(TR_ID_MENU_MESS_PARAM_ID_MOTOR_TORQUE), NULL, 0, MENU_FLAG_VALUE, 0, 0),
        // Цифровые входа
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_DIGITAL_INPUTS_STATE), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, PARAM_ID_DIGITAL_IN_1_STATE, TEXT(TR_ID_MENU_DIGITAL_IN_1_STATE), NULL, 0, MENU_FLAG_VALUE, 0, &menu_val_gui_digital_states),
            MENU_DESCR(2, PARAM_ID_DIGITAL_IN_2_STATE, TEXT(TR_ID_MENU_DIGITAL_IN_2_STATE), NULL, 0, MENU_FLAG_VALUE, 0, &menu_val_gui_digital_states),
            MENU_DESCR(2, PARAM_ID_DIGITAL_IN_3_STATE, TEXT(TR_ID_MENU_DIGITAL_IN_3_STATE), NULL, 0, MENU_FLAG_VALUE, 0, &menu_val_gui_digital_states),
            MENU_DESCR(2, PARAM_ID_DIGITAL_IN_4_STATE, TEXT(TR_ID_MENU_DIGITAL_IN_4_STATE), NULL, 0, MENU_FLAG_VALUE, 0, &menu_val_gui_digital_states),
            MENU_DESCR(2, PARAM_ID_DIGITAL_IN_5_STATE, TEXT(TR_ID_MENU_DIGITAL_IN_5_STATE), NULL, 0, MENU_FLAG_VALUE, 0, &menu_val_gui_digital_states),
        // Цифровые выхода
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_DIGITAL_OUTPUTS_STATE), NULL, 0, 0, 0, 0), 
            MENU_DESCR(2, PARAM_ID_DIGITAL_OUT_1_STATE, TEXT(TR_ID_MENU_DIGITAL_OUT_1_STATE), NULL, 0, MENU_FLAG_VALUE, 0, &menu_val_gui_digital_states),
            MENU_DESCR(2, PARAM_ID_DIGITAL_OUT_2_STATE, TEXT(TR_ID_MENU_DIGITAL_OUT_2_STATE), NULL, 0, MENU_FLAG_VALUE, 0, &menu_val_gui_digital_states),
            MENU_DESCR(2, PARAM_ID_DIGITAL_OUT_3_STATE, TEXT(TR_ID_MENU_DIGITAL_OUT_3_STATE), NULL, 0, MENU_FLAG_VALUE, 0, &menu_val_gui_digital_states),
            MENU_DESCR(2, PARAM_ID_DIGITAL_OUT_4_STATE, TEXT(TR_ID_MENU_DIGITAL_OUT_4_STATE), NULL, 0, MENU_FLAG_VALUE, 0, &menu_val_gui_digital_states),
        // Наработка привода
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_RUNTIMES), NULL, 0, 0, 0, 0),
            // Время работы после последнего включения
            MENU_DESCR(2, PARAM_ID_LAST_RUNTIME, TEXT(TR_ID_MENU_LAST_RUNTIME), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            // Время работы
            MENU_DESCR(2, PARAM_ID_RUNTIME, TEXT(TR_ID_MENU_RUNTIME), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            // Время работы вентиляторов
            MENU_DESCR(2, PARAM_ID_FAN_RUNTIME, TEXT(TR_ID_MENU_FAN_RUNTIME), NULL, 0, MENU_FLAG_VALUE, 0, 0),
            // Время во вкл.состоянии (включая готовность)
            MENU_DESCR(2, PARAM_ID_LIFETIME, TEXT(TR_ID_MENU_LIFETIME), NULL, 0, MENU_FLAG_VALUE, 0, 0),
    // Настройки интерфейса
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_GUI), NULL, 0, MENU_FLAG_ROOT, 0, 0),
        // Пароль администратора
        MENU_DESCR(1, PARAM_ID_GUI_PASSWORD_ADMIN, TEXT(TR_ID_MENU_GUI_PASSWORD_ADMIN), NULL, 0, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0, 0),
        // Пароль ROOT
        MENU_DESCR(1, PARAM_ID_GUI_PASSWORD_ROOT, TEXT(TR_ID_MENU_GUI_PASSWORD_ROOT), NULL, 0, MENU_FLAG_DATA | MENU_FLAG_ROOT, 0, 0),
        // Плитки
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_GUI_TILES), NULL, 0, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0, 0),
            MENU_DESCR(2, PARAM_ID_GUI_TILE_1, TEXT(TR_ID_MENU_GUI_TILE_1), NULL, 0, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0, &menu_val_gui_tiles),
            MENU_DESCR(2, PARAM_ID_GUI_TILE_2, TEXT(TR_ID_MENU_GUI_TILE_2), NULL, 0, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0, &menu_val_gui_tiles),
            MENU_DESCR(2, PARAM_ID_GUI_TILE_3, TEXT(TR_ID_MENU_GUI_TILE_3), NULL, 0, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0, &menu_val_gui_tiles),
            MENU_DESCR(2, PARAM_ID_GUI_TILE_4, TEXT(TR_ID_MENU_GUI_TILE_4), NULL, 0, MENU_FLAG_DATA | MENU_FLAG_ADMIN, 0, &menu_val_gui_tiles),
        // Граничные значения цветовых предупреждений на плитках
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUES), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Напряжение фазы
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_U_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_U_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_U_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_U_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Напряжение якоря
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_U_ROT_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_U_ROT_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_U_ROT_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_U_ROT_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Ток фазы
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Ток якоря
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_ROT_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_ROT_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_ROT_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_ROT_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Ток возбуждения
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_EXC_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_EXC_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_EXC_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_EXC_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Температура радиатора
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_HEATSINK_TEMP_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_HEATSINK_TEMP_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_HEATSINK_TEMP_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_HEATSINK_TEMP_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Ток вентилятора.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_FAN_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_FAN_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_FAN_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_FAN_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Ток 4-20.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_REF_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_REF_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_REF_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_POWER_I_REF_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Обороты вентилятора радиатора.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_HEATSINK_FAN_RPM_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_HEATSINK_FAN_RPM_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_HEATSINK_FAN_RPM_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_HEATSINK_FAN_RPM_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Угол открытия тиристоров.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Угол открытия возбуждения.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIAC_EXC_OPEN_ANGLE_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIAC_EXC_OPEN_ANGLE_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIAC_EXC_OPEN_ANGLE_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIAC_EXC_OPEN_ANGLE_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            /////////// Мотор //////////// 
            // Вычисленный коэффициент полезного действия.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_EFF_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_EFF_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_EFF_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_EFF_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Вычисленное сопротивление якоря.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_R_ROT_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_R_ROT_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_R_ROT_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_R_ROT_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Вычисленные обороты. 
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_RPM_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_RPM_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_RPM_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_RPM_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            // Вычисленный момент.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_TORQUE_ALARM_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_ALARM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_TORQUE_WARN_MIN, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_WARN_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_TORQUE_WARN_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_WARN_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_MOTOR_TORQUE_ALARM_MAX, TEXT(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_ALARM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
        // Отображение текста ошибок и предупреждений на плитке
        MENU_DESCR(1, PARAM_ID_MENU_GUI_TILE_WARNINGS, TEXT(TR_ID_MENU_GUI_TILE_WARNINGS), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_gui_tiles_for_warnings),
    // Сеть.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_SUPPLY_IN), TEXT(TR_ID_HELP_POWER_IN), 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_U_NOM, TEXT(TR_ID_MENU_U_NOM), TEXT(TR_ID_HELP_U_NOM), 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_I_NOM, TEXT(TR_ID_MENU_I_NOM), TEXT(TR_ID_HELP_I_NOM), 0, MENU_FLAG_DATA, 0, 0),
    // Двигатель.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_MOTOR), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_MOTOR_P_NOM, TEXT(TR_ID_MENU_MOTOR_P_NOM), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_MOTOR_RPM_NOM, TEXT(TR_ID_MENU_MOTOR_RPM_NOM), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_MOTOR_RPM_MAX, TEXT(TR_ID_MENU_MOTOR_RPM_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_MOTOR_U_ROT_NOM, TEXT(TR_ID_MENU_MOTOR_U_ROT_NOM), TEXT(TR_ID_HELP_U_ROT_NOM), 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_MOTOR_I_ROT_NOM, TEXT(TR_ID_MENU_MOTOR_I_ROT_NOM), TEXT(TR_ID_HELP_I_ROT_NOM), 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_MOTOR_U_EXC_NOM, TEXT(TR_ID_MENU_MOTOR_U_EXC_NOM), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_MOTOR_I_EXC_NOM, TEXT(TR_ID_MENU_MOTOR_I_EXC_NOM), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_MOTOR_EFF_NOM, TEXT(TR_ID_MENU_MOTOR_EFF_NOM), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_MOTOR_R_ROT_NOM, TEXT(TR_ID_MENU_MOTOR_R_ROT_NOM), NULL, 0, MENU_FLAG_DATA, 0, 0),
    // Возбуждение.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_EXCITATION), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_I_EXC, TEXT(TR_ID_MENU_I_EXC), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_EXC_PHASE, TEXT(TR_ID_MENU_EXC_PHASE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_phase),
        MENU_DESCR(1, PARAM_ID_EXC_MODE, TEXT(TR_ID_MENU_EXC_MODE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_exc_mode),
    // Разгон.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_RAMP), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_RAMP_START_TIME, TEXT(TR_ID_MENU_RAMP_START_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_RAMP_STOP_TIME, TEXT(TR_ID_MENU_RAMP_STOP_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_RAMP_FAST_STOP_TIME, TEXT(TR_ID_MENU_RAMP_FAST_STOP_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_RAMP_REFERENCE_TIME, TEXT(TR_ID_MENU_RAMP_REFERENCE_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_RAMP_STOP_MODE, TEXT(TR_ID_MENU_RAMP_STOP_MODE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_stop_mode),
    // Синхронизация с фазами.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_PHASE_SYNC), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_PHASE_SYNC_ACCURACY, TEXT(TR_ID_MENU_PHASE_SYNC_ACCURACY), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_PHASE_SYNC_PID), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, PARAM_ID_PHASE_SYNC_PLL_PID_K_P, TEXT(TR_ID_MENU_PID_K_P), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, PARAM_ID_PHASE_SYNC_PLL_PID_K_I, TEXT(TR_ID_MENU_PID_K_I), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, PARAM_ID_PHASE_SYNC_PLL_PID_K_D, TEXT(TR_ID_MENU_PID_K_D), NULL, 0, MENU_FLAG_DATA, 0, 0),
    // Регулятор.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_REGULATOR), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_REGULATOR_MODE, TEXT(TR_ID_MENU_REGULATOR_MODE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_regulator_mode),
    // ПИД.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_PID), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_PID_SPD), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, PARAM_ID_SPD_PID_K_P, TEXT(TR_ID_MENU_PID_K_P), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, PARAM_ID_SPD_PID_K_I, TEXT(TR_ID_MENU_PID_K_I), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, PARAM_ID_SPD_PID_K_D, TEXT(TR_ID_MENU_PID_K_D), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_PID_ROT), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, PARAM_ID_ROT_PID_K_P, TEXT(TR_ID_MENU_PID_K_P), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, PARAM_ID_ROT_PID_K_I, TEXT(TR_ID_MENU_PID_K_I), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, PARAM_ID_ROT_PID_K_D, TEXT(TR_ID_MENU_PID_K_D), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_PID_EXC), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, PARAM_ID_EXC_PID_K_P, TEXT(TR_ID_MENU_PID_K_P), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, PARAM_ID_EXC_PID_K_I, TEXT(TR_ID_MENU_PID_K_I), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, PARAM_ID_EXC_PID_K_D, TEXT(TR_ID_MENU_PID_K_D), NULL, 0, MENU_FLAG_DATA, 0, 0),
    // Запуск и останов.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_START_STOP), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_ROT_STOP_TIME, TEXT(TR_ID_MENU_ROT_STOP_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_EXC_STOP_TIME, TEXT(TR_ID_MENU_EXC_STOP_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_EXC_START_TIME, TEXT(TR_ID_MENU_EXC_START_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_PHASES_CHECK_TIME, TEXT(TR_ID_MENU_PHASES_CHECK_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ZERO_SENSOR_TIME, TEXT(TR_ID_MENU_ZERO_SENSOR_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
    // Тиристоры.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_TRIACS), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_TRIACS_OPEN_TIME), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, PARAM_ID_TRIACS_PAIRS_OPEN_TIME, TEXT(TR_ID_MENU_TRIACS_PAIRS_OPEN_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, PARAM_ID_TRIAC_EXC_OPEN_TIME, TEXT(TR_ID_MENU_TRIAC_EXC_OPEN_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_TRIACS_OPEN_ANGLES), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_ROTOR), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIACS_PAIRS_ANGLE_MIN, TEXT(TR_ID_MENU_TRIACS_OPEN_ANGLE_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIACS_PAIRS_ANGLE_MAX, TEXT(TR_ID_MENU_TRIACS_OPEN_ANGLE_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_EXCITATION), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIAC_EXC_ANGLE_MIN, TEXT(TR_ID_MENU_TRIACS_OPEN_ANGLE_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_TRIAC_EXC_ANGLE_MAX, TEXT(TR_ID_MENU_TRIACS_OPEN_ANGLE_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
    // Защита.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_PROTECTION), NULL, 0, 0, 0, 0),
        // Тепловая защита.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_THERMAL_OVERLOAD_PROT), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, PARAM_ID_THERMAL_OVERLOAD_PROT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
            MENU_DESCR(2, PARAM_ID_THERMAL_OVERLOAD_PROT_TIME_6I, TEXT(TR_ID_MENU_THERMAL_OVERLOAD_PROT_TIME_6I), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, PARAM_ID_THERMAL_OVERLOAD_PROT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
        // Грибок.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_EMERGENCY_STOP), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, PARAM_ID_EMERGENCY_STOP_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
        // Обрыв якоря.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_ROT_BREAK_PROT), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_ROT_BREAK_ENABLED, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_ROT_BREAK_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_ROT_BREAK_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_ROT_BREAK_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_ROT_BREAK_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
        // Защита фаз.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_PHASES_PROT), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PHASE_STATE_PROT), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_PROT_PHASES_STATE_ENABLED, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                MENU_DESCR(3, PARAM_ID_PROT_PHASES_STATE_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_PROT_PHASES_STATE_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PHASES_ANGLES_PROT), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_ANGLES_FAULT_ENABLED, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_ANGLES_FAULT_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_ANGLES_FAULT_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_ANGLES_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_ANGLES_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_ANGLES_WARN_ENABLED, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_ANGLES_WARN_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_ANGLES_WARN_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_ANGLES_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_ANGLES_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PHASES_SYNC_PROT), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_SYNC_FAULT_ENABLED, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_SYNC_FAULT_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_SYNC_FAULT_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_SYNC_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_SYNC_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_SYNC_WARN_ENABLED, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_SYNC_WARN_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_SYNC_WARN_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_SYNC_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_PHASES_SYNC_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_HEATSINK_TEMP_PROT), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_PROT_HEATSINK_TEMP_FAULT_ENABLED, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                MENU_DESCR(3, PARAM_ID_PROT_HEATSINK_TEMP_FAULT_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_PROT_HEATSINK_TEMP_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_PROT_HEATSINK_TEMP_WARN_ENABLED, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                MENU_DESCR(3, PARAM_ID_PROT_HEATSINK_TEMP_WARN_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                MENU_DESCR(3, PARAM_ID_PROT_HEATSINK_TEMP_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
        // Напряжение сети.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_POWER_VOLTAGE), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_CUTOFF), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_PROT_U_IN_CUTOFF_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PROT_OVERFLOW), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_OVF_FAULT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_OVF_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_OVF_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_OVF_WARN_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_OVF_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_OVF_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PROT_UNDERFLOW), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_UDF_FAULT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_UDF_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_UDF_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_UDF_WARN_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_UDF_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_IN_UDF_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
        // Ток сети.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_POWER_CURRENT), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_CUTOFF), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_PROT_I_IN_CUTOFF_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PROT_OVERFLOW), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_OVF_FAULT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_OVF_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_OVF_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_OVF_WARN_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_OVF_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_OVF_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
        // Напряжение якоря.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_ROTOR_VOLTAGE), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_CUTOFF), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_PROT_U_ROT_CUTOFF_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PROT_OVERFLOW), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_OVF_FAULT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_OVF_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_OVF_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_OVF_WARN_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_OVF_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_OVF_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
        // Ток якоря.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_ROTOR_CURRENT), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_CUTOFF), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_PROT_I_ROT_CUTOFF_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PROT_OVERFLOW), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_OVF_FAULT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_OVF_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_OVF_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_OVF_WARN_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_OVF_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_OVF_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
        // Ток возбуждения.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_EXC_CURRENT), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_CUTOFF), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_PROT_I_EXC_CUTOFF_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PROT_OVERFLOW), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_OVF_FAULT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_OVF_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_OVF_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_OVF_WARN_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_OVF_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_OVF_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_PROT_UNDERFLOW), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_UDF_FAULT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_UDF_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_UDF_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_UDF_WARN_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_UDF_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_UDF_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
        // Дрейф нуля.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_ZERO_DRIFT), NULL, 0, 0, 0, 0),
            // Токи сети.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_POWER_CURRENT), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_IDLE_FAULT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_IDLE_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_IDLE_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_IDLE_WARN_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_IDLE_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_IN_IDLE_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
            // Напряжение якоря.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_ROTOR_VOLTAGE), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_IDLE_FAULT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_IDLE_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_IDLE_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_IDLE_WARN_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_IDLE_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_U_ROT_IDLE_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
            // Ток якоря.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_ROTOR_CURRENT), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_IDLE_FAULT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_IDLE_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_IDLE_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_IDLE_WARN_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_IDLE_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_ROT_IDLE_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
            // Ток возбуждения.
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_EXC_CURRENT), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_ERROR_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_IDLE_FAULT_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_IDLE_FAULT_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_IDLE_FAULT_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
                MENU_DESCR(3, 0, TEXT(TR_ID_MENU_PROT_WARNING_LEVEL), NULL, 0, 0, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_IDLE_WARN_ENABLE, TEXT(TR_ID_MENU_PROT_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_VALUE, TEXT(TR_ID_MENU_PROT_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_TIME_MS, TEXT(TR_ID_MENU_PROT_LEVEL_TIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_IDLE_WARN_LATCH_ENABLE, TEXT(TR_ID_MENU_PROT_LATCH_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
                    MENU_DESCR(4, PARAM_ID_PROT_I_EXC_IDLE_WARN_ACTION, TEXT(TR_ID_MENU_PROT_ACTION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_prot_action),
    // Вычисления.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_CALCULATION), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_CALC_PHASE_CURRENT, TEXT(TR_ID_MENU_CALC_PHASE_CURRENT), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_phase),
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_CONFIGURATION), NULL, 0, 0, 0, 0),
        // Общие
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_MAIN_CONFIG), NULL, 0, 0, 0, 0), 
            // Время
            
            // Дата
            
            // Язык
            MENU_DESCR(2, PARAM_ID_GUI_LANGUAGE, TEXT(TR_ID_MENU_GUI_LANGUAGE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_gui_languages),    
            // Звуковые оповещения
            MENU_DESCR(2, PARAM_ID_GUI_BUZZER, TEXT(TR_ID_MENU_GUI_BUZZER), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_gui_buzzer),
            // Модель
            
            // Версия прошивки
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_VERSION), NULL, 0, MENU_FLAG_VALUE, 0, &menu_val_firmware_version),
            
            // Дата/время прошивки
            
        // Коммуникация.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_COMMUNICATION), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, PARAM_ID_MODBUS_BAUD, TEXT(TR_ID_MENU_MODBUS_BAUD), NULL, 0, MENU_FLAG_DATA, 0, 0),
            MENU_DESCR(2, PARAM_ID_MODBUS_ADDRESS, TEXT(TR_ID_MENU_MODBUS_ADDRESS), NULL, 0, MENU_FLAG_DATA, 0, 0),
    // Управление вентиляторами.
    MENU_DESCR(0, 0, TEXT(TR_ID_FAN_CONTROL), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_FAN_CONTROL_ENABLE, TEXT(TR_ID_FAN_CONTROL_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
        MENU_DESCR(1, PARAM_ID_FAN_TEMP_MIN, TEXT(TR_ID_FAN_TEMP_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_FAN_TEMP_MAX, TEXT(TR_ID_FAN_TEMP_MAX), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_FAN_RPM_MIN, TEXT(TR_ID_FAN_RPM_MIN), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_FAN_ECO_MODE_ENABLE, TEXT(TR_ID_FAN_ECO_MODE_ENABLE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
        MENU_DESCR(1, PARAM_ID_FAN_ECO_COOLING_TIME, TEXT(TR_ID_FAN_ECO_COOLING_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_FAN_I_NOM, TEXT(TR_ID_FAN_I_NOM), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_FAN_I_ZERO_NOISE, TEXT(TR_ID_FAN_I_ZERO_NOISE), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_FAN_PROT_TIME, TEXT(TR_ID_FAN_PROT_TIME), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_FAN_PROT_OVF_LEVEL, TEXT(TR_ID_FAN_PROT_OVF_LEVEL), NULL, 0, MENU_FLAG_DATA, 0, 0),
    // Цифровые входа/выхода.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_DIGITAL_IO), NULL, 0, 0, 0, 0),
        // Цифровые входа.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_DIGITAL_INPUTS), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_DIO_INPUT1), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_DIGITAL_IN_1_TYPE, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_dio_in_type),
                MENU_DESCR(3, PARAM_ID_DIGITAL_IN_1_INVERSION, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_DIO_INPUT2), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_DIGITAL_IN_2_TYPE, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_dio_in_type),
                MENU_DESCR(3, PARAM_ID_DIGITAL_IN_2_INVERSION, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_DIO_INPUT3), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_DIGITAL_IN_3_TYPE, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_dio_in_type),
                MENU_DESCR(3, PARAM_ID_DIGITAL_IN_3_INVERSION, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_DIO_INPUT4), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_DIGITAL_IN_4_TYPE, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_dio_in_type),
                MENU_DESCR(3, PARAM_ID_DIGITAL_IN_4_INVERSION, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_DIO_INPUT5), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_DIGITAL_IN_5_TYPE, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_dio_in_type),
                MENU_DESCR(3, PARAM_ID_DIGITAL_IN_5_INVERSION, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
        // Цифровые выхода.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_DIGITAL_OUTPUTS), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_DIO_OUTPUT1), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_DIGITAL_OUT_1_TYPE, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_dio_out_type),
                MENU_DESCR(3, PARAM_ID_DIGITAL_OUT_1_INVERSION, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_DIO_OUTPUT2), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_DIGITAL_OUT_2_TYPE, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_dio_out_type),
                MENU_DESCR(3, PARAM_ID_DIGITAL_OUT_2_INVERSION, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_DIO_OUTPUT3), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_DIGITAL_OUT_3_TYPE, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_dio_out_type),
                MENU_DESCR(3, PARAM_ID_DIGITAL_OUT_3_INVERSION, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),
            MENU_DESCR(2, 0, TEXT(TR_ID_MENU_DIO_OUTPUT4), NULL, 0, 0, 0, 0),
                MENU_DESCR(3, PARAM_ID_DIGITAL_OUT_4_TYPE, TEXT(TR_ID_MENU_DIO_TYPE), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_dio_out_type),
                MENU_DESCR(3, PARAM_ID_DIGITAL_OUT_4_INVERSION, TEXT(TR_ID_MENU_DIO_INVERSION), NULL, 0, MENU_FLAG_DATA, 0, &menu_val_bool),

        // Параметры цифровых входов/выходов.
        MENU_DESCR(1, 0, TEXT(TR_ID_MENU_DIGITAL_IO_SETTINGS), NULL, 0, 0, 0, 0),
            MENU_DESCR(2, PARAM_ID_DIGITAL_IO_DEADTIME_MS, TEXT(TR_ID_MENU_DIGITAL_IO_DEADTIME_MS), NULL, 0, MENU_FLAG_DATA, 0, 0),

    // Коэффициенты АЦП.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_ADC_VALUE_MULTS), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_VALUE_MULTIPLIER_Ua, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ua), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_VALUE_MULTIPLIER_Ub, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ub), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_VALUE_MULTIPLIER_Uc, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Uc), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_VALUE_MULTIPLIER_Urot, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Urot), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_VALUE_MULTIPLIER_Ia, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ia), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_VALUE_MULTIPLIER_Ib, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ib), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_VALUE_MULTIPLIER_Ic, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ic), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_VALUE_MULTIPLIER_Irot, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Irot), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_VALUE_MULTIPLIER_Iexc, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Iexc), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_VALUE_MULTIPLIER_Iref, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Iref), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_VALUE_MULTIPLIER_Ifan, TEXT(TR_ID_MENU_ADC_VALUE_MULT_Ifan), NULL, 0, MENU_FLAG_DATA, 0, 0),
    // Калибровочные данные.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_CALIBRATION_DATA_Ua, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ua), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_CALIBRATION_DATA_Ub, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ub), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_CALIBRATION_DATA_Uc, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Uc), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_CALIBRATION_DATA_Urot, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Urot), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_CALIBRATION_DATA_Ia, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ia), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_CALIBRATION_DATA_Ib, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ib), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_CALIBRATION_DATA_Ic, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ic), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_CALIBRATION_DATA_Irot, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Irot), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_CALIBRATION_DATA_Iexc, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Iexc), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_CALIBRATION_DATA_Iref, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Iref), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_ADC_CALIBRATION_DATA_Ifan, TEXT(TR_ID_MENU_ADC_CALIBRATION_DATA_Ifan), NULL, 0, MENU_FLAG_DATA, 0, 0),
    // Коэффициенты значений.
    MENU_DESCR(0, 0, TEXT(TR_ID_MENU_VALUE_MULTS), NULL, 0, 0, 0, 0),
        MENU_DESCR(1, PARAM_ID_VALUE_MULTIPLIER_Ua, TEXT(TR_ID_MENU_VALUE_MULT_Ua), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_VALUE_MULTIPLIER_Ub, TEXT(TR_ID_MENU_VALUE_MULT_Ub), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_VALUE_MULTIPLIER_Uc, TEXT(TR_ID_MENU_VALUE_MULT_Uc), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_VALUE_MULTIPLIER_Urot, TEXT(TR_ID_MENU_VALUE_MULT_Urot), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_VALUE_MULTIPLIER_Ia, TEXT(TR_ID_MENU_VALUE_MULT_Ia), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_VALUE_MULTIPLIER_Ib, TEXT(TR_ID_MENU_VALUE_MULT_Ib), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_VALUE_MULTIPLIER_Ic, TEXT(TR_ID_MENU_VALUE_MULT_Ic), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_VALUE_MULTIPLIER_Irot, TEXT(TR_ID_MENU_VALUE_MULT_Irot), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_VALUE_MULTIPLIER_Iexc, TEXT(TR_ID_MENU_VALUE_MULT_Iexc), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_VALUE_MULTIPLIER_Iref, TEXT(TR_ID_MENU_VALUE_MULT_Iref), NULL, 0, MENU_FLAG_DATA, 0, 0),
        MENU_DESCR(1, PARAM_ID_VALUE_MULTIPLIER_Ifan, TEXT(TR_ID_MENU_VALUE_MULT_Ifan), NULL, 0, MENU_FLAG_DATA, 0, 0),
    //MENU_DESCR(0, 0, TEXT(TR_ID_MENU_), NULL, 0, 0, 0, 0)
};

*/

#endif // PARAMETERS_MENU_H

#ifndef GUI_ICON_CONDITIONS_H
#define GUI_ICON_CONDITIONS_H

// другие ошибки
#define DRIVE_ERROR_ETC     DRIVE_ERROR_ROT_BREAK |\
                            DRIVE_ERROR_POWER_DATA_NOT_AVAIL |\
                            DRIVE_ERROR_PHASE

// ошибки по перегреву
#define DRIVE_ERROR_OVERHEAT    DRIVE_ERROR_THERMAL_OVERLOAD |\
                                DRIVE_ERROR_HEATSINK_TEMP

//другие предупреждения
#define DRIVE_WARNING_ETC 0xffffff

// предупреждения по перегреву
#define DRIVE_WARNING_OVERHEAT      DRIVE_WARNING_THERMAL_OVERLOAD |\
                                    DRIVE_WARNING_HEATSINK_TEMP

// ошибка по току возбуждения
#define DRIVE_POWER_ERROR_Iexc  DRIVE_POWER_ERROR_UNDERFLOW_Iexc |\
                                DRIVE_POWER_ERROR_OVERFLOW_Iexc |\
                                DRIVE_POWER_ERROR_IDLE_Iexc

// предупреждение по току возбуждения
#define DRIVE_POWER_WARNING_Iexc  DRIVE_POWER_WARNING_UNDERFLOW_Iexc |\
                                DRIVE_POWER_WARNING_OVERFLOW_Iexc |\
                                DRIVE_POWER_WARNING_IDLE_Iexc

// ошибка по напряжению
#define DRIVE_POWER_ERROR_U DRIVE_POWER_ERROR_UNDERFLOW_Ua |\
                            DRIVE_POWER_ERROR_UNDERFLOW_Ub |\
                            DRIVE_POWER_ERROR_UNDERFLOW_Uc |\
                            DRIVE_POWER_ERROR_UNDERFLOW_Urot |\
                            DRIVE_POWER_ERROR_OVERFLOW_Ua |\
                            DRIVE_POWER_ERROR_OVERFLOW_Ub |\
                            DRIVE_POWER_ERROR_OVERFLOW_Uc |\
                            DRIVE_POWER_ERROR_OVERFLOW_Urot |\
                            DRIVE_POWER_ERROR_IDLE_Urot

// предупреждение по напряжению
#define DRIVE_POWER_WARNING_U   DRIVE_POWER_WARNING_UNDERFLOW_Ua |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Ub |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Uc |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Urot |\
                                DRIVE_POWER_WARNING_OVERFLOW_Ua |\
                                DRIVE_POWER_WARNING_OVERFLOW_Ub |\
                                DRIVE_POWER_WARNING_OVERFLOW_Uc |\
                                DRIVE_POWER_WARNING_OVERFLOW_Urot |\
                                DRIVE_POWER_WARNING_IDLE_Urot
// ошибка по току
#define DRIVE_POWER_ERROR_I     DRIVE_POWER_ERROR_IDLE_Ia |\
                                DRIVE_POWER_ERROR_IDLE_Ib |\
                                DRIVE_POWER_ERROR_IDLE_Ic |\
                                DRIVE_POWER_ERROR_IDLE_Irot |\
                                DRIVE_POWER_ERROR_UNDERFLOW_Ia |\
                                DRIVE_POWER_ERROR_UNDERFLOW_Ib |\
                                DRIVE_POWER_ERROR_UNDERFLOW_Ic |\
                                DRIVE_POWER_ERROR_UNDERFLOW_Irot |\
                                DRIVE_POWER_ERROR_OVERFLOW_Ia |\
                                DRIVE_POWER_ERROR_OVERFLOW_Ib |\
                                DRIVE_POWER_ERROR_OVERFLOW_Ic |\
                                DRIVE_POWER_ERROR_OVERFLOW_Irot

// предупреждение по току
#define DRIVE_POWER_WARNING_I   DRIVE_POWER_WARNING_IDLE_Ia |\
                                DRIVE_POWER_WARNING_IDLE_Ib |\
                                DRIVE_POWER_WARNING_IDLE_Ic |\
                                DRIVE_POWER_WARNING_IDLE_Irot |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Ia |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Ib |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Ic |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Irot |\
                                DRIVE_POWER_WARNING_OVERFLOW_Ia |\
                                DRIVE_POWER_WARNING_OVERFLOW_Ib |\
                                DRIVE_POWER_WARNING_OVERFLOW_Ic |\
                                DRIVE_POWER_WARNING_OVERFLOW_Irot

// ошибка по току вентилятора
#define DRIVE_POWER_ERROR_Ifan  DRIVE_POWER_ERROR_UNDERFLOW_Ifan |\
                                DRIVE_POWER_ERROR_OVERFLOW_Ifan

// предупреждение по току вентилятора
#define DRIVE_POWER_WARNING_Ifan  DRIVE_POWER_WARNING_UNDERFLOW_Ifan |\
                                  DRIVE_POWER_WARNING_OVERFLOW_Ifan

// ошибка по току 4-20
#define DRIVE_POWER_ERROR_Iref    DRIVE_POWER_ERROR_UNDERFLOW_Iref |\
                                    DRIVE_POWER_ERROR_OVERFLOW_Iref
// предупреждение по току 4-20
#define DRIVE_POWER_WARNING_Iref    DRIVE_POWER_WARNING_UNDERFLOW_Iref |\
                                    DRIVE_POWER_WARNING_OVERFLOW_Iref

#endif /* GUI_ICON_CONDITIONS_H */


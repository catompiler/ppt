#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#include "localization/localization.h"
#include "translation_en.h"
#include "translation_ru.h"


/**
 * Идентификаторы языков.
 */
#define TR_LANG_ID_EN 0
#define TR_LANG_ID_RU 1


/**
 * Переводы для локализации.
 */
TRANSLATIONS_BEGIN(trs_main)
TRANSLATION(TR_LANG_ID_EN, text_trs_en)
TRANSLATION(TR_LANG_ID_RU, text_trs_ru)
TRANSLATIONS_END()


#endif /* TRANSLATIONS_H */


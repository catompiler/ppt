#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#ifdef TRANSLATION_DECL_ATTRIBS
#undef TRANSLATION_DECL_ATTRIBS
#endif
#define TRANSLATION_DECL_ATTRIBS static const

#ifdef TEXT_TR_DECL_ATTRIBS
#undef TEXT_TR_DECL_ATTRIBS
#endif
#define TEXT_TR_DECL_ATTRIBS static const
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


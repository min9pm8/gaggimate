#ifndef UI_NEW_PROFILE_SCREEN_H
#define UI_NEW_PROFILE_SCREEN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t *ui_NewProfileScreen;
extern lv_obj_t *ui_NewProfileScreen_card1;
extern lv_obj_t *ui_NewProfileScreen_card2;
extern lv_obj_t *ui_NewProfileScreen_name1;
extern lv_obj_t *ui_NewProfileScreen_name2;
extern lv_obj_t *ui_NewProfileScreen_detail1;
extern lv_obj_t *ui_NewProfileScreen_detail2;

void ui_NewProfileScreen_screen_init(void);

#ifdef __cplusplus
}
#endif

#endif

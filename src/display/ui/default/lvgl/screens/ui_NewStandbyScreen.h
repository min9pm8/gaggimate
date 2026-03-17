#ifndef UI_NEW_STANDBY_SCREEN_H
#define UI_NEW_STANDBY_SCREEN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t *ui_NewStandbyScreen;
extern lv_obj_t *ui_NewStandbyScreen_wifiIcon;
extern lv_obj_t *ui_NewStandbyScreen_btIcon;
extern lv_obj_t *ui_NewStandbyScreen_waterZone;
extern lv_obj_t *ui_NewStandbyScreen_brewZone;
extern lv_obj_t *ui_NewStandbyScreen_steamZone;

void ui_NewStandbyScreen_screen_init(void);

#ifdef __cplusplus
}
#endif

#endif

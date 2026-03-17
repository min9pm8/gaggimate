#ifndef UI_NEW_EVENTS_H
#define UI_NEW_EVENTS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_event_NewStandbyScreen(lv_event_t *e);
void ui_event_NewBrewScreen_gesture(lv_event_t *e);
void ui_event_NewBrewScreen_flush(lv_event_t *e);
void ui_event_NewBrewScreen_brew(lv_event_t *e);
void ui_event_NewBrewScreen_stop(lv_event_t *e);
void ui_event_NewBrewScreen_complete_dismiss(lv_event_t *e);
void ui_event_NewWaterScreen_gesture(lv_event_t *e);
void ui_event_NewWaterScreen_pump(lv_event_t *e);
void ui_event_NewSteamScreen_gesture(lv_event_t *e);

// Unified screen events
void ui_event_UnifiedScreen_flush(lv_event_t *e);
void ui_event_UnifiedScreen_action(lv_event_t *e);
void ui_event_UnifiedScreen_stop(lv_event_t *e);
void ui_event_UnifiedScreen_complete(lv_event_t *e);
void ui_event_UnifiedScreen_tap_left(lv_event_t *e);
void ui_event_UnifiedScreen_tap_right(lv_event_t *e);


#ifdef __cplusplus
}
#endif

#endif

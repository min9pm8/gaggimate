#include "ui_new_events.h"
#include "../../../main.h"
#include "screens/ui_NewStandbyScreen.h"
#include "screens/ui_NewBrewScreen.h"
#include "screens/ui_NewWaterScreen.h"
#include "screens/ui_NewSteamScreen.h"
#include "screens/ui_UnifiedScreen.h"
#include <display/core/constants.h>

// --- Standby: tap anywhere → Unified screen (brew mode) ---
void ui_event_NewStandbyScreen(lv_event_t *e) {
    // Just set mode — DefaultUI's mode handler will changeScreen and call set_mode_brew
    controller.setMode(MODE_BREW);
}

// --- Standby zone handlers ---
void ui_event_StandbyZone_water(lv_event_t *e) {
    // Disable zone to prevent double-tap
    if (ui_NewStandbyScreen_waterZone != NULL) {
        lv_obj_clear_flag(ui_NewStandbyScreen_waterZone, LV_OBJ_FLAG_CLICKABLE);
    }
    controller.setMode(MODE_WATER);
}

void ui_event_StandbyZone_brew(lv_event_t *e) {
    if (ui_NewStandbyScreen_brewZone != NULL) {
        lv_obj_clear_flag(ui_NewStandbyScreen_brewZone, LV_OBJ_FLAG_CLICKABLE);
    }
    controller.setMode(MODE_BREW);
}

void ui_event_StandbyZone_steam(lv_event_t *e) {
    if (ui_NewStandbyScreen_steamZone != NULL) {
        lv_obj_clear_flag(ui_NewStandbyScreen_steamZone, LV_OBJ_FLAG_CLICKABLE);
    }
    controller.setMode(MODE_STEAM);
}

// --- Brew: swipe navigation ---
void ui_event_NewBrewScreen_gesture(lv_event_t *e) {
    lv_indev_t *indev = lv_indev_get_act();
    if (indev == NULL) return;
    lv_dir_t dir = lv_indev_get_gesture_dir(indev);
    lv_indev_wait_release(indev);

    if (dir == LV_DIR_RIGHT) {
        controller.deactivate();
        controller.setMode(MODE_WATER);
    } else if (dir == LV_DIR_BOTTOM) {
        controller.deactivate();
        controller.clear();
        controller.activateStandby();
    }
}

void ui_event_NewBrewScreen_flush(lv_event_t *e) {
    controller.onFlush();
}

void ui_event_NewBrewScreen_brew(lv_event_t *e) {
    controller.activate();
}

void ui_event_NewBrewScreen_stop(lv_event_t *e) {
    controller.deactivate();
    controller.clear();
}

void ui_event_NewBrewScreen_complete_dismiss(lv_event_t *e) {
    ui_NewBrewScreen_set_idle();
}

// --- Water: swipe navigation + pump ---
void ui_event_NewWaterScreen_gesture(lv_event_t *e) {
    lv_indev_t *indev = lv_indev_get_act();
    if (indev == NULL) return;
    lv_dir_t dir = lv_indev_get_gesture_dir(indev);
    lv_indev_wait_release(indev);

    if (dir == LV_DIR_LEFT) {
        controller.deactivate();
        controller.setMode(MODE_BREW);
    } else if (dir == LV_DIR_RIGHT) {
        controller.deactivate();
        controller.setMode(MODE_STEAM);
    } else if (dir == LV_DIR_BOTTOM) {
        controller.deactivate();
        controller.activateStandby();
    }
}

void ui_event_NewWaterScreen_pump(lv_event_t *e) {
    controller.isActive() ? controller.deactivate() : controller.activate();
}

// --- Steam: swipe navigation ---
void ui_event_NewSteamScreen_gesture(lv_event_t *e) {
    lv_indev_t *indev = lv_indev_get_act();
    if (indev == NULL) return;
    lv_dir_t dir = lv_indev_get_gesture_dir(indev);
    lv_indev_wait_release(indev);

    if (dir == LV_DIR_LEFT) {
        controller.setMode(MODE_WATER);
    } else if (dir == LV_DIR_BOTTOM) {
        controller.activateStandby();
    }
}

// === Unified Screen Events ===

static int currentUIMode = MODE_BREW;

void ui_event_UnifiedScreen_flush(lv_event_t *e) {
    controller.onFlush();
}

void ui_event_UnifiedScreen_action(lv_event_t *e) {
    if (currentUIMode == MODE_WATER) {
        controller.isActive() ? controller.deactivate() : controller.activate();
    } else {
        controller.activate();
    }
}

void ui_event_UnifiedScreen_stop(lv_event_t *e) {
    controller.deactivate();
    controller.clear();
}

void ui_event_UnifiedScreen_complete(lv_event_t *e) {
    ui_UnifiedScreen_set_idle();
}

void ui_event_UnifiedScreen_standby(lv_event_t *e) {
    // Prevent double-tap by hiding button immediately
    if (ui_UnifiedScreen_standbyBtn != NULL) {
        lv_obj_add_flag(ui_UnifiedScreen_standbyBtn, LV_OBJ_FLAG_HIDDEN);
    }
    controller.setMode(MODE_STANDBY);
}

// Navigation order: Steam <-- Brew --> Water
// Left from Brew -> Steam, Left from Water -> Brew, Left from Steam -> no-op
// Right from Brew -> Water, Right from Water -> no-op, Right from Steam -> Brew
void ui_event_UnifiedScreen_left(lv_event_t *e) {
    if (currentUIMode == MODE_STEAM) return;
    if (currentUIMode == MODE_BREW) {
        currentUIMode = MODE_STEAM;
        controller.setMode(MODE_STEAM);
        ui_UnifiedScreen_set_mode_steam();
    } else if (currentUIMode == MODE_WATER) {
        currentUIMode = MODE_BREW;
        controller.setMode(MODE_BREW);
        ui_UnifiedScreen_set_mode_brew();
    }
    controller.getUI()->markDirty();
}

void ui_event_UnifiedScreen_right(lv_event_t *e) {
    if (currentUIMode == MODE_WATER) return;
    if (currentUIMode == MODE_BREW) {
        currentUIMode = MODE_WATER;
        controller.setMode(MODE_WATER);
        ui_UnifiedScreen_set_mode_water();
    } else if (currentUIMode == MODE_STEAM) {
        currentUIMode = MODE_BREW;
        controller.setMode(MODE_BREW);
        ui_UnifiedScreen_set_mode_brew();
    }
    controller.getUI()->markDirty();
}


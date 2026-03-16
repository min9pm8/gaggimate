#include "ui_new_events.h"
#include "../../../main.h"
#include "screens/ui_NewStandbyScreen.h"
#include "screens/ui_NewBrewScreen.h"
#include "screens/ui_NewWaterScreen.h"
#include "screens/ui_NewSteamScreen.h"
#include "screens/ui_UnifiedScreen.h"
#include <display/core/constants.h>

// --- Standby: tap anywhere → Brew ---
void ui_event_NewStandbyScreen(lv_event_t *e) {
    controller.getUI()->changeScreen(&ui_NewBrewScreen, &ui_NewBrewScreen_screen_init);
    controller.setMode(MODE_BREW);
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
    controller.deactivate();
    controller.activateStandby();
}

void ui_event_UnifiedScreen_left(lv_event_t *e) {
    if (currentUIMode == MODE_BREW) return;
    if (currentUIMode == MODE_WATER) {
        currentUIMode = MODE_BREW;
        controller.setMode(MODE_BREW);
        ui_UnifiedScreen_set_mode_brew();
    } else if (currentUIMode == MODE_STEAM) {
        currentUIMode = MODE_WATER;
        controller.setMode(MODE_WATER);
        ui_UnifiedScreen_set_mode_water();
    }
}

void ui_event_UnifiedScreen_right(lv_event_t *e) {
    if (currentUIMode == MODE_STEAM) return;
    if (currentUIMode == MODE_BREW) {
        currentUIMode = MODE_WATER;
        controller.setMode(MODE_WATER);
        ui_UnifiedScreen_set_mode_water();
    } else if (currentUIMode == MODE_WATER) {
        currentUIMode = MODE_STEAM;
        controller.setMode(MODE_STEAM);
        ui_UnifiedScreen_set_mode_steam();
    }
}

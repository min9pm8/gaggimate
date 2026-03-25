# Profile Selection Screen Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a profile selection screen that appears on first tap from standby after boot, showing two stacked profile cards. Tapping a card selects the profile and enters brew mode.

**Architecture:** New LVGL screen (`ui_NewProfileScreen`) with two card widgets. Standby tap handler checks a `profileSelectedThisSession` flag via the controller's UI — if false, navigates to profile screen instead of brew. Profile card taps call `profileManager->selectProfile()`, set the flag, and trigger `MODE_BREW`. The flag is exposed via `DefaultUI::needsProfileSelection()` for the C event handler to check.

**Tech Stack:** C/C++ (ESP32/LVGL 8.x)

**Spec:** `docs/superpowers/specs/2026-03-25-profile-selection-screen-design.md`

---

### Task 1: Create the profile screen (ui_NewProfileScreen)

**Files:**
- Create: `src/display/ui/default/lvgl/screens/ui_NewProfileScreen.h`
- Create: `src/display/ui/default/lvgl/screens/ui_NewProfileScreen.c`

- [ ] **Step 1: Create the header file**

Create `src/display/ui/default/lvgl/screens/ui_NewProfileScreen.h`:

```c
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
```

- [ ] **Step 2: Create the screen implementation**

Create `src/display/ui/default/lvgl/screens/ui_NewProfileScreen.c`:

```c
#include "ui_NewProfileScreen.h"
#include "../ui_new_colors.h"
#include "../ui_new_events.h"
#include "../ui.h"

lv_obj_t *ui_NewProfileScreen = NULL;
lv_obj_t *ui_NewProfileScreen_card1 = NULL;
lv_obj_t *ui_NewProfileScreen_card2 = NULL;
lv_obj_t *ui_NewProfileScreen_name1 = NULL;
lv_obj_t *ui_NewProfileScreen_name2 = NULL;
lv_obj_t *ui_NewProfileScreen_detail1 = NULL;
lv_obj_t *ui_NewProfileScreen_detail2 = NULL;

static lv_obj_t *create_profile_card(lv_obj_t *parent, lv_obj_t **name_out, lv_obj_t **detail_out,
                                      void (*tap_cb)(lv_event_t *)) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_size(card, 280, 80);
    lv_obj_set_style_bg_color(card, UI_COLOR_SURFACE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(card, UI_COLOR_BTN_BORDER, LV_PART_MAIN);
    lv_obj_set_style_border_width(card, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(card, 16, LV_PART_MAIN);
    lv_obj_set_style_pad_all(card, 12, LV_PART_MAIN);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Make card tappable
    lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(card, tap_cb, LV_EVENT_CLICKED, NULL);

    // Profile name
    *name_out = lv_label_create(card);
    lv_label_set_text(*name_out, "");
    lv_obj_set_style_text_font(*name_out, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(*name_out, UI_COLOR_TEXT_PRI, LV_PART_MAIN);

    // Profile detail
    *detail_out = lv_label_create(card);
    lv_label_set_text(*detail_out, "");
    lv_obj_set_style_text_font(*detail_out, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(*detail_out, UI_COLOR_TEXT_SEC, LV_PART_MAIN);

    return card;
}

void ui_NewProfileScreen_screen_init(void) {
    ui_NewProfileScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_NewProfileScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_NewProfileScreen, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewProfileScreen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewProfileScreen, 0, LV_PART_MAIN);

    // Swipe down → standby
    lv_obj_add_event_cb(ui_NewProfileScreen, ui_event_NewProfileScreen_gesture, LV_EVENT_GESTURE, NULL);

    // Center column layout
    lv_obj_t *col = lv_obj_create(ui_NewProfileScreen);
    lv_obj_remove_style_all(col);
    lv_obj_set_size(col, 320, 280);
    lv_obj_center(col);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(col, 16, LV_PART_MAIN);

    // Title
    lv_obj_t *title = lv_label_create(col);
    lv_label_set_text(title, "SELECT PROFILE");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(title, 2, LV_PART_MAIN);

    // Profile cards
    ui_NewProfileScreen_card1 = create_profile_card(col, &ui_NewProfileScreen_name1,
                                                     &ui_NewProfileScreen_detail1,
                                                     ui_event_NewProfileScreen_card1);
    ui_NewProfileScreen_card2 = create_profile_card(col, &ui_NewProfileScreen_name2,
                                                     &ui_NewProfileScreen_detail2,
                                                     ui_event_NewProfileScreen_card2);
}
```

- [ ] **Step 3: Verify build compiles**

Run: `pio run -e display`
Expected: Fails — event handler functions not yet declared. That's expected, they come in Task 2.

- [ ] **Step 4: Commit**

```bash
git add src/display/ui/default/lvgl/screens/ui_NewProfileScreen.h src/display/ui/default/lvgl/screens/ui_NewProfileScreen.c
git commit -m "feat(ui): add profile selection screen layout with two stacked cards"
```

---

### Task 2: Add event handlers for profile screen

**Files:**
- Modify: `src/display/ui/default/lvgl/ui_new_events.h`
- Modify: `src/display/ui/default/lvgl/ui_new_events.cpp`

- [ ] **Step 1: Declare new event functions in ui_new_events.h**

After the standby event declaration (line 10), add:

```c
void ui_event_NewProfileScreen_gesture(lv_event_t *e);
void ui_event_NewProfileScreen_card1(lv_event_t *e);
void ui_event_NewProfileScreen_card2(lv_event_t *e);
```

- [ ] **Step 2: Add include and profile selection helpers in ui_new_events.cpp**

Add include at top (after existing includes):
```cpp
#include "screens/ui_NewProfileScreen.h"
```

After the `deferred_go_brew` function (line 16), add:

```cpp
// Profile selection state — reset on boot, set after first profile pick
static bool profileSelectedThisSession = false;

bool ui_needs_profile_selection(void) {
    return !profileSelectedThisSession;
}

// Deferred profile select — called after card tap event is processed
static String pendingProfileId;

static void deferred_select_profile(void *data) {
    int cardIndex = (int)(intptr_t)data;
    std::vector<String> profiles = controller.getProfileManager()->listProfiles();
    if (cardIndex < (int)profiles.size()) {
        controller.getProfileManager()->selectProfile(profiles[cardIndex]);
    }
    profileSelectedThisSession = true;
    controller.setMode(MODE_BREW);
}
```

- [ ] **Step 3: Modify standby tap handler**

Replace the existing `ui_event_NewStandbyScreen` (lines 19-22) with:

```cpp
void ui_event_NewStandbyScreen(lv_event_t *e) {
    if (!profileSelectedThisSession) {
        // First tap after boot — show profile selection
        controller.getUI()->changeScreen(&ui_NewProfileScreen, &ui_NewProfileScreen_screen_init);
        controller.getUI()->markDirty();
        return;
    }
    currentUIMode = MODE_BREW;
    lv_async_call(deferred_go_brew, NULL);
}
```

- [ ] **Step 4: Add profile card tap handlers**

After the standby handler, add:

```cpp
void ui_event_NewProfileScreen_card1(lv_event_t *e) {
    lv_async_call(deferred_select_profile, (void *)(intptr_t)0);
}

void ui_event_NewProfileScreen_card2(lv_event_t *e) {
    lv_async_call(deferred_select_profile, (void *)(intptr_t)1);
}

void ui_event_NewProfileScreen_gesture(lv_event_t *e) {
    lv_indev_t *indev = lv_indev_get_act();
    if (indev == NULL) return;
    lv_dir_t dir = lv_indev_get_gesture_dir(indev);
    lv_indev_wait_release(indev);

    if (dir == LV_DIR_BOTTOM) {
        controller.activateStandby();
    }
}
```

- [ ] **Step 5: Also add declaration for ui_needs_profile_selection to the header**

In `ui_new_events.h`, add (inside the extern "C" block):

```c
bool ui_needs_profile_selection(void);
```

- [ ] **Step 6: Verify build compiles**

Run: `pio run -e display`
Expected: May fail if `getUI()` or `getProfileManager()` are not accessible. Check next task.

- [ ] **Step 7: Commit**

```bash
git add src/display/ui/default/lvgl/ui_new_events.h src/display/ui/default/lvgl/ui_new_events.cpp
git commit -m "feat(ui): add profile screen event handlers with session gate"
```

---

### Task 3: Expose getUI() and getProfileManager() on Controller

**Files:**
- Modify: `src/display/core/Controller.h`

- [ ] **Step 1: Check if getProfileManager() already exists**

Search `Controller.h` for `getProfileManager`. If it exists, skip to checking `getUI()`.

- [ ] **Step 2: Add missing accessors if needed**

If `getProfileManager()` is missing, add to the public section of Controller:
```cpp
ProfileManager *getProfileManager() const { return profileManager; }
```

If `getUI()` is missing, add:
```cpp
DefaultUI *getUI() const { return ui; }
```

Note: `getUI()` returns `DefaultUI *` — the event handler needs it to call `changeScreen()`. Check what type `ui` is in Controller.h and whether DefaultUI.h needs to be included or if a forward declaration suffices. The event handler file (`ui_new_events.cpp`) includes `main.h` which includes `Controller.h` — so the return type needs to be available. A forward declaration of DefaultUI in Controller.h with the implementation in Controller.cpp may be needed to avoid circular includes.

**Alternative approach if circular includes are a problem:** Instead of calling `controller.getUI()->changeScreen()` from the event handler, add a helper function on Controller:

```cpp
// In Controller.h
void showProfileScreen();

// In Controller.cpp
void Controller::showProfileScreen() {
    if (ui) {
        ui->changeScreen(&ui_NewProfileScreen, &ui_NewProfileScreen_screen_init);
        ui->markDirty();
    }
}
```

Then the event handler calls `controller.showProfileScreen()` instead.

- [ ] **Step 3: Verify build compiles**

Run: `pio run -e display`
Expected: Compiles successfully.

- [ ] **Step 4: Commit**

```bash
git add src/display/core/Controller.h src/display/core/Controller.cpp
git commit -m "feat(controller): expose profile screen navigation helper"
```

---

### Task 4: Populate profile cards with data from ProfileManager

**Files:**
- Modify: `src/display/ui/default/DefaultUI.cpp`
- Modify: `src/display/ui/default/DefaultUI.h`

- [ ] **Step 1: Add include for the new profile screen**

In `DefaultUI.cpp`, add with the other screen includes (if not already transitively included):
```cpp
#include <display/ui/default/lvgl/screens/ui_NewProfileScreen.h>
```

- [ ] **Step 2: Add profile screen to forceReinitScreen in applyTheme()**

In the `applyTheme()` method's screen re-init block, add after the standby screen check:
```cpp
else if (currentScreen == ui_NewProfileScreen)
    forceReinitScreen(&ui_NewProfileScreen, &ui_NewProfileScreen_screen_init);
```

- [ ] **Step 3: Add profile screen update in the render loop**

In the `loop()` method, after the existing screen update calls (around line 303-305), add:

```cpp
if (currentScreen == ui_NewProfileScreen) {
    updateNewProfileScreen();
}
```

- [ ] **Step 4: Add updateNewProfileScreen method declaration in DefaultUI.h**

In the private section (near other update methods around line 59-60), add:
```cpp
void updateNewProfileScreen();
```

- [ ] **Step 5: Implement updateNewProfileScreen in DefaultUI.cpp**

Add after the existing update methods:

```cpp
void DefaultUI::updateNewProfileScreen() {
    if (ui_NewProfileScreen_name1 == NULL) return;

    std::vector<String> profiles = profileManager->listProfiles();

    if (profiles.size() >= 1) {
        Profile p1;
        if (profileManager->loadProfile(profiles[0], p1)) {
            lv_label_set_text(ui_NewProfileScreen_name1, p1.label.c_str());
            char detail[32];
            snprintf(detail, sizeof(detail), "%.0fs", p1.getTotalDuration() / 1000.0f);
            lv_label_set_text(ui_NewProfileScreen_detail1, detail);
        }
    }

    if (profiles.size() >= 2) {
        Profile p2;
        if (profileManager->loadProfile(profiles[1], p2)) {
            lv_label_set_text(ui_NewProfileScreen_name2, p2.label.c_str());
            char detail[32];
            snprintf(detail, sizeof(detail), "%.0fs", p2.getTotalDuration() / 1000.0f);
            lv_label_set_text(ui_NewProfileScreen_detail2, detail);
        }
    }

    // Hide card 2 if only one profile
    if (profiles.size() < 2 && ui_NewProfileScreen_card2 != NULL) {
        lv_obj_add_flag(ui_NewProfileScreen_card2, LV_OBJ_FLAG_HIDDEN);
    }
}
```

**Note:** `updateNewProfileScreen()` is called in the render loop, but profile loading from SD is slow. To avoid loading every frame, add a `profileScreenPopulated` bool that gets set after first population and reset when the screen changes. Or simply guard with a static flag:

```cpp
void DefaultUI::updateNewProfileScreen() {
    static bool populated = false;
    if (ui_NewProfileScreen_name1 == NULL) return;
    if (currentScreen != ui_NewProfileScreen) { populated = false; return; }
    if (populated) return;
    populated = true;

    // ... load and set labels as above ...
}
```

- [ ] **Step 6: Verify build compiles**

Run: `pio run -e display`
Expected: Compiles successfully.

- [ ] **Step 7: Commit**

```bash
git add src/display/ui/default/DefaultUI.h src/display/ui/default/DefaultUI.cpp
git commit -m "feat(ui): populate profile cards from ProfileManager, add screen to theme/render loop"
```

---

### Task 5: Wire standby tap through profile gate and handle mode change for profile screen

**Files:**
- Modify: `src/display/ui/default/DefaultUI.cpp`

- [ ] **Step 1: Handle MODE_BREW when profile not yet selected**

In the `controller:mode:change` event handler (around line 135), the `MODE_BREW` case currently transitions to the unified screen. When `profileSelectedThisSession` is false (first boot, before any profile selected), the standby tap handler in `ui_new_events.cpp` already routes to the profile screen directly. But the mode change handler also needs to know about the profile screen — if we're on the profile screen and MODE_BREW is set (from the card tap), it should transition to the unified screen normally. No change needed here since `deferred_select_profile` calls `controller.setMode(MODE_BREW)` which triggers the mode change handler, and at that point the profile screen is current, so the `else` branch fires `changeScreen(&ui_UnifiedScreen, ...)`. This should work.

- [ ] **Step 2: Handle standby from profile screen**

When MODE_STANDBY is set while on the profile screen, the existing handler (line 132) will call `changeScreen(&ui_NewStandbyScreen, ...)`. This is correct — swiping down from profile screen returns to standby.

- [ ] **Step 3: Verify the full flow works**

Run: `pio run -e display`
Expected: Compiles successfully. Test flow:
1. Boot → standby screen
2. Tap → profile screen shows two cards
3. Tap a card → profile selected, transitions to brew (unified screen)
4. Swipe down to standby
5. Tap → goes straight to brew (skips profile screen)

- [ ] **Step 4: Commit if any changes were made**

```bash
git add src/display/ui/default/DefaultUI.cpp
git commit -m "feat(ui): wire profile selection gate into navigation flow"
```

---

### Task 6: Full build verification

**Files:** None (verification only)

- [ ] **Step 1: Build display target**

Run: `pio run -e display`
Expected: Compiles with no errors.

- [ ] **Step 2: Copy firmware**

```bash
cp .pio/build/display/firmware.bin ~/Documents/code/firmware.bin
```

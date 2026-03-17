# Unified Screen UI Fixes — Iteration 2

**Date:** 2026-03-16
**Target:** Waveshare ESP32-S3 Touch AMOLED 1.43" (466x466 round)
**Branch:** `claude/code-session-work-MuAf8`
**Files:** Primarily `ui_UnifiedScreen.c`, `ui_UnifiedScreen.h`, `ui_new_events.cpp`, `DefaultUI.cpp`

## Issues from Hardware Testing

All issues below were identified during on-device testing of the unified screen.

---

### Fix 1: Power/Standby Button Not Functional (CRITICAL)

**Problem:** The standby button does not respond to taps. It's inside the center_stack flex layout but something is intercepting clicks or the button isn't receiving events.

**Root cause candidates:**
- The center_stack has no explicit `LV_OBJ_FLAG_CLICKABLE` — but flex children should still be clickable
- The left/right transparent zones (116x300px) might still overlap the button area
- The button's `bg_opa = LV_OPA_TRANSP` might make LVGL skip hit-testing

**Fix:**
- Move standby button OUT of center_stack — place it directly on `ui_UnifiedScreen` as an absolutely positioned child
- Create it LAST (highest z-order)
- Use `lv_obj_move_foreground(ui_UnifiedScreen_standbyBtn)` to ensure it's on top
- Set `lv_obj_add_flag(ui_UnifiedScreen_standbyBtn, LV_OBJ_FLAG_CLICKABLE)`
- Give it a subtle visible background: `bg_color = UI_COLOR_SURFACE, bg_opa = UI_OPA(30)` instead of fully transparent
- Position: `LV_ALIGN_BOTTOM_MID, 0, -25` (move up slightly from edge)
- Size: 56x56px with border 2px
- Add `lv_obj_set_ext_click_area(ui_UnifiedScreen_standbyBtn, 15)` for larger touch target

**Verification:** Tap the power button → display must transition to standby screen with WiFi/BT icons.

---

### Fix 2: Remove Left/Right Arrow Indicators

**Problem:** The `◀` `▶` arrow labels in the left/right tap zones are no longer needed. The tap zones themselves work fine without visual indicators.

**Fix:** Remove the `leftArrow` and `rightArrow` label creation code from `ui_UnifiedScreen_screen_init()`. Keep the tap zones themselves (they handle mode switching).

**Files:** `ui_UnifiedScreen.c` — remove ~8 lines (leftArrow and rightArrow label creation).

---

### Fix 3: Flush Button Overlaps Rings + Remove White Ring Border

**Problem:** The flush button (84px) is positioned in the center_stack flex and visually overlaps the concentric ring gauges. It also has a white border (`border_color: UI_COLOR_BTN_BORDER, border_width: 2`) that looks out of place.

**Fix:**
- Remove the border: `lv_obj_set_style_border_width(ui_UnifiedScreen_flushBtn, 0, LV_PART_MAIN)`
- Reduce flush button size slightly: 72px instead of 84px (still large enough for half-asleep use)
- The overlap with rings is a spacing issue — increase ring inner radius gap or reduce center_stack height. Since rings are purely visual (non-clickable arcs), slight overlap is acceptable if it looks clean without the border.
- Update `UI_FLUSH_BTN_SIZE` to 72 and `UI_FLUSH_BTN_ACTIVE_SIZE` to 84 in `ui_new_colors.h`

---

### Fix 4: Power Button Too Close to Action Buttons

**Problem:** The standby/power button is positioned too close to the brew play button and the hot water pump button, making accidental taps likely.

**Fix:**
- Move standby button to absolute position at bottom of screen: `LV_ALIGN_BOTTOM_MID, 0, -25`
- It should NOT be in the center_stack flex — use absolute positioning on the screen object
- This creates visual separation from the action buttons (which are in the center_stack flex)
- Reduce standby button size to 48px (smaller than action buttons) to de-emphasize it

---

### Fix 5: Temperature Display — Current/Target Format

**Problem:** The target temp is shown in a separate "target 93°" row above the current temp. This takes too much vertical space and isn't how you'd normally read it.

**Fix:**
- Remove `ui_UnifiedScreen_targetLabel` entirely (header, .c declaration, creation code)
- Change the temp reactive effect in `DefaultUI.cpp` to format as `current/target`:
  ```
  "80/93°" in brew mode
  "76/83°" in water mode
  "118/142°" in steam mode
  ```
- Format: `snprintf(buf, sizeof(buf), "%d/%d\xC2\xB0", currentTemp, target)`
- When target is 0 or unavailable, just show `"80°"` (current only)

**Files:** `ui_UnifiedScreen.h`, `ui_UnifiedScreen.c`, `DefaultUI.cpp`

---

### Fix 6: Larger Ring Diameter

**Problem:** The rings could be larger now that arrows are removed and the layout is simpler.

**Fix:** Update `ui_new_colors.h`:
- `UI_RING_OUTER_RADIUS` — increase from 200 to 215 (430px diameter on 466px display)
- `UI_RING_INNER_RADIUS` — increase from 178 to 190
- `UI_RING_OUTER_WIDTH` — keep at 12
- `UI_RING_INNER_WIDTH` — keep at 10
- `UI_RING_SINGLE_RADIUS` — increase from 190 to 210

---

### Fix 7: Left/Right Tap Zones — Reduce Width

**Problem:** With arrows removed, the tap zones can be narrower, giving more room to the center content and rings.

**Fix:** Reduce left/right zone width from 116px to 80px. This still provides adequate tap area while allowing larger rings.

---

## Implementation Checklist

1. [ ] Fix 1: Make standby button functional (absolute position, foreground, extended click area)
2. [ ] Fix 2: Remove left/right arrow labels
3. [ ] Fix 3: Remove flush button border, reduce size to 72px
4. [ ] Fix 4: Move standby out of center_stack, absolute bottom position
5. [ ] Fix 5: Temp display as current/target format (80/93°)
6. [ ] Fix 6: Increase ring diameters (215/190/210)
7. [ ] Fix 7: Narrow left/right zones to 80px
8. [ ] Build and verify: `pio run -e display`
9. [ ] Flash and test on hardware
10. [ ] Commit

## Notes

- Fixes 1 and 4 are related — both involve the standby button positioning
- Fix 5 requires changes in both the screen .c file (remove target label) and DefaultUI.cpp (change reactive effect format)
- All fixes should be applied together as one commit since they affect the same layout

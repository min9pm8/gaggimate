# Profile Selection Screen

## Overview

Insert a profile selection screen between standby and brew. On first tap from standby after boot, show a screen with two stacked profile cards. Tapping a card selects that profile and transitions to brew (same as current standby → tap → brew flow). Subsequent standby → tap interactions skip the profile screen and go straight to brew with the last selected profile.

## Navigation Flow

**Current:** Standby → tap → Brew → swipe to Water/Steam

**New:** Standby → tap → **Profile Select** → tap profile → Brew → swipe to Water/Steam

After first profile selection, the flow reverts to current behavior:
Standby → tap → Brew (with previously selected profile)

## Screen Layout

Round 466x466 AMOLED display, black background.

- "Select Profile" title — small, muted, uppercase, centered near top
- Two vertically stacked cards, centered in display
- Each card shows:
  - Profile name (primary text, ~18px)
  - Brief details: duration and target info (~11px, muted)
- Cards styled with `UI_COLOR_SURFACE` background, `UI_COLOR_BTN_BORDER` border
- Tap a card → select profile, transition to brew

## Behavior

- `profileSelectedThisSession` bool in DefaultUI, initialized `false` on boot
- Standby tap handler checks `profileSelectedThisSession`:
  - `false` → navigate to profile screen
  - `true` → navigate to brew (current behavior)
- Profile screen loads both profiles from `ProfileManager::listProfiles()`
- On card tap:
  - Call `profileManager->selectProfile(profileId)`
  - Set `profileSelectedThisSession = true`
  - Set mode to `MODE_BREW` (same as current standby tap → `deferred_go_brew()`)
- No swipe gestures on this screen — just tap to select
- Swipe down returns to standby (consistent with other screens)

## Files to modify

| File | Change |
|------|--------|
| `src/display/ui/default/lvgl/screens/ui_NewProfileScreen.c` (new) | New screen with two stacked profile cards, tap handlers |
| `src/display/ui/default/lvgl/screens/ui_NewProfileScreen.h` (new) | Screen declarations (widget pointers, init function, set_profile helpers) |
| `src/display/ui/default/lvgl/ui_new_events.cpp` | Add profile card tap handlers, modify standby tap to route through profile check |
| `src/display/ui/default/lvgl/ui_new_events.h` | Declare new event handler functions |
| `src/display/ui/default/DefaultUI.h` | Add `profileSelectedThisSession` member, profile screen state |
| `src/display/ui/default/DefaultUI.cpp` | Route standby tap through profile selection gate, handle profile card selection, load profile data into screen |

## Constraints

- Only two profiles — no scrolling, pagination, or dynamic layout needed
- Profile screen uses the same color theme system as other screens (accent colors follow selected theme)
- Does not change any existing screen behavior — brew, water, steam, standby all work as before
- The existing `ui_ProfileScreen` (legacy/old screen system) is untouched

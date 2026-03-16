# Display UI Redesign — Waveshare 1.43" AMOLED

**Date:** 2026-03-16
**Target hardware:** Waveshare ESP32-S3 Touch AMOLED 1.43" (466x466, round)
**Branch:** `claude/code-session-work-MuAf8`

## Problem

The current UI was designed for the larger LilyGo T-RGB display and has several issues on the 1.43" round AMOLED:

- Play button at the very bottom of the screen is hard to reach on a round display (corners are dead zones)
- Home button also at the bottom — same problem
- Flush requires a long-press on the play button, which is clumsy and error-prone
- Too many screens and navigation steps for what is essentially a 3-mode workflow (brew, hot water, steam)
- Visual design doesn't leverage the AMOLED's true black capability or feel premium

## Design Principles

1. **Minimize screen engagement** — fewest taps/swipes to accomplish any task
2. **Pre-coffee usable** — large tap targets, obvious affordances, works half-asleep
3. **No long-press interactions** — all actions are simple taps
4. **Leverage AMOLED** — true black backgrounds (pixels off), vibrant accents on black

## Screens

The UI consists of 4 screens: Standby, Brew, Hot Water, Steam. No menu screen. No profile screen (profiles managed via Web UI).

### 1. Standby Screen

- **Palette:** Monochrome luxury — pure black (#000000) background, white/grey icons only
- **Content:** Two centered icons only — WiFi and Bluetooth
- **Icon size:** 64px each, spaced ~40px apart, centered on screen
- **No text** — no logo, no labels, no hints
- **Interaction:** Tap anywhere → transitions to Brew screen (fade-in, 300ms)
- **Purpose:** Minimal power draw, at-a-glance connectivity status

**Icon states:**

| Icon | Connected | Disconnected | Not configured |
|------|-----------|--------------|----------------|
| WiFi | #FFFFFF (white) | #FF3B30 (red) | Hidden |
| Bluetooth | #B0B0B0 (grey, `LV_OPA_50`) | #FF3B30 (red) | Hidden |

When in AP mode, WiFi icon uses a hotspot variant at #FFFFFF.

### 2. Brew Screen — Idle State

- **Palette:** Material Dark — true black background with vibrant accents
- **Layout:** Concentric ring gauges with vertical center stack
- **Outer ring:** Pressure gauge — #00E676 (Material green), 8px stroke
- **Inner ring:** Temperature gauge — #448AFF (Material blue), 6px stroke
- **Ring background:** #1A1A1A (barely visible track)
- **Mode label:** "BREW" at top, Montserrat 10, #666666, `lv_obj_set_style_text_letter_space(2)`

**Center stack (top to bottom):**

1. **Flush button** — 48px circle, 2px solid #666666 border, water droplet icon (22px), bg_color #FFFFFF at `LV_OPA_3`. Tap to start flushing, tap again to stop.
2. **Temperature** — Montserrat 48, regular weight, color #E0E0E0. Shows current boiler temp (e.g., "93°")
3. **Pressure** — Montserrat 14, regular weight, color #00E676. Shows current pressure (e.g., "8.2 bar")
4. **Brew button** — 52px circle, solid #00E676 background, play triangle icon (16px, black). Shadow: `shadow_color` #00E676, `shadow_width` 20, `shadow_opa` `LV_OPA_20`. Tap to start brewing.

**Navigation dots:** Bottom center, 3 dots (5px each), first dot #00E676 (active), others #333333. Indicates position in Brew ↔ Water ↔ Steam sequence.

### 3. Brew Screen — Flushing State

When flush is tapped:

- Flush button grows to 56px, border changes to #0069AD (blue), bg_color #0069AD at `LV_OPA_15`, shadow: `shadow_color` #0069AD, `shadow_width` 24, `shadow_opa` `LV_OPA_40`
- Icon changes from droplet to stop square (16px, #0069AD, 3px border-radius)
- "FLUSHING" label appears below button — Montserrat 10, #0069AD, `lv_obj_set_style_text_letter_space(2)`
- Ring gauges dim (`LV_OPA_30`)
- Temperature and pressure still visible but dimmed (`LV_OPA_60`)
- Brew button hidden — cannot brew while flushing
- Navigation dots hidden during flushing
- Tap flush button to stop flushing
- Swipe-down during flush: sends stop-flush command and immediately begins standby transition (atomic — no intermediate state)

### 4. Brew Screen — Active Brewing State

When brew is tapped:

- **Timer** replaces flush button position at top — Montserrat 16, #00E676, `lv_obj_set_style_text_letter_space(1)` (e.g., "0:24")
- **Temperature** — same position, Montserrat 48, #E0E0E0
- **Pressure** — same position, Montserrat 14, #00E676
- **Phase label** — below pressure, Montserrat 10, #666666, `lv_obj_set_style_text_letter_space(1)` (e.g., "PRE-INFUSION")
- **Stop button** — replaces brew button, 48px circle, bg_color #FF3B30 at `LV_OPA_15`, 2px solid #FF3B30 border, stop square icon (16px, #FF3B30, 3px border-radius)
- Ring gauges show live pressure and temperature data
- Navigation dots hidden during active brew

### 4a. Brew Screen — Complete State

When brew profile finishes (all phases done):

- Timer freezes at final value, color changes to #FFFFFF
- Phase label shows "COMPLETE"
- Stop button replaced by a checkmark icon (green #00E676) — tap to dismiss back to idle
- Rings freeze at final values
- Auto-dismisses back to Brew idle after 5 seconds if no interaction

### 5. Hot Water Screen

Reached by swiping right from Brew.

- **Mode label:** "HOT WATER" at top, Montserrat 10, #666666, `lv_obj_set_style_text_letter_space(2)`
- **Single ring:** Temperature gauge — #448AFF (blue), 8px stroke
- **Target temp:** "target 83°" (from `Settings::targetWaterTemp`) — Montserrat 12, #555555, above temperature reading
- **Current temp:** Montserrat 48, #E0E0E0 (e.g., "76°")
- **Status:** "heating..." — Montserrat 12, #448AFF (changes to "ready" when at target)
- **Pump button:** 50px circle, solid #448AFF, water droplet icon (22px). Shadow: `shadow_color` #448AFF, `shadow_width` 16, `shadow_opa` `LV_OPA_25`. Tap to start pump, tap again to stop.
- **Navigation dots:** Middle dot active (#448AFF)

### 6. Steam Screen

Reached by swiping right from Hot Water.

- **Mode label:** "STEAM" at top, Montserrat 10, #666666, `lv_obj_set_style_text_letter_space(2)`
- **Single ring:** Temperature gauge — #FF3B30 (red) while heating, transitions to #00E676 (green) when target reached
- **Target temp:** "target 143°" — Montserrat 12, #555555
- **Current temp:** Montserrat 48, #E0E0E0 (e.g., "118°")
- **Status:** "heating..." — Montserrat 12, #FF3B30 (changes to "ready" in #00E676 when at target)
- **No action button** — steam is controlled physically, screen is just a thermometer
- **Navigation dots:** Third dot active (#FF3B30, or #00E676 when ready)

## Navigation

### Screen Order

```
Standby → Brew ↔ Hot Water ↔ Steam
```

### Gestures

| Action | Result |
|--------|--------|
| Tap anywhere on Standby | → Brew screen |
| Swipe right from Brew | → Hot Water |
| Swipe right from Hot Water | → Steam |
| Swipe left from Steam | → Hot Water |
| Swipe left from Hot Water | → Brew |
| Swipe left from Brew | No-op (edge bounce animation) |
| Swipe right from Steam | No-op (edge bounce animation) |
| Swipe down from any active screen | → Standby (stops any active operation) |

### Auto-Return to Standby

- After a configurable period of inactivity, auto-return to Standby
- Default timeout: 60 seconds
- Configurable via Web UI settings
- Timer resets on any touch interaction
- Timer does NOT run during active operations (flushing, brewing, pumping)

## Color System

### Standby Palette (Monochrome Luxury)

| Token | Value | Usage |
|-------|-------|-------|
| Background | #000000 | True black — AMOLED pixels off |
| Icon primary | #FFFFFF | WiFi icon |
| Icon secondary | #B0B0B0 at 50% opacity | Bluetooth icon |

### Active Palette (Material Dark)

| Token | Value | Usage |
|-------|-------|-------|
| Background | #000000 | True black |
| Surface | #1A1A1A | Ring tracks, subtle backgrounds |
| Text primary | #E0E0E0 | Temperature readings |
| Text secondary | #666666 | Mode labels, phase labels |
| Text tertiary | #555555 | Target temps, hints |
| Muted | #333333 | Inactive dots, inactive elements |
| Green (accent) | #00E676 | Pressure ring, brew button, active states |
| Blue (accent) | #448AFF | Temperature ring, water mode, flush active |
| Red (accent) | #FF3B30 | Stop button, steam heating, alerts |
| Button border | #666666 | Flush button idle state |

### Mode Accent Colors

| Mode | Primary Accent | Usage |
|------|---------------|-------|
| Brew | #00E676 (green) | Pressure ring, brew button, active dot |
| Hot Water | #448AFF (blue) | Temp ring, pump button, active dot |
| Steam | #FF3B30 → #00E676 | Red while heating, green when ready |

## Typography

All text uses LVGL Montserrat font (regular weight only — LVGL does not ship light/thin weights). The visual "lightness" of temperature readings comes from the large font size on black background creating natural contrast.

**Required fonts in `lv_conf.h`:** Enable `LV_FONT_MONTSERRAT_10`, `LV_FONT_MONTSERRAT_12`, `LV_FONT_MONTSERRAT_14`, `LV_FONT_MONTSERRAT_16`, `LV_FONT_MONTSERRAT_48`. Disable unused sizes to save flash. Note: Montserrat 48 is ~60-80KB of flash — verify flash headroom during implementation. Montserrat 10 at 326 PPI renders at ~3.5mm physical height; acceptable for labels but verify readability on hardware.

| Element | Font | Color |
|---------|------|-------|
| Temperature reading | Montserrat 48 | #E0E0E0 |
| Pressure reading | Montserrat 14 | #00E676 |
| Timer | Montserrat 16 | #00E676 |
| Mode label | Montserrat 10 | #666666 |
| Phase label | Montserrat 10 | #666666 |
| Target temp | Montserrat 12 | #555555 |
| Status text | Montserrat 12 | Mode accent color |
| Flushing label | Montserrat 10 | #0069AD |

## Component Specifications

### Concentric Ring Gauges

- **Outer ring (pressure):** radius ~125px from center, 8px stroke, #1A1A1A track, accent color fill
- **Inner ring (temperature):** radius ~108px from center, 6px stroke, #1A1A1A track, accent color fill
- **Fill direction:** Clockwise from 12 o'clock (`lv_arc` with rotation -90°)
- **Line cap:** Round (`LV_ARC_MODE_NORMAL`)
- **Arc range:** 270 degrees (from 135° to 45°, leaving a gap at the bottom)
- Rings animate smoothly as values change (`lv_anim` with 200ms duration)

**Gauge value ranges:**

| Gauge | Min | Max | Notes |
|-------|-----|-----|-------|
| Pressure | 0 bar | 12 bar | Covers espresso range |
| Temperature (brew) | 0°C | 110°C | Covers brew range |
| Temperature (water) | 0°C | 100°C | Covers hot water range |
| Temperature (steam) | 0°C | 160°C | Covers steam range |

### Single Ring Gauge (Water/Steam)

- **Ring:** radius ~118px from center, 8px stroke, #1A1A1A track, mode accent color fill
- Same fill direction, arc range, and line cap as concentric rings

### Navigation Dots

- **Count:** 3 (Brew, Water, Steam)
- **Size:** 5px diameter circles
- **Spacing:** 5px gap
- **Position:** Bottom center of screen, 16px from edge
- **Active:** Mode accent color
- **Inactive:** #333333
- Hidden during active operations (brewing, flushing)

### Buttons

| Button | Size | Style |
|--------|------|-------|
| Flush (idle) | 48px circle | 2px #666 border, droplet icon, subtle bg |
| Flush (active) | 56px circle | 2px #0069AD border, stop square, blue glow |
| Brew | 52px circle | Solid #00E676, play triangle, green glow |
| Stop | 48px circle | 2px #FF3B30 border, stop square, red bg 15% |
| Pump (water) | 50px circle | Solid #448AFF, droplet icon, blue glow |

## Screens Not Included

The following screens from the current UI are **removed** in this redesign:

- **Menu screen** — replaced by swipe navigation between modes
- **Profile screen** — profiles managed via Web UI only
- **Grind screen** — not used on this hardware
- **Scale display** — not used

## Settings (Web UI)

New settings exposed via Web UI:

- **Standby timeout:** Duration of inactivity before auto-return to standby (default: 60s)

## Interaction Flow — Typical Session

1. Machine powers on → **Standby** (black screen, WiFi + BT icons)
2. User taps screen → **Brew idle** (rings appear, temp rising to 93°)
3. Temp reaches target → user taps **Flush** → screen enters flushing state
4. User taps to stop flush → back to **Brew idle**
5. User taps **Brew** → screen enters active brewing (timer, phase, live data)
6. Brew completes or user taps stop → back to **Brew idle**
7. User swipes right → **Hot Water** (blue ring, temp display, pump button)
8. User taps pump → hot water dispensing → taps again to stop
9. User swipes right → **Steam** (red ring heating to 143°) → ring turns green at target
10. User steams milk (physical) → swipes down → **Standby**

## Error & Warning States

### Error Overlay

When `Controller::isErrorState()` is true, display an error overlay on top of any screen:

- Full screen red-tinted overlay: bg_color #FF3B30 at `LV_OPA_80`
- Error text centered: Montserrat 16, #FFFFFF, mapped from `Controller::getError()` int codes:

| Code | Constant | Display Text |
|------|----------|-------------|
| 1 | ERROR_CODE_COMM_SEND | "COMM ERROR" |
| 2 | ERROR_CODE_COMM_RCV | "COMM ERROR" |
| 3 | ERROR_CODE_PROTO_ERR | "PROTOCOL ERROR" |
| 4 | ERROR_CODE_RUNAWAY | "THERMAL RUNAWAY" |
| 5 | ERROR_CODE_TIMEOUT | "CONNECTION LOST" |

- Tap to dismiss overlay (if error is recoverable) or auto-dismiss when error clears

### Low Water Warning

When `Controller::isLowWaterLevel()` returns true (below 20%):

- Small water droplet icon with exclamation mark appears at top-right of active screens
- Icon: 16px, #FF3B30, pulsing animation
- Does not block interaction — informational only

### Waiting for Controller

When `waitingForController` is true (BLE connection to Gaggia lost):

- Mode label text changes to "CONNECTING..." in #FF3B30
- Ring gauges show empty tracks (#1A1A1A only)
- Action buttons disabled (dimmed to `LV_OPA_30`)

## Screen Transitions

| Transition | Animation | Duration |
|------------|-----------|----------|
| Standby → Brew | `LV_SCR_LOAD_ANIM_FADE_ON` | 300ms |
| Brew ↔ Water | `LV_SCR_LOAD_ANIM_MOVE_LEFT` / `MOVE_RIGHT` | 250ms |
| Water ↔ Steam | `LV_SCR_LOAD_ANIM_MOVE_LEFT` / `MOVE_RIGHT` | 250ms |
| Any → Standby (swipe down) | `LV_SCR_LOAD_ANIM_FADE_ON` | 300ms |
| Edge bounce (no-op swipe) | Elastic overscroll, 150ms snap-back | 150ms |

## Round Display Clipping

The 466x466 panel is round — corner pixels are not visible. Apply a circular clip mask (`lv_obj_set_style_radius(LV_RADIUS_CIRCLE)`) to the screen container to prevent rendering artifacts outside the visible area.

## Technical Considerations

- All screens built with LVGL 8.4.0
- Color depth: 16-bit RGB565 — all colors specified as hex values, transparency via `lv_obj_set_style_bg_opa()` / `lv_obj_set_style_opa()`
- True black (#000000) background critical for AMOLED power savings
- Ring gauges use LVGL arc objects (`lv_arc`)
- Swipe gestures use LVGL gesture detection (`LV_DIR_LEFT`, `LV_DIR_RIGHT`, `LV_DIR_BOTTOM`)
- Button tap targets must be at least 48px (Apple HIG minimum ~44px) for half-asleep usability
- Existing event-driven architecture (PluginManager) used for temperature/pressure updates
- DefaultUI class handles screen state management and transitions
- SquareLine Studio-generated screens will be replaced with hand-coded LVGL screens for this layout
- Standby timeout default (60s) differs from existing `DEFAULT_STANDBY_TIMEOUT_MS` (900000ms / 15min) — update the constant and settings default

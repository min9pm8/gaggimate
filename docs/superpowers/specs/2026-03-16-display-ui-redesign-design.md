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
- **Content:** Two centered icons only — WiFi (white, #FFFFFF) and Bluetooth (grey, #B0B0B0 at 50% opacity)
- **Icon size:** 64px each, spaced ~40px apart, centered on screen
- **No text** — no logo, no labels, no hints
- **Interaction:** Tap anywhere → transitions to Brew screen
- **Purpose:** Minimal power draw, at-a-glance connectivity status

### 2. Brew Screen — Idle State

- **Palette:** Material Dark — true black background with vibrant accents
- **Layout:** Concentric ring gauges with vertical center stack
- **Outer ring:** Pressure gauge — #00E676 (Material green), 8px stroke
- **Inner ring:** Temperature gauge — #448AFF (Material blue), 6px stroke
- **Ring background:** #1A1A1A (barely visible track)
- **Mode label:** "BREW" at top, 9px, #666666, letter-spacing 2px

**Center stack (top to bottom):**

1. **Flush button** — 48px circle, 2px solid #666666 border, water droplet icon (22px), subtle background rgba(255,255,255,0.03). Tap to start flushing, tap again to stop.
2. **Temperature** — 46px font, weight 200, color #E0E0E0. Shows current boiler temp (e.g., "93°")
3. **Pressure** — 14px font, weight 300, color #00E676. Shows current pressure (e.g., "8.2 bar")
4. **Brew button** — 52px circle, solid #00E676 background, play triangle icon (16px, black). Subtle green glow: box-shadow 0 0 20px rgba(0,230,118,0.2). Tap to start brewing.

**Navigation dots:** Bottom center, 3 dots (5px each), first dot #00E676 (active), others #333333. Indicates position in Brew ↔ Water ↔ Steam sequence.

### 3. Brew Screen — Flushing State

When flush is tapped:

- Flush button grows to 56px, border changes to #0069AD (blue), background rgba(0,105,173,0.15), blue glow box-shadow 0 0 24px rgba(0,105,173,0.4)
- Icon changes from droplet to stop square (16px, #0069AD, 3px border-radius)
- "FLUSHING" label appears below button — 10px, #0069AD, letter-spacing 2px
- Ring gauges dim (opacity 0.3)
- Temperature and pressure still visible but dimmed (opacity 0.6)
- Brew button hidden — cannot brew while flushing
- "tap to stop" hint at bottom — 9px, #555555
- Tap flush button or tap anywhere to stop flushing

### 4. Brew Screen — Active Brewing State

When brew is tapped:

- **Timer** replaces flush button position at top — 16px, #00E676, letter-spacing 1px (e.g., "0:24")
- **Temperature** — same position, 46px, #E0E0E0
- **Pressure** — same position, 14px, #00E676
- **Phase label** — below pressure, 9px, #666666, letter-spacing 1px (e.g., "PRE-INFUSION")
- **Stop button** — replaces brew button, 48px circle, rgba(255,59,48,0.15) background, 2px solid #FF3B30 border, stop square icon (16px, #FF3B30, 3px border-radius)
- Ring gauges show live pressure and temperature data
- Navigation dots hidden during active brew

### 5. Hot Water Screen

Reached by swiping right from Brew.

- **Mode label:** "HOT WATER" at top, same style as brew
- **Single ring:** Temperature gauge — #448AFF (blue), 8px stroke
- **Target temp:** "target 83°" — 11px, #555555, above temperature reading
- **Current temp:** 46px, weight 200, #E0E0E0 (e.g., "76°")
- **Status:** "heating..." — 11px, #448AFF (changes to "ready" when at target)
- **Pump button:** 50px circle, solid #448AFF, water droplet icon (22px). Blue glow: box-shadow 0 0 16px rgba(68,138,255,0.25). Tap to start pump, tap again to stop.
- **Navigation dots:** Middle dot active (#448AFF)

### 6. Steam Screen

Reached by swiping right from Hot Water.

- **Mode label:** "STEAM" at top, same style
- **Single ring:** Temperature gauge — #FF3B30 (red) while heating, transitions to #00E676 (green) when target reached
- **Target temp:** "target 143°" — 11px, #555555
- **Current temp:** 46px, weight 200, #E0E0E0 (e.g., "118°")
- **Status:** "heating..." — 11px, #FF3B30 (changes to "ready" in green when at target)
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
| Swipe down from any active screen | → Standby |

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

All text uses the system/LVGL default sans-serif font.

| Element | Size | Weight | Color |
|---------|------|--------|-------|
| Temperature reading | 46px | 200 (light) | #E0E0E0 |
| Pressure reading | 14px | 300 | #00E676 |
| Timer | 16px | 400 | #00E676 |
| Mode label | 9px | 400 | #666666 |
| Phase label | 9px | 400 | #666666 |
| Target temp | 11px | 400 | #555555 |
| Status text | 11px | 400 | Mode accent color |

## Component Specifications

### Concentric Ring Gauges

- **Outer ring (pressure):** radius ~125px from center, 8px stroke, #1A1A1A track, accent color fill
- **Inner ring (temperature):** radius ~108px from center, 6px stroke, #1A1A1A track, accent color fill
- **Fill direction:** Clockwise from 12 o'clock (rotate -90°)
- **Line cap:** Round
- Rings animate smoothly as values change

### Single Ring Gauge (Water/Steam)

- **Ring:** radius ~118px from center, 8px stroke, #1A1A1A track, mode accent color fill
- Same fill direction and line cap as concentric rings

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

## Technical Considerations

- All screens built with LVGL 8.4.0
- True black (#000000) background critical for AMOLED power savings
- Ring gauges use LVGL arc objects (`lv_arc`)
- Swipe gestures use LVGL gesture detection (`LV_DIR_LEFT`, `LV_DIR_RIGHT`, `LV_DIR_BOTTOM`)
- Screen transitions should use smooth fade or slide animations
- Button tap targets must be at least 48px (Apple HIG minimum ~44px) for half-asleep usability
- Existing event-driven architecture (PluginManager) used for temperature/pressure updates
- DefaultUI class handles screen state management and transitions
- SquareLine Studio-generated screens will need to be replaced with hand-coded LVGL screens for this layout

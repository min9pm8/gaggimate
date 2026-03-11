# ESP32 Display Storage Reference

## Storage Partitions

The ESP32 display module has three distinct storage areas:

| Storage | Contents | Survives firmware flash? | Survives SPIFFS flash? |
|---------|----------|--------------------------|------------------------|
| **NVS** | WiFi credentials, PID values, GitLab token, all Settings page values | Yes | Yes |
| **SPIFFS** | Web UI (HTML/JS/CSS), profiles (`/p/*.json`), shot history (`/h/*.slog`) | Yes | **No — wiped** |
| **Firmware** | Compiled C++ application code | Replaced each flash | Yes |

The **controller** (separate MCU) has its own firmware and settings, independent of the display.

## What Each Binary Contains

- **`firmware.bin`** — The compiled C++ code: plugin logic, API handlers, sensor reading, PID control, OTA handler. Built with `pio run -e display`.
- **`spiffs.bin`** — The SPIFFS filesystem image: web UI assets, default profiles, shot history. Built with `scripts/build_spiffs.sh` then `pio run -e display -t buildfs`.

## NVS (Non-Volatile Storage)

Key-value store in a dedicated flash partition. Persists across all updates (firmware and SPIFFS). Only cleared by a full flash erase (`pio run -e display -t erase`).

Settings stored include:
- WiFi SSID and password
- PID tuning parameters
- GitLab Blog credentials (host, project ID, token, content path)
- Home Assistant credentials
- OTA channel, timezone, display brightness
- All other Settings page values

Settings are managed by `src/display/core/Settings.cpp` using the ESP32 Preferences library with short NVS keys (e.g., `gl_t` for GitLab token, `gl_h` for GitLab host).

## SPIFFS (SPI Flash File System)

Filesystem partition containing:
- `/w/` — Web UI assets (built from `web/` via Vite, gzipped)
- `/p/` — Brew profiles (JSON files)
- `/h/` — Shot history (`.slog` binary files, `.json` notes, `index.bin`)

**Flashing `spiffs.bin` overwrites the entire partition**, including any custom profiles and shot history created via the web UI. Default profiles from `data/p/` are included in the image.

## Build and Flash Commands

```bash
# Build firmware only
pio run -e display

# Build and flash firmware via USB
pio run -e display -t upload

# Build web UI, then build SPIFFS image
bash scripts/build_spiffs.sh
pio run -e display -t buildfs

# Flash SPIFFS via USB
pio run -e display -t uploadfs

# Full flash erase (wipes NVS + SPIFFS + firmware)
pio run -e display -t erase
```

## OTA Updates via Web UI

The web UI OTA page (`/ota`) supports uploading `firmware.bin` or `spiffs.bin` directly. The file type is auto-detected by filename. The device reboots after a successful upload.

## Version Identification

- **Display Version** — Shown on OTA page, derived from `git describe` + build timestamp at compile time
- **Web UI Build** — Shown on OTA page, Vite build timestamp baked into the JS bundle
- **Controller Version** — Reported by the controller MCU over serial

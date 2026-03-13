# JC3248W535 first-light baseline

Known-good PlatformIO baseline for the JC3248W535 capacitive-touch ESP32-S3 board.

## Working status
- Display working
- Landscape orientation working
- Touch working and aligned with display
- Display rotation in code set to `ROTATION_270`

## Important notes
- `lib/JC3248W535-Driver` is vendored locally
- `lib/Arduino_GFX` is the exact working library copy used for this baseline
- `platformio.ini` pins the PlatformIO Espressif platform version used for this baseline

## Purpose
This repo preserves a known-good recovery point before further development.

Credit's for creating the working library for this board:
https://github.com/me-processware/JC3248W535-Driver
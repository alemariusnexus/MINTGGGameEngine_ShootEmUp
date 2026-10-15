#pragma once

// For desktop, any BufferedScreen will do, even the null screen. We choose the native resolution the screen was
// developed for, since that avoids any baked borders due to aspect ratio mismatch, and it lets Qt do the scaling up
// and down to the window size in software, with potentially better quality than what the engine would do.
#define MGG_SCREEN_MAIN_TYPE            MGG_SCREEN_TYPE_ST7735_NULL
#define MGG_SCREEN_MAIN_WIDTH           160
#define MGG_SCREEN_MAIN_HEIGHT          128

// Pin doesn't matter, but it needs to be positive to enable audio
#define MGG_AUDIO_SPEAKER_PIN           0

// Game-specific
#define SHOOTEMUP_I2C_PIN_SCL           -1
#define SHOOTEMUP_I2C_PIN_SDA           -1

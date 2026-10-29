#pragma once

// For MCP230XX expander
#define MGG_BUS_I2C_0_PIN_SCL           6
#define MGG_BUS_I2C_0_PIN_SDA           5

// For ST7735 screen, SD card
#define MGG_BUS_SPI_0_PIN_MISO          8
#define MGG_BUS_SPI_0_PIN_MOSI          9
#define MGG_BUS_SPI_0_PIN_SCK           7

#define MGG_STORAGE_SDCARD_PIN_CS       43

#define MGG_SCREEN_MAIN_TYPE            MGG_SCREEN_TYPE_ST7735_SPI
#define MGG_SCREEN_MAIN_PIN_CS          44
#define MGG_SCREEN_MAIN_PIN_DC          1

#define MGG_AUDIO_SPEAKER_PIN           4

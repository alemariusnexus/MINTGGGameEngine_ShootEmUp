#pragma once

// SPI pins (e.g. for display, SD card)
#define SPI_MISO    9
#define SPI_MOSI    10
#define SPI_SCK     8

// SD card pins (in additon to SPI)
#define SD_CS	    21

// I2C pins (e.g. for IO expander)
#define I2C_SCL		7
#define I2C_SDA		6

// Speaker pin
#define SPEAKER_PIN 5

// Display type that's used
#define DISPLAY_TYPE_ST7735_SPI
//#define DISPLAY_TYPE_ILI9341_8080

// Display pin configuration (depending on type used)
#if defined(DISPLAY_TYPE_ST7735_SPI)
#   define DISPLAY_CS   20
#   define DISPLAY_DC   2
#   define DISPLAY_RST  -1
#elif defined(DISPLAY_TYPE_ILI9341_8080)
#   define DISPLAY_CS   21
#   define DISPLAY_DC   19
#   define DISPLAY_RST  22
#   define DISPLAY_WR   18
#   define DISPLAY_RD   -1
#   define DISPLAY_DAT0 23
#   define DISPLAY_DAT1 17
#   define DISPLAY_DAT2 16
#   define DISPLAY_DAT3 15
#   define DISPLAY_DAT4 14
#   define DISPLAY_DAT5 25
#   define DISPLAY_DAT6 26
#   define DISPLAY_DAT7 27
#endif

#pragma once

// SPI pins (e.g. for TFT, SD card)
#define SPI_BUS   SPI
#define SPI_MISO  D9
#define SPI_MOSI  D10
#define SPI_SCK   D8

// TFT pins (in addition to SPI)
#define TFT_CS		D7
#define TFT_RST   -1
#define TFT_DC    D0

// SD card pins (in additon to SPI)
#define SD_CS		  D6

// I2C pins (e.g. for IO expander)
#define I2C_SCL		D5
#define I2C_SDA		D4

// Speaker pin
#define SPEAKER_PIN   D3

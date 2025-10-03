#pragma once

#include <SPI.h>

#include "Arduino_GFX_Lite.h"

class Arduino_GC9A01_Lite : public Arduino_GFX {
public:
  Arduino_GC9A01_Lite(SPIClass &spi, int8_t dcPin, int8_t csPin, int8_t rstPin,
                      int8_t blPin, int8_t sckPin, int8_t mosiPin,
                      int8_t misoPin = -1, uint32_t freq = 60000000UL);

  bool begin(int32_t speed = 0) override;

  void startWrite() override;
  void endWrite() override;

  void writePixel(int16_t x, int16_t y, uint16_t color) override;
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                     uint16_t color) override;
  void writePixels(const uint16_t *colors, size_t len) override;

  void setRotation(uint8_t r) override;

  void invertDisplay(bool invert);
  void setBacklight(bool on);
  void setBacklightLevel(uint8_t level);

private:
  void hardwareReset();
  void executeInitSequence();
  void sendCommand(uint8_t cmd, const uint8_t *data = nullptr, size_t len = 0,
                   uint32_t delayMs = 0);
  void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  void writeColorRepeated(uint16_t color, uint32_t count);

  SPIClass &_spi;
  const int8_t _dcPin;
  const int8_t _csPin;
  const int8_t _rstPin;
  const int8_t _blPin;
  const int8_t _sckPin;
  const int8_t _mosiPin;
  const int8_t _misoPin;

  uint32_t _spiFreq;
  bool _inTransaction;
  uint16_t _cachedX0;
  uint16_t _cachedX1;
  uint16_t _cachedY0;
  uint16_t _cachedY1;
  uint16_t _colStart;
  uint16_t _rowStart;
  uint8_t _madctl;
  uint8_t _backlightLevel;
};


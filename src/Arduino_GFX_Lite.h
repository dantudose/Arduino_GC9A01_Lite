#pragma once

#include <Arduino.h>

class Arduino_GFX {
public:
  Arduino_GFX(int16_t w, int16_t h);
  virtual ~Arduino_GFX() = default;

  virtual bool begin(int32_t speed = 0) = 0;

  virtual void startWrite() = 0;
  virtual void endWrite() = 0;

  virtual void writePixel(int16_t x, int16_t y, uint16_t color) = 0;
  virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  virtual void writePixels(const uint16_t *colors, size_t len);

  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void fillScreen(uint16_t color);

  virtual void setRotation(uint8_t r);

  inline int16_t width() const { return _width; }
  inline int16_t height() const { return _height; }
  inline uint8_t rotation() const { return _rotation; }

protected:
  bool clipRect(int16_t &x, int16_t &y, int16_t &w, int16_t &h) const;

  int16_t _width;
  int16_t _height;
  const int16_t _rawWidth;
  const int16_t _rawHeight;
  uint8_t _rotation;
};


#include "Arduino_GFX_Lite.h"

Arduino_GFX::Arduino_GFX(int16_t w, int16_t h)
    : _width(w), _height(h), _rawWidth(w), _rawHeight(h), _rotation(0) {}

void Arduino_GFX::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || y < 0 || x >= _width || y >= _height) {
    return;
  }
  startWrite();
  writePixel(x, y, color);
  endWrite();
}

void Arduino_GFX::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  int16_t h = 1;
  if (!clipRect(x, y, w, h)) {
    return;
  }
  startWrite();
  writeFastHLine(x, y, w, color);
  endWrite();
}

void Arduino_GFX::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  int16_t w = 1;
  if (!clipRect(x, y, w, h)) {
    return;
  }
  startWrite();
  writeFastVLine(x, y, h, color);
  endWrite();
}

void Arduino_GFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (!clipRect(x, y, w, h)) {
    return;
  }
  startWrite();
  writeFillRect(x, y, w, h, color);
  endWrite();
}

void Arduino_GFX::fillScreen(uint16_t color) {
  fillRect(0, 0, _width, _height, color);
}

void Arduino_GFX::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  writeFillRect(x, y, w, 1, color);
}

void Arduino_GFX::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  writeFillRect(x, y, 1, h, color);
}

void Arduino_GFX::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  for (int16_t row = 0; row < h; ++row) {
    for (int16_t col = 0; col < w; ++col) {
      writePixel(x + col, y + row, color);
    }
  }
}

void Arduino_GFX::writePixels(const uint16_t *colors, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    writePixel(static_cast<int16_t>(i % _width), static_cast<int16_t>(i / _width), colors[i]);
  }
}

void Arduino_GFX::setRotation(uint8_t r) {
  _rotation = r & 0x03;
  if (_rotation & 0x01) {
    _width = _rawHeight;
    _height = _rawWidth;
  } else {
    _width = _rawWidth;
    _height = _rawHeight;
  }
}

bool Arduino_GFX::clipRect(int16_t &x, int16_t &y, int16_t &w, int16_t &h) const {
  if (w <= 0 || h <= 0) {
    return false;
  }
  if (x >= _width || y >= _height || (x + w) <= 0 || (y + h) <= 0) {
    return false;
  }
  if (x < 0) {
    w += x;
    x = 0;
  }
  if (y < 0) {
    h += y;
    y = 0;
  }
  if ((x + w) > _width) {
    w = _width - x;
  }
  if ((y + h) > _height) {
    h = _height - y;
  }
  return (w > 0 && h > 0);
}


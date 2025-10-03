#include "Arduino_GC9A01_Lite.h"

namespace {
struct InitCmd {
  uint8_t cmd;
  uint8_t len;
  uint8_t delayMs;
  const uint8_t *data;
};

constexpr uint8_t MADCTL_MY = 0x80;
constexpr uint8_t MADCTL_MX = 0x40;
constexpr uint8_t MADCTL_MV = 0x20;
constexpr uint8_t MADCTL_BGR = 0x08;

const uint8_t CMD_EF[] = {0x14};
const uint8_t CMD_EB[] = {0x14};
const uint8_t CMD_84[] = {0x40};
const uint8_t CMD_85[] = {0xFF};
const uint8_t CMD_86[] = {0xFF};
const uint8_t CMD_87[] = {0xFF};
const uint8_t CMD_88[] = {0x0A};
const uint8_t CMD_89[] = {0x21};
const uint8_t CMD_8A[] = {0x00};
const uint8_t CMD_8B[] = {0x80};
const uint8_t CMD_8C[] = {0x01};
const uint8_t CMD_8D[] = {0x01};
const uint8_t CMD_8E[] = {0xFF};
const uint8_t CMD_8F[] = {0xFF};
const uint8_t CMD_B6[] = {0x00, 0x20};
const uint8_t CMD_3A[] = {0x05};
const uint8_t CMD_90[] = {0x08, 0x08, 0x08, 0x08};
const uint8_t CMD_BD[] = {0x06};
const uint8_t CMD_BC[] = {0x00};
const uint8_t CMD_FF[] = {0x60, 0x01, 0x04};
const uint8_t CMD_C3[] = {0x13};
const uint8_t CMD_C4[] = {0x13};
const uint8_t CMD_C9[] = {0x22};
const uint8_t CMD_BE[] = {0x11};
const uint8_t CMD_E1[] = {0x10, 0x0E};
const uint8_t CMD_DF[] = {0x21, 0x0C, 0x02};
const uint8_t CMD_F0[] = {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A};
const uint8_t CMD_F1[] = {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F};
const uint8_t CMD_F2[] = {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A};
const uint8_t CMD_F3[] = {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F};
const uint8_t CMD_ED[] = {0x1B, 0x0B};
const uint8_t CMD_AE[] = {0x77};
const uint8_t CMD_CD[] = {0x63};
const uint8_t CMD_70[] = {0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03};
const uint8_t CMD_E8[] = {0x34};
const uint8_t CMD_62[] = {0x18, 0x0D, 0x71, 0xED, 0x70, 0x70,
                          0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70};
const uint8_t CMD_63[] = {0x18, 0x11, 0x71, 0xF1, 0x70, 0x70,
                          0x18, 0x13, 0x71, 0xF3, 0x70, 0x70};
const uint8_t CMD_64[] = {0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07};
const uint8_t CMD_66[] = {0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00};
const uint8_t CMD_67[] = {0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98};
const uint8_t CMD_74[] = {0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00};
const uint8_t CMD_98[] = {0x3E, 0x07};

const InitCmd GC9A01_INIT_CMDS[] = {
    {0xEF, 1, 0, CMD_EF}, {0xEB, 1, 0, CMD_EB}, {0xFE, 0, 0, nullptr},
    {0xEF, 0, 0, nullptr}, {0xEB, 1, 0, CMD_88}, {0x84, 1, 0, CMD_84},
    {0x85, 1, 0, CMD_85}, {0x86, 1, 0, CMD_86}, {0x87, 1, 0, CMD_87},
    {0x88, 1, 0, CMD_88}, {0x89, 1, 0, CMD_89}, {0x8A, 1, 0, CMD_8A},
    {0x8B, 1, 0, CMD_8B}, {0x8C, 1, 0, CMD_8C}, {0x8D, 1, 0, CMD_8D},
    {0x8E, 1, 0, CMD_8E}, {0x8F, 1, 0, CMD_8F}, {0xB6, 2, 0, CMD_B6},
    {0x3A, 1, 0, CMD_3A}, {0x90, 4, 0, CMD_90}, {0xBD, 1, 0, CMD_BD},
    {0xBC, 1, 0, CMD_BC}, {0xFF, 3, 0, CMD_FF}, {0xC3, 1, 0, CMD_C3},
    {0xC4, 1, 0, CMD_C4}, {0xC9, 1, 0, CMD_C9}, {0xBE, 1, 0, CMD_BE},
    {0xE1, 2, 0, CMD_E1}, {0xDF, 3, 0, CMD_DF}, {0xF0, 6, 0, CMD_F0},
    {0xF1, 6, 0, CMD_F1}, {0xF2, 6, 0, CMD_F2}, {0xF3, 6, 0, CMD_F3},
    {0xED, 2, 0, CMD_ED}, {0xAE, 1, 0, CMD_AE}, {0xCD, 1, 0, CMD_CD},
    {0x70, 9, 0, CMD_70}, {0xE8, 1, 0, CMD_E8}, {0x62, 12, 0, CMD_62},
    {0x63, 12, 0, CMD_63}, {0x64, 7, 0, CMD_64}, {0x66, 10, 0, CMD_66},
    {0x67, 10, 0, CMD_67}, {0x74, 7, 0, CMD_74}, {0x98, 2, 0, CMD_98},
    {0x35, 0, 0, nullptr}, {0x21, 0, 0, nullptr}, {0x11, 0, 120, nullptr},
    {0x29, 0, 20, nullptr}, {0x00, 0, 0, nullptr}};
}  // namespace

Arduino_GC9A01_Lite::Arduino_GC9A01_Lite(SPIClass &spi, int8_t dcPin,
                                         int8_t csPin, int8_t rstPin,
                                         int8_t blPin, int8_t sckPin,
                                         int8_t mosiPin, int8_t misoPin,
                                         uint32_t freq)
    : Arduino_GFX(240, 240), _spi(spi), _dcPin(dcPin), _csPin(csPin),
      _rstPin(rstPin), _blPin(blPin), _sckPin(sckPin), _mosiPin(mosiPin),
      _misoPin(misoPin), _spiFreq(freq), _inTransaction(false),
      _cachedX0(0xFFFF), _cachedX1(0xFFFF), _cachedY0(0xFFFF),
      _cachedY1(0xFFFF), _colStart(0), _rowStart(0), _madctl(MADCTL_BGR),
      _backlightLevel(255) {}

bool Arduino_GC9A01_Lite::begin(int32_t speed) {
  if (speed > 0) {
    _spiFreq = static_cast<uint32_t>(speed);
  }

  pinMode(_dcPin, OUTPUT);
  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);

  if (_rstPin >= 0) {
    pinMode(_rstPin, OUTPUT);
    digitalWrite(_rstPin, HIGH);
  }
  if (_blPin >= 0) {
    pinMode(_blPin, OUTPUT);
    digitalWrite(_blPin, LOW);
  }

  _spi.begin(_sckPin, _misoPin, _mosiPin, _csPin);

  hardwareReset();

  startWrite();
  executeInitSequence();
  setRotation(_rotation);
  endWrite();

  if (_blPin >= 0) {
    setBacklight(true);
  }

  return true;
}

void Arduino_GC9A01_Lite::startWrite() {
  if (_inTransaction) {
    return;
  }
  _spi.beginTransaction(SPISettings(_spiFreq, MSBFIRST, SPI_MODE0));
  digitalWrite(_csPin, LOW);
  _inTransaction = true;
}

void Arduino_GC9A01_Lite::endWrite() {
  if (!_inTransaction) {
    return;
  }
  digitalWrite(_csPin, HIGH);
  _spi.endTransaction();
  _inTransaction = false;
}

void Arduino_GC9A01_Lite::writePixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || y < 0 || x >= _width || y >= _height) {
    return;
  }
  setAddrWindow(static_cast<uint16_t>(x), static_cast<uint16_t>(y), 1, 1);
  uint8_t data[2] = {static_cast<uint8_t>(color >> 8),
                     static_cast<uint8_t>(color & 0xFF)};
  digitalWrite(_dcPin, HIGH);
  _spi.writeBytes(data, 2);
}

void Arduino_GC9A01_Lite::writeFastHLine(int16_t x, int16_t y, int16_t w,
                                         uint16_t color) {
  if (w <= 0) {
    return;
  }
  setAddrWindow(static_cast<uint16_t>(x), static_cast<uint16_t>(y),
                static_cast<uint16_t>(w), 1);
  writeColorRepeated(color, static_cast<uint32_t>(w));
}

void Arduino_GC9A01_Lite::writeFastVLine(int16_t x, int16_t y, int16_t h,
                                         uint16_t color) {
  if (h <= 0) {
    return;
  }
  setAddrWindow(static_cast<uint16_t>(x), static_cast<uint16_t>(y), 1,
                static_cast<uint16_t>(h));
  writeColorRepeated(color, static_cast<uint32_t>(h));
}

void Arduino_GC9A01_Lite::writeFillRect(int16_t x, int16_t y, int16_t w,
                                        int16_t h, uint16_t color) {
  if (w <= 0 || h <= 0) {
    return;
  }
  setAddrWindow(static_cast<uint16_t>(x), static_cast<uint16_t>(y),
                static_cast<uint16_t>(w), static_cast<uint16_t>(h));
  writeColorRepeated(color,
                     static_cast<uint32_t>(w) * static_cast<uint32_t>(h));
}

void Arduino_GC9A01_Lite::writePixels(const uint16_t *colors, size_t len) {
  if (colors == nullptr || len == 0) {
    return;
  }
  digitalWrite(_dcPin, HIGH);
  constexpr size_t chunk = 64;
  uint8_t buffer[chunk * 2];
  size_t remaining = len;
  const uint16_t *src = colors;
  while (remaining) {
    size_t batch = (remaining > chunk) ? chunk : remaining;
    for (size_t i = 0; i < batch; ++i) {
      uint16_t c = src[i];
      buffer[2 * i] = static_cast<uint8_t>(c >> 8);
      buffer[2 * i + 1] = static_cast<uint8_t>(c & 0xFF);
    }
    _spi.writeBytes(buffer, batch * 2);
    src += batch;
    remaining -= batch;
  }
}

void Arduino_GC9A01_Lite::setRotation(uint8_t r) {
  Arduino_GFX::setRotation(r);
  static const uint8_t madctl_lut[4] = {
      MADCTL_BGR | MADCTL_MX | MADCTL_MY,  // 0
      MADCTL_BGR | MADCTL_MV | MADCTL_MY,  // 1
      MADCTL_BGR,                          // 2
      MADCTL_BGR | MADCTL_MV | MADCTL_MX   // 3
  };
  _madctl = madctl_lut[_rotation & 0x03];
  _colStart = 0;
  _rowStart = 0;
  _cachedX0 = _cachedX1 = _cachedY0 = _cachedY1 = 0xFFFF;
  sendCommand(0x36, &_madctl, 1, 0);
}

void Arduino_GC9A01_Lite::invertDisplay(bool invert) {
  sendCommand(invert ? 0x21 : 0x20);
}

void Arduino_GC9A01_Lite::setBacklight(bool on) {
  if (_blPin < 0) {
    return;
  }
  _backlightLevel = on ? 255 : 0;
  digitalWrite(_blPin, on ? HIGH : LOW);
}

void Arduino_GC9A01_Lite::setBacklightLevel(uint8_t level) {
  if (_blPin < 0) {
    return;
  }
  _backlightLevel = level;
  digitalWrite(_blPin, level ? HIGH : LOW);
}

void Arduino_GC9A01_Lite::hardwareReset() {
  if (_rstPin < 0) {
    sendCommand(0x01, nullptr, 0, 150);
    return;
  }
  digitalWrite(_rstPin, HIGH);
  delay(10);
  digitalWrite(_rstPin, LOW);
  delay(20);
  digitalWrite(_rstPin, HIGH);
  delay(120);
}

void Arduino_GC9A01_Lite::executeInitSequence() {
  for (size_t i = 0;; ++i) {
    const auto &entry = GC9A01_INIT_CMDS[i];
    if (entry.cmd == 0x00 && entry.len == 0) {
      break;
    }
    sendCommand(entry.cmd, entry.data, entry.len, entry.delayMs);
  }
}

void Arduino_GC9A01_Lite::sendCommand(uint8_t cmd, const uint8_t *data,
                                       size_t len, uint32_t delayMs) {
  startWrite();
  digitalWrite(_dcPin, LOW);
  _spi.write(cmd);
  if (len && data != nullptr) {
    digitalWrite(_dcPin, HIGH);
    _spi.writeBytes(data, len);
  }
  digitalWrite(_dcPin, HIGH);
  if (delayMs) {
    delay(delayMs);
  }
}

void Arduino_GC9A01_Lite::setAddrWindow(uint16_t x, uint16_t y, uint16_t w,
                                        uint16_t h) {
  uint16_t x0 = x + _colStart;
  uint16_t y0 = y + _rowStart;
  uint16_t x1 = x0 + w - 1;
  uint16_t y1 = y0 + h - 1;

  uint8_t data[4];
  if (_cachedX0 != x0 || _cachedX1 != x1) {
    data[0] = static_cast<uint8_t>(x0 >> 8);
    data[1] = static_cast<uint8_t>(x0 & 0xFF);
    data[2] = static_cast<uint8_t>(x1 >> 8);
    data[3] = static_cast<uint8_t>(x1 & 0xFF);
    sendCommand(0x2A, data, 4, 0);
    _cachedX0 = x0;
    _cachedX1 = x1;
  }

  if (_cachedY0 != y0 || _cachedY1 != y1) {
    data[0] = static_cast<uint8_t>(y0 >> 8);
    data[1] = static_cast<uint8_t>(y0 & 0xFF);
    data[2] = static_cast<uint8_t>(y1 >> 8);
    data[3] = static_cast<uint8_t>(y1 & 0xFF);
    sendCommand(0x2B, data, 4, 0);
    _cachedY0 = y0;
    _cachedY1 = y1;
  }

  sendCommand(0x2C);
}

void Arduino_GC9A01_Lite::writeColorRepeated(uint16_t color, uint32_t count) {
  if (!count) {
    return;
  }
  digitalWrite(_dcPin, HIGH);
  uint8_t hi = static_cast<uint8_t>(color >> 8);
  uint8_t lo = static_cast<uint8_t>(color & 0xFF);
  constexpr size_t chunkPixels = 64;
  uint8_t buffer[chunkPixels * 2];
  for (size_t i = 0; i < chunkPixels; ++i) {
    buffer[2 * i] = hi;
    buffer[2 * i + 1] = lo;
  }

  uint32_t pixelsLeft = count;
  while (pixelsLeft) {
    size_t batch = (pixelsLeft > chunkPixels) ? chunkPixels : pixelsLeft;
    _spi.writeBytes(buffer, batch * 2);
    pixelsLeft -= batch;
  }
}


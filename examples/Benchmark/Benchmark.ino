#include <Arduino.h>
#include <math.h>

#include "Arduino_GFX_Lite_Library.h"

#ifndef PI
#define PI 3.14159265f
#endif

constexpr int8_t TFT_SCK = 12;
constexpr int8_t TFT_MOSI = 11;
constexpr int8_t TFT_MISO = -1;
constexpr int8_t TFT_CS = 4;
constexpr int8_t TFT_DC = 5;
constexpr int8_t TFT_RST = 3;
constexpr int8_t TFT_BL = 7;
constexpr uint32_t TFT_SPI_FREQ = 80000000UL;  // 80 MHz for maximum throughput

constexpr int16_t kSafeRadius = 108;

#if defined(SPI3_HOST)
SPIClass spiBus(SPI3_HOST);
#elif defined(VSPI)
SPIClass spiBus(VSPI);
#elif defined(FSPI)
SPIClass spiBus(FSPI);
#elif defined(HSPI)
SPIClass spiBus(HSPI);
#else
SPIClass spiBus;
#endif

Arduino_GC9A01_Lite gfx(spiBus, TFT_DC, TFT_CS, TFT_RST, TFT_BL, TFT_SCK,
                        TFT_MOSI, TFT_MISO, TFT_SPI_FREQ);

static inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return static_cast<uint16_t>((r & 0xF8) << 8) |
         static_cast<uint16_t>((g & 0xFC) << 3) |
         static_cast<uint16_t>(b >> 3);
}

uint16_t colorWheel(uint8_t position) {
  position = 255 - position;
  if (position < 85) {
    return color565(static_cast<uint8_t>(255 - position * 3), 0,
                    static_cast<uint8_t>(position * 3));
  }
  if (position < 170) {
    position -= 85;
    return color565(0, static_cast<uint8_t>(position * 3),
                    static_cast<uint8_t>(255 - position * 3));
  }
  position -= 170;
  return color565(static_cast<uint8_t>(position * 3),
                  static_cast<uint8_t>(255 - position * 3), 0);
}

struct Glyph5x7 {
  char c;
  uint8_t width;
  uint8_t rows[7];
};

constexpr Glyph5x7 kFont5x7[] = {
  {' ', 3, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {'0', 5, {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}},
  {'1', 4, {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}},
  {'2', 5, {0x0E, 0x11, 0x01, 0x06, 0x08, 0x10, 0x1F}},
  {'3', 5, {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E}},
  {'4', 5, {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}},
  {'5', 5, {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E}},
  {'6', 5, {0x0E, 0x10, 0x1E, 0x11, 0x11, 0x11, 0x0E}},
  {'7', 5, {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}},
  {'8', 5, {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}},
  {'9', 5, {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C}},
  {':', 3, {0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00}},
  {'A', 5, {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}},
  {'B', 5, {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E}},
  {'C', 5, {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}},
  {'D', 5, {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C}},
  {'E', 5, {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}},
  {'F', 5, {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10}},
  {'G', 5, {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0E}},
  {'H', 5, {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}},
  {'I', 4, {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E}},
  {'J', 5, {0x07, 0x02, 0x02, 0x02, 0x12, 0x12, 0x0C}},
  {'K', 5, {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}},
  {'L', 5, {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}},
  {'M', 5, {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11}},
  {'N', 5, {0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11}},
  {'O', 5, {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}},
  {'P', 5, {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10}},
  {'Q', 5, {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D}},
  {'R', 5, {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}},
  {'S', 5, {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E}},
  {'T', 5, {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
  {'U', 5, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}},
  {'V', 5, {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04}},
  {'W', 5, {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A}},
  {'X', 5, {0x11, 0x0A, 0x04, 0x04, 0x04, 0x0A, 0x11}},
  {'Y', 5, {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04}},
  {'Z', 5, {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F}},
};

constexpr int8_t kFont5x7Index[] = {
    0,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, -1, -1, -1, -1, -1,
    -1, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
    27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};

constexpr char kFontFirst = 32;
constexpr char kFontLast = 90;

enum class FontStyle : uint8_t { Normal, Wide, Italic };

const Glyph5x7 *getGlyph(char c) {
  if (c < kFontFirst || c > kFontLast) {
    if (c == ' ') {
      return &kFont5x7[0];
    }
    return nullptr;
  }
  const int8_t index = kFont5x7Index[c - kFontFirst];
  if (index < 0) {
    if (c == ' ') {
      return &kFont5x7[0];
    }
    return nullptr;
  }
  return &kFont5x7[static_cast<size_t>(index)];
}

void drawText(int16_t x, int16_t y, const char *text, uint16_t color,
              FontStyle style, uint8_t scale);

int16_t measureText(const char *text, FontStyle style, uint8_t scale) {
  if (text == nullptr || scale == 0) {
    return 0;
  }
  const uint8_t pixelWidth =
      static_cast<uint8_t>(style == FontStyle::Wide ? scale * 2 : scale);
  int16_t maxWidth = 0;
  int16_t lineWidth = 0;
  for (const char *p = text; *p; ++p) {
    const char c = *p;
    if (c == '\n') {
      if (lineWidth > maxWidth) {
        maxWidth = lineWidth;
      }
      lineWidth = 0;
      continue;
    }
    const Glyph5x7 *glyph = getGlyph(c);
    if (glyph == nullptr) {
      lineWidth += static_cast<int16_t>(pixelWidth * 3);
    } else {
      lineWidth +=
          static_cast<int16_t>(glyph->width * pixelWidth + pixelWidth);
    }
  }
  if (lineWidth > maxWidth) {
    maxWidth = lineWidth;
  }
  return maxWidth > 0 ? static_cast<int16_t>(maxWidth - pixelWidth) : 0;
}

void drawCenteredText(int16_t centerX, int16_t y, const char *text,
                      uint16_t color, FontStyle style, uint8_t scale) {
  const int16_t width = measureText(text, style, scale);
  const int16_t startX = static_cast<int16_t>(centerX - width / 2);
  drawText(startX, y, text, color, style, scale);
}

void drawText(int16_t x, int16_t y, const char *text, uint16_t color,
              FontStyle style, uint8_t scale) {
  if (scale == 0 || text == nullptr) {
    return;
  }
  const uint8_t pixelWidth =
      static_cast<uint8_t>(style == FontStyle::Wide ? scale * 2 : scale);
  int16_t cursorX = x;
  int16_t baselineY = y;

  gfx.startWrite();
  for (const char *p = text; *p; ++p) {
    char c = *p;
    if (c == '\n') {
      baselineY += static_cast<int16_t>(scale * 8);
      cursorX = x;
      continue;
    }

    const Glyph5x7 *glyph = getGlyph(c);
    if (glyph == nullptr) {
      cursorX += static_cast<int16_t>(pixelWidth * 3);
      continue;
    }

    for (uint8_t row = 0; row < 7; ++row) {
      const uint8_t bits = glyph->rows[row];
      const int16_t rowY = baselineY + static_cast<int16_t>(row * scale);
      const int16_t skew =
          (style == FontStyle::Italic) ? static_cast<int16_t>((6 - row) * scale / 3)
                                       : 0;
      for (uint8_t col = 0; col < glyph->width; ++col) {
        const bool pixelOn = (bits & (0x10 >> col)) != 0;
        if (pixelOn) {
          const int16_t colX = cursorX + skew + static_cast<int16_t>(col * pixelWidth);
          gfx.writeFillRect(colX, rowY, pixelWidth, scale, color);
        }
      }
    }

    cursorX += static_cast<int16_t>(glyph->width * pixelWidth + pixelWidth);
  }
  gfx.endWrite();
}

inline int16_t abs16(int16_t v) { return v >= 0 ? v : -v; }

void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t dx = abs16(x1 - x0);
  int16_t sx = (x0 < x1) ? 1 : -1;
  int16_t dy = -abs16(y1 - y0);
  int16_t sy = (y0 < y1) ? 1 : -1;
  int16_t err = dx + dy;

  while (true) {
    gfx.writePixel(x0, y0, color);
    if (x0 == x1 && y0 == y1) {
      break;
    }
    const int16_t e2 = static_cast<int16_t>(err << 1);
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  gfx.startWrite();
  writeLine(x0, y0, x1, y1, color);
  gfx.endWrite();
}

inline void swap16(int16_t &a, int16_t &b) {
  int16_t tmp = a;
  a = b;
  b = tmp;
}

void writeFillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                       int16_t x2, int16_t y2, uint16_t color) {
  if (y0 > y1) {
    swap16(y0, y1);
    swap16(x0, x1);
  }
  if (y1 > y2) {
    swap16(y1, y2);
    swap16(x1, x2);
  }
  if (y0 > y1) {
    swap16(y0, y1);
    swap16(x0, x1);
  }

  if (y0 == y2) {
    int16_t minX = x0;
    int16_t maxX = x0;
    if (x1 < minX) minX = x1;
    if (x2 < minX) minX = x2;
    if (x1 > maxX) maxX = x1;
    if (x2 > maxX) maxX = x2;
    gfx.writeFastHLine(minX, y0, static_cast<int16_t>(maxX - minX + 1), color);
    return;
  }

  int32_t dx01 = x1 - x0;
  int32_t dy01 = y1 - y0;
  int32_t dx02 = x2 - x0;
  int32_t dy02 = y2 - y0;
  int32_t dx12 = x2 - x1;
  int32_t dy12 = y2 - y1;

  int32_t sa = 0;
  int32_t sb = 0;

  int16_t y;
  int16_t last = (y1 == y2) ? y1 : static_cast<int16_t>(y1 - 1);

  for (y = y0; y <= last; ++y) {
    int16_t a = x0;
    if (dy01 != 0) {
      a = static_cast<int16_t>(x0 + sa / dy01);
    }
    int16_t b = x0;
    if (dy02 != 0) {
      b = static_cast<int16_t>(x0 + sb / dy02);
    }
    sa += dx01;
    sb += dx02;
    if (a > b) {
      swap16(a, b);
    }
    gfx.writeFastHLine(a, y, static_cast<int16_t>(b - a + 1), color);
  }

  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);

  for (; y <= y2; ++y) {
    int16_t a = x1;
    if (dy12 != 0) {
      a = static_cast<int16_t>(x1 + sa / dy12);
    }
    int16_t b = x0;
    if (dy02 != 0) {
      b = static_cast<int16_t>(x0 + sb / dy02);
    }
    sa += dx12;
    sb += dx02;
    if (a > b) {
      swap16(a, b);
    }
    gfx.writeFastHLine(a, y, static_cast<int16_t>(b - a + 1), color);
  }
}

void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                  int16_t y2, uint16_t color) {
  gfx.startWrite();
  writeFillTriangle(x0, y0, x1, y1, x2, y2, color);
  gfx.endWrite();
}

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                  int16_t y2, uint16_t color) {
  gfx.startWrite();
  writeLine(x0, y0, x1, y1, color);
  writeLine(x1, y1, x2, y2, color);
  writeLine(x2, y2, x0, y0, color);
  gfx.endWrite();
}

void demoLines(uint8_t repeat) {
  const int16_t cx = gfx.width() / 2;
  const int16_t cy = gfx.height() / 2;
  const uint8_t rays = 96;

  for (uint8_t iter = 0; iter < repeat; ++iter) {
    gfx.fillScreen(0x0000);

    gfx.startWrite();
    for (uint8_t i = 0; i < rays; ++i) {
      const float angle = static_cast<float>(i) * (2.0f * PI) / rays;
      const int16_t x1 = static_cast<int16_t>(
          cx + cosf(angle) * static_cast<float>(kSafeRadius));
      const int16_t y1 = static_cast<int16_t>(
          cy + sinf(angle) * static_cast<float>(kSafeRadius));
      writeLine(cx, cy, x1, y1,
                colorWheel(static_cast<uint8_t>(i * 255 / rays)));
    }
    gfx.endWrite();
  }
}

void demoTriangles(uint8_t repeat) {
  const int16_t cx = gfx.width() / 2;
  const int16_t cy = gfx.height() / 2;

  for (uint8_t iter = 0; iter < repeat; ++iter) {
    gfx.fillScreen(0x0000);

    gfx.startWrite();
    for (uint8_t i = 0; i < 12; ++i) {
      const float radius = 30.0f + static_cast<float>(i) * 6.5f;
      const float base = static_cast<float>(i) * (2.0f * PI / 12.0f);
      const float a0 = base;
      const float a1 = base + 2.0943951f;  // +120 degrees
      const float a2 = base + 4.1887902f;  // +240 degrees

      const int16_t x0 = static_cast<int16_t>(cx + cosf(a0) * radius);
      const int16_t y0 = static_cast<int16_t>(cy + sinf(a0) * radius);
      const int16_t x1 = static_cast<int16_t>(cx + cosf(a1) * radius);
      const int16_t y1 = static_cast<int16_t>(cy + sinf(a1) * radius);
      const int16_t x2 = static_cast<int16_t>(cx + cosf(a2) * radius);
      const int16_t y2 = static_cast<int16_t>(cy + sinf(a2) * radius);
      const uint16_t color = colorWheel(static_cast<uint8_t>(i * 20));

      writeLine(x0, y0, x1, y1, color);
      writeLine(x1, y1, x2, y2, color);
      writeLine(x2, y2, x0, y0, color);
    }
    gfx.endWrite();
  }
}

void demoFilledTriangles(uint8_t repeat) {
  const int16_t cx = gfx.width() / 2;
  const int16_t cy = gfx.height() / 2;

  for (uint8_t iter = 0; iter < repeat; ++iter) {
    gfx.fillScreen(0x0000);

    gfx.startWrite();
    for (uint8_t i = 0; i < 10; ++i) {
      const float outer = 25.0f + static_cast<float>(i) * 8.0f;
      const float middle = outer * 0.78f;
      const float inner = outer * 0.55f;
      const float base = static_cast<float>(i) * (2.0f * PI / 10.0f);
      const float a0 = base;
      const float a1 = base + 2.0943951f;  // +120 degrees
      const float a2 = base + 4.1887902f;  // +240 degrees

      const int16_t x0 = static_cast<int16_t>(cx + cosf(a0) * outer);
      const int16_t y0 = static_cast<int16_t>(cy + sinf(a0) * outer);
      const int16_t x1 = static_cast<int16_t>(cx + cosf(a1) * middle);
      const int16_t y1 = static_cast<int16_t>(cy + sinf(a1) * middle);
      const int16_t x2 = static_cast<int16_t>(cx + cosf(a2) * inner);
      const int16_t y2 = static_cast<int16_t>(cy + sinf(a2) * inner);
      const uint16_t color = colorWheel(static_cast<uint8_t>(i * 25 + 40));

      writeFillTriangle(x0, y0, x1, y1, x2, y2, color);
    }
    gfx.endWrite();
  }
}

void demoText(uint8_t repeat) {
  const int16_t cx = gfx.width() / 2;
  const int16_t cy = gfx.height() / 2;

  for (uint8_t iter = 0; iter < repeat; ++iter) {
    gfx.fillScreen(0x0000);

    drawCenteredText(cx, static_cast<int16_t>(cy - 48), "FONT NORMAL", 0xFFFF,
                     FontStyle::Normal, 2);
    drawCenteredText(cx, cy, "FONT WIDE", colorWheel(32), FontStyle::Wide, 2);
    drawCenteredText(cx, static_cast<int16_t>(cy + 48), "FONT ITALIC",
                     colorWheel(96), FontStyle::Italic, 2);
  }
}

struct Vec3 {
  float x;
  float y;
  float z;
};

struct Point2D {
  int16_t x;
  int16_t y;
};

constexpr Vec3 kCubeVertices[] = {
    {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},  {1.0f, 1.0f, -1.0f},
    {-1.0f, 1.0f, -1.0f},  {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},    {-1.0f, 1.0f, 1.0f},
};

constexpr uint8_t kCubeEdges[][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
    {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

constexpr uint8_t kCubeFaces[][4] = {
    {0, 1, 2, 3},  // front
    {4, 5, 6, 7},  // back
    {0, 3, 7, 4},  // left
    {1, 5, 6, 2},  // right
    {3, 2, 6, 7},  // top
    {0, 1, 5, 4}}; // bottom

void demoWireframe(uint8_t repeat) {
  const int16_t cx = gfx.width() / 2;
  const int16_t cy = gfx.height() / 2;
  const float scale = 80.0f;

  Vec3 transformed[8];
  Point2D projected[8];

  constexpr size_t frames = 48;
  const size_t totalFrames = frames * repeat;

  for (size_t frame = 0; frame < totalFrames; ++frame) {
    gfx.fillScreen(0x0000);

    const float angle = static_cast<float>(frame) * 0.12f;
    const float sinZ = sinf(angle);
    const float cosZ = cosf(angle);
    const float sinY = sinf(angle * 0.7f);
    const float cosY = cosf(angle * 0.7f);
    const float sinX = sinf(angle * 1.3f);
    const float cosX = cosf(angle * 1.3f);

    for (size_t i = 0; i < 8; ++i) {
      const Vec3 &v = kCubeVertices[i];
      // Rotate around Z
      float x = v.x * cosZ - v.y * sinZ;
      float y = v.x * sinZ + v.y * cosZ;
      float z = v.z;
      // Rotate around Y
      float x2 = x * cosY + z * sinY;
      float z2 = -x * sinY + z * cosY;
      // Rotate around X
      float y3 = y * cosX - z2 * sinX;
      float z3 = y * sinX + z2 * cosX;

      transformed[i] = {x2, y3, z3 + 4.0f};

      const float inv = 1.0f / transformed[i].z;
      projected[i].x = static_cast<int16_t>(cx + transformed[i].x * scale * inv);
      projected[i].y = static_cast<int16_t>(cy + transformed[i].y * scale * inv);
    }

    gfx.startWrite();
    for (size_t edge = 0; edge < 12; ++edge) {
      const uint8_t a = kCubeEdges[edge][0];
      const uint8_t b = kCubeEdges[edge][1];
      const uint16_t color = colorWheel(static_cast<uint8_t>(frame * 5 + edge * 12));
      writeLine(projected[a].x, projected[a].y, projected[b].x, projected[b].y,
                color);
    }
    gfx.endWrite();
  }
}

void demoFilledCube(uint8_t repeat) {
  const int16_t cx = gfx.width() / 2;
  const int16_t cy = gfx.height() / 2;
  const float scale = 72.0f;

  Vec3 transformed[8];
  Point2D projected[8];
  float faceDepth[6];
  uint8_t order[6];

  constexpr size_t frames = 48;
  const size_t totalFrames = frames * repeat;

  for (size_t frame = 0; frame < totalFrames; ++frame) {
    gfx.fillScreen(0x0000);

    const float angle = static_cast<float>(frame) * 0.11f;
    const float sinZ = sinf(angle);
    const float cosZ = cosf(angle);
    const float sinY = sinf(angle * 0.83f);
    const float cosY = cosf(angle * 0.83f);
    const float sinX = sinf(angle * 1.27f);
    const float cosX = cosf(angle * 1.27f);

    for (size_t i = 0; i < 8; ++i) {
      const Vec3 &v = kCubeVertices[i];
      float x = v.x * cosZ - v.y * sinZ;
      float y = v.x * sinZ + v.y * cosZ;
      float z = v.z;

      float x2 = x * cosY + z * sinY;
      float z2 = -x * sinY + z * cosY;

      float y3 = y * cosX - z2 * sinX;
      float z3 = y * sinX + z2 * cosX;

      transformed[i] = {x2, y3, z3 + 4.0f};

      const float inv = 1.0f / transformed[i].z;
      projected[i].x = static_cast<int16_t>(cx + transformed[i].x * scale * inv);
      projected[i].y = static_cast<int16_t>(cy + transformed[i].y * scale * inv);
    }

    for (uint8_t face = 0; face < 6; ++face) {
      const uint8_t *idx = kCubeFaces[face];
      faceDepth[face] = (transformed[idx[0]].z + transformed[idx[1]].z +
                         transformed[idx[2]].z + transformed[idx[3]].z) *
                        0.25f;
      order[face] = face;
    }

    for (uint8_t i = 0; i < 6; ++i) {
      for (uint8_t j = static_cast<uint8_t>(i + 1); j < 6; ++j) {
        if (faceDepth[order[i]] < faceDepth[order[j]]) {
          uint8_t tmp = order[i];
          order[i] = order[j];
          order[j] = tmp;
        }
      }
    }

    gfx.startWrite();
    for (uint8_t idx = 0; idx < 6; ++idx) {
      const uint8_t face = order[idx];
      const uint8_t *fv = kCubeFaces[face];
      const uint16_t color = colorWheel(static_cast<uint8_t>(face * 40 + frame * 5));

      writeFillTriangle(projected[fv[0]].x, projected[fv[0]].y,
                        projected[fv[1]].x, projected[fv[1]].y,
                        projected[fv[2]].x, projected[fv[2]].y, color);
      writeFillTriangle(projected[fv[0]].x, projected[fv[0]].y,
                        projected[fv[2]].x, projected[fv[2]].y,
                        projected[fv[3]].x, projected[fv[3]].y, color);

      writeLine(projected[fv[0]].x, projected[fv[0]].y, projected[fv[1]].x,
                projected[fv[1]].y, 0x0000);
      writeLine(projected[fv[1]].x, projected[fv[1]].y, projected[fv[2]].x,
                projected[fv[2]].y, 0x0000);
      writeLine(projected[fv[2]].x, projected[fv[2]].y, projected[fv[3]].x,
                projected[fv[3]].y, 0x0000);
      writeLine(projected[fv[3]].x, projected[fv[3]].y, projected[fv[0]].x,
                projected[fv[0]].y, 0x0000);
    }
    gfx.endWrite();
  }
}

struct DemoEntry {
  const char *name;
  void (*fn)(uint8_t repeat);
};

constexpr DemoEntry kDemos[] = {
    {"Lines", demoLines},
    {"Triangles", demoTriangles},
    {"Filled triangles", demoFilledTriangles},
    {"Text", demoText},
    {"Wireframe", demoWireframe},
    {"Filled cube", demoFilledCube},
};

constexpr uint8_t kDemoRepeat = 2;

void runBenchmark(const DemoEntry &entry, uint8_t repeatFactor) {
  Serial.print("Running ");
  Serial.print(entry.name);
  Serial.print(" ... ");
  const uint32_t start = millis();
  entry.fn(repeatFactor);
  const uint32_t elapsed = millis() - start;
  Serial.print("done in ");
  Serial.print(elapsed);
  Serial.println(" ms");
  delay(500);
}

void setup() {
  Serial.begin(115200);
  pinMode(18, OUTPUT);
  digitalWrite(18, HIGH);
  delay(200);
  Serial.println("GC9A01 Lite benchmark starting...");

  if (!gfx.begin(TFT_SPI_FREQ)) {
    Serial.println("GC9A01 init failed");
    while (true) {
      delay(1000);
    }
  }

  gfx.setRotation(0);
  gfx.fillScreen(0x0000);
}

void loop() {
  static size_t demoIndex = 0;
  runBenchmark(kDemos[demoIndex], kDemoRepeat);
  demoIndex = (demoIndex + 1) % (sizeof(kDemos) / sizeof(kDemos[0]));
}

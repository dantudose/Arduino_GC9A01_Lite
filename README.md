# Arduino_GC9A01_Lite

A minimal, high-performance driver for GC9A01 240x240 round TFT panels on the ESP32. The API mirrors the core of [Arduino_GFX](https://github.com/moononournation/Arduino_GFX.git) so sketches can drop in with minimal changes while achieving lower overhead on the SPI bus.

## Key features
- Tight SPI transactions at up to 80 MHz for fast full-screen updates
- GC9A01 power-on sequence tuned for common round panels
- Rotation, inversion, and clipping handled in hardware via `MADCTL`
- Backlight helper for simple on/off control (GPIO driven)

## Pin configuration example
```cpp
constexpr int8_t TFT_SCK = 12;
constexpr int8_t TFT_MOSI = 11;
constexpr int8_t TFT_MISO = -1;
constexpr int8_t TFT_CS = 4;
constexpr int8_t TFT_DC = 5;
constexpr int8_t TFT_RST = 3;
constexpr int8_t TFT_BL = 7;
SPIClass spiBus(VSPI);
Arduino_GC9A01_Lite gfx(spiBus, TFT_DC, TFT_CS, TFT_RST, TFT_BL,
                        TFT_SCK, TFT_MOSI, TFT_MISO, 80000000UL);
```

## Usage
```cpp
void setup() {
  gfx.begin();                 // optional: pass custom SPI frequency in Hz
  gfx.setRotation(1);          // 0-3, same as Arduino_GFX
  gfx.fillScreen(0x0000);      // RGB565 color helpers work as usual
  gfx.drawFastHLine(0, 120, 240, 0xFFFF);
}

void loop() {
  gfx.startWrite();
  // Prefer writeFast*/writeFillRect inside a transaction for best speed
  for (int x = 0; x < gfx.width(); ++x) {
    gfx.writeFastVLine(x, 0, gfx.height(), 0x07E0);
  }
  gfx.endWrite();
}
```

### Backlight
`setBacklight(true)` drives the configured GPIO high. For PWM control provide your own LEDC setup or extend `setBacklightLevel()`.

## Benchmark demo sketch
The provided `examples/Benchmark/Benchmark.ino` runs a loop of lightweight graphics benchmarks on the
GC9A01 round display:
- multi-directional line sweep
- outlined and filled triangle batches
- bitmap text rendered with normal, wide, and italic styles
- a rotating 3D cube wireframe using fast SPI transactions
- solid-shaded cube animation with painter-style face ordering

Geometries are confined to the ~108 px radius active glass so they appear fully
within the round viewing areaof the panel.

Each scene reports its render time over USB serial (`115200` baud) so you can
gauge throughput changes while experimenting with the driver or your own
optimisations.
Each demo currently runs two passes per loop iteration so you have more time to
inspect the output.

## Notes
- The driver caches address windows, so repeated writes to adjacent areas avoid redundant register updates.
- If the hardware reset pin is unavailable, pass `-1` and the driver falls back to the GC9A01 software reset command.
- The library does not cover fonts or widgets; pair it with lightweight graphics helpers as needed.

# PenumBra-Dome

Firmware for a custom Penumbra Dome board based on an **Arduino Mega 2560** and
the local **Reeltwo** library. The project can be built with PlatformIO or
opened directly in the Arduino IDE.

## Serial interfaces

| Function | Mega UART | Pins | Baud rate |
| --- | --- | --- | --- |
| Direct-command backup | `Serial1` | RX1 = 19, TX1 = 18 | 9600 |
| PSI Pro | `Serial3` | RX3 = 15, TX3 = 14 | 2400 |
| USB monitor/debug | `Serial` | USB | 115200 |

The normal command route is an I2C gateway based on an Arduino Pro Mini. The
external command sender is connected to the Pro Mini, not directly to the
Mega. `Serial1` remains available as a direct wired fallback.

## Serial-to-I2C command gateway

### Why the gateway is necessary

Marcduino commands such as `:SE00` occasionally arrived as `:SE0` or `:S00`.
The serial trace proved that the missing character was already absent before
Marcduino parsed the command. There was no command-buffer overflow.

On an AVR Mega, WS2812/SK6812 frames can temporarily prevent timely UART
handling. The FLD has 80 LEDs, the RLD 96 LEDs, and the Holo displays also
use this timing-sensitive LED protocol. A hardware UART is still the best
direct connection, but it cannot recover a byte which arrived while the LED
driver was timing-critical.

The gateway decouples the two time domains: the Pro Mini receives and buffers
the complete 9600-baud UART stream, while the Mega fetches buffered bytes over
I2C when it is available.

### Architecture

```text
External sender -- UART 9600 --> Pro Mini -- I2C --> Mega 2560 -- I2C --> PCA9685
```

The Mega is the **only I2C master**. It polls the Pro Mini at address `0x12`
every 2 ms and fetches up to 28 bytes per request. The Pro Mini is an I2C
slave only and never starts an I2C transfer, so it can safely share the bus
with the PCA9685.

### Wiring

| Connection | Connect to |
| --- | --- |
| Sender TX | Pro Mini D0 / RX |
| Sender GND | Pro Mini GND and Mega GND |
| Pro Mini A4 / SDA | Mega SDA (pin 20) and PCA9685 SDA |
| Pro Mini A5 / SCL | Mega SCL (pin 21) and PCA9685 SCL |

Use a **5 V / 16 MHz Pro Mini**. The existing I2C pull-ups should remain; do
not add excessive additional pull-ups. Disconnect the sender from Mega RX1
while it is connected to the Pro Mini. Do not send commands through the I2C
gateway and direct `Serial1` at the same time.

### Pro Mini firmware

Flash [pro-mini-serial-i2c-gateway.ino](pro-mini-serial-i2c-gateway/pro-mini-serial-i2c-gateway.ino)
to the Pro Mini using the Arduino IDE:

1. Select **Arduino Pro or Pro Mini**.
2. Select **ATmega328P, 5 V, 16 MHz**.
3. Disconnect the external sender from D0/RX during upload.
4. Upload and reconnect the sender afterwards.

The Mega gateway is enabled by default in `PenumBra-Dome.ino`:

```cpp
#define ENABLE_I2C_COMMAND_GATEWAY 1
#define I2C_COMMAND_GATEWAY_ADDRESS 0x12
```

The startup trace confirms it with:

```text
SETUP: I2C command gateway enabled
```

### LED test switches

The current repository state is the LED-free gateway reference test:

```cpp
#define ENABLE_LOGIC_DISPLAYS 0
#define ENABLE_HOLO_LEDS 0
```

After the I2C gateway has been verified, set both values to `1` to restore all
Holo, FLD and RLD outputs. The command receiver itself uses a fixed-size,
non-blocking buffer rather than `readStringUntil()`.

## LED libraries

The current library combination is:

```text
Reeltwo 23.5.7
FastLED 3.3.2
Adafruit NeoPixel 1.15.5
```

`USE_LEDLIB 0` selects ReelTwo's FastLED backend. FastLED 3.10.4 was tested
but is incompatible with the current AVR/PlatformIO toolchain; FastLED 3.3.2
is the working version for this project.

## PlatformIO

Open this folder in VS Code with the PlatformIO extension. The target is
already configured in [platformio.ini](platformio.ini):

```ini
[env:megaatmega2560]
platform = atmelavr
board = megaatmega2560
framework = arduino
```

The Reeltwo library is expected at:

```text
C:\Users\info\Documents\Arduino\libraries\Reeltwo
```

Use PlatformIO **Build**, **Upload** and **Monitor**. The monitor speed is
115200 baud.

## Arduino IDE

The original sketch remains [PenumBra-Dome.ino](PenumBra-Dome.ino), so it can
also be used directly in the Arduino IDE:

1. Keep this folder name as `PenumBra-Dome` and open `PenumBra-Dome.ino` using
   **File -> Open**.
2. Ensure the Reeltwo folder is installed at
   `C:\Users\info\Documents\Arduino\libraries\Reeltwo`.
3. In **Tools**, select **Board: Arduino Mega or Mega 2560** and
   **Processor: ATmega2560 (Mega 2560)**.
4. Select the correct COM port and upload.
5. Use the Arduino Serial Monitor at **115200 baud** for the optional trace.

`platformio.ini` and `src/main.cpp` are only PlatformIO support files; the
Arduino IDE compiles `PenumBra-Dome.ino` and the headers in this same folder.

## Diagnostic trace

`COMMAND_RX_TRACE` in `PenumBra-Dome.ino` prints received commands and raw
bytes to the USB monitor. It is useful while testing. To disable the diagnostic
output after verification, change:

```cpp
#define COMMAND_RX_TRACE
```

to:

```cpp
//#define COMMAND_RX_TRACE
```

The fixed-size command parser and the optional I2C gateway remain active when
the trace is disabled.

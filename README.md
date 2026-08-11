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

The normal command route is an I2C gateway based on an Arduino Pro Micro. The
external command sender is connected to the Pro Micro, not directly to the
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

The gateway decouples the two time domains: the Pro Micro receives and buffers
the complete 9600-baud UART stream, while the Mega fetches buffered bytes over
I2C when it is available.

### Architecture

```text
External sender -- UART 9600 --> Pro Micro -- I2C --> Mega 2560 -- I2C --> PCA9685
```

The Mega is the **only I2C master**. It polls the Pro Micro at address `0x12`
every 2 ms and fetches up to 28 bytes per request. The Pro Micro is an I2C
slave only and never starts an I2C transfer, so it can safely share the bus
with the PCA9685.

### Wiring

| Connection | Connect to |
| --- | --- |
| Sender TX | Pro Micro D0 / RX1 |
| Sender GND | Pro Micro GND and Mega GND |
| Pro Micro D2 / SDA | Mega SDA (pin 20) and PCA9685 SDA |
| Pro Micro D3 / SCL | Mega SCL (pin 21) and PCA9685 SCL |

Use a **5 V / 16 MHz Pro Micro**. The existing I2C pull-ups should remain; do
not add excessive additional pull-ups. Disconnect the sender from Mega RX1
while it is connected to the Pro Micro. Do not send commands through the I2C
gateway and direct `Serial1` at the same time.

### Pro Micro firmware

Flash [pro-micro-serial-i2c-gateway.ino](pro-micro-serial-i2c-gateway/pro-micro-serial-i2c-gateway.ino)
to the Pro Micro using the Arduino IDE:

1. Select **Arduino Micro** (or the matching Pro Micro board profile).
2. The board is **ATmega32U4, 5 V, 16 MHz**.
3. Upload through the Pro Micro's native USB port.
4. Sender UART uses `Serial1` on D0/RX1, so it does not share the USB serial
   interface used for upload.

The Mega gateway is enabled by default in `PenumBra-Dome.ino`:

```cpp
#define ENABLE_I2C_COMMAND_GATEWAY 1
#define I2C_COMMAND_GATEWAY_ADDRESS 0x12
```

The startup trace confirms it with:

```text
SETUP: I2C command gateway enabled
```

### Runtime switches

The current repository state enables all outputs and startup animations:

```cpp
#define ENABLE_LOGIC_DISPLAYS 1
#define ENABLE_HOLO_LEDS 1
#define ENABLE_BOOT_ANIMATIONS 1
#define ENABLE_LEGACY_BOOT_EFFECTS 1
```

Set individual values to `0` only for isolated hardware diagnostics. The
`ENABLE_LEGACY_BOOT_EFFECTS` switch restores the original FLD/RLD boot texts
and the `HPA199` Holo twitch. The command receiver itself uses a fixed-size,
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

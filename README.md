# PenumBra-Dome

Firmware for a custom Penumbra Dome board based on an **Arduino Mega 2560** and
the local **Reeltwo** library. The project can be built with PlatformIO or
opened directly in the Arduino IDE.

## Serial interfaces

| Function | Mega UART | Pins | Baud rate |
| --- | --- | --- | --- |
| Marcduino commands | `Serial3` | RX3 = 15, TX3 = 14 | 9600 |
| PSI Pro | `Serial2` | RX2 = 17, TX2 = 16 | 2400 |
| USB monitor/debug | `Serial` | USB | 115200 |

The external command sender must be connected **TX sender -> RX3 / pin 15**
and share GND with the Mega. Do not connect two TX outputs together.

## Serial3 receive fix

### Symptom

Marcduino commands such as `:SE00` occasionally arrived as `:SE0` or `:S00`.
The serial trace proved that the missing character was already absent before
Marcduino parsed the command. There was no software buffer overflow.

### Root cause

`AnimatedEvent::process()` runs the three `HoloLights` instances and the two
Logic Engine displays. The original `Adafruit_NeoPixel` backend disables AVR
interrupts while each SK6812/WS2812 LED frame is sent. If this coincides with
a command byte, the UART can lose a byte. The FLD has 80 LEDs and the RLD has
96 LEDs, so their frames are particularly long.

This happens even if no LED hardware is connected: the sketch still sends the
NeoPixel waveform on pins 22, 23 and 24.

### Implemented solution

The sketch now selects Reeltwo's FastLED backend with
`FASTLED_ALLOW_INTERRUPTS = 1`. FastLED yields to the UART interrupt during
the LED output, preferring a repeat of an LED frame over a lost serial byte.
`processAnimatedEvents()` retains the complete normal event set and runs LED
frames only after the command UART has been quiet for at least **4 ms**:

```cpp
if ((uint32_t)(micros() - lastComByteMicros) >= 4000UL)
{
    frontHolo.animate();
    rearHolo.animate();
    topHolo.animate();
    FLD.animate();
    RLD.animate();
}
```

At 9600 baud, command bytes are about 1 ms apart. Therefore a running command
postpones only LED-frame updates; all commands, panels, servos, logic displays
and buttons remain active. After the command, normal LED updates resume
automatically.

The local Arduino library `Reeltwo` was adapted for this FastLED configuration.
Its original state is preserved at:

```text
C:\Users\info\Documents\Arduino\libraries\Reeltwo-backup-before-fastled
```

The Arduino IDE and PlatformIO use the same adapted library from:

```text
C:\Users\info\Documents\Arduino\libraries\Reeltwo
```

Current tested library combination:

```text
Reeltwo 23.5.7
FastLED 3.3.2
Adafruit NeoPixel 1.15.5
```

The prior Adafruit NeoPixel 1.1.3 installation is preserved at
`C:\Users\info\Documents\Arduino\libraries\Adafruit_NeoPixel-backup-1.1.3`.

`readCom()` also uses a fixed-size, non-blocking receive buffer instead of
`readStringUntil()`. This avoids dynamic `String` allocation and blocking waits
in the command path.

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

The Serial3 receive fix remains active when the trace is disabled.

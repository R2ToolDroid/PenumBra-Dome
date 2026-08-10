/*
 * PenumBra Serial-to-I2C command gateway
 * Target: Arduino Pro Mini 5 V / 16 MHz
 *
 * Wiring
 *   External sender TX -> Pro Mini RX (D0)
 *   Pro Mini GND       -> Mega GND
 *   Pro Mini A4 (SDA)  -> Mega SDA / PCA9685 SDA
 *   Pro Mini A5 (SCL)  -> Mega SCL / PCA9685 SCL
 *
 * The Pro Mini is an I2C SLAVE only.  It never initiates an I2C transfer.
 * The Mega polls address 0x12 and receives up to 28 buffered UART bytes per
 * request.  This prevents I2C master collisions with the Mega's PCA9685 use.
 */

#include <Wire.h>

constexpr uint8_t kI2cAddress = 0x12;
constexpr uint8_t kI2cPayloadSize = 28; // 1 extra Wire byte is used for count
constexpr uint8_t kFifoSize = 128;      // Must be a power of two.
constexpr uint8_t kFifoMask = kFifoSize - 1;

volatile uint8_t fifo[kFifoSize];
volatile uint8_t fifoHead = 0;
volatile uint8_t fifoTail = 0;
volatile uint16_t fifoOverflows = 0;

void onI2cRequest()
{
    uint8_t count = 0;
    uint8_t cursor = fifoTail;
    while (cursor != fifoHead && count < kI2cPayloadSize)
    {
        cursor = (cursor + 1) & kFifoMask;
        count++;
    }

    Wire.write(count);
    while (count-- != 0)
    {
        Wire.write(fifo[fifoTail]);
        fifoTail = (fifoTail + 1) & kFifoMask;
    }
}

void setup()
{
    Serial.begin(9600);
    Wire.begin(kI2cAddress);
    Wire.onRequest(onI2cRequest);
}

void loop()
{
    while (Serial.available() > 0)
    {
        const uint8_t nextHead = (fifoHead + 1) & kFifoMask;
        const uint8_t received = (uint8_t)Serial.read();
        if (nextHead == fifoTail)
        {
            fifoOverflows++;
            continue;
        }
        fifo[fifoHead] = received;
        fifoHead = nextHead;
    }
}

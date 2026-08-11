/*
 * PenumBra Serial-to-I2C command gateway
 * Target: Arduino Pro Micro 5 V / 16 MHz (ATmega32U4)
 *
 * Wiring
 *   External sender TX -> Pro Micro D0 / RX1
 *   Pro Micro GND      -> Mega GND and sender GND
 *   Pro Micro D2 / SDA -> Mega SDA / PCA9685 SDA
 *   Pro Micro D3 / SCL -> Mega SCL / PCA9685 SCL
 *
 * Serial is the native USB port on the ATmega32U4. The sender therefore uses
 * the hardware UART Serial1 on D0/RX1 and D1/TX1. The Pro Micro is an I2C
 * SLAVE only; the Mega remains the only I2C master.
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
    Serial1.begin(9600);
    Wire.begin(kI2cAddress);
    Wire.onRequest(onI2cRequest);
}

void loop()
{
    while (Serial1.available() > 0)
    {
        const uint8_t nextHead = (fifoHead + 1) & kFifoMask;
        const uint8_t received = (uint8_t)Serial1.read();
        if (nextHead == fifoTail)
        {
            fifoOverflows++;
            continue;
        }
        fifo[fifoHead] = received;
        fifoHead = nextHead;
    }
}

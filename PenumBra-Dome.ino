//#define USE_DEBUG 
//#define DEBUG_SERIAL Serial 
//#define USE_HOLO_DEBUG

// Echo fully received Serial1 commands to the USB serial monitor without
// enabling the verbose Reeltwo debug output.
#define COMMAND_RX_TRACE
//#define SERIAL3_LOOPBACK_TEST

// Test switches retained for future diagnostics. Keep all disabled in normal
// operation.
//#define SERIAL3_RX_ONLY_TEST
//#define SERIAL3_RX_ANIMATED_TEST
//#define SERIAL3_RX_SKIP_HOLOS_TEST

#if defined(COMMAND_RX_TRACE)
#define SETUP_TRACE(message) Serial.println(F(message))
#else
#define SETUP_TRACE(message)
#endif

struct CommandRxDiagnostics
{
    uint16_t bytes = 0;
    uint16_t lines = 0;
    uint16_t lineOverflows = 0;
    uint8_t maxPending = 0;
    uint8_t raw[8] = {};
    uint8_t rawCount = 0;
    uint16_t rawDropped = 0;
};

CommandRxDiagnostics commandRxDiagnostics;
static uint32_t lastComByteMicros;

// Capture AVR reset flags before the normal runtime can change them.
uint8_t resetFlags __attribute__((section(".noinit")));

//Penumbra Mega PIN Mapping
//SV1-12 2-13
//Dout 1-8  22-29

//SERIAL 3 for PSI Pro

//SERIAL direct  9600 bd mit Zeilenumbruch (CR)

/*
 --- Magic panel ---
 SV5   CS
 SV6   CLK
 SV7   DATA
 --- Logic Engine ---
 DOUT 7  REAR
 DOUT 8  FRONT 
 --- HoloLights ---
 DOUT 1   FRONT
 DOUT 2   REAR
 DOUT 3   TOP
  
 --- Holo Servos ---
 PCA 1:::
 --- Dome Button --
 Left  Analog1   A0
 Right Analog2   A1


#include "dome/Logics.h"
#include "i2c/I2CReceiver.h"
  
 */

#include "ReelTwo.h"
#include <avr/wdt.h>
#include "core/Animation.h"
#include "core/DelayCall.h"
#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"
#include "core/Marcduino.h"

#include "dome/HoloLights.h"

#define FRONT_LOGIC_PIN 29
#define REAR_LOGIC_PIN 28

#include "dome/Logics.h"   //HACK to switch PINs to different Position  FRONT 29 REAR 28 
//#include "i2c/I2CReceiver.h"

#include "dome/MagicPanel.h"   /// PIN 8 DATA | PIN 7 CLK | PIN 6 CS

void captureResetFlags(void) __attribute__((naked, used, section(".init3")));
void captureResetFlags(void)
{
    resetFlags = MCUSR;
    MCUSR = 0;
    wdt_disable();
}

void printResetCause()
{
#if defined(COMMAND_RX_TRACE)
    Serial.print(F("RESET: "));
    if (resetFlags & _BV(PORF)) Serial.print(F("power "));
    if (resetFlags & _BV(EXTRF)) Serial.print(F("external "));
    if (resetFlags & _BV(BORF)) Serial.print(F("brown-out "));
    if (resetFlags & _BV(WDRF)) Serial.print(F("watchdog "));
    if (resetFlags == 0) Serial.print(F("unknown"));
    Serial.println();
#endif
}

//#define COMMAND_SERIAL Serial1 //   Serial1 for LIVE 

#define COM_SERIAL Serial1 //   Serial1 for LIVE 

//#ifdef RECEIVE_MARCDUINO_COMMANDS

#define PSI_COM Serial3 //  serial3 for PSI Pro
#define DOME_BTN_L A0
#define DOME_BTN_R A1

#define SMALL_PANEL         0x0001
#define MEDIUM_PANEL        0x0002
#define BIG_PANEL           0x0004
#define PIE_PANEL           0x0008



#define HOLO_HSERVO        0x1000
#define HOLO_VSERVO        0x2000

#define DOME_PANELS_MASK        (SMALL_PANEL|MEDIUM_PANEL|BIG_PANEL)
#define PIE_PANELS_MASK         (PIE_PANEL)
#define ALL_DOME_PANELS_MASK    (DOME_PANELS_MASK|PIE_PANELS_MASK)
#define HOLO_SERVOS_MASK        (HOLO_HSERVO|HOLO_VSERVO)

#define PANEL_GROUP_1      (1L<<14)
#define PANEL_GROUP_2      (1L<<15)
#define PANEL_GROUP_3      (1L<<16)
#define PANEL_GROUP_4      (1L<<17)
#define PANEL_GROUP_5      (1L<<18)
#define PANEL_GROUP_6      (1L<<19)
#define PANEL_GROUP_7      (1L<<20)
#define PANEL_GROUP_8      (1L<<21)
#define PANEL_GROUP_9      (1L<<22)
#define PANEL_GROUP_10     (1L<<23)





const ServoSettings servoSettings[] PROGMEM = {

    //* PIN, closed 0, Open 1, GRoupe//

    { 1, 800, 1200, HOLO_HSERVO },  /* 0: horizontal front holo */
    { 2, 800, 1200, HOLO_VSERVO },  /* 1: vertical front holo */
    { 3, 800, 1200, HOLO_HSERVO },  /* 2: horizontal top holo */
    { 4, 800, 1200, HOLO_VSERVO },  /* 3: vertical top holo */
    { 5, 800, 1200, HOLO_VSERVO },  /* 4: vertical rear holo */
    { 6, 800, 1200, HOLO_HSERVO },  /* 5: horizontal rear holo */

    { 7,  1950, 1150, PANEL_GROUP_1|SMALL_PANEL },  /* 1: door 1 */
    { 8,  2080, 1200,PANEL_GROUP_2|SMALL_PANEL },  /* 2: door 2 */
    { 9,  2150, 1200, PANEL_GROUP_3|SMALL_PANEL },  /* 3: door 3 */
    { 10, 1900, 1000, PANEL_GROUP_4|SMALL_PANEL },  /* 4: door 4 */
    
    { 11, 2050, 1200, PANEL_GROUP_5|MEDIUM_PANEL }, /* 5: door 5 */
    { 12, 2000, 1300, PANEL_GROUP_6|BIG_PANEL },    /* 6: door 6 */
    
    { 13, 2050, 1100, PANEL_GROUP_7|PIE_PANEL },   /* 8: pie panel 1 */
    { 14, 2050, 1100, PANEL_GROUP_8|PIE_PANEL },    /* 9: pie panel 2 */
    { 15, 2050, 800, PANEL_GROUP_9|PIE_PANEL },    /* 10: pie panel 3 */
    { 16, 2050, 800, PANEL_GROUP_10|PIE_PANEL },    /* 11: pie panel 4 */


    
};

MagicPanel magicPanel;
ServoDispatchPCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);
AnimationPlayer player(servoSequencer);

//MarcduinoSerial<> marcduinoSerial(COM_SERIAL, player);



HoloLights frontHolo(22,HoloLights::kRGB, HoloLights::kFrontHolo,12);        // PIN Dout1
HoloLights rearHolo(23, HoloLights::kRGB, HoloLights::kRearHolo);         // PIN Dout2
HoloLights topHolo(24, HoloLights::kRGB, HoloLights::kTopHolo);          // PIN Dout3

//HoloDisplay frontHolo(HoloLightHoloLights::kRGBW, HoloLights::kTopHolos::kRGBW, HoloLights::kFrontHolo);
//HoloLights rearHolo(HP_REAR_LED_PIN, HoloLights::kRGBW, HoloLights::kRearHolo);
//HoloLights topHolo(HP_TOP_LED_PIN, HoloLights::kRGB, HoloLights::kTopHolo, 12);

LogicEngineDeathStarFLDInverted<> FLD(LogicEngineFLDDefault);
LogicEngineDeathStarRLDInverted<> RLD(LogicEngineRLDDefault);



// Front Logic Device (Jawa ID#1)
//LogicEngineDeathStarFLD<> FLD(LogicEngineFLDDefault, 1);

// Rear Logic Device (Jawa ID#2)
//LogicEngineDeathStarRLDInverted<> RLD(LogicEngineRLDDefault, 2);


//SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK);

// === Command queue for Serial1 (COM_SERIAL) ===

void readCom()
{
    static char data[96];
    static uint8_t length = 0;

    const int pending = COM_SERIAL.available();
    if (pending > commandRxDiagnostics.maxPending)
        commandRxDiagnostics.maxPending = pending;

    while (COM_SERIAL.available() > 0)
    {
        const int received = COM_SERIAL.read();
        lastComByteMicros = micros();
        commandRxDiagnostics.bytes++;
        if (commandRxDiagnostics.rawCount < sizeof(commandRxDiagnostics.raw))
            commandRxDiagnostics.raw[commandRxDiagnostics.rawCount++] = received;
        else
            commandRxDiagnostics.rawDropped++;
        if (received == '\r' || received == '\n')
        {
            if (length == 0)
                continue;

            data[length] = '\0';
            commandRxDiagnostics.lines++;
#if defined(COMMAND_RX_TRACE)
            // Do not wait for a full USB TX buffer; receive timing wins over tracing.
            if (Serial.availableForWrite() >= length + 2)
            {
                Serial.write('>');
                Serial.write(data, length);
                Serial.write('\n');
            }
#endif
#if !defined(SERIAL3_RX_ONLY_TEST)
            // Keep Marcduino aliases (:SE00, #ON00, ...) and native Reeltwo
            // commands (HPA..., HPF..., LE..., MP...) available on the same
            // serial input. Non-matching commands are ignored by each layer.
            Marcduino::processCommand(player, data);
            CommandEvent::process(data);
#endif
            length = 0;
        }
        else if (length < sizeof(data) - 1)
        {
            data[length++] = static_cast<char>(received);
        }
        else
        {
            // Discard an oversized command instead of overrunning the buffer.
            commandRxDiagnostics.lineOverflows++;
            length = 0;
        }
    }
}

void reportComDiagnostics()
{
#if defined(COMMAND_RX_TRACE)
    static uint32_t lastReport;
    if ((lastReport != 0 && millis() - lastReport < 1000) || Serial.availableForWrite() < 60)
        return;

    Serial.print(F("RX B="));
    Serial.print(commandRxDiagnostics.bytes);
    Serial.print(F(" L="));
    Serial.print(commandRxDiagnostics.lines);
    Serial.print(F(" max="));
    Serial.print(commandRxDiagnostics.maxPending);
    Serial.print(F(" overflow="));
    Serial.print(commandRxDiagnostics.lineOverflows);
    Serial.print(F(" raw="));
    for (uint8_t i = 0; i < commandRxDiagnostics.rawCount; i++)
    {
        if (commandRxDiagnostics.raw[i] < 16)
            Serial.write('0');
        Serial.print(commandRxDiagnostics.raw[i], HEX);
        Serial.write(' ');
    }
    if (commandRxDiagnostics.rawDropped != 0)
    {
        Serial.write('+');
        Serial.print(commandRxDiagnostics.rawDropped);
    }
    Serial.println();

    commandRxDiagnostics = {};
    lastReport = millis();
#endif
}

void sendSerial3Loopback()
{
#if defined(SERIAL3_LOOPBACK_TEST)
    static uint32_t lastSend;
    if (millis() - lastSend >= 1000)
    {
        // This is intentionally not a Marcduino command.
        COM_SERIAL.print(F("!LB00\r"));
        lastSend = millis();
    }
#endif
}

void processAnimatedEvents()
{
    // These are the non-HoloLights AnimatedEvent instances constructed by this
    // sketch. They do not use a NeoPixel bitstream.
    magicPanel.animate();
    servoSequencer.animate();
    player.animate();

    // Adafruit_NeoPixel::show() temporarily masks AVR interrupts. Do not run
    // a HoloLights frame while a Serial3 command is being received; at 9600
    // baud the 4 ms quiet period is longer than the gap between command bytes.
    if ((uint32_t)(micros() - lastComByteMicros) >= 4000UL)
    {
        frontHolo.animate();
        rearHolo.animate();
        topHolo.animate();
    }
}






void resetSequence()
{
   // ensure any running Marcduino animation is terminated immediately
   player.end();

   SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK);

   //servoDispatch.setServosEasingMethod(HOLO_SERVOS_MASK, Easing::CircularEaseIn);
   // SEQUENCE_PLAY_ONCE_SPEED(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK, 1000);
    
    CommandEvent::process(F( 
        "LE000000|0\n"  //Logic Off
        "HPA000|0\n"    // HP Off
        "MP00000"));    /// Magic off
        
    PSI_COM.print("0T1\r");   //PSI off
    DEBUG_PRINTLN("reset"); 

    DEBUG_PRINTLN(FRONT_LOGIC_PIN); 
    DEBUG_PRINTLN(REAR_LOGIC_PIN); 
    
      
}


#include "MarduinoHolo.h"
#include "MarcduinoSequence.h"
#include "MarcduinoMagicPanel.h"
#include "MarcduinoPanel.h"
#include "DomeButton.h"




void setup()
{
    pinMode(DOME_BTN_L, INPUT_PULLUP);
    pinMode(DOME_BTN_R, INPUT_PULLUP);

    REELTWO_READY();

#if defined(DEBUG_SERIAL) || defined(USE_DEBUG) || defined(COMMAND_RX_TRACE)
    // USB Serial is used for optional debug output and command tracing.
    Serial.begin(115200);
    delay(10);
#if defined(COMMAND_RX_TRACE)
    Serial.println(F("COMMAND TRACE READY"));
    printResetCause();
#endif
#endif

    SETUP_TRACE("SETUP: Wire.begin");
    Wire.begin();
    SETUP_TRACE("SETUP: COM begin");

    COM_SERIAL.begin(9600);
    // small timeout for readBytesUntil()
    //COMMAND_SERIAL.setTimeout(20); // ms; tune between 10..50

    // Ensure only our poller reads the hardware serial
    //marcduinoSerial.setStream((Stream*)nullptr);

    // Normal Reeltwo setup: initialize every registered setup component.
    SetupEvent::ready();
    
   // Wire.setClock(400000); //Set i2c frequency to 400 kHz.

    randomSeed(analogRead(3));

    // servoDispatch.setOutputEnablePin(OUTPUT_ENABLED_PIN, true);
    //servoDispatch.setClockCalibration((const uint32_t[]) { 27570000, 27190000 });
    //servoDispatch.setServosEasingMethod(HOLO_SERVOS_MASK, Easing::CircularEaseIn);


    frontHolo.assignServos(&servoDispatch, 0, 1);
    topHolo.assignServos(&servoDispatch, 2, 3);
    rearHolo.assignServos(&servoDispatch, 4, 5);
    SETUP_TRACE("SETUP: holos assigned");
  
    PSI_COM.begin(2400);
    SETUP_TRACE("SETUP: PSI serial ready");

#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
    DEBUG_PRINTLN("ready.."); 
#endif


    FLD.selectScrollTextLeft("R2 D2-\n-by Doc", LogicEngineRenderer::kBlue, 1, 5);
    RLD.selectScrollTextLeft("... RSeries Doc Snyder ....", LogicEngineRenderer::kYellow, 0, 3);

    //CommandEvent::process(F("HPF104"));  
    //servoDispatch.moveTo(0, 150, 1000, 1.0);  
    
    // Do not start the random all-holo twitch during boot. It caused a reset
    // loop on this Mega configuration. The same HPA199 function remains
    // available when it is explicitly requested by a command.
    // CommandEvent::process(F("HPA199"));
    
   //CommandEvent::process(F("HPS9|45"));

    CommandEvent::process("MP20005");
    SETUP_TRACE("SETUP: magic command");
    //servoDispatch.setServosEasingMethod(ALL_DOME_PANELS_MASK, Easing::BounceEaseOut);
    //SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK);

    //servoDispatch.setServosEasingMethod(HOLO_SERVOS_MASK, Easing::CircularEaseIn);
    SEQUENCE_PLAY_ONCE_SPEED(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK, 2000);
    SETUP_TRACE("SETUP: complete");

    //PSI_COM.print("0T2\r");

}



void loop()
{
    readCom();
    reportComDiagnostics();
#if !defined(SERIAL3_RX_ONLY_TEST)
    // Regular processing
    processAnimatedEvents();
    DomeButton();
    readCom();
    sendSerial3Loopback();
#elif defined(SERIAL3_RX_ANIMATED_TEST)
    // Keep the command parser and button code out of this test.
    processAnimatedEvents();
#endif

}

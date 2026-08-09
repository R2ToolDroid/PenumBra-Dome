#//#define USE_DEBUG 
//#define DEBUG_SERIAL
//#define USE_SERVO_DEBUG
//#define USE_HOLO_DEBUG

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

#define COMMAND_SERIAL Serial1 //   Serial1 for LIVE 

//#ifdef RECEIVE_MARCDUINO_COMMANDS

#define PSI_COM Serial3 //  serial for PSI Pro
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

MarcduinoSerial<> marcduinoSerial(COMMAND_SERIAL, player);



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

// === Command queue for Serial1 (COMMAND_SERIAL) ===
#define CMD_QUEUE_SIZE 8
#define CMD_MAX_LEN 64
static char cmdQueue[CMD_QUEUE_SIZE][CMD_MAX_LEN];
static volatile uint8_t cmdQueueHead = 0;
static volatile uint8_t cmdQueueTail = 0;
static char rxBuf[CMD_MAX_LEN];
static size_t rxPos = 0;
static unsigned long lastByteMs = 0;
const unsigned long INTERBYTE_TIMEOUT_MS = 30; // ms

void pollSerial1AndQueue()
{
    // COMMAND_SERIAL is a HardwareSerial object, not a pointer — don't compare to nullptr

    // Drain available bytes quickly into rxBuf
    while (COMMAND_SERIAL.available()) {
        int c = COMMAND_SERIAL.read();
        if (c < 0) break;
        lastByteMs = millis();

        if (c == '\r' || c == '\n') {
            if (rxPos > 0) {
                rxBuf[rxPos] = '\0';
                uint8_t nextTail = (cmdQueueTail + 1) % CMD_QUEUE_SIZE;
                if (nextTail == cmdQueueHead) {
                    // queue full: drop oldest to make room
                    cmdQueueHead = (cmdQueueHead + 1) % CMD_QUEUE_SIZE;
                }
                strncpy(cmdQueue[cmdQueueTail], rxBuf, CMD_MAX_LEN - 1);
                cmdQueue[cmdQueueTail][CMD_MAX_LEN - 1] = '\0';
                cmdQueueTail = nextTail;
                rxPos = 0;
            }
            // else ignore consecutive terminators
        } else {
            if (rxPos < CMD_MAX_LEN - 1) {
                rxBuf[rxPos++] = (char)c;
            } else {
                // overflow: reset
                rxPos = 0;
            }
        }
    }

    // If partial data exists but no terminator arrived and timeout expired -> enqueue
    if (rxPos > 0 && (millis() - lastByteMs) > INTERBYTE_TIMEOUT_MS) {
        rxBuf[rxPos] = '\0';
        uint8_t nextTail = (cmdQueueTail + 1) % CMD_QUEUE_SIZE;
        if (nextTail == cmdQueueHead) {
            cmdQueueHead = (cmdQueueHead + 1) % CMD_QUEUE_SIZE;
        }
        strncpy(cmdQueue[cmdQueueTail], rxBuf, CMD_MAX_LEN - 1);
        cmdQueue[cmdQueueTail][CMD_MAX_LEN - 1] = '\0';
        cmdQueueTail = nextTail;
        rxPos = 0;
    }
}

void processQueuedCommands()
{
    while (cmdQueueHead != cmdQueueTail) {
        char cmd[CMD_MAX_LEN];
        strcpy(cmd, cmdQueue[cmdQueueHead]);
        cmdQueueHead = (cmdQueueHead + 1) % CMD_QUEUE_SIZE;

#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
        Serial.print("[CMD Q] ");
        Serial.println(cmd);
#endif

        // Use Marcduino's command processor with the global player
        Marcduino::processCommand(player, cmd);
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

#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
    // Ensure USB Serial initialized for debug logs
    Serial.begin(115200);
    delay(10);
#endif

    Wire.begin();

    COMMAND_SERIAL.begin(9600);
    // Disable marcduinoSerial reading from the hardware serial since we
    // now use a dedicated poll+queue mechanism to avoid lost/fragmented
    // bytes when logging or processing is blocking.
    marcduinoSerial.setStream(nullptr);

    SetupEvent::ready();
    
   // Wire.setClock(400000); //Set i2c frequency to 400 kHz.

    randomSeed(analogRead(3));

    // servoDispatch.setOutputEnablePin(OUTPUT_ENABLED_PIN, true);
    //servoDispatch.setClockCalibration((const uint32_t[]) { 27570000, 27190000 });
    //servoDispatch.setServosEasingMethod(HOLO_SERVOS_MASK, Easing::CircularEaseIn);


    frontHolo.assignServos(&servoDispatch, 0, 1);
    topHolo.assignServos(&servoDispatch, 2, 3);
    rearHolo.assignServos(&servoDispatch, 4, 5);
  
    PSI_COM.begin(2400);

#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
    DEBUG_PRINTLN("ready.."); 
#endif


    FLD.selectScrollTextLeft("R2 D2-\n-by Doc", LogicEngineRenderer::kBlue, 1, 5);
    RLD.selectScrollTextLeft("... RSeries Doc Snyder ....", LogicEngineRenderer::kYellow, 0, 3);

    //CommandEvent::process(F("HPF104"));  
    //servoDispatch.moveTo(0, 150, 1000, 1.0);  
    
    CommandEvent::process(F("HPA199"));  //Twitch for all
    
   //CommandEvent::process(F("HPS9|45"));

    CommandEvent::process("MP20005");
    //servoDispatch.setServosEasingMethod(ALL_DOME_PANELS_MASK, Easing::BounceEaseOut);
    //SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK);

    //servoDispatch.setServosEasingMethod(HOLO_SERVOS_MASK, Easing::CircularEaseIn);
    SEQUENCE_PLAY_ONCE_SPEED(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK, 2000);

    //PSI_COM.print("0T2\r");

}



void loop()
{
    // Fast, non-blocking serial receive and queuing
    pollSerial1AndQueue();

    // Process queued commands (can be longer-running)
    processQueuedCommands();

    // Regular processing
    AnimatedEvent::process();
    
    DomeButton();


}

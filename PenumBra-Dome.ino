#define USE_DEBUG 
//#define DEBUG_SERIAL
//#define USE_SERVO_DEBUG
//#define USE_HOLO_DEBUG

//Penumbra Mega PIN Mapping
//SV1-12 2-13
//Dout 1-8  22-29

//SERIAL 3 for PSI Pro

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

  
 */

#include "ReelTwo.h"
#include "core/Animation.h"
#include "core/DelayCall.h"
//#include "ServoDispatchDirect.h"
#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"
#include "core/Marcduino.h"
#include "core/JawaCommander.h"
#include "dome/HoloLights.h"
#include "dome/Logics.h"   //HACK to switch PINs to different Position  FRONT 29 REAR 28 
#include "dome/MagicPanel.h"   /// PIN 8 DATA | PIN 7 CLK | PIN 6 CS

#define COMMAND_SERIAL Serial1 //   Serial1 for LIVE 

#define PSI_COM Serial3 //  serial for PSI Pro
#define DOME_BTN_L A0
#define DOME_BTN_R A1


#define HOLO_HSERVO        0x1000
#define HOLO_VSERVO        0x2000

#define HOLO_SERVOS_MASK        (HOLO_HSERVO|HOLO_VSERVO)

const ServoSettings servoSettings[] PROGMEM = {

    //* PIN, closed 0, Open 1, GRoupe//

    { 1, 800, 1200, HOLO_HSERVO },  /* 0: horizontal front holo */
    { 2, 800, 1200, HOLO_VSERVO },  /* 1: vertical front holo */
    { 3, 800, 1200, HOLO_HSERVO },  /* 2: horizontal top holo */
    { 4, 800, 1200, HOLO_VSERVO },  /* 3: vertical top holo */
    { 5, 800, 1200, HOLO_VSERVO },  /* 4: vertical rear holo */
    { 6, 800, 1200, HOLO_HSERVO },  /* 5: horizontal rear holo */
};

//ServoDispatchDirect<SizeOfArray(servoSettings)> servoDispatch(servoSettings);

MagicPanel magicPanel;
ServoDispatchPCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);
AnimationPlayer player(servoSequencer);
JawaCommander<> jawaCommander;
MarcduinoSerial<> marcduinoSerial(COMMAND_SERIAL, player);


HoloLights frontHolo(22,HoloLights::kRGB, HoloLights::kFrontHolo,12);        // PIN Dout1
HoloLights rearHolo(23, HoloLights::kRGB, HoloLights::kRearHolo);         // PIN Dout2
HoloLights topHolo(24, HoloLights::kRGB, HoloLights::kTopHolo);          // PIN Dout3

//HoloDisplay frontHolo(HoloLightHoloLights::kRGBW, HoloLights::kTopHolos::kRGBW, HoloLights::kFrontHolo);
//HoloLights rearHolo(HP_REAR_LED_PIN, HoloLights::kRGBW, HoloLights::kRearHolo);
//HoloLights topHolo(HP_TOP_LED_PIN, HoloLights::kRGB, HoloLights::kTopHolo, 12);

//LogicEngineDeathStarFLDInverted<> FLD(LogicEngineFLDDefault);
//LogicEngineDeathStarRLDInverted<> RLD(LogicEngineRLDDefault);



// Front Logic Device (Jawa ID#1)
LogicEngineDeathStarFLD<> FLD(LogicEngineFLDDefault, 1);
// Rear Logic Device (Jawa ID#2)
LogicEngineDeathStarRLDInverted<> RLD(LogicEngineRLDDefault, 2);


void resetSequence()
{
   // Marcduino::send(F("$s"));
    CommandEvent::process(F( 
        "LE000000|0\n"
        "FSOFF\n"
        "BMOFF\n"
        "HPA000|0\n"
        "CB00000\n"
        "DP00000\n"));

      //CommandEvent::process(F("HPA000|0"));
}


#include "MarduinoHolo.h"
#include "MarcduinoSequence.h"
#include "MarcduinoMagicPanel.h"

#include "DomeButton.h"


void setup()
{
    pinMode(DOME_BTN_L, INPUT_PULLUP);
    pinMode(DOME_BTN_R, INPUT_PULLUP);

    REELTWO_READY();
    Wire.begin();
    //Wire.setClock(400000); //Set i2c frequency to 400 kHz.

    randomSeed(analogRead(3));

    // servoDispatch.setOutputEnablePin(OUTPUT_ENABLED_PIN, true);
    servoDispatch.setClockCalibration((const uint32_t[]) { 27570000, 27190000 });
    //servoDispatch.setServosEasingMethod(HOLO_SERVOS_MASK, Easing::CircularEaseIn);


    frontHolo.assignServos(&servoDispatch, 0, 1);
    topHolo.assignServos(&servoDispatch, 2, 3);
    rearHolo.assignServos(&servoDispatch, 4, 5);



    COMMAND_SERIAL.begin(9600);
    PSI_COM.begin(2400);
    SetupEvent::ready();

    DEBUG_PRINTLN("ready.."); 


    FLD.selectScrollTextLeft("R2\n    D2", LogicEngineRenderer::kBlue, 1, 15);
    RLD.selectScrollTextLeft("... RSeries Doc Snyder ....", LogicEngineRenderer::kYellow, 0, 15);

    //CommandEvent::process(F("HPF104"));  
    //servoDispatch.moveTo(0, 150, 1000, 1.0);  
    CommandEvent::process(F("HPA199"));  //Twitch for all
   //CommandEvent::process(F("HPS9|45"));

    CommandEvent::process("MP20005");

    //PSI_COM.print("0T2\r");


/*  Magic Panel Commands:
kNormal   
kSolid  
kToggle   
kFlash  
kAlert  
kHorizontalScan   
kVerticalScan   
kLife   
kExpandSolid  
kCollapseSolid  
kExpandHollow   
kCollapseHollow   
kForwardQ   
kReverseQ 
#### PSI PRO Commands : ###
Mode 0 - Turn Panel off (This will also turn stop the Teeces if they share the serial connection and the "0" address is used)
Mode 1 - Default (Swipe) The default mode can be changed on the config.h tab
Mode 2 - Flash (fast flash) (4 seconds) Use caution around those sensitive to flashing lights.
Mode 3 - Alarm (slow flash) (4 seconds)
Mode 4 - Short Circuit (10 seconds)
Mode 5 - Scream (4 seconds)
Mode 6 - Leia Message (34 seconds)
Mode 7 - I heart U (10 seconds)
Mode 8 - Quarter Panel sweep (7 seconds)
Mode 9 - Flashing Red Heart (Front PSI), Pulse Monitor (Rear PSI)
Mode 10 - Star Wars - Title Scroll (15 seconds)
Mode 11 - Imperial March (47 seconds)
Mode 12 - Disco Ball (4 seconds)
Mode 13 - Disco Ball -Runs Indefinitely
Mode 14 - Rebel Symbol (5 seconds)
Mode 15 - Knight Rider (20 seconds)
Mode 16 - Test Sequence (White on Indefinitely)
Mode 17 - Red on Indefinitely
Mode 18 - Green on Indefinitely
Mode 19 - Light Saber Battle
Mode 20 - Star Wars Intro (scrolling yellow "text" getting smaller and dimmer)
Mode 21 - VU Meter (4 seconds)
Mode 92 - VU Meter -Runs Indefinitely (Spectrum on Teeces)

The RSeries use a command input string to tell it what effects combination should be run. This is comprised of a 8 digit long int.

TLEECSNN
T - the text message designator - if not provided defaults 0 (first message)

0 - Astromech
1 - Astromech
2 - Excuse me sir, but that R2-D2 is in prime condition, a real bargain.
3 - That malfunctioning little twerp.
4 - The city's central computer told me.
5 - Beep
6 - Beep-bee-bee-boop-bee-doo-weep
7 - R2-D2
8 - Beep Boop
9 - Bite my shiny metal ... Beep ... Boop ...
L - the logic designator - if not provided, defaults to 0 (all)

0 - All
1 - Front
2 - Rear
EE - the effect - use two digits if logic designator provided

00 - Normal
01 - Alarm - flips between color and red with mic effects
02 - Failure - cycles colors and brightness fading - roughly timed to 128 screa-3.mp3
03 - Leia - pale green with mic effects
04 - March - sequence timed to Imperial March
05 - Single Color - single hue shown
06 - Flashing Color - single hue on and off
07 - Flip Flop Color - boards flip back and forth - similar to march
08 - Flip Flop Alt - other direction of flips on back board, front is same to flip flop
09 - Color Swap - switches between color specified and inverse compliment color
10 - Rainbow - rotates through colors over time
11 - Red Alert - shows color specified based on mic input
12 - Mic Bright - brightness of color specified back on mic input
13 - Mic Rainbow - color goes from default specified through color range based on mic input
14 - Lights Out - turns off displays
15 - Static Text
16 - Text Scrolling Left
17 - Text Scrolling Right
18 - Text Scrolling Up
19 - Roaming Pixel (pixel roams from top left to bottom right - for testing)
20 - Horizontal Scanline
21 - Vertical Scanline
22 - Fire
99 - Select Random Effect
00 - Reset to Normal
C - color designator

1 - Red
2 - Orange
3 - Yellow
4 - Green
5 - Cyan (Aqua)
6 - Blue
7 - Purple
8 - Magenta
9 - Pink
0 - Default color on alarm / default to red on many effects / color cycle on march / ignored on failure and rainbow
S - speed or sensitivity (1-9 scale) with 5 generally considered default for speed

Flip Flop and Rainbow - 200ms x speed
Flash - 250ms x speed
March - 150ms x speed
Color Swap - 350ms x speed
Red Alert - sets mic sensitivity - as a fraction of speed / 10 - we recommend 3
Mic Bright - sets minimum brightness - fraction of speed / 10
NN - 2 digit time length in seconds

00 for continuous use on most
00 for default length on Leia
Not used on March or Failure
Some sequence examples:

Note: Leading 0s drop off as these are long ints
Solid Red: 51000
Solid Orange: 52000
Solid Yellow: 53000
Solid Green: 54000
Solid Cyan: 55000
Solid Blue: 56000
Solid Purple: 57000
Solid Magenta: 58000
Solid Pink: 59000
Alarm (default): 10500
Failure: 20000
Leia: 30000
March: 40500
March (Red Only): 41500
Flash (Yellow): 63500
Color Swap (pink): 99500
Rainbow: 100500
Red Alert: 111300
Mic Bright (Green): 124200
Mic Rainbow (Cyan): 135000
Fire: 220000
Text Scroll Left (Cyan): 40165118 (message #4 "The city's central computer told me." for 18 seconds)

54008 - solid green for 8 seconds
63315 - flashing yellow at slightly higher speed for 15 seconds
30008 - leia effect for only 8 seconds 


*/


}  /*END SETUP */



void loop()
{
    AnimatedEvent::process();
    DomeButton();


}

#define USE_DEBUG 
#define DEBUG_SERIAL
#define USE_SERVO_DEBUG
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

  
 */

#include "ReelTwo.h"
#include "core/Animation.h"
#include "core/DelayCall.h"
//#include "ServoDispatchDirect.h"
#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"
#include "core/Marcduino.h"

#include "dome/HoloLights.h"

//#define FRONT_LOGIC_PIN 29
//#define REAR_LOGIC_PIN 28

#include "dome/Logics.h"   //HACK to switch PINs to different Position  FRONT 29 REAR 28 
#include "dome/MagicPanel.h"   /// PIN 8 DATA | PIN 7 CLK | PIN 6 CS

#define COMMAND_SERIAL Serial //   Serial1 for LIVE 

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

//ServoDispatchDirect<SizeOfArray(servoSettings)> servoDispatch(servoSettings);

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
//#LogicEngineDeathStarFLD<> FLD(LogicEngineFLDDefault, 1);

// Rear Logic Device (Jawa ID#2)
//#LogicEngineDeathStarRLDInverted<> RLD(LogicEngineRLDDefault, 2);


//SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK);

void resetSequence()
{
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


//#include "MarduinoHolo.h"
#include "MarcduinoSequence.h"
//#include "MarcduinoMagicPanel.h"
#include "MarcduinoPanel.h"
//#include "DomeButton.h"




void setup()
{
    pinMode(DOME_BTN_L, INPUT_PULLUP);
    pinMode(DOME_BTN_R, INPUT_PULLUP);

    REELTWO_READY();
    Wire.begin();

    COMMAND_SERIAL.begin(9600);

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
    
 

    DEBUG_PRINTLN("ready.."); 


 //#   FLD.selectScrollTextLeft("R2 D2-\n-by Doc", LogicEngineRenderer::kBlue, 1, 5);
 //#   RLD.selectScrollTextLeft("... RSeries Doc Snyder ....", LogicEngineRenderer::kYellow, 0, 3);

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


/*    
 *     #### Magic Panel Commands:
 *   
0   kNormal   
1   kSolid  
2   kToggle   
3   kFlash  
4   kAlert  
5   kHorizontalScan   
6   kVerticalScan   
7   kLife   
8   kExpandSolid  
9   kCollapseSolid  
10  kExpandHollow   
11  kCollapseHollow   
12  kForwardQ   
13  kReverseQ 

#### PSI PRO Commands : ###
 Sample PSI_COM.print("0T19\r")  // Light Saber Battle

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



### Holo Commands ###
## - the Sequence Value including leading zero if necessary, ie sequence 3 is 03

C - (Optional), the Color integer value from list below:
    Basic Color Integer Values
        1 = Red
        2 = Yellow
        3 = Green
        4 = Cyan (Aqua)
        5 = Blue
        6 = Magenta
        7 = Orange
        8 = Purple
        9 = White
        0 = Random

S - (Optional), Speed setting integer for the Dim Pulse LED function below (0-9)

R - (Optional), Random State for clearing LED displays
    Random State Integer Values
        1 = Use Default Sequences
        2 = Use Random Sequences

P - (Optional), the Position integer value from list below:
    Preset Position Integer Values
        0 = Down
        1 = Center
        2 = Up
        3 = Left
        4 = Upper Left
        5 = Lower Left
        6 = Right
        7 = Upper Right
        8 = Lower Right

D001    - Leia Sequence, Random shades of blue to mimic Leia Hologram
D002C   - Color Projector Sequence, Like Leia above but using color command value
D003CS  - Dim Pulse Sequence, Color slowly pulses on and off
D004C   - Cycle Sequence, using color command value
D005C   - Toggles Color, Simply sets LEDs tp solid color value
D006    - Rainbow Sequence
D007C   - Short Circuit, Led flashes on and off with interval slowing over time
D096    - Clears LED, Disables Auto LED Sequence & "Off Color"
D0971   - Clears LED, Enables Auto LED Sequence,Enables Default Sequences, Disables "Off Color"
D0972   - Clears LED, Enables Auto LED Sequence,Enables Random Sequences, Disables "Off Color"
D098    - Clears LED, Disables Auto LED Sequence, enables "Off Color"
D0991   - Clears LED, Enables Auto LED Sequence,Enables Default Sequences, Enables "Off Color"
D0992   - Clears LED, Enables Auto LED Sequence,Enables Random Sequences, Enables "Off Color"

D101P   - Sends HP to a Preset Position
D102    - Enables RC Control on HP (Left/Right)
D103    - Enables RC Control on HP (Up/Down)
D104    - Sends HP to a Random Position
D105    - Wags HP Left/Right 5 times
D106    - Wags HP Up/Down 5 times
D198    - Disables Auto HP Twitch
D199    - Enables Auto HP Twitch

  S1    - Leia Mode (Front HP in Down Position, Leia LED Sequence, all other HPs disabled)*
  S2    - Play R2 Cartoon [OLED]
  S3    - Play Deathstar Plans movie [OLED]
  S4    - Clear all LEDs, Disable Auto HP Twitch, Disable Auto LED Sequence, Disables Off Color
  S5    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ default seqs.),
          Disables Off
  S9    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ random seqs.),
          Disables Off Color
  S7    - Clear all LEDs, Disable Auto HP Twitch, Disable Auto LED Sequence, Enables Off Color
  S8    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ default seqs.),
          Enables Off Color
  S9    - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence (w/ random seqs.),
          Enables Off Color

*/

DEBUG_PRINTLN("Debug Serial:");
DEBUG_PRINTLN(DEBUG_SERIAL);



}  /*END SETUP */




void loop()
{
    AnimatedEvent::process();
    
   // DomeButton();


}

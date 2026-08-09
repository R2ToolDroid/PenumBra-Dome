// Ensure USB Serial initialized for debug logs and make debug defines explicit

#ifndef DEBUG_SERIAL
// #define DEBUG_SERIAL
#endif

#ifndef USE_DEBUG
// #define USE_DEBUG
#endif

void setup()
{
    pinMode(DOME_BTN_L, INPUT_PULLUP);
    pinMode(DOME_BTN_R, INPUT_PULLUP);

    REELTWO_READY();

#if defined(DEBUG_SERIAL) || defined(USE_DEBUG)
    // Ensure USB Serial initialized for debug logs
    Serial.begin(115200);
    // small delay to allow Serial connection establishment
    delay(10);
#endif

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

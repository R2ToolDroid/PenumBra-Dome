////////////////


MARCDUINO_ACTION(StopSequence, :SE00, ({   
        resetSequence();
}))

MARCDUINO_ACTION(Mode0Sequence, mode0, ({   
        RLD.selectScrollTextLeft("... R2 in Mode 0 Random.", LogicEngineRenderer::kBlue, 0, 15);
        FLD.selectScrollTextLeft("MOD 0", LogicEngineRenderer::kBlue, 1, 15);
}))

MARCDUINO_ACTION(Mode1Sequence, mode1, ({   
        RLD.selectScrollTextLeft("... R2 in Mode 1 RC Control.", LogicEngineRenderer::kGreen, 0, 15);
        FLD.selectScrollTextLeft("MOD 1", LogicEngineRenderer::kGreen, 1, 15);
}))

MARCDUINO_ACTION(Mode2Sequence, mode2, ({   
        RLD.selectScrollTextLeft("... R2 in Mode 2 Human.", LogicEngineRenderer::kOrange, 0, 15);
        FLD.selectScrollTextLeft("MOD 2", LogicEngineRenderer::kOrange, 1, 15);
}))

MARCDUINO_ACTION(Mode3Sequence, mode3, ({   
        RLD.selectScrollTextLeft("... R2 in Mode 3 Service.", LogicEngineRenderer::kRed, 0, 15);
        FLD.selectScrollTextLeft("MOD 3", LogicEngineRenderer::kRed, 1, 15);
}))






MARCDUINO_ANIMATION (ScreamSequence, :SE01){
    DO_START()
    DO_COMMAND(F( 
      "HPA0031|5\n" //Puse All Red
      "HPA1012\n"   // Holos UP
      "LE10505\n"   //Logic Alarm 5 Sec
      "MP40005\n"   //Magic Panel Alarm 5 Sec
    ))
    DO_ONCE (
      PSI_COM.print("0T5\r");
      )
    
    DO_SEQUENCE(SeqPanelAllOpenClose, ALL_DOME_PANELS_MASK)
    //SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpenClose, ALL_DOME_PANELS_MASK);
    DO_END()
}

MARCDUINO_ACTION(WaveSequence, :SE02, ({

      CommandEvent::process(F( 
        "LE63305\n"  //Flash Yellow 5 second
        "HPA00325|5\n"    // HP Yellow
        "HPA199\n"      //Move all to RND Position
        "MP30005"));    /// Magic Flash 5 Sec
        
      PSI_COM.print("0T15\r");  ///Heard
      
      //CommandEvent::process("MP30005");
     // CommandEvent::process(F("HPA00305")); 
     // CommandEvent::process("LE10005");
    //Marcduino::send(F("$213"));
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelWave, ALL_DOME_PANELS_MASK);
}))

MARCDUINO_ACTION(SmirkWaveSequence, :SE03, ({
      PSI_COM.print("0T2\r");

      CommandEvent::process(F( 
        "LE65504\n"     //Flash Back and Front
        "HPA004|4\n"    // HP Yellow
        "HPA199\n"      //Move all to RND Position
        "MP30005"));    /// Magic Flash 5 Sec
      
      //CommandEvent::process("MP30005");
      //CommandEvent::process("LE70005");
      //CommandEvent::process(F("HPF0040"));

      
    //Marcduino::send(F("$34"));
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelWaveFast, ALL_DOME_PANELS_MASK);
}))

MARCDUINO_ANIMATION (Wave2Sequence, :SE04){
    DO_START()
    DO_SEQUENCE(SeqPanelAllOpenClose, ALL_DOME_PANELS_MASK)
    DO_COMMAND(F( 
      "HPA0031|5\n" //Puse All Red
      "HPA1012\n"   // Holos UP
      "LE63500\n"   //Logic Alarm 5 Sec
      "MP40005\n"   //Magic Panel Alarm 5 Sec
    ))
    DO_ONCE (
      PSI_COM.print("0T5\r");
      )
    DO_WAIT_SEC(5)
    
    DO_COMMAND(F( 
      "HPA0049\n" //Front Holo ON White
      "HPA199\n"   // Holos Random Pos
      "LE52003\n"   //Logic Alarm 5 Sec
      "MP50003\n"   //Magic Panel H SCan 5 Sec
    ))
    DO_WAIT_SEC(3)
    
    DO_RESET({
        resetSequence();
    })
       
    DO_END()
    
   // SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpenClose, ALL_DOME_PANELS_MASK)
}


MARCDUINO_ANIMATION(BeepCantinaSequence, :SE05)
{
    DO_START()
    //DO_SEQUENCE(SeqPanelLongDisco, ALL_DOME_PANELS_MASK)
    DO_SEQUENCE_VARSPEED(SeqPanelMarchingAnts, ALL_DOME_PANELS_MASK, 50, 100);
    
    DO_ONCE({
     PSI_COM.print("0T9\r"); ///Flashing Heard Ball
     
    })
    
    DO_COMMAND(F(
        // Fire logics
        "LE84115\n"
        "HPF106|5\n"
        "MP50015\n"
        // Holo Short Circuit
        "HPA004|15\n"
        ))
    
    // Wait 15 seconds
    DO_WAIT_SEC(60)
    DO_RESET({
      resetSequence();
    })
    DO_END()
}



MARCDUINO_ANIMATION(ShortSequence, :SE06)
{
    DO_START()
    DO_ONCE({
     PSI_COM.print("0T4\r"); //Short Circuit
    })
    // Logic engine alarm
    DO_COMMAND_AND_WAIT(F(
        "LE105000\n"
        "HPF0059\n"
        
        ),1000)
    // Play scream-3 and wait 500ms
    // DO_MARCDUINO_AND_WAIT(F("$623"), 500);
    // Logic engine failure
    DO_COMMAND(F(
        // Logic engine failure
        "LE20000\n"
        // Holo Short Circuit
        "HPA007|7\n"
        //Magic Panel Game of Live
        "MP70000\7n"
        // Fire strip spark for 1000ms
        //"FS11000\n"
        "HPA1010\n"
        // Charge Bay Indicator flicker for 6s
        //"CB20006\n"
        // Data Panel flicker for 6s
        //"DP20006\n"
        // Smoke on
        //"BMON\n"
        ))
    // Wait 3 seconds
    DO_WAIT_SEC(3)
    DO_ONCE({
     PSI_COM.print("0T0\r"); //off
    })
    
    DO_COMMAND(F(
        // Smoke off
        "MP80005\n"
        // Fire strip off
        "HPA1012\n"))
    // Wait 3 seconds
    DO_WAIT_SEC(3)   
    DO_COMMAND(F(
        // Charge Bay Indicator disabled for 8s
        //"CB10008\n"
        // Data Panel disabled for 8s
        "MP90005\n"
        // Holo off
        "HPA000|0\n"))
    DO_SEQUENCE_VARSPEED(SeqPanelAllOpenCloseLong, ALL_DOME_PANELS_MASK, 700, 900);
    // Fake being dead for 8 seconds
    DO_WAIT_SEC(8)
    // Ok We are back!
    DO_RESET({
        resetSequence();
    })
    DO_END()
}


MARCDUINO_ANIMATION(CantinaSequence, :SE07)
{
    DO_START()
    DO_SEQUENCE(SeqPanelDance, ALL_DOME_PANELS_MASK)
    DO_ONCE({
     PSI_COM.print("0T13\r");  // Disco Ball long  
        FLD.selectSequence(LogicEngineRenderer::RAINBOW);
        RLD.selectScrollTextLeft("STAR WARS R2-D2 ASTROMECH", LogicEngineRenderer::ColorVal(random(10)));
   
    })
    // Play Orchestral Cantina
    DO_COMMAND(F(
      "MP60000\n"
      "HPF00308\n"
    ))
    // Wait 1 second
    DO_WAIT_SEC(1)
    DO_COMMAND(F(
        // Disco Logics
        // "LE104146\n"
        // Holo Short Circuit
        "HPA006|46\n"))
    //DO_SEQUENCE(SeqPanelDance, ALL_DOME_PANELS_MASK)
    // Wait 46 seconds
    DO_WAIT_SEC(46)
    DO_RESET({
        resetSequence();
    })
    DO_END()
}

MARCDUINO_ANIMATION(LeiaMessage, :SE08)
{
    DO_START()
    //DO_MARCDUINO(F("$73"))

    DO_ONCE({
     PSI_COM.print("0T6\r"); 
    })
    DO_COMMAND_AND_WAIT(F(
        "LE30045\n"
        "HPS1|45\n"
        "MP130045\n"), 45000)
    DO_RESET({
        resetSequence();
    })
    DO_END()
}

MARCDUINO_ANIMATION(DiscoSequence, :SE09)
{
    DO_START()
    DO_ONCE({
     PSI_COM.print("0T19\r"); //Light Saber Battle
    })
    DO_COMMAND(F(
      "HPA0023\n"   // Color Projector Sequence in green
      "MP20000\n"
      "LE40500\n"
      ))
    DO_WAIT_SEC(10)
    //DO_MARCDUINO(F("$D"))
    DO_SEQUENCE(SeqPanelLongDisco, ALL_DOME_PANELS_MASK)
    DO_ONCE({
        FLD.selectSequence(LogicEngineRenderer::RAINBOW);
        RLD.selectScrollTextLeft("STAR WARS R2-D2 ASTROMECH", LogicEngineRenderer::ColorVal(random(10)));
    })
    DO_COMMAND_AND_WAIT(F(
        "HPS1|35\n"
        //"CB10000\n"
        "DP10000\n"), 35000)
    DO_RESET({
        resetSequence();
    })
    DO_END()
}

MARCDUINO_ACTION(Stop2Sequence, :SE10, ({   
        resetSequence();
}))

MARCDUINO_ANIMATION(FullAwakeSequence, :SE11)
{
    DO_START()
    DO_COMMAND(F(
        "MP30005\n"
        "HPA0059\n"
        "LE57004\n"
        "HPA104\n"
        ))
    
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
    ))
    DO_WAIT_SEC(1) 
    DO_COMMAND(F( 
      "HPA104\n" 
      //"HPA006\n"
    ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
      
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "HPA04\n"
      
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F(
       
      "HPA1011\n"
      "HPA0000\n"
      ))
   
    DO_END()
}

MARCDUINO_ACTION(MidAwakeSequence, :SE13, ({   
        resetSequence();
}))


MARCDUINO_ANIMATION(AwakePlusSequence, :SE14)
{
    DO_START()
    DO_COMMAND(F(
        "MP30005\n"
        "HPA0059\n"  /// Weiss ON
       
        //"LE60005\n"
        "HPA104\n"    ///
        ))
    
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
    ))
    DO_WAIT_SEC(1) 
    DO_COMMAND(F( 
      "HPA104\n" 
      //"HPA006\n"
    ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
      
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "HPA04\n"
      
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F(
       
      "HPA1011\n"
      "HPA0000\n"
      ))
   
    DO_END()
}

MARCDUINO_ANIMATION (ScreamNPSequence, :SE15){
    DO_START()
    DO_COMMAND(F( 
      "HPA0031|5\n" //Puse All Red
      "HPA1012\n"   // Holos UP
      "LE10505\n"   //Logic Alarm 5 Sec
      "MP40005\n"   //Magic Panel Alarm 5 Sec
    ))
    DO_ONCE (
      PSI_COM.print("0T5\r");
      )
      
   // DO_SEQUENCE(SeqPanelAllOpenClose, ALL_DOME_PANELS_MASK)
      
    DO_END()
    
   
}

MARCDUINO_ANIMATION (ScreamNPSSequence, :SE51){
    DO_START()
    DO_COMMAND(F( 
      "HPA0031|5\n" //Puse All Red
      "HPA1012\n"   // Holos UP
      "LE10505\n"   //Logic Alarm 5 Sec
      "MP40005\n"   //Magic Panel Alarm 5 Sec
    ))
    DO_ONCE (
      PSI_COM.print("0T5\r");
      )
    DO_SEQUENCE(SeqPanelAllFlutter, ALL_DOME_PANELS_MASK)
    DO_END()
    
    //SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpenClose, ALL_DOME_PANELS_MASK);
}


MARCDUINO_ANIMATION (WaveOneSequence, :SE52){
    DO_START()
    DO_SEQUENCE(SeqPanelOpenCloseWave, ALL_DOME_PANELS_MASK)
    DO_COMMAND(F( 
        "LE56000\n"  //Blue
        "HPA00388\n"    // HP Pink
        "HPA199\n"      //Move all to RND Position
        "MP60008"))   /// Magic Flash 5 Sec
    DO_ONCE (     
      PSI_COM.print("0T8\r");  ///Sweep
    )
    DO_WAIT_SEC (8)
    DO_RESET({
        resetSequence();
    })
    DO_END()
}

MARCDUINO_ANIMATION (FastSmirkSequence, :SE53){
    DO_START()
    DO_SEQUENCE(SeqPanelWaveFast, ALL_DOME_PANELS_MASK)
    DO_COMMAND(F( 
        "LE63503\n"     //Flash Back and Front
        "HPA0043\n"    // HP Yellow
        "HPA199\n"      //Move all to RND Position
        "MP30003"))   /// Magic Flash 5 Sec

    DO_ONCE (     
      PSI_COM.print("0T2\r");  ///Sweep
    )
    DO_WAIT_SEC (3)
    DO_RESET({
        resetSequence();
    })
    DO_END()
}

MARCDUINO_ANIMATION (WaveTwoSequence, :SE54){
    DO_START()
    DO_COMMAND(F( 
        "LE10505\n"  //Blue
        "HPA00385\n"    // HP Pink
        "HPA199\n"      //Move all to RND Position
        "MP50008"))   /// Magic Flash 5 Sec
    DO_ONCE (     
      PSI_COM.print("0T8\r");  ///Sweep
    )
    DO_SEQUENCE(SeqPanelWaveFast, ALL_DOME_PANELS_MASK)
    DO_WAIT_SEC (10)
    DO_RESET({
        resetSequence();
    })
    DO_END()
}

MARCDUINO_ANIMATION (MarchingAntsSequence, :SE55){
    DO_START()
    //SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelMarchingAnts, ALL_DOME_PANELS_MASK);
    DO_SEQUENCE(SeqPanelMarchingAnts, ALL_DOME_PANELS_MASK)
    DO_COMMAND(F( 
        "LE40505\n"  //Blue
        "HPA0040\n"    // HP 
        "HPA199\n"      //Move all to RND Position
        "MP50008"))   /// Magic Flash 5 Sec
    DO_ONCE (     
      PSI_COM.print("0T11\r");  ///Sweep
    )
    DO_WAIT_SEC (5)
    DO_COMMAND(F( 
        "LE41505\n"  //Blue
        "HPA00305\n"    // HP 
        "HPA199\n"      //Move all to RND Position
        "MP100005"))   /// Magic Flash 5 Sec
    DO_ONCE (     
      PSI_COM.print("0T21\r");  ///Sweep
    )

    DO_WAIT_SEC (5)
    
    
    DO_RESET({
        resetSequence();
    })
    DO_END()
}

MARCDUINO_ANIMATION(FaintShortSequence, :SE55)
{
    DO_START()
    DO_ONCE({
     PSI_COM.print("0T4\r"); //Short Circuit
    })
    // Logic engine alarm
    DO_COMMAND_AND_WAIT(F(
        "LE220000\n"
        "HPF0059\n"
        
        ),2000)
    // Play scream-3 and wait 500ms
    // DO_MARCDUINO_AND_WAIT(F("$623"), 500);
    // Logic engine failure
    DO_COMMAND(F(
        // Logic engine failure
        "LE20000\n"
        // Holo Short Circuit
        "HPA007|7\n"
        //Magic Panel Game of Live
        "MP70000\7n"
        // Fire strip spark for 1000ms
        //"FS11000\n"
        "HPA1010\n"
        // Charge Bay Indicator flicker for 6s
        //"CB20006\n"
        // Data Panel flicker for 6s
        //"DP20006\n"
        // Smoke on
        //"BMON\n"
        ))
    // Wait 3 seconds
    DO_WAIT_SEC(3)
    DO_COMMAND(F(
        // Smoke off
        "MP80005\n"
        // Fire strip off
        "HPA1012\n"))
    // Wait 3 seconds
    DO_WAIT_SEC(3)   
    DO_COMMAND(F(
        // Charge Bay Indicator disabled for 8s
        //"CB10008\n"
        // Data Panel disabled for 8s
        "MP90005\n"
        // Holo off
        "HPA000|0\n"))
    DO_SEQUENCE_VARSPEED(SeqPanelAllOpenCloseLong, ALL_DOME_PANELS_MASK, 700, 900);
    // Fake being dead for 8 seconds
    DO_WAIT_SEC(8)
    // Ok We are back!
    DO_RESET({
        resetSequence();
    })
    DO_END()
}


MARCDUINO_ANIMATION(RhytmicCantinaSequence, :SE57)
{
    DO_START()
    DO_COMMAND(F(
        "MP50000\n"
        "HPA0040\n"
        "LE40500\n"
        "HPA104\n"
        ))
    DO_SEQUENCE_VARSPEED(SeqPanelAllOpenCloseLong, ALL_DOME_PANELS_MASK, 700, 900);
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
    ))
    DO_WAIT_SEC(1) 
    DO_COMMAND(F( 
      "MP60000\n"
      "HPA104\n" 
      "HPA006\n"
      "LE99500\n"
    ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "LE63315\n"
      "HPA104\n"
      "MP40000\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
      
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "HPA04\n"
      
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F(
       
      "HPA1011\n"
      "HPA0000\n"
      
      ))
    DO_RESET({
        resetSequence();
    })
    DO_END()
}


////////////////  Weiter mit SE..
////////////////  Old Sequences will follow after that Line
////////////////

  
///////////////



////////////////



MARCDUINO_ANIMATION(ShortRandHolo, *RND)
{
  DO_START()

    DO_ONCE({
     PSI_COM.print("0T7\r"); //Short Circuit
    })
    
    DO_COMMAND(F(
        
        "HPA0040\n"
        //"LE20000\n"
        "HPA104\n"
        ))
    
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
    ))
    DO_WAIT_SEC(1) 
    DO_COMMAND(F( 
      "HPA104\n" 
      "HPA006\n"
    ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
      
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "HPA04\n"
      
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F(
       
      "HPA1011\n"
      "HPA0000\n"
      ))
   
    DO_END()
}

MARCDUINO_ANIMATION(Short1RandHolo, *RD00)
{
  DO_START()

    DO_ONCE({
     PSI_COM.print("0T7\r"); //Short Circuit
    })
    
    DO_COMMAND(F(
        
        "HPA0040\n"
        //"LE20000\n"
        "HPA104\n"
        ))
    
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
    ))
    DO_WAIT_SEC(1) 
    DO_COMMAND(F( 
      "HPA104\n" 
      "HPA006\n"
    ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPA104\n"
      ))
      
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "HPA04\n"
      
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F(
       
      "HPA1011\n"
      "HPA0000\n"
      ))
   
    DO_END()
}


MARCDUINO_ANIMATION(ShortARandHolo, *RD01)
{
  DO_START()

    
    
    DO_COMMAND(F(
        
        "HPF0059\n"
        //"LE20000\n"
        "HPF104\n"
        ))
    
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPF104\n"
    ))
    DO_WAIT_SEC(1) 
    DO_COMMAND(F( 
      "HPF104\n" 
      
    ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPF104\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPF104\n"
      ))
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "HPA104\n"
      //"HPF006\n"
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F( 
      "HPF104\n"
      ))
      
    DO_WAIT_SEC(3) 
    DO_COMMAND(F( 
      "HPF104\n"
      
      ))
    DO_WAIT_SEC(2) 
    DO_COMMAND(F(
       
      "HPF1011\n"
      "HPF0000\n"
      ))
   
    DO_END()
}



////////////////



////////////////



////////////////



////////////////

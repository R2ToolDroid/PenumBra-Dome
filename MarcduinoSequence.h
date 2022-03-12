////////////////


MARCDUINO_ACTION(StopSequence, :SE00, ({
  PSI_COM.print("0T1\r");
}))

////////////////

MARCDUINO_ACTION(ScreamSequence, :SE01, ({
    CommandEvent::process("LE10003"); ///Logic
    PSI_COM.print("0T5\r");
    //CommandEvent::process("MP20005");
    DEBUG_PRINTLN("ready..SE01"); 
    CommandEvent::process("MP40005");
    //Marcduino::send(F("$S"));
    CommandEvent::process(F("HPA105|5"));
    CommandEvent::process(F("HPA00305")); 
    
    //SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpenClose, ALL_DOME_PANELS_MASK);
}))

///////////////


MARCDUINO_ACTION(WaveSequence, :SE02, ({
      PSI_COM.print("0T15\r");  ///Heard
      CommandEvent::process("MP30005");
      CommandEvent::process(F("HPA00305")); 
      CommandEvent::process("LE10005");
    //Marcduino::send(F("$213"));
    //SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelWave, ALL_DOME_PANELS_MASK);
}))

////////////////

MARCDUINO_ACTION(SmirkWaveSequence, :SE03, ({
      PSI_COM.print("0T2\r");
      CommandEvent::process("MP30005");
      CommandEvent::process("LE70005");
      CommandEvent::process(F("HPF0040"));
    //Marcduino::send(F("$34"));
    //SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelWaveFast, ALL_DOME_PANELS_MASK);
}))

MARCDUINO_ANIMATION(ShortRandHolo, *RND)
{
  DO_START()
    DO_COMMAND(F(
        
        "HPA0040\n"
       "LE20000\n"
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





MARCDUINO_ANIMATION(BeepCantinaSequence, :SE05)
{
    DO_START()
    DO_ONCE({
     PSI_COM.print("0T13\r"); 
    })
    //DO_MARCDUINO(F("$c"))
    // Wait 1 second
    DO_COMMAND(F(
        // Fire logics
        "LE84115\n"
        "HPF106|5\n"
        "MP50015\n"
        // Holo Short Circuit
        "HPA002|15\n"))
    //DO_SEQUENCE(SeqPanelMarchingAnts, ALL_DOME_PANELS_MASK)
    // Wait 15 seconds
    DO_WAIT_SEC(15)
    DO_RESET({
        resetSequence();
    })
    DO_END()
}

////////////////

MARCDUINO_ANIMATION(ShortSequence, :SE06)
{
    DO_START()
    DO_ONCE({
     PSI_COM.print("0T4\r"); 
    })
    // Logic engine alarm
    DO_COMMAND(F("LE105000"))
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
    //DO_SEQUENCE_VARSPEED(SeqPanelAllOpenCloseLong, ALL_DOME_PANELS_MASK, 700, 900);
    // Fake being dead for 8 seconds
    DO_WAIT_SEC(8)
    // Ok We are back!
    DO_RESET({
        resetSequence();
    })
    DO_END()
}


////////////////

MARCDUINO_ANIMATION(CantinaSequence, :SE07)
{
    DO_START()
    DO_ONCE({
     PSI_COM.print("0T12\r"); 
    })
    // Play Orchestral Cantina
    ///DO_MARCDUINO(F("$C"))
    // Wait 1 second
    DO_WAIT_SEC(1)
    DO_COMMAND(F(
        // Disco Logics
        "LE104146\n"
        // Holo Short Circuit
        "HPA006|46\n"))
    //DO_SEQUENCE(SeqPanelDance, DOME_DANCE_PANELS_MASK)
    // Wait 46 seconds
    DO_WAIT_SEC(46)
    DO_RESET({
        resetSequence();
    })
    DO_END()
}

////////////////

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
        "CB10000\n"
        "DP10000\n"), 45000)
    DO_RESET({
        resetSequence();
    })
    DO_END()
}

////////////////

MARCDUINO_ANIMATION(DiscoSequence, :SE09)
{
    DO_START()
    DO_ONCE({
     PSI_COM.print("0T21\r"); 
    })
    //DO_MARCDUINO(F("$D"))
    //DO_SEQUENCE(SeqPanelLongDisco, DOME_DANCE_PANELS_MASK)
    DO_ONCE({
        FLD.selectSequence(LogicEngineRenderer::RAINBOW);
        RLD.selectScrollTextLeft("STAR WARS R2-D2 ASTROMECH", LogicEngineRenderer::ColorVal(random(10)));
    })
    DO_COMMAND_AND_WAIT(F(
        "HPS1|45\n"
        //"CB10000\n"
        "DP10000\n"), 45000)
    DO_RESET({
        resetSequence();
    })
    DO_END()
}

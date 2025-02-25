void parseCommand(String cmd) {

    Serial.println(cmd);

    if (cmd == "66") {
      resetSequence();
    }

    
    if (cmd == ":OP00") {
      servoDispatch.setServosEasingMethod(ALL_DOME_PANELS_MASK, Easing::CircularEaseOut);
            SEQUENCE_PLAY_ONCE_SPEED(servoSequencer, SeqPanelAllOpen, ALL_DOME_PANELS_MASK, 1000);
            
    //SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllOpen, ALL_DOME_PANELS_MASK);
    //RLD.selectScrollTextLeft("... RSeries LogicEngine ....", LogicEngineRenderer::kYellow, 0, 15);
    Serial.println("-OP00"); 
    }

    if (cmd == ":CL00") {
    
      //Marcduino::processCommand(player, "@4S3");
    //servoDispatch.setServosEasingMethod(HOLO_SERVOS_MASK, Easing::CircularEaseIn);
    SEQUENCE_PLAY_ONCE_SPEED(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK, 1000);
    //FLD.selectScrollTextLeft("R2\n D2", LogicEngineRenderer::kBlue, 1, 15);
    //Serial.print("-CL00");
    Serial.println("-CL00"); 
    
    }

    cmd = "";
  
}

////////////////

MARCDUINO_ACTION(HoloONN, 1, ({
    // Front Holo Dim cycle random color
    //CommandEvent::process(F("HPF0059"));  //weiss einschalten mit 9
    /*
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
    */
    
    CommandEvent::process(F("HPF0059"));
    

    //CommandEvent::process(F("HPA0992"));
}))

MARCDUINO_ACTION(HoloRes, 0, ({

    // Front Holo Dim cycle random color
   //frontHolo.moveHP(HoloLights::kUp, 500);
   CommandEvent::process(F("HPA198"));  //Alle Holos Auto twitch off
   resetSequence();
}))


MARCDUINO_ACTION(HoloTop, 8, ({

    // Front Holo Dim cycle random color
   frontHolo.moveHP(HoloLights::kUp, 500);
   CommandEvent::process(F("HPF007"));  //Short Circuit
}))

MARCDUINO_ACTION(HoloBottom, 2, ({
    // Front Holo Dim cycle random color
    frontHolo.moveHP(HoloLights::kDown, 500);
    CommandEvent::process(F("HPF001")); //Leia Sequence Wht blue
}))
MARCDUINO_ACTION(HoloRight, 6, ({
    // Front Holo Dim cycle random color
    frontHolo.moveHP(HoloLights::kRight, 500);
    CommandEvent::process(F("HPF006")); //roll slide
}))
MARCDUINO_ACTION(HoloLeft, 4, ({
    // Front Holo Dim cycle random color
    frontHolo.moveHP(HoloLights::kLeft, 500);
    CommandEvent::process(F("HPF00308"));  // Pulse mit 8 sec und 0 rnd COlor
    //003CS
}))

MARCDUINO_ACTION(HoloCenter, 5, ({
    // Front Holo Dim cycle random color
    frontHolo.moveHP(HoloLights::kCenter, 500);
}))

MARCDUINO_ACTION(HoloCen, 7, ({
    // Front Holo Dim cycle random color
    //frontHolo.moveHP(HoloLights::kCenter, 500);

    CommandEvent::process(F("HPF106|5"));  // Wag left Right  106 top down
}))

MARCDUINO_ACTION(AllHoloOn, *ON00, ({
    // Front Holo Dim cycle random color
    CommandEvent::process(F("HPA0040"));
}))

MARCDUINO_ACTION(FrontHoloOn, *ON01, ({
    // Front Holo Dim cycle random color
    CommandEvent::process(F("HPF0040"));
}))

////////////////

MARCDUINO_ACTION(AllHoloOff, *OF00, ({
    // Front Holo Off
    CommandEvent::process(F("HPA0000"));
}))


MARCDUINO_ACTION(FrontHoloOff, *OF01, ({
    // Front Holo Off
    CommandEvent::process(F("HPF0000"));
}))

////////////////

MARCDUINO_ACTION(RearHoloOn, *ON02, ({
    // Rear Holo Dim cycle random color
    CommandEvent::process(F("HPR0040"));
}))

////////////////

MARCDUINO_ACTION(RearHoloOff, *OF02, ({
    // Rear Holo Off
    CommandEvent::process(F("HPR0000"));
}))

////////////////

MARCDUINO_ACTION(TopHoloOn, *ON03, ({
    // Top Holo Dim cycle random color
    CommandEvent::process(F("HPT0040"));
}))

////////////////

MARCDUINO_ACTION(TopHoloOff, *OF03, ({
    // Top Holo Off
    CommandEvent::process(F("HPT0000"));
}))

////////////////

MARCDUINO_ACTION(ResetAllHolos, *ST00, ({
    // Reset all holos
    CommandEvent::process(F("HPA0000"));
}))

MARCDUINO_ACTION(AllHoloRandomMove, *RD00, ({
    // Front Holo Move Random
    CommandEvent::process(F("HPA199"));
    
}))

////////////////

MARCDUINO_ACTION(FrontHoloRandomMove, *RD01, ({
    // Front Holo Move Random
    CommandEvent::process(F("HPF199"));
    
}))


////////////////

MARCDUINO_ACTION(RearHoloRandomMove, *RD02, ({
    // Rear Holo Move Random
    CommandEvent::process(F("HPR104"));
}))

////////////////

MARCDUINO_ACTION(TopHoloRandomMove, *RD03, ({
    // Top Holo Move Random
    CommandEvent::process(F("HPT104"));
}))

////////////////

MARCDUINO_ACTION(AllHoloPulse, *HPS300, ({
    // Front Holo Dim pulse random color
    CommandEvent::process(F("HPA0030"));
}))

MARCDUINO_ACTION(FrontHoloPulse, *HPS301, ({
    // Front Holo Dim pulse random color
    CommandEvent::process(F("HPF0030"));
}))

////////////////

MARCDUINO_ACTION(RearHoloPulse, *HPS302, ({
    // Rear Holo Dim pulse random color
    CommandEvent::process(F("HPR0030"));
}))

////////////////

MARCDUINO_ACTION(TopHoloPulse, *HPS303, ({
    // Top Holo Dim pulse random color
    CommandEvent::process(F("HPT0030"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloRainbow, *HPS601, ({
    // Front Holo rainbow
    CommandEvent::process(F("HPF006"));
}))

////////////////

MARCDUINO_ACTION(RearHoloRainbow, *HPS602, ({
    // Rear Holo rainbow
    CommandEvent::process(F("HPR006"));
}))

////////////////

MARCDUINO_ACTION(TopHoloRainbow, *HPS603, ({
    // Top Holo rainbow
    CommandEvent::process(F("HPT006"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloPosDown, *HP001, ({
    // Front Holo Position Down
    CommandEvent::process(F("HPF1010"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloPosCenter, *HP101, ({
    // Front Holo Position Center
    CommandEvent::process(F("HPF1011"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloPosUp, *HP201, ({
    // Front Holo Position Up
    CommandEvent::process(F("HPF1012"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloPosLeft, *HP301, ({
    // Front Holo Position Left
    CommandEvent::process(F("HPF1013"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloPosUpperLeft, *HP401, ({
    // Front Holo Position Upper Left
    CommandEvent::process(F("HPF1014"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloPosLowerLeft, *HP501, ({
    // Front Holo Position Lower Left
    CommandEvent::process(F("HPF1015"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloPosRight, *HP601, ({
    // Front Holo Position Right
    CommandEvent::process(F("HPF1016"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloPosUpperRight, *HP701, ({
    // Front Holo Position Upper Right
    CommandEvent::process(F("HPF1017"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloPosLowerRight, *HP801, ({
    // Front Holo Position Lower Right
    CommandEvent::process(F("HPF1018"));
}))

////////////////

MARCDUINO_ACTION(RearHoloPosDown, *HP002, ({
    // Rear Holo Position Down
    CommandEvent::process(F("HPR1010"));
}))

////////////////

MARCDUINO_ACTION(RearHoloPosCenter, *HP102, ({
    // Rear Holo Position Center
    CommandEvent::process(F("HPR1011"));
}))

////////////////

MARCDUINO_ACTION(RearHoloPosUp, *HP202, ({
    // Rear Holo Position Up
    CommandEvent::process(F("HPR1012"));
}))

////////////////

MARCDUINO_ACTION(RearHoloPosLeft, *HP302, ({
    // Rear Holo Position Left
    CommandEvent::process(F("HPR1013"));
}))

////////////////

MARCDUINO_ACTION(RearHoloPosUpperLeft, *HP402, ({
    // Rear Holo Position Upper Left
    CommandEvent::process(F("HPR1014"));
}))

////////////////

MARCDUINO_ACTION(RearHoloPosLowerLeft, *HP502, ({
    // Rear Holo Position Lower Left
    CommandEvent::process(F("HPR1015"));
}))

////////////////

MARCDUINO_ACTION(RearHoloPosRight, *HP602, ({
    // Rear Holo Position Right
    CommandEvent::process(F("HPR1016"));
}))

////////////////

MARCDUINO_ACTION(RearHoloPosUpperRight, *HP702, ({
    // Rear Holo Position Upper Right
    CommandEvent::process(F("HPR1017"));
}))

////////////////

MARCDUINO_ACTION(RearHoloPosLowerRight, *HP802, ({
    // Rear Holo Position Lower Right
    CommandEvent::process(F("HPR1018"));
}))

////////////////

MARCDUINO_ACTION(TopHoloPosDown, *HP003, ({
    // Top Holo Position Down
    CommandEvent::process(F("HPT1010"));
}))

////////////////

MARCDUINO_ACTION(TopHoloPosCenter, *HP103, ({
    // Top Holo Position Center
    CommandEvent::process(F("HPT1011"));
}))

////////////////

MARCDUINO_ACTION(TopHoloPosUp, *HP203, ({
    // Top Holo Position Up
    CommandEvent::process(F("HPT1012"));
}))

////////////////

MARCDUINO_ACTION(TopHoloPosLeft, *HP303, ({
    // Top Holo Position Left
    CommandEvent::process(F("HPT1013"));
}))

////////////////

MARCDUINO_ACTION(TopHoloPosUpperLeft, *HP403, ({
    // Top Holo Position Upper Left
    CommandEvent::process(F("HPT1014"));
}))

////////////////

MARCDUINO_ACTION(TopHoloPosLowerLeft, *HP503, ({
    // Top Holo Position Lower Left
    CommandEvent::process(F("HPT1015"));
}))

////////////////

MARCDUINO_ACTION(TopHoloPosRight, *HP603, ({
    // Top Holo Position Right
    CommandEvent::process(F("HPT1016"));
}))

////////////////

MARCDUINO_ACTION(TopHoloPosUpperRight, *HP703, ({
    // Top Holo Position Upper Right
    CommandEvent::process(F("HPT1017"));
}))

////////////////

MARCDUINO_ACTION(TopHoloPosLowerRight, *HP803, ({
    // Top Holo Position Lower Right
    CommandEvent::process(F("HPT1018"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloWag, *HW01, ({
    // Front Holo Wags Left/Right 5 times
    CommandEvent::process(F("HPF105|5"));
}))

////////////////

MARCDUINO_ACTION(RearHoloWag, *HW02, ({
    // Rear Holo Wags Left/Right 5 times
    CommandEvent::process(F("HPR105|5"));
}))

////////////////

MARCDUINO_ACTION(TopHoloWag, *HW03, ({
    // Top Holo Wags Left/Right 5 times
    CommandEvent::process(F("HPT105|5"));
}))

////////////////

MARCDUINO_ACTION(FrontHoloNod, *HN01, ({
    // Front Holo Nods Up/Down 5 times
    CommandEvent::process(F("HPF106|5"));
}))

////////////////

MARCDUINO_ACTION(RearHoloNod, *HN02, ({
    // Rear Holo Nods Up/Down 5 times
    CommandEvent::process(F("HPR106|5"));
}))

////////////////

MARCDUINO_ACTION(TopHoloNod, *HN03, ({
    // Top Holo Nods Up/Down 5 times
    CommandEvent::process(F("HPT106|5"));
}))

////////////////

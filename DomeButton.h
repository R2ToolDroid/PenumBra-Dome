Button BtnL(A0);       // define the button
Button BtnR(A1);       // define the button

//ezButton buttonL(A0);  // create ezButton object
//ezButton buttonR(A1);  // create ezButton object

DomeButton(){
  
    BtnL.read();               // read the button

    if (BtnL.wasReleased())    // if the button was released, change the LED state
    {
         Serial.println("  --  released "); 
    }
   
   
  
  }

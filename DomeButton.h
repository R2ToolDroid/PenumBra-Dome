
byte BtnR_Pressed = false;
byte BtnL_Pressed = false;
byte BtnBooth_Pressed = false;

byte BtnR = false;
byte BtnL = false;
byte BtnBooth = false;
int countL = 0;
int countR = 0;
unsigned long BtnTA, BtnTB;
const long trig = 100;



void MakeCommand(int row, int col){

          if (row == 0){
              ///PSI Events
              switch (col) {

                case 1:
                
                DEBUG_PRINTLN("PSI "); 
                PSI_COM.print("0T1\r");
                
                break;

                case 2:
                DEBUG_PRINTLN("PSI 2"); 
                PSI_COM.print("0T2\r");
                break;

                case 3:
                DEBUG_PRINTLN("PSI 3");
                PSI_COM.print("0T3\r");
                break;   

                case 4:
                DEBUG_PRINTLN("PSI 4");
                PSI_COM.print("0T4\r");
                break;

                case 5:
                DEBUG_PRINTLN("PSI 5");
                PSI_COM.print("0T5\r");
                break;

                case 6:
                DEBUG_PRINTLN("PSI 6");
                PSI_COM.print("0T6\r");
                break;

                case 7:
                DEBUG_PRINTLN("PSI 7");
                PSI_COM.print("0T7\r");
                break;

                case 8:
                DEBUG_PRINTLN("PSI 8");
                PSI_COM.print("0T8\r");
                break;

                case 9:
                DEBUG_PRINTLN("PSI 9");
                PSI_COM.print("0T9\r");
                break;

                case 10:
                DEBUG_PRINTLN("PSI 10");
                PSI_COM.print("0T10\r");
                break;

                case 11:
                DEBUG_PRINTLN("PSI 11");
                PSI_COM.print("0T11\r");
                break;

                case 12:
                DEBUG_PRINTLN("PSI 12");
                PSI_COM.print("0T12\r");
                break;

                case 13:
                DEBUG_PRINTLN("PSI 13");
                PSI_COM.print("0T13\r");
                break;

                case 14:
                DEBUG_PRINTLN("PSI 14");
                PSI_COM.print("0T14\r");
                break;

                case 15:
                DEBUG_PRINTLN("PSI 15");
                PSI_COM.print("0T15\r");
                break;
                
                case 16:
                DEBUG_PRINTLN("PSI 16");
                PSI_COM.print("0T16\r");
                break;

                case 17:
                DEBUG_PRINTLN("PSI 17");
                PSI_COM.print("0T17\r");
                break;

                case 18:
                DEBUG_PRINTLN("PSI 18");
                PSI_COM.print("0T18\r");
                break;

                case 19:
                DEBUG_PRINTLN("PSI 19");
                PSI_COM.print("0T19\r");
                break;

                case 20:
                DEBUG_PRINTLN("PSI 20");
                PSI_COM.print("0T20\r");
                break;

                case 21:
                DEBUG_PRINTLN("PSI 21");
                PSI_COM.print("0T21\r");
                break;
                
                }
             
          }

          if (row == 1) {
            //Command Magic panel
                switch (col) {

                case 0:
                DEBUG_PRINTLN("com 1SE01"); 
                break;

                case 1:
                //DEBUG_PRINTLN("com 2"); 
                CommandEvent::process("MP00000");
                
                break;

                case 2:
                CommandEvent::process("MP20005");
                break;  

                case 3:
                CommandEvent::process("MP30005");
                break; 

                case 4:
                CommandEvent::process("MP40005");
                break;

                case 5:
                CommandEvent::process("MP50005");
                break;

                case 6:
                CommandEvent::process("MP60005");
                break;

                case 7:
                CommandEvent::process("MP70005");
                break;

                case 8:
                CommandEvent::process("MP80005");
                break;

                case 9:
                CommandEvent::process("MP90005");
                break;

                case 10:
                CommandEvent::process("MP100005");
                break;

                case 11:
                CommandEvent::process("MP110005");
                break;

                case 12:
                CommandEvent::process("MP120005");
                break;

                case 13:
                CommandEvent::process("MP130005");
                break;

                case 14:
                CommandEvent::process("MP140005");
                break;



                 
                }
            
          }

          
     
  
}



void CheckBtnState(){


    if ( (analogRead(DOME_BTN_L) <= 60) && (analogRead(DOME_BTN_R) <= 60) && ( BtnBooth_Pressed == false) ){
      Serial.println(" Button Booth pressed");
      BtnBooth = true;
      BtnBooth_Pressed = true;
     
      } 
    if ( (analogRead(DOME_BTN_L) >= 60) && (analogRead(DOME_BTN_R) >= 60) && ( BtnBooth_Pressed == true) ){
      Serial.println(" Button Booth released");
      BtnBooth = false;
      BtnBooth_Pressed = false;
     
      } 
    
  
    
    if ((analogRead(DOME_BTN_R) <= 60) && ( BtnR_Pressed == false))
      {
      //Serial.println(" Button Right pressed");
      BtnR = true;             
      BtnR_Pressed = true;
    } 
    
    if ((analogRead(DOME_BTN_R) >= 60)&& ( BtnR_Pressed == true))
    {
      //Serial.println(" Button Right relased");
      BtnR == false;
      BtnR_Pressed = false;
    }

    if ((analogRead(DOME_BTN_L) <= 60) && ( BtnL_Pressed == false))
      {
      //Serial.println(" Button Left pressed");
      BtnL = true; //for true           
      BtnL_Pressed = true;
    } 
    
    if ((analogRead(DOME_BTN_L) >= 60)&& ( BtnL_Pressed == true))
    {
      //Serial.println(" Button Left relased");
      BtnL == false;  //before false
      BtnL_Pressed = false;
    }
    
    
    

     /* 
    Serial.print (" Status: ");
    Serial.print (BtnR);
    Serial.print (" Btn State: ");
    Serial.println (BtnR_Pressed);
    */
      

}

void DomeButton(){

  BtnTA = millis();

  // if (BtnTA - BtnTB >= trig) {

      BtnTB = BtnTA;
      CheckBtnState();

      

      if (BtnBooth){
        Serial.println("---EXECUTE----");
        Serial.print("Data = L: ");
        Serial.print(countL);
        Serial.print(" - R: ");
        Serial.print(countR);

        Serial.print(" Comando = ");
        //Serial.print(Commando [countL] [countR]);

        MakeCommand(countL,countR);
        //Marcduino::send(F(":SE01"));
        
        countL = 0;
        countR = 0;

        ///Auflösen
        BtnBooth = false;
        
        BtnR = false;
        BtnL = false;
       
      } 

      if ( (BtnR == true) && ( BtnBooth == false)){
        
        countR++;
        Serial.print("Right = ");
        Serial.println(countR);
        BtnR = false;

        
      }

      if (BtnL){
        countL++;
        Serial.print("Left = ");
        Serial.println(countL);
        BtnL = false;     
      }
      
      

     // } //delay
}

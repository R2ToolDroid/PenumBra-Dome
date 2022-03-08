
byte BtnR_UP true;
byte BtnL_UP true;

byte BtnR = false;
byte BtnL = false;
byte BtnBooth = false;
int mode = 0;
unsigned long BtnTA, BtnTB;
const long trig = 100;


void CheckBtnState(){



    if ( (analogRead(DOME_BTN_L) <= 60) && (analogRead(DOME_BTN_R) <= 60)){
      Serial.println(" Button Booth pressed");
      BtnBooth = true;


    } else {
      BtnBooth = false;
    }

    if (analogRead(DOME_BTN_R) <= 60){
      Serial.println(" Button Right pressed");
      BtnR = true;

    } else {
      BtnR = false;
    }

    if (analogRead(DOME_BTN_L) <= 60){
      Serial.println(" Button Left pressed");
      BtnR = true;

    } else {
      BtnR = false;
    }



}

void DomeButton(){

  BtnTA = millis();

   if (BtnTA - BtnTB >= trig) {

      BtnTB = BtnTA;
      CheckBtnState();

      if (BtnR){
        mode++;
        Serial.print("Mode= ");
        Serial.println(mode);
      }

      if (BtnL){
        Serial.println("---EXECUTE----");

      }

      if (BtnBooth){
        Serial.print(" - RESET - ");
        Serial.print("Mode= ");
        Serial.println(mode);
        mode = 0;
      }
      //Serial.print(analogRead(DOME_BTN_R));
      //Serial.print(" - ");
      //Serial.println(analogRead(DOME_BTN_L));

      } //delay
}
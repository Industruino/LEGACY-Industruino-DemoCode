#include <EEPROM.h>

/*
* Industruino Demo Code - Default code loaded onto Industruino REV3
*
* Copyright (c) 2013 Loic De Buck <contact.industruino@gmail.com>
*
* Industruino is a DIN-rail mountable Arduino Leonardo compatible product
* Please visit www.industruino.com for further information and code examples.
* Standard peripherals connected to Industruino are:
* PCD8544 compatible LCD; rst:D19 dc:D20 dn:D21 sclk:D22 (set pin configuration in PCD8544 library header) 
* 3-button membrane panel; analog pin A5


/*
 * PCD8544 - Interface with Philips PCD8544 (or compatible) LCDs.
 *
 * Copyright (c) 2010 Carlos Rodrigues <cefrodrigues@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal`
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */



#include <PCD8544.h>
//Download libary from http://code.google.com/p/pcd8544/


// A custom glyph (a smiley)...
static const byte glyph[] = { B00010000, B00110100, B00110000, B00110100, B00010000 };


static PCD8544 lcd;

//menu defines

int coll = 0;
int channel = 0;
int lastChannel = 0;

int currentMenuDepth = 0;
int currentSubMenu = 0;
int currentMenuBranch = 0;
int channelLimit = 5;
int channelLowLimit = 0;

int valueEditing = 0;
int row = 0;
int constrainEnc = 1;
float valueEditingInc = 0;
float TargetValue = 0;

int changeValue = 0;

int buttonUpState = 0;
int buttonEnterState = 0;
int buttonDownState = 0;

int prevBtnEnt = 0;
int prevBtnUp = 0;
int prevBtnDown = 0;

int lastBtnEnt = LOW;
int lastBtnUp = LOW;
int lastBtnDown = LOW;

int enterPressed = 0;
int rowselector = 0;

int transEntInt = 250;
int transInt = 100;
unsigned long lastAdminActionTime = 0;

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = A5;  // Analog input pin that the potentiometer is attached to
const int backlightPin = 13; // Analog output pin that the LED is attached to
const int D0 = 0;
const int D1 = 1;
const int D2 = 2;
const int D3 = 3;
const int D4 = 4;
const int D5 = 5;
const int D6 = 6;
const int D7 = 7;
const int D8 = 8;
const int D9 = 9;
const int D10 = 10;
const int D11 = 11;
const int D12 = 12;
const int D14 = 14;
const int D15 = 15;
const int D16 = 16;
const int D17 = 17;


int sensorValue = 0;        // value read from the pot
int backlightIntensity = 0;
int backlightIntensityDef = 0;
int BacklightTimeOut = 0;
unsigned long GUIupdateInterval = 0;

void setup() {
  backlightIntensity = EEPROM.read(0);
  SetInput();
  pinMode(backlightPin, OUTPUT);
  analogWrite(backlightPin, (map(backlightIntensity, 0, 5, 255, 0)));
  
  
  //digitalWrite(D3, LOW);
  // PCD8544-compatible displays may have a different resolution...
  lcd.begin(84, 48);
  
  // Add the smiley to position "0" of the ASCII table...
  lcd.createChar(0, glyph);
   Serial.begin(9600); 
  // while (!Serial) {
  // }
  
 MenuSplash();

}



void loop() {
  
  ReadButtons();
  DrawMenu();
  delay(50);
}


void MenuSplash() {
  channel = 0;
  channelLimit = 0;
  channelLowLimit = 0;
  currentSubMenu = 0;
  currentMenuDepth = 0;
  enterPressed = 0;
  lcd.clear();
  lcd.setCursor(5, 2);
  lcd.print("Industruino");
}

void MenuMain() {
  channel = 0;
  channelLimit = 2;
  currentSubMenu = 1;
  currentMenuDepth = 1;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  lcd.setCursor(6, 0);
  lcd.print("Setup");
  lcd.setCursor(6, 1);
  lcd.print("Demo");
  lcd.setCursor(6, 2);
  lcd.print("Back");
  
}

void MenuSetup() {
 channel = 0;
  channelLimit = 2;
  channelLowLimit = 0;
  currentSubMenu = 2;
  currentMenuDepth = 2;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  lcd.setCursor(6, 0);
  lcd.print("BackLight     ");
  lcd.setCursor(65, 0);
  lcd.print(backlightIntensity,1);
  lcd.setCursor(6, 1);
  lcd.print("Reset param.");
  lcd.setCursor(6, 2);
  lcd.print("Back");
}

void MenuParametersReset() {
  channel = 6;
  channelLimit = 5;
  channelLowLimit = 4;
  currentSubMenu = 10;
  currentMenuDepth = 3;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  lcd.setCursor(6, 0);
  lcd.print("Set system");
  lcd.setCursor(6, 1);
  lcd.print("to default");
  lcd.setCursor(6, 2);
  lcd.print("settings?");
  lcd.setCursor(6, 4);
  lcd.print("OK?");
  lcd.setCursor(6, 5);
  lcd.print("Cancel");
  
}

void MenuDemo() {
  channel = 0;
  channelLimit = 3;
  channelLowLimit = 0;
  currentSubMenu = 3;
  currentMenuDepth = 2;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  lcd.setCursor(6, 0);
  lcd.print("DigitalOut");
  lcd.setCursor(6, 1);
  lcd.print("DigitalIn");
  lcd.setCursor(6, 2);
  lcd.print("AnalogIn");
  lcd.setCursor(6, 3);
  lcd.print("Back");
}



void MenuDigitalOut1() {
  SetOutput();
  channel = 0;
  channelLimit = 6;
  channelLowLimit = 0;
  currentSubMenu = 1;
  currentMenuDepth = 3;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  lcd.setCursor(6, 0);
  lcd.print("D0");
  lcd.setCursor(6, 1);
  lcd.print("D1");
  lcd.setCursor(6, 2);
  lcd.print("D2");
  lcd.setCursor(6, 3);
  lcd.print("D3");
  lcd.setCursor(6, 4);
  lcd.print("D4");
  lcd.setCursor(6, 5);
  lcd.print("D5");
}

void MenuDigitalOut2() {
  channel = 0;
  channelLimit = 6;
  channelLowLimit = -1;
  currentSubMenu = 2;
  currentMenuDepth = 3;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  lcd.setCursor(6, 0);
  lcd.print("D6");
  lcd.setCursor(6, 1);
  lcd.print("D7");
  lcd.setCursor(6, 2);
  lcd.print("D8");
  lcd.setCursor(6, 3);
  lcd.print("D9");
  lcd.setCursor(6, 4);
  lcd.print("D10");
  lcd.setCursor(6, 5);
  lcd.print("D11");

}

void MenuDigitalOut3() {
  channel = 0;
  channelLimit = 5;
  channelLowLimit = -1;
  currentSubMenu = 3;
  currentMenuDepth = 3;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  lcd.setCursor(6, 0);
  lcd.print("D12");
  lcd.setCursor(6, 1);
  lcd.print("D14");
  lcd.setCursor(6, 2);
  lcd.print("D15");
  lcd.setCursor(6, 3);
  lcd.print("D16");
  lcd.setCursor(6, 4);
  lcd.print("D17");
  lcd.setCursor(6, 5);
  lcd.print("Back");
}


void MenuDigitalIn1() {
  SetInput();
  channel = 0;
  channelLimit = 1;
  channelLowLimit = 0;
  currentSubMenu = 4;
  currentMenuDepth = 3;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  MenuDigitalIn1Live();
}

void MenuDigitalIn1Live() {
  lcd.setCursor(6, 0);
  lcd.print("D0  ");
  lcd.print(digitalRead(0));
  lcd.setCursor(6, 1);
  lcd.print("D1  ");
  lcd.print(digitalRead(1));
  lcd.setCursor(6, 2);
  lcd.print("D2  ");
  lcd.print(digitalRead(2));
  lcd.setCursor(6, 3);
  lcd.print("D3  ");
  lcd.print(digitalRead(3));
  lcd.setCursor(6, 4);
  lcd.print("D4  ");
  lcd.print(digitalRead(4));
  lcd.setCursor(6, 5);
  lcd.print("D5  ");
  lcd.print(digitalRead(5));


}

void MenuDigitalIn2() {
  channel = 0;
  channelLimit = 1;
  channelLowLimit = -1;
  currentSubMenu = 5;
  currentMenuDepth = 3;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  MenuDigitalIn2Live();
}

void MenuDigitalIn2Live() {
  lcd.setCursor(6, 0);
  lcd.print("D6  ");
  lcd.print(digitalRead(6));
  lcd.setCursor(6, 1);
  lcd.print("D7  ");
  lcd.print(digitalRead(7));
  lcd.setCursor(6, 2);
  lcd.print("D8  ");
  lcd.print(digitalRead(8));
  lcd.setCursor(6, 3);
  lcd.print("D9  ");
  lcd.print(digitalRead(9));
  lcd.setCursor(6, 4);
  lcd.print("D10 ");
  lcd.print(digitalRead(10));
  lcd.setCursor(6, 5);
  lcd.print("D11 ");
  lcd.print(digitalRead(11));
}


void MenuDigitalIn3() {
  lcd.clear();
  channel = 5;
  channelLimit = 5;
  channelLowLimit = 4;
  currentSubMenu = 6;
  currentMenuDepth = 3;
  enterPressed = 0;
  ScrollMenu();
  MenuDigitalIn2Live();
}

void MenuDigitalIn3Live() {
  lcd.setCursor(6, 0);
  lcd.print("D12 ");
  lcd.print(digitalRead(12));
  lcd.setCursor(6, 1);
  lcd.print("D14 ");
  lcd.print(digitalRead(14));
  lcd.setCursor(6, 2);
  lcd.print("D15 ");
  lcd.print(digitalRead(15));
  lcd.setCursor(6, 3);
  lcd.print("D16 ");
  lcd.print(digitalRead(16));
  lcd.setCursor(6, 4);
  lcd.print("D17 ");
  lcd.print(digitalRead(17));
  lcd.setCursor(6, 5);
  lcd.print("Back   ");
}


void AnalogOut1() {
  channel = 0;
  channelLimit = 6;
  channelLowLimit = 0;
  currentSubMenu = 8;
  currentMenuDepth = 3;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  lcd.setCursor(6, 0);
  lcd.print("D0");
  lcd.setCursor(6, 1);
  lcd.print("D1");
  lcd.setCursor(6, 2);
  lcd.print("D3");
  lcd.setCursor(6, 3);
  lcd.print("D5");
  lcd.setCursor(6, 4);
  lcd.print("D6");
  lcd.setCursor(6, 5);
  lcd.print("D7");
}

void MenuAnalogIn1() {
  SetInput();
  channel = 0;
  channelLimit = 1;
  channelLowLimit = 0;
  currentSubMenu = 7;
  currentMenuDepth = 3;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  MenuAnalogIn1Live();
}

void MenuAnalogIn1Live() {
  lcd.setCursor(6, 0);
  lcd.print("A0           ");
  lcd.setCursor(30, 0);
  lcd.print(analogRead(0));
  lcd.setCursor(6, 1);
  lcd.print("A6           ");
  lcd.setCursor(30, 1);
  lcd.print(analogRead(6));
  lcd.setCursor(6, 2);
  lcd.print("A7           ");
  lcd.setCursor(30, 2);
  lcd.print(analogRead(7));
  lcd.setCursor(6, 3);
  lcd.print("A8           ");
  lcd.setCursor(30, 3);
  lcd.print(analogRead(8));
  lcd.setCursor(6, 4);
  lcd.print("A9           ");
  lcd.setCursor(30, 4);
  lcd.print(analogRead(9));
  lcd.setCursor(6, 5);
  lcd.print("A10          ");
  lcd.setCursor(30, 5);
  lcd.print(analogRead(10));
}


void MenuAnalogIn2() {
  SetInput();
  channel = 1;
  channelLimit = 1;
  channelLowLimit = 0;
  currentSubMenu = 8;
  currentMenuDepth = 3;
  enterPressed = 0;
  lcd.clear();
  ScrollMenu();
  MenuAnalogIn2Live();
}

void MenuAnalogIn2Live() {
  lcd.setCursor(6, 0);
  lcd.print("A11          ");
  lcd.setCursor(30, 0);
  lcd.print(analogRead(11));
  lcd.setCursor(6, 1);
  lcd.print("Back   ");
}


void DrawMenu()
{
  if (valueEditing != 1){
  Navigate();
  }
}


void Navigate()
{
  
  if (currentMenuDepth == 0)
  {
      {
      if (enterPressed == 1) MenuMain(); 
      }
  }
  
   if (currentMenuDepth == 1){
    
    if (channel == 0 && enterPressed == 1) MenuSetup(); 
    if (channel == 1 && enterPressed == 1) MenuDemo();
    if (channel == 2 && enterPressed == 1) MenuSplash(); 
     }
  
    
    
    if (currentMenuDepth == 2){
      
      if (currentSubMenu == 2){
        if (channel == 0 && enterPressed == 1)
       {
        TargetValue = backlightIntensity;
        backlightIntensity = EditValue();
        analogWrite(backlightPin, (map(backlightIntensity, 0, 5, 255, 0)));
        EEPROM.write(0, backlightIntensity);
        
       }
        if (channel == 1 && enterPressed == 1) MenuParametersReset();
        if (channel == 2 && enterPressed == 1) MenuDemo();   
        }   
      if (currentSubMenu == 3){
        if (channel == 0 && enterPressed == 1) MenuDigitalOut1(); 
        if (channel == 1 && enterPressed == 1) MenuDigitalIn1(); 
        if (channel == 2 && enterPressed == 1) MenuAnalogIn1(); 
        if (channel == 3 && enterPressed == 1) MenuMain(); 
        }
       }
     
    if (currentMenuDepth == 3){
      if (currentSubMenu == 1){
        if (channel == 0 && buttonEnterState == HIGH) digitalWrite(D0, HIGH); 
        if (channel == 0 && buttonEnterState == LOW) digitalWrite(D0, LOW); 
        if (channel == 1 && buttonEnterState == HIGH) digitalWrite(D1, HIGH); 
        if (channel == 1 && buttonEnterState == LOW) digitalWrite(D1, LOW); 
        if (channel == 2 && buttonEnterState == HIGH) digitalWrite(D2, HIGH); 
        if (channel == 2 && buttonEnterState == LOW) digitalWrite(D2, LOW); 
        if (channel == 3 && buttonEnterState == HIGH) digitalWrite(D3, HIGH); 
        if (channel == 3 && buttonEnterState == LOW) digitalWrite(D3, LOW); 
        if (channel == 4 && buttonEnterState == HIGH) digitalWrite(D4, HIGH); 
        if (channel == 4 && buttonEnterState == LOW) digitalWrite(D4, LOW);
        if (channel == 5 && buttonEnterState == HIGH) digitalWrite(D5, HIGH); 
        if (channel == 5 && buttonEnterState == LOW) digitalWrite(D5, LOW);
        if (channel == 6) MenuDigitalOut2();
        } 
      if (currentSubMenu == 2){
        if (channel == 0 && buttonEnterState == HIGH) digitalWrite(D6, HIGH); 
        if (channel == 0 && buttonEnterState == LOW) digitalWrite(D6, LOW); 
        if (channel == 1 && buttonEnterState == HIGH) digitalWrite(D7, HIGH); 
        if (channel == 1 && buttonEnterState == LOW) digitalWrite(D7, LOW); 
        if (channel == 2 && buttonEnterState == HIGH) digitalWrite(D8, HIGH); 
        if (channel == 2 && buttonEnterState == LOW) digitalWrite(D8, LOW); 
        if (channel == 4 && buttonEnterState == HIGH) digitalWrite(D9, HIGH); 
        if (channel == 3 && buttonEnterState == LOW) digitalWrite(D9, LOW); 
        if (channel == 4 && buttonEnterState == HIGH) digitalWrite(D10, HIGH); 
        if (channel == 4 && buttonEnterState == LOW) digitalWrite(D10, LOW); 
        if (channel == 5 && buttonEnterState == HIGH) digitalWrite(D11, HIGH); 
        if (channel == 5 && buttonEnterState == LOW) digitalWrite(D11, LOW); 
        if (channel == 6) MenuDigitalOut3();
        if (channel == -1) MenuDigitalOut1();
        }
      if (currentSubMenu == 3){
        if (channel == 0 && buttonEnterState == HIGH) digitalWrite(D12, HIGH); 
        if (channel == 0 && buttonEnterState == LOW) digitalWrite(D12, LOW);
        if (channel == 1 && buttonEnterState == HIGH) digitalWrite(D14, HIGH); 
        if (channel == 1 && buttonEnterState == LOW) digitalWrite(D14, LOW); 
        if (channel == 2 && buttonEnterState == HIGH) digitalWrite(D15, HIGH); 
        if (channel == 2 && buttonEnterState == LOW) digitalWrite(D15, LOW); 
        if (channel == 3 && buttonEnterState == HIGH) digitalWrite(D16, HIGH); 
        if (channel == 3 && buttonEnterState == LOW) digitalWrite(D16, LOW); 
        if (channel == 4 && buttonEnterState == HIGH) digitalWrite(D17, HIGH); 
        if (channel == 4 && buttonEnterState == LOW) digitalWrite(D17, LOW);
        if (channel == 5 && enterPressed == 1) MenuDemo(); 
        if (channel == -1) MenuDigitalOut2();
        }
      if (currentSubMenu == 4){
        if ((millis()-GUIupdateInterval) > 300){
        MenuDigitalIn1Live();
        GUIupdateInterval = millis();
        }
        if (channel == 1) MenuDigitalIn2();
        }
      if (currentSubMenu == 5){
        if ((millis()-GUIupdateInterval) > 300){
        MenuDigitalIn2Live();
        GUIupdateInterval = millis();
        }
        if (channel == 1) MenuDigitalIn3();
        if (channel == -1) MenuDigitalIn1();
        }
      if (currentSubMenu == 6){
        if ((millis()-GUIupdateInterval) > 300){
        MenuDigitalIn3Live();
        GUIupdateInterval = millis();
        }
        if (channel == 4) MenuDigitalIn2();
        if (channel == 5 && enterPressed == 1) MenuDemo(); 
        }    
      if (currentSubMenu == 7){
        if ((millis()-GUIupdateInterval) > 300){
        MenuAnalogIn1Live();
        GUIupdateInterval = millis();
        }
        if (channel == 1) MenuAnalogIn2();
        }   
      if (currentSubMenu == 8){
        if ((millis()-GUIupdateInterval) > 300){
        MenuAnalogIn2Live();
        GUIupdateInterval = millis();
        }
        if (channel == 0) MenuAnalogIn1();
        if (channel == 1 && enterPressed == 1) MenuDemo(); 
        }   
        
        
      if (currentSubMenu == 10){
        if (channel == 4 && enterPressed == 1) ResetParameters();
        if (channel == 5 && enterPressed == 1) MenuSetup(); 
        } 
    
       }
       
  
 if (channel != lastChannel && valueEditing != 1 && currentSubMenu != 0){
  ScrollMenu();
  }
}


void ScrollMenu()
{
  lastChannel = channel;
  for (int i=0; i <= 6; i++){
      lcd.setCursor(coll, i);
      lcd.print(" ");
   } 
  lcd.setCursor(coll, channel);
  lcd.print(">");
 
  }
  
  
  float EditValue()
{
  row = channel;
  channel = 0;
  constrainEnc = 0;
  valueEditingInc = 1;
  valueEditing = 1;
  enterPressed = 0;
  while (enterPressed != 1){
  ReadButtons();
  if (channel != lastChannel){
       if (channel < lastChannel && TargetValue <=4){
         TargetValue+= valueEditingInc;
       }
       if (channel > lastChannel && TargetValue > 0){
         TargetValue-= valueEditingInc ;
       }
       
      for (int i=60; i <= 70; i++){
      lcd.setCursor(i, row);
      lcd.print("   ");
       }
       
       lcd.setCursor(66, row);
       Serial.println(TargetValue);
       lcd.print(TargetValue, 0);
       lastChannel = channel;
       }  
  delay(50);
  }  
  channel = row;
  constrainEnc = 1;
  channelLimit = 2;
  valueEditing = 0;
  enterPressed = 0;
  return TargetValue;
}
  




void ReadButtons(){ 
  
  sensorValue = analogRead(analogInPin);  
 
   if (sensorValue>400 && sensorValue< 700 )
  {
  buttonEnterState = HIGH;
  }
  else
  {
  buttonEnterState = LOW;
  }
  
    if (sensorValue>100 && sensorValue<400)
  {
   buttonUpState = HIGH;
  }
  else
  {
  buttonUpState = LOW;
  }
   
   
    if (sensorValue>700 && sensorValue<1000 )
  {
  buttonDownState = HIGH;
  }
  else
  {
  buttonDownState = LOW;
  }
  
  
  
  
      if (buttonEnterState == HIGH && prevBtnEnt == LOW)
      {
         if ((millis() - lastBtnEnt) > transEntInt) 
         {
         enterPressed = 1;
         }
         lastBtnEnt = millis();
         lastAdminActionTime = millis();
         //Serial.println("Enter Pressed");
       }
      prevBtnEnt = buttonEnterState;
      
      
           if (buttonUpState == HIGH && prevBtnUp == LOW)
      {
         if ((millis() - lastBtnUp) > transInt) 
         {
         channel--;
         }
         lastBtnUp = millis();
         lastAdminActionTime = millis();
         //Serial.println("Up Pressed");
       }
      prevBtnUp = buttonUpState;
      
      
           if (buttonDownState == HIGH && prevBtnDown == LOW)
      {
         if ((millis() - lastBtnDown) > transInt) 
         {
         channel++;
         }
         lastBtnDown = millis();
         lastAdminActionTime = millis();
         //Serial.println("Down Pressed");
       }
      prevBtnDown = buttonDownState;
      
       if (constrainEnc == 1){
   channel = constrain(channel, channelLowLimit, channelLimit);
   }
      
 }
 
 
 void SetOutput(){
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT); 
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT); 
  pinMode(D6, OUTPUT); 
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);   
  pinMode(D9, OUTPUT); 
  pinMode(D10, OUTPUT);
  pinMode(D11, OUTPUT);
  pinMode(D12, OUTPUT);
  pinMode(D14, OUTPUT); 
  pinMode(D15, OUTPUT);  
  pinMode(D16, OUTPUT);
  pinMode(D17, OUTPUT); 
 }
 
void SetInput(){
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);  
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT); 
  pinMode(D6, INPUT); 
  pinMode(D7, INPUT);
  pinMode(D8, INPUT);  
  pinMode(D9, INPUT); 
  pinMode(D10, INPUT);
  pinMode(D11, INPUT);
  pinMode(D12, INPUT);
  pinMode(D14, INPUT); 
  pinMode(D15, INPUT);  
  pinMode(D16, INPUT);
  pinMode(D17, INPUT); 
 }
 
 void ResetParameters(){
   
   
   backlightIntensity = backlightIntensityDef;
   analogWrite(backlightPin, (map(backlightIntensity, 0, 5, 255, 0)));
   EEPROM.write(0, backlightIntensity);
   MenuSetup();
 }

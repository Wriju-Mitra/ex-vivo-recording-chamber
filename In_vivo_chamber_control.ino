/*Code for controlling optogenetic stimulation and monitoring temperature
 * in a recording chamber built to record Ca2+ signals in explanted Drosophila brains
 * The Arduino controls the brightness and frequency of LED stimulation
 * Temp of the saline is recorded. Control of saline temp is to be achieved through heating/cooling the 
 * reservoir.
 * 
 * 
 * Steinhart-Hart coefficients were determined using the web-based tool Thermistor Calculator at https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html
 * 
 * 
 * 
 * MIT License

Copyright (c) 2026 Wriju Mitra and Kamakshi Singh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


 * 02.06.25
 * Chouhan Lab
 * TIFR Mumbai
 * 
 */


#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27,16,2);

int sensorPin =A0;
int PotentiometerPin =A1;
int LEDPin = 13;
int buzzerPin = 12;
int ButtonPin = 3;
//int interruptPin = 2;    //Uncomment this and comment out the previous line if using the BNC Trigger input for switching on stimulation
int V0=0;
float R1 = 10000; //For 10k (fixed)resistor change to appropiate value if using a different resistor
float R2,logR2, T, Tcel;   //May the lord forgive me!
float A = 0.3518829716e-03, B = 3.624399442e-04, C= -4.255586818e-07;  //experimentally determined Steinhart-Hart coefficients

volatile bool State = 0;
//volatile byte UpdateTimeFlag = false;

bool LEDState = HIGH;                  ////default is high since we are using a PNP transistor

unsigned long TimePeriod;
int freq=0;

static unsigned long lastInterruptTime = 0;
 

void setup() 
{
 // Serial.begin(9600);       //Comment out once done

  pinMode(LEDPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(LEDPin, HIGH);              //default is high since we are using a PNP transistor
  digitalWrite(buzzerPin, LOW);
  pinMode(ButtonPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ButtonPin), Trigger, LOW);

  
  lcd.init();
  delay(100);
  lcd.backlight();
  WelcomeMessage();

 
  
  
  
}

void loop() 
{
  
  DisplayTemperature();                    //Display temp and frequency while running

  TimePeriod = CalculateTimePeriod();     //function returns time period

  delay(100);    
 

         while(State)
        {
          digitalWrite(buzzerPin, HIGH);

          digitalWrite(LEDPin, LOW);
          delay(TimePeriod/2);
          digitalWrite(LEDPin, HIGH); 
          delay(TimePeriod/2);
    
        }
  
         digitalWrite(buzzerPin, LOW);
 
 
  lcd.clear();
}


void DisplayTemperature()
{ 
  V0 = analogRead(sensorPin);
  R2 = R1 * (1023.0 / (float)V0 - 1.0);
  logR2 =log(R2);
  T = (1.0/(A + B*logR2 + C*pow(logR2,3)));   //Steinhart-Hart equation
  Tcel = T - 273.15;
  lcd.setCursor(0,0);
  lcd.print("Temp= ");
  //lcd.print(T);
  //lcd.print("K");
  //lcd.print(" (");
  lcd.print(Tcel);
  lcd.print("C");

  lcd.setCursor(0,1);
  lcd.print("Frequency = ");
  lcd.print(freq);
  lcd.print("Hz");
  
  
  /*Serial.print("Resistance");
  Serial.println(R2);
  Serial.print("Temperature =");
  Serial.print(Tcel);

  //Also display Freq in another line
  */
  
}

void WelcomeMessage()
{

String message1 = "Frequency and intensity control for Ex-vivo Stimulation                ";
String message2 = "https://github.com/Wriju-Mitra/ex-vivo-recording-chamber                ";
String message3 = "MIT License Copyright (c) 2026 Wriju & Kamakshi                ";

lcd.setCursor(0,0);
lcd.print("HELLO"); // first row statis
 delay(500);
 
//scrolling within the second row only
 for(int i = 0; i < message1.length(); i++) 
 {

 lcd.setCursor(0,1);
 lcd.print(message1.substring(i,i+16));
 delay(300);
}
 lcd.clear();



for(int i = 0; i < message2.length(); i++) 
 {
 //lcd.clear();
 lcd.setCursor(0,0);
 lcd.print(message2.substring(i,i+16));
 
 lcd.setCursor(0,1);
 lcd.print(message3.substring(i,i+16));
 delay(300);
}
 lcd.clear();



 //Print Welcome Message
}


unsigned long CalculateTimePeriod()
{
  freq = map(analogRead(PotentiometerPin), 0, 1023, 50, 0);
  unsigned long t=(1000/freq);
  return t;
}


void Trigger()
{

  if(millis() - lastInterruptTime < 50)
    return;            //do nothing as this is probably a bounce


  else
  {
  State=!State;
  lastInterruptTime = millis();
  }

}

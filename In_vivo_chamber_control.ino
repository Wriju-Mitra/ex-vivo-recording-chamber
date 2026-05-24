/*Code for controlling optogenetic stimulation and monitoring temperature
 * in a recording chamber built to record Ca2+ signals in explanted Drosophila brains
 * The Arduino controls the brightness and frequency of LED stimulation
 * Temp of the saline is recorded. Control of saline temp is to be achieved through heating/cooling the 
 * reservoir.
 * 
 * 
 * Steinhart-Hart coefficients were determined using the web-based tool Thermistor Calculator at https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html
 */


#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27,16,2);

int sensorPin =A0;
int LEDPin = 13;
int buzzerPin = 12;
int interruptPin = 2;
//int interruptPin = 3;    //Uncomment this and comment out the previous line if using the BNC Trigger input for switching on stimulation
int V0=0;
float R1 = 10000; //For 10k (fixed)resistor change to appropiate value if using a different resistor
float R2,logR2, T, Tcel;   //May the lord forgive me!
float A = 0.3518829716e-03, B = 3.624399442e-04, C= -4.255586818e-07;  //experimentally determined Steinhart-Hart coefficients

volatile byte State = 0;
volatile byte UpdateTimeFlag = false;

byte LEDState = HIGH;                  ////default is high since we are using a PNP transistor

unsigned long TimePeriod;

//unsigned long interval;
 

void setup() 
{
  Serial.begin(9600);       //Comment out once done
  lcd.init();
  delay(100);
  WelcomeMessage();

  pinMode(LEDPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(LEDPin, HIGH);              //default is high since we are using a PNP transistor
  digitalWrite(buzzerPin, LOW);
  pinMode(interruptPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interruptPin), Trigger, LOW);

  
  
  
}

void loop() 
{
  
  DisplayTemperature();

  CalculateTimePeriod();

  digitalWrite(buzzerPin, State);        //Toggles buzzer on/off

  unsigned long StartTime = 0;

  if(UpdateTimeFlag)                        //Updates StartTime only once per button press. Can't do this inside ISR
  {
    StartTime = millis();
    UpdateTimeFlag = false;
    
  }

  if(State)
  {
   if(millis() - StartTime > (TimePeriod/2))
   {
    LEDState = !LEDState;
    digitalWrite(LEDPin, LEDState); 

    StartTime = StartTime + TimePeriod/2;     //Update Time by half cycle(time elapsed)

    }

/*
    if(millis() - StartTime > (TimePeriod/2))
   {
    LEDState = !LEDState;
    digitalWrite(LEDPin, LEDState); 

    StartTime = millis();     //Update Time by half cycle

    }

*/

   }
 
}


void DisplayTemperature()
{ 
  V0 = analogRead(sensorPin);
  R2 = R1 * (1023.0 / (float)V0 - 1.0);
  logR2 =log(R2);
  T = (1.0/(A + B*logR2 + C*pow(logR2,3)));   //Steinhart-Hart equation
  Tcel = T - 273.15;
 /* lcd.setCursor(0,0);
  lcd.print("Temperature = ");
  lcd.print(T);
  lcd.print("K");
  lcd.print(" (");
  lcd.print(Tcel);
  lcd.print("C)");
  */
  Serial.print("Resistance");
  Serial.println(R2);
  Serial.print("Temperature =");
  Serial.print(Tcel);
  
  
}

void WelcomeMessage()
{

String message = "Frequency and intensity control for Ex-vivo Stimulation";
lcd.setCursor(0,0);
 lcd.print("HELLO");
//scrolling within the second row
 for(int i = o; i < message.length(); i++) {
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print(message.substring(i,i+16))
 delay(100);
 
}
 lcd.clear(); 

 //Print Welcome Message
}


void CalculateTimePeriod()
{
  
}


void Trigger()
{
  State=!State;
  UpdateTimeFlag = true;
}

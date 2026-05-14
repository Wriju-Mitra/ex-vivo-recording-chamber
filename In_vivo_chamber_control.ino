/*Code for controlling optogenetic stimulation and monitoring temperature
 * in a recording chamber built to record Ca2+ signals in explanted Drosophila brains
 * The Arduino controls the brightness and frequency of LED stimulation
 * Temp of the saline is recorded. Control of saline temp is to be achieved through heating/cooling the 
 * reservoir.
 * 
 * 
 * 
 */


#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  //RS pin=d12,Enable=d11,D4=d5,D5=d4,D6=3,D7=d2
                                        //LCD R/W -> GND, VSS->GND, VCC->5V,LED+ ->5V through 220ohm resistor, LED- ->GND

int sensorPin =A0;
int V0=0;
float R1 = 10000; //For 10k (fixed)resistor change to appropiate value if using a different resistor
float R2,logR2, T, Tcel;   //May the lord forgive me!
float A = 0.3518829716e-03, B = 3.624399442e-04, C= -4.255586818e-07;  //for steinhar-hart coefficients need to be experimentally determined

void setup() 
{
  Serial.begin(9600);
  //lcd.begin(16,2);
  //lcd.setCursor(0,0);
  //lcd.print("Hello!");
  delay(3000);
  //lcd.clear();

}

void loop() 
{
  // put your main code here, to run repeatedly:  
  measureTemperature();
  delay(1000);
  
// lcd.clear();
}


void measureTemperature()
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


//Anthony Torres & Eric Macias 
//cpsc42
//
// automatic lizard pellet feeder with hum/temp sensor 

#include <AccelStepper.h> // Load the AccelStepper library
#include <LiquidCrystal.h>
#include "DHT.h"
#include "RTClib.h"

//
#define DHTPIN 19            // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11       // DHT 11
#define motorPin1  7        // IN1 pin on the ULN2003A driver
#define motorPin2  6        // IN2 pin on the ULN2003A driver
#define motorPin3  5        // IN3 pin on the ULN2003A driver
#define motorPin4  4        // IN4 pin on the ULN2003A driver

//
int stepsPerRevolution = 64;        // steps per revolution
float degreePerRevolution = 5.625;  // degree per revolution
const int buttonPress = 18;          //
int count = 0;
int buttonStatePress = 0;
volatile byte state = LOW;
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;

//
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
AccelStepper stepper(AccelStepper::HALF4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);
RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);



void setup() {
   Serial.begin(57600);               // initialise the serial monitor
   stepper.setMaxSpeed(1000.0);       // set the max motor speed
   stepper.setAcceleration(100.0);    // set the acceleration
   stepper.setSpeed(900);             // set the current speed

   //lcd num of colums and rows
   lcd.clear();
   lcd.begin(16, 2);
   lcd.print("Lizard FHT 3000");
   delay(1000);
   lcd.clear();
 
   //button to swtich between info
   pinMode(buttonPress, INPUT_PULLUP); //uses the arduino's 10 pull up resistor 
   // interrupt fuction, uses the int 5 to represent pin 18 on a mega,and RISING part is to trigger
   attachInterrupt(5, buttonPressed,RISING);
  
   //humidity and temp
   Serial.begin(9600);
   dht.begin();

   //real time clock 
   Serial.begin(57600);
   if (! rtc.begin()) {
      Serial.flush();
   while (1) delay(10);
   
  }

   if (! rtc.isrunning()) {
   
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // When time needs to be re-set on a previously configured device, the
      // following line sets the RTC to the date & time this sketch was compiled
    
  }
  
}//end setup

void loop() {
  buttonStatePress = digitalRead(buttonPress);
     
   RTC();
   DateTime now = rtc.now();

   if ((now.hour() == 9) and (now.minute() == 30)){
       stepper.moveTo(degToSteps(45));   // order the motor to rotate 90 degrees forward
       stepper.run();
       return;
      }

   if ((now.hour() == 19) and (now.minute() == 30)){
         stepper.moveTo(degToSteps(90));   // order the motor to rotate 90 degrees forward
         stepper.run();
         return;
      }
      
   count = count + 1;
   if (count = 1000){
     HumitNTemp();
     count = 0;
     return;

    }
     
}//end loop

void HumitNTemp(){
 
  float h = dht.readHumidity();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  
  lcd.setCursor(0,0);
  lcd.print(F("H:"));
  lcd.print(h);
  lcd.print(F("%"));
  lcd.setCursor(0,1);
  lcd.print(F("Temp: "));
  lcd.print(f);
  lcd.print(F("F"));
  return; 
}//
void RTC(){

  DateTime now = rtc.now();
  lcd.setCursor(8,0);
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  lcd.print(now.second(), DEC);
  return;
}

float degToSteps(float deg) {
  return (stepsPerRevolution / degreePerRevolution) * deg;
}

void buttonPressed(){
  state = !state; 
}
//end void buttonPressed

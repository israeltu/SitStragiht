#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
//char auth[] = "b5fd5ac1155e41e89a39005a01ef89f0";
char auth[] = "dvxpbLGXkLrnIH_Cr85hNLQCrndarCNv";
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Isrphone";
char pass[] = "hello1234";
int sensorData1;
int sensorData2;
int sensorData3;
int calibratedVal1=0;
int calibratedVal2=0;
int delta=100;
int threshold=800;
int vibrator = 8;
int button = 7;
unsigned long unPressedTime=0;
unsigned long currentTime=0;
int cond=1;
int interval=10000; //10sec default


WidgetLCD lcd(V9);

// Hardware Serial on Mega, Leonardo, Micro...
//#define EspSerial Serial

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX

// Set your ESP-01 baud rate to 9600 with this command AT+UART_DEF=9600,8,1,0,0
// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

BlynkTimer timer;
//BlynkTimer timer2;

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable

  if (pinValue==1){
    calibratedVal1=analogRead(A0);
    calibratedVal2=analogRead(A1);
    threshold=analogRead(A4);
    lcd.print(0,0,"Posture");
    lcd.print(0,1,"Calibrated!!!");
    delay(100);
    lcd.clear();
    lcd.print(0,0,"Posture");
    lcd.print(0,1,"Calibrated!!!");
    delay(100);
    lcd.clear();
  }
}
BLYNK_WRITE(V2)
{
  interval = param.asInt(); // assigning incoming value from pin V2 to a variable

}
// This function sends Arduino's up time every second to Virtual Pin.
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  lcd.clear();
  sensorData1 = analogRead(A0);
  Blynk.virtualWrite(V6, sensorData1);
  sensorData2 = analogRead(A1);
  Blynk.virtualWrite(V7, sensorData2);
  sensorData3 = analogRead(A4);
  Blynk.virtualWrite(V8, sensorData3);
  Blynk.virtualWrite(V5, millis() / 1000);
  
  //only if user sits on the chair
  if(sensorData3 >threshold-delta){
    if((cond==1)&&((sensorData1 < calibratedVal1-delta) || (sensorData2 < calibratedVal2-delta))){
      unPressedTime= millis();
      cond=0;
    }
    if((sensorData1 >= calibratedVal1-delta) && (sensorData2 >= calibratedVal2-delta)){
      unPressedTime=0;
      cond=1;
    }
    currentTime=millis();  
    if((currentTime-unPressedTime)>0 && cond==0 ){
      lcd.print(0,0,"Bad Posture!");
      lcd.print(0,1,"Pls Sit Straight");
    }
    if((currentTime-unPressedTime)>interval*1000 && cond==0 ){
      tone(vibrator,500);
      delay(200);
      noTone(vibrator);
    }
  }
}

void setup()
{
  pinMode(vibrator, OUTPUT);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);
  
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
    timer.run(); // Initiates BlynkTimer
}

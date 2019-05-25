#include <LiquidCrystal.h>
#include <SoftwareSerial.h> // Library for using serial communication
SoftwareSerial SIM900(10, 11); // Pins 10, 11 are used as used as software serial pins
LiquidCrystal lcd(2,3,4,5,6,7);  // sets the interfacing pins

String incomingData;   // for storing incoming serial data
String message = "";   // A String for storing the message
int relay_pin = 8;    // Initialized a pin for relay module for LED
int relay_pin1 = 9;    // Initialized a pin for relay module for LM35
float LdrPin=A0;
float Sensor=A1;
float tempc;
float tempf;
float vout;

void setup()
{
  Serial.begin(115200); // baudrate for serial monitor
  SIM900.begin(19200); // baudrate for GSM shield

  pinMode(relay_pin, OUTPUT);   // Setting erlay pin as output pin
  digitalWrite(relay_pin, HIGH);  // Making relay pin initailly low for LED 
  digitalWrite(relay_pin1, HIGH);  // Making relay pin initailly low for LM35

  // set SMS mode to text mode
  SIM900.print("AT+CMGF=1\r");  
  delay(100);
  
  // set gsm module to tp show the output on serial out
  SIM900.print("AT+CNMI=2,2,0,0,0\r"); 
  delay(100);

  lcd.begin(16,2);  // initializes the 16x2 LCD
  pinMode(LdrPin,INPUT);
  pinMode(Sensor,INPUT);
  
}

void loop()
{
  //Function for receiving sms
  receive_message();
  

  // if received command is to turn on relay
  if(incomingData.indexOf("Led_on")>=0)
  {
    digitalWrite(relay_pin, LOW);
    message = "Led is turned ON";
    // Send a sms back to confirm that the relay is turned on
    send_message(message);
  }
  
  // if received command is to turn off relay
  if(incomingData.indexOf("Led_off")>=0)
  {
    digitalWrite(relay_pin, HIGH);
    message = "Led is turned OFF";
    // Send a sms back to confirm that the relay is turned off
    send_message(message);
  }

          temp_measure();
          check_ldr();
}

void receive_message()
{
  if (SIM900.available() > 0)
  {
    incomingData = SIM900.readString(); // Get the data from the serial port.
    Serial.print(incomingData); 
    delay(10); 
  }
}

void send_message(String message)
{
  SIM900.println("AT+CMGF=1");    //Set the GSM Module in Text Mode
  delay(100);  
  SIM900.println("AT+CMGS=\"+918550901610\""); // Replace it with your mobile number
  delay(100);
  SIM900.println(message);   // The SMS text you want to send
  delay(100);
  SIM900.println((char)26);  // ASCII code of CTRL+Z
  delay(100);
  SIM900.println();
  delay(1000);  
}

void check_ldr()
{
  int LdrStatus=analogRead(LdrPin);
  lcd.setCursor(0,0);
  if (LdrStatus <=700) 
  {
    digitalWrite(relay_pin, HIGH);
    Serial.println("Lights status: ON");
    lcd.println("Lights status: ON");
    }
    else
    {
      digitalWrite(relay_pin, LOW);
      Serial.println("Lights status: OFF");
      lcd.println("Lights status: OFF");
      }  
  delay(1000);
}

void temp_measure()
{
   vout=analogRead(Sensor);
   tempc=(vout/9.31);
   tempf=((tempc*1.8)+32);
   
   lcd.setCursor(0,0);   //sets the cursor at row 0 colum 
   lcd.print("Temp. is=");  // prints 16x2 LCD MODULE
   lcd.print(tempc);  //Prints temprature in degree centigrate
   lcd.print(" , ");
   lcd.print(tempf);  //Prints temprature in farenheit
   delay(1000);
}

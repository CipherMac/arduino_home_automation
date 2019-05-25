#include <SoftwareSerial.h> // Library for using serial communication
#include<LiquidCrystal.h>


SoftwareSerial SIM900(10, 11); // Pins 10, 11 are used as used as software serial pins
LiquidCrystal Lcd(2,3,4,5,6,7);  // sets the interfacing pins

float LdrPin=A0;
int RelayPin1=8;
int RelayPin2=9;
float Sensor=A1;
float tempc;
float tempf;
float vout;

String incomingData;   // for storing incoming serial data
String message = "";   // A String for storing the message


void setup() {
 
 Lcd.begin(16,2);  // initializes the 16x2 LCD
 pinMode(RelayPin1,OUTPUT); // Setting erlay pin as output pin
 pinMode(RelayPin2,OUTPUT);
 pinMode(LdrPin,INPUT);
 pinMode(Sensor,INPUT);

  Serial.begin(115200); // baudrate for serial monitor
  SIM900.begin(19200); // baudrate for GSM shield

    
  digitalWrite(RelayPin1, HIGH);  // Making relay pin initailly low

  // set SMS mode to text mode
  SIM900.print("AT+CMGF=1\r");  
  delay(100);
  
  // set gsm module to tp show the output on serial out
  SIM900.print("AT+CNMI=2,2,0,0,0\r"); 
  delay(100);
}

void loop() {


 int LdrStatus=analogRead(LdrPin);
 
 vout=analogRead(Sensor);
 vout=(vout*500)/1023;
 tempc=vout;
 tempf=((vout*1.8)+32);
 
 Lcd.setCursor(0,0);   //sets the cursor at row 0 colum
 Lcd.print("Temp. is=");  // prints 16x2 LCD MODULE
 Lcd.print(tempc);  //Prints temprature in degree centigrate
 Lcd.print(tempf);  //Prints temprature in farenheit
 delay(1000);

 Lcd.setCursor(2,1);  //sets the cursor at row 1 column 2
 if(LdrStatus<=200)
 {
  digitalWrite(RelayPin1,HIGH);
  Serial.print("Its DARK, Turn on the LED : ");
  Serial.println(LdrStatus);
  } 
  else {
    digitalWrite(RelayPin1, LOW);
    Serial.print("Its BRIGHT, Turn off the LED : ");
    Serial.println(LdrStatus);
    }

//Function for receiving sms
  receive_message();

  // if received command is to turn on relay
  if(incomingData.indexOf("Led_on")>=0)
  {
    digitalWrite(RelayPin1, LOW);
    message = "Led is turned ON";
    // Send a sms back to confirm that the relay is turned on
    send_message(message);
  }
  
  // if received command is to turn off relay
  if(incomingData.indexOf("Led_off")>=0)
  {
    digitalWrite(RelayPin1, HIGH);
    message = "Led is turned OFF";
    // Send a sms back to confirm that the relay is turned off
    send_message(message);
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

void receive_message()
{
  if (SIM900.available() > 0)
  {
    incomingData = SIM900.readString(); // Get the data from the serial port.
    Serial.print(incomingData); 
    delay(10); 
  }
}

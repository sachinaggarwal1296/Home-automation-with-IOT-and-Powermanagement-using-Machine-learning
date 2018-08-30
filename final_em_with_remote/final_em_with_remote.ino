#include<SD.h>
#include <SPI.h>
#include "DHT.h"
#include <Wire.h>
#include "RTClib.h"
#include <Ethernet.h>
#define RELAY1  7

EthernetServer server(80);
String readString;
#define DHTPIN 2 
#define DHTTYPE DHT11
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,150);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255, 255, 255, 0);
 
File my_file;
RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);

float sample1=0; // for voltage
float sample2=0; // for current
float voltage=0.0;
float val; // current callibration
float actualval; // read the actual current from ACS 712
float amps=0.0;
float totamps=0.0; 
float avgamps=0.0;
float amphr=0.0;
float watt=0.0;
float energy=0.0;
float humidity=0.0;
float temprature=0.0;
int i = 1;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define the string for our datastream ID
char sensorId[] = "POWER";
char sensorId2[] = "ENERGY";



void setup() {
  pinMode(RELAY1, OUTPUT);
  Serial.begin(9600);
    if(!SD.begin()){
    while(1){
      Serial.println("Cannot open sd card..!");
    }
  }

  my_file = SD.open("DATA.CSV",FILE_WRITE);

  if(!my_file){
    while(1){
      Serial.println("Cannot open file..!");
    }
  }

  my_file.print("DATE,DAY,TIME,POWER,ENERGY,CURRENT,VOLTAGE,TIME,HUMIDITY,TEMPRATURE\n");
  dht.begin();

   if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

    server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
  Serial.println("LED Controller Test 1.0");
}

void loop() {
 EthernetClient client = server.available();
  if (client)

  {
    Serial.println("new client");

    while (client.connected())
    {
      if (client.available())

      {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100)

        {

          //store characters to string
          readString += c;
          //Serial.print(c);


          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          //if HTTP request has ended
          if (c == '\n') {
            Serial.println(readString); //print to serial monitor for debuging
//--------------------------------------------------------------------------------------------------------
// Needed to Display Site:
client.println("HTTP/1.1 200 OK"); //send new page
            client.println("Content-Type: text/html");
            client.println();
            client.println("<HTML>");
            client.println("<HEAD>");

//--------------------------------------------------------------------------------------------------------
//-------------------------------------------------

// what is being Displayed :     
        
            client.println("<TITLE>Home Automation</TITLE>");
             client.println("<center>");
            client.println("</HEAD>");
            client.println("<BODY>");
            client.println("<H1>Home Automation</H1>");
            client.println("<hr />");
            client.println("<center>");

            client.println("<a href=\"/?lighton\"\">Turn On Light</a>");
            client.println("<br />");
            client.println("<br />");
            client.println("<a href=\"/?lightoff\"\">Turn Off Light</a><br />");     

            client.println("</BODY>");
            client.println("</HTML>");

            delay(1);
            //stopping client
            client.stop();

            //-------------------------------------------------
            // Code which needs to be Implemented:
            if(readString.indexOf("?lighton") >0)//checks for on
            {
              digitalWrite(RELAY1, HIGH);    // set pin 8 high
              Serial.println("LED On");
              sendData(); // send data to xively
              i = i + 1;
              if(i==2000){
              my_file.close();
              }
    
            }
            else{
              if(readString.indexOf("?lightoff") >0)//checks for off
              {
                digitalWrite(RELAY1, LOW);    // set pin 8 low
                Serial.println("LED Off");
                sendData(); // send data to xively
                i = i + 1;
                if(i==2000){
                  my_file.close();
                   }
    
              }
            }
            //clearing string for next read
            readString="";

            // give the web browser time to receive the data
            delay(1);
            // close the connection:
            client.stop();
            Serial.println("client disonnected");

          }
        }
      }
    }
  }
}

// send the supplied values to Cosm, printing some debug information as we go
void sendData() {
long milisec = millis(); // calculate time in milisec
long time=milisec/1000; // convert time to sec

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
                      /// taking 150 samples from sensors with a inerval of 2sec and then average the samples data collected
  for(int i=0;i<150;i++)
  {
    sample1+=analogRead(A2);  //read the voltage from the sensor
    sample2+=analogRead(A3); //read the current from sensor
    delay(2);
  }
   sample1=sample1/150; 
   sample2=sample2/150;
   
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  /////voltage calculation//////////////////////
   voltage=4.669*2*sample1/1000; //  callibration // 3.25v from voltage div is eqv 696 in A2 reading  // multiply 2 to get actual voltage
  /// current calculation //////////////////////
   val =(5.0*sample2)/1024.0; 
   actualval =val-2.5; // offset voltage is 2.5v 
   amps =actualval*10; // 100mv/A from data sheet 
   totamps=totamps+amps; // total amps 
   avgamps=totamps/time; // average amps
   amphr=(avgamps*time)/3600;  // amphour
   watt =voltage*amps;    // power=voltage*current
   energy=(watt*time)/3600;   //energy in watt hour
   humidity = dht.readHumidity();
   temprature = dht.readTemperature();// Read temperature as Celsius (the default)
   // energy=(watt*time)/(1000*3600);   // energy in kWh


////////////////////////////////////TO DISPLAY TIME//////////////////////////////////////////////////////////   

    DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();   
    Serial.println();
    //delay(3000);

////////////////////////////////////DISPLAY IN SERIAL MONITOR//////////////////////////////////////////////////////////

Serial.print("VOLTAGE : ");
Serial.print(voltage);
Serial.println("Volt");
Serial.print("CURRENT :");
Serial.print(amps);
Serial.println("Amps");
Serial.print("POWER :");
Serial.print(watt);
Serial.println("Watt");
Serial.print("ENERGY CONSUMED :");
Serial.print(energy);
Serial.println("Watt-Hour");
Serial.print("Humidity : ");
Serial.print(humidity);
Serial.println("%");
Serial.print("Temprature : ");
Serial.println(temprature);
Serial.print(""); // print the next sets of parameter after a blank line
Serial.println(time);


///////////////////////////////////SAVE IN SD CARD//////////////////////////////////////////////////////////////////
  

 // lcd.print("POWER :");
  my_file.print(watt);
  my_file.print(",");
  my_file.print(energy);
  my_file.print(",");
  my_file.print(voltage);
  my_file.print(",");
  my_file.print(amps);
  my_file.print(",");
  my_file.print(time);
  my_file.print(",");
  my_file.print(humidity);
  my_file.print(",");
  my_file.print(temprature);
  my_file.print(",");
  my_file.print(now.day(), DEC);
  my_file.print("/");
  my_file.print(now.month(), DEC);
  my_file.print("/");
  my_file.print(now.year(), DEC);
  my_file.print(",");
  my_file.print(daysOfTheWeek[now.dayOfTheWeek()]);
  my_file.print(",");
  my_file.print(now.hour(), DEC);
  my_file.print(":");
  my_file.print(now.minute(), DEC);
  my_file.print(":");
  my_file.print(now.second(), DEC);
  my_file.print("\n");
  delay(3000); 
  

  
}



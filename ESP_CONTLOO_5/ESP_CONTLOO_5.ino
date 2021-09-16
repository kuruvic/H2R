#include <ArduinoJson.h>
#include <DHT.h>
//#include <pm2008_i2c.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>

#define DHTPIN 14        // DHT pin number, 온습도 센서 핀 번호
#define DHTTYPE DHT22   // DHT Sensor Type, 온습도 센서 타입

//#define ONE_WIRE_BUS 19

DHT dht(DHTPIN, DHTTYPE);
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);

//PM2008_I2C pm2008_i2c;

String  DATAINT;
String  inputString = "";          // a string to hold incoming data
boolean stringComplete = false;    // whether the string is complete
String  receivedData = "";
String  resultINT;

double currentTime = 0;
double lastTime = 0;
double getLastTime()
{
      return currentTime = millis()/1000.0 - lastTime;
}

typedef struct protocolSt 
{
      int id;
      uint16_t relay;
      uint16_t sel;
      
} _bmProtocolST;

_bmProtocolST stPROTO;

bool isCommandJSON(String _comm)
{
    String jsonString = _comm;
    bool ret = false;

    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(jsonString);

    if (root.success())
    {    
        int _id = root["id"];
        uint16_t _relay = root["relay"];
        uint16_t _sel = root["sel"];
        
        stPROTO.id = _id;
        stPROTO.relay = _relay;
        stPROTO.sel = _sel;
        
        ret = true;
    }

    return ret;
}

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  
  dht.begin();
  //pm2008_i2c.begin();
  //pm2008_i2c.command();
  //sensors.begin();

  delay(1000);
  lastTime = millis()/1000.0;
}

void loop() {
  /*int pm2_5;
  int pm10;

  uint8_t ret = pm2008_i2c.read();
  if (ret == 0) {
    pm2_5 = pm2008_i2c.pm2p5_tsi;
    pm10 = pm2008_i2c.pm10_tsi;
  }*/
  //sensors.requestTemperatures();
  
  float hum = dht.readHumidity();    //read humidity,  습도 센서값 읽기 
  float temp = dht.readTemperature();//read temperature,  온도 센서값 읽기 
  int gasval = analogRead(A6);
  int soiltemp = map(gasval, 0, 1023, 0, 100);//sensors.getTempCByIndex(0);
  int shvalue = analogRead(A2);
  int soilhum = map(shvalue, 0, 1023, 0, 100);
  
  DATAINT = "{\"id\":\""+String(stPROTO.id)+"\"";
  DATAINT += ",\"temp\":\""+String(temp)+"\"";
  DATAINT += ",\"hum\":\""+String(hum)+"\"";
  DATAINT += ",\"soiltemp\":\""+String(soiltemp)+"\"";
  DATAINT += ",\"soilhum\":\""+String(soilhum)+"\"";
  DATAINT += "}";

  if((60) < (getLastTime()))
  {    
      Serial.println(DATAINT);
      lastTime = millis()/1000.0;
  }
  
  
  if (stringComplete == true)
  {
         if(stPROTO.relay == 1)
         {
               if(stPROTO.sel == 1)
               {
                   digitalWrite(2, HIGH);
                   digitalWrite(3, LOW);
                   delay(6000);
                   digitalWrite(2, LOW);
                   digitalWrite(3, LOW);
               }
          
               else if(stPROTO.sel == 0)
               {
                   digitalWrite(2, LOW);
                   digitalWrite(3, HIGH);
                   delay(6000);
                   digitalWrite(2, LOW);
                   digitalWrite(3, LOW);
               }
         }
    
         /*else if(stPROTO.relay == 2)
         {
              if(stPROTO.sel == 1)
              {
                  digitalWrite(3, HIGH);
              }
        
              else if(stPROTO.sel == 0)
              {
                  digitalWrite(3, LOW);
              }
         }*/
    
         else if(stPROTO.relay == 3)
         {
              if(stPROTO.sel == 1)
              {
                  digitalWrite(4, HIGH);
              }
        
              else if(stPROTO.sel == 0)
              {
                  digitalWrite(4, LOW);
              }
         }
    
         else if(stPROTO.relay == 4)
         {
              if(stPROTO.sel == 1)
              {
                  digitalWrite(5, HIGH);
              }
        
              else if(stPROTO.sel == 0)
              {
                  digitalWrite(5, LOW);
              }
         }

         else if(stPROTO.relay == 5)
         {
              if(stPROTO.sel == 1)
              {
                  digitalWrite(8, LOW);
                  digitalWrite(9, LOW);
              }
        
              else if(stPROTO.sel == 0)
              {
                  digitalWrite(8, HIGH);
                  digitalWrite(9, HIGH);
              }
         }
         
         cmdResponse();
      
         stringComplete =  false;
    }
}

void cmdResponse()
{
    resultINT = "{\"id\":\""+String(stPROTO.id)+"\""; 
    resultINT += ",\"relay\":\""+String(stPROTO.relay)+"\"";
    resultINT += ",\"result\":\""+String(stPROTO.sel)+"\"";
    resultINT += "}";

    Serial.println(resultINT); 

}

void serialEvent() {
  while (Serial.available()) {

    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;

    if (inChar == '}') {
      receivedData = inputString;
      isCommandJSON(receivedData);
      inputString = "";
      stringComplete = true;
    }
  }
}

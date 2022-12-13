#include <SoftwareSerial.h>
#include "SNIPE.h"


#define TXpin 11
#define RXpin 10
#define ATSerial Serial
#define DHTPIN 2
#define DHTTYPE DHT11

#define ADDR 10
int LED = 3;
int INA = 9;
int INB = 8;

DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";

char buffer[20];
int addr;

void setup() {
  ATSerial.begin(115200);
  

  // put your setup code here, to run once:
  while(ATSerial.read()>= 0) {}
  while(!ATSerial);

  DebugSerial.begin(115200);

  /* SNIPE LoRa Initialization */
  if (!SNIPE.lora_init()) {
    DebugSerial.println("SNIPE LoRa Initialization Fail!");
    while (1);
  }

  /* SNIPE LoRa Set App Key */
  if (!SNIPE.lora_setAppKey(lora_app_key)) {
    DebugSerial.println("SNIPE LoRa app key value has not been changed");
  }
  
  /* SNIPE LoRa Set Frequency */
  if (!SNIPE.lora_setFreq(LORA_CH_1)) {
    DebugSerial.println("SNIPE LoRa Frequency value has not been changed");
  }

  /* SNIPE LoRa Set Spreading Factor */
  if (!SNIPE.lora_setSf(LORA_SF_7)) {
    DebugSerial.println("SNIPE LoRa Sf value has not been changed");
  }
  
  /* SNIPE LoRa Set Rx Timeout 
   * If you select LORA_SF_12, 
   * RX Timout use a value greater than 5000  
  */
  if (!SNIPE.lora_setRxtout(5000)) {
    DebugSerial.println("SNIPE LoRa Rx Timout value has not been changed");
  }     
      
  DebugSerial.println("SNIPE LoRa LED Recv");
  dht.begin();

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED, OUTPUT);
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
}

void loop() {
        String ver = SNIPE.lora_recv();
        char temp[8];
        char humi[8];
        
        
        if (ver!= "AT_RX_TIMEOUT" && ver!="AT_RX_ERROR" )
        {
            debugSerial.println(ver);
            memset(buffer, 0x0, sizeof(buffer));
            ver.toCharArray(buffer,sizeof(buffer));
            sscanf(buffer, "%d", &addr);
            if(addr == ADDR){
             String send_value;
             DebugSerial.println(SNIPE.lora_getRssi());
              DebugSerial.println(SNIPE.lora_getSnr());
              // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
              float h = dht.readHumidity();
      // Read temperature as Celsius
              float t = dht.readTemperature();
      // Read temperature as Fahrenheit
              float f = dht.readTemperature(true);
               if (isnan(h) || isnan(t) || isnan(f)) {
        DebugSerial.println("Failed to read from DHT sensor!");
        return;
      }
  
      // Compute heat index
      // Must send in temp in Fahrenheit!
      float hi = dht.computeHeatIndex(f, h);

      memset(buffer, 0x0, sizeof(buffer));
      dtostrf(t, 5, 2, temp);
      dtostrf(h, 5, 2, humi);

      sprintf(buffer, "%d:%s:%s", addr, temp, humi);
      send_value = String(buffer);

      DebugSerial.println("-----LoRa_send_value-----");
      DebugSerial.println(send_value);
  
      DebugSerial.print("Humidity: "); 
      DebugSerial.print(h);
      DebugSerial.print(" %\t");
      DebugSerial.print("Temperature: "); 
      DebugSerial.print(t);
      DebugSerial.print(" *C ");
      
            digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
            digitalWrite(INA, HIGH);
            digitalWrite(INB, LOW);
            DebugSerial.println("SNIPE LoRa status on");
            SNIPE.lora_send("Ack");
            }
        }
        else
        {
            
            digitalWrite(LED, LOW);
            digitalWrite(INA, LOW);
            digitalWrite(INB, LOW);
            DebugSerial.println("SNIPE LoRa status off");
            SNIPE.lora_send("Ack");      
        }
        else
        {
            DebugSerial.println("recv Error!!!");
            delay(1000);
        }
        delay(1000);
}

/* simple socket client on arduino */
#include <Ethernet.h>
#include <SPI.h>

/* Temperature Humidity DHT11 */
#include "DHT.h"
#define DHTPIN 9
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* LED */
const int LED_RED = 5;
const int LED_GREEN = 6;
const int LED_ORANGE = 7;

/* VENTILATOR */
const int VENTILATOR = 8;

/* VAR */
int max_temperature = 25;
int min_temperature = 20;
int lapse_send = 0;
float temperature = 0;
String recived_data = "";
String send_data = "";
char c = '*';

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// byte ip[] = { 192, 168, 0, 7 };       // client ip
byte server[] = { 192, 168, 1, 77 }; // server ip
EthernetClient client;

void setup(){
  Ethernet.begin(mac);
  Serial.begin(9600);
  delay(1);
  /* DHT11 */
  dht.begin();
  /* LED */
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_ORANGE, OUTPUT);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_ORANGE, LOW);
  /* VENTILATOR */
  pinMode(VENTILATOR, OUTPUT);
  digitalWrite(VENTILATOR, LOW);
  /* Connect to Server */  
  ConnectServer(); 
}
void loop(){
  temperature = GetTemperature();
  if (temperature > max_temperature){
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_ORANGE, LOW);
    digitalWrite(VENTILATOR, HIGH);
  }else{
    if(temperature < min_temperature){
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_ORANGE, HIGH);
      digitalWrite(VENTILATOR, LOW);
    }else{
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_ORANGE, LOW);
      digitalWrite(VENTILATOR, LOW);
      digitalWrite(VENTILATOR, LOW);
    }
  }
  if(client.connected()){
    lapse_send++;
    if(lapse_send == 5){
      lapse_send = 0;
      send_data = "t," + String(temperature);
      client.print(send_data);
    }
    while (client.available()) {
      c = client.read();
      if (c == '\n') {
        Serial.println(recived_data);
        if(recived_data.charAt(0) == 'r'){
          max_temperature = (recived_data.substring(recived_data.indexOf(',') + 1, recived_data.lastIndexOf(','))).toInt();
          min_temperature = (recived_data.substring(recived_data.lastIndexOf(',') + 1, recived_data.length())).toInt();
        }else{
           if(recived_data.charAt(0) == 'R'){
            send_data = "r," + String(max_temperature) + "," + String(min_temperature);
            client.print(send_data);
           }
        }
        recived_data = "";
        break;
      }else{
        recived_data += c;
      }
    }
  } else{ 
    Serial.println("disconnecting.");
    ConnectServer();    
  }
  Serial.println(temperature);
  delay(1000);
}

float GetTemperature() {
  return dht.readTemperature();
}

float GetHumidity() {
  return dht.readHumidity();
}

void ConnectServer(){
  /* Connect to Server */  
  while(true){
    delay(1000);
    Serial.println("connecting...");
    if (client.connect(server, 3001)) {
      Serial.println("connected");
      break;
    } else { 
      Serial.println("connection failed");
      break;
    } 
  }
}


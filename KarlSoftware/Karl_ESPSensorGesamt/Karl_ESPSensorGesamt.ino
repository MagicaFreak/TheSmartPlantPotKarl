//ESP Zeug
#include <esp_now.h>
#include <WiFi.h>
//DHT Zeug
#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11   // DHT 11

typedef struct SensorData
{ 
  float h; // humidity
  float t; // temperatur
  float B; // Soil humidity

  // Light sensors
  int L1; int L2;
  int L3; int L4;
  int L5; int L6;
  int L7; int L8;
};

//ESPNOW zeug AUskommentiert
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
esp_now_peer_info_t peerInfo;



//Bodenfeuchtigkeit analogpins
int adc_id = 0;
//Photowiderstand analogpins
int adc_id1 = 1;
int adc_id2 = 2;
int adc_id3 = 3;
int adc_id4 = 4;
int adc_id5 = 5;
int adc_id6 = 6;
int adc_id7 = 7;
int adc_id8 = 8;

DHT dht(DHTPIN, DHTTYPE);

void setup() 

{
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  dht.begin();

  //ESP Now Transmitt Zeug
  WiFi.mode(WIFI_STA);
  
  esp_now_init();


  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  // Add peer        
  esp_now_add_peer(&peerInfo);

}



void loop() {
  // Wait 5 seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  delay(2000);
  // Value of Capacitor for Groundhumidity
  float B = analogRead(adc_id);
/* Berechnungeng kommen noch hier rein*/
  
  
  delay(2000);
  // Value of Licht
  int L1 = analogRead(adc_id1);
  int L2 = analogRead(adc_id2);
  int L3 = analogRead(adc_id3);
  int L4 = analogRead(adc_id4);
  int L5 = analogRead(adc_id5);
  int L6 = analogRead(adc_id6);
  int L7 = analogRead(adc_id7);
  int L8 = analogRead(adc_id8);
  
  /* Berechnungen kommen hier rein für licht */

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    
  }
  // Check if any reads failed and exit early (to try again).
  if (isnan(B))
  {
    Serial.println(F("Failed to read from Groundhumidity sensor!"));
  }


  //Serialmontior Data DHT11

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C \n"));

  //Serialmonitor Ground Humidity

  Serial.print(F("Groundhumidity: "));
  Serial.print(B);
  Serial.print(F("% \n"));

  //Serialmonitor Fotoresistor

  Serial.print(F("Lightsensor 1: "));
  Serial.print(L1);
  Serial.print(F("Lightsensor 2: "));
  Serial.print(L2);
  Serial.print(F("Lightsensor 3: "));
  Serial.print(L3);
  Serial.print(F("Lightsensor 4: "));
  Serial.print(L4);
  Serial.print(F("Lightsensor 5: "));
  Serial.print(L5);
  Serial.print(F("Lightsensor 6: "));
  Serial.print(L6);
  Serial.print(F("Lightsensor 7: "));
  Serial.print(L7);
  Serial.print(F("Lightsensor 8: "));
  Serial.print(L8);

  //Sendata ESP NOW
  //esp_now_send(receiverAdress, Sensordaten, sizeof(Sensordaten)-1);
 

}
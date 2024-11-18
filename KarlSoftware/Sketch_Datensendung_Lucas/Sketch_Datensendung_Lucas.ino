#include <esp_now.h>
#include <WiFi.h>

typedef struct sensorData
{
  float Air_Temp;
  float Air_Hum;
  float Soil_Hum;

  int Light_intense_sensor_1; int Light_intense_sensor_2;
  int Light_intense_sensor_3; int Light_intense_sensor_4;
  int Light_intense_sensor_5; int Light_intense_sensor_6;
  int Light_intense_sensor_7; int Light_intense_sensor_8;

} sensorData;

sensorData mySensorData;

esp_now_peer_info_t peerInfo;

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void OnDataSent(const uint8_t* mac_address, esp_now_send_status status)
{
  Serial.print("Sending Transmitter data: ");
  if(status != ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("Transmission failed");
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  
  if(esp_now_init() != ESP_OK)
  {
    Serial.println("ESPNow Init Failed");
    return;
  }
  esp_now_register_send_cb(OnDataSent);


  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  esp_now_send(mySensorData);
}

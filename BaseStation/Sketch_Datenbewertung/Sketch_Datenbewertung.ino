#include <esp_now.h>
#include <WiFi.h>
#include <math.h>
//Constants stored in Header-File
#include "Bewegungskonstanten.h"

//includes for the upload to website, not yet done



//Data I would receive from Kevin
typedef struct SensorData
{
  float t;
  float h;
  float B;

  int L1; int L2;
  int L3; int L4;
  int L5; int L6;
  int L7; int L8;
  
} sensorData;

sensorData mySensorData;

void OnDataRecv(const esp_now_recv_info *info, const uint8_t* incomingData, int len)
{
  memcpy(&mySensorData, incomingData, sizeof(mySensorData));
  Serial.printf("Transmitter MAC Address: %02X:%02X:%02X:%02X:%02X:%02X \n\r", 
          info->src_addr[0], info->src_addr[1], info->src_addr[2], info->src_addr[3], info->src_addr[4], info->src_addr[5]);    
  Serial.println("Received Data");
  Serial.print("Temperature in Celsius: ");
  Serial.println(mySensorData.t);
  Serial.print("Air Humidity: ");
  Serial.println(mySensorData.h);
  Serial.print("Soil Humidity: ");
  Serial.println(mySensorData.B);
  //String output = mySensorData.L1 + ", " + mySensorData.L2 + ", " + mySensorData.L3 + ", " + mySensorData.L4
  //  + ", " + mySensorData.L5 + ", " + mySensorData.L6 + ", " + mySensorData.L7 + ", " + mySensorData.L8;
  Serial.print("Light intensity sensors: ");
  //Serial.print(output);
}

//Calculate a Rating for the conditions temperature and both humidities
//it goes form -100 to 100, where  at -100 is too little and at 100 is too much
float mean_value(int top, int bottom)
{
  return (top + bottom) / 2.0;
}
float step_value(int top, int bottom)
{
  return (top - bottom) / 2.0;
}
int calculate_rating_humidity(int top, int bottom, float value_read)
{
  if(value_read >= (float) top)
  {
    return 100;
  }
  if(value_read <= (float) bottom)
  {
    return -100;
  }
  return round((100.0 / step_value(top, bottom)) * (value_read - mean_value(top, bottom)));
}


int calculate_rating_temp(int top, int bottom, float value_read)
{
  if(value_read >= (float) top)
    return 100;
  
  if(value_read <= (float) bottom)
    return -100;
  
  float rating = (100.0/(step_value(top, bottom) * step_value(top, bottom))) * (value_read - mean_value(top, bottom)) * (value_read - mean_value(top, bottom));
  if(rating < mean_value(top, bottom))
    return round(-1.0 * rating);
  return round(rating);
}


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  
  if(esp_now_init() != ESP_OK)
  {
    Serial.println("ESPNow Init Failed");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() 
{
  //In case I receive the light-intensity data as individual floats
  int Light_intense[N_LIGHT_SENSORS] = {mySensorData.L1, mySensorData.L2,
                                          mySensorData.L3, mySensorData.L4,
                                          mySensorData.L5, mySensorData.L6,
                                          mySensorData.L7, mySensorData.L8};

  
  //Array to determine which type of imbalance we have for opposing sensors
  //Meaning of values:
    //0 = no imbalance
    //1 = imbalance: higher-number sensor is receiving too much light in comparison to lower-number sensor
    //-1 = imbalance: higher-number sensor is receiving too much light in comparison to lower-number sensor
  int balance_light_sensor_pairs[N_LIGHT_SENSORS] = {0, 0, 0, 0};
  
  //Calculate the individual rating for every variable
  int Rating_Temp = calculate_rating_temp(MAX_TEMP, MIN_TEMP, mySensorData.t);
  int Rating_Air_Humid = calculate_rating_humidity(MAX_AIR_HUM, MIN_AIR_HUM, mySensorData.h);
  int Rating_Soil_Humid = calculate_rating_humidity(MAX_SOI_HUM, MIN_SOI_HUM, mySensorData.B);


  for(int i = 0; i < N_LIGHT_SENSORS/2; i++)
  {
    int index_2 = i + N_LIGHT_SENSORS/2;
    if(abs(Light_intense[i] - Light_intense[index_2]) > MAX_LIGHT_DIFF)
    {
      Serial.print("Light imbalance found: Sensor No. ");
      if(Light_intense[i] > Light_intense[index_2])
      {
        balance_light_sensor_pairs[i] += 1;
        Serial.print(index_2 + 1);
      }
      if(Light_intense[i] < Light_intense[index_2])
      {
        balance_light_sensor_pairs[i] -= 1;
        Serial.print(i + 1);
      }
      Serial.println(" is not receiving enough light compared to its opposite");
    }
  }

  //Possible errors to be found according to each individual rating
  if(Rating_Temp > 100)
  {
    Serial.println("Plant temperature conditions too hot");
  }
  if(Rating_Temp < -100)
  {
    Serial.println("Plant temperature conditions too cold");
  }

  if(Rating_Air_Humid > 100)
  {
    Serial.println("Plant air humidity conditions too humid");
  }

  if(Rating_Air_Humid < -100)
  {
    Serial.println("Plant air humidity conditions too dry");
  }
  if(Rating_Soil_Humid > 100)
  {
    Serial.println("Plant soil humidity conditions too humid");
  }

  if(Rating_Soil_Humid < -100)
  {
    Serial.println("Plant soil humidity conditions too dry");
  }


  delay(5000);
}

#include <esp_now.h>
#include <WiFi.h>
#include <math.h>
//includes for the upload to website, not yet done


//ideale Bedingungen fuer Temperatur zwischen 16 und 30 Grad Celsius
#define MAX_TEMP 30
#define MIN_TEMP 16
//ideale Bedingungen zwischen 40% und 80% relative Luftfeuchtigkeit
#define MAX_AIR_HUM 80
#define MIN_AIR_HUM 40
//ideale Bedingungen zwischen 50% und 80% relative Bodenfeuchtigkeit
#define MAX_SOI_HUM 80
#define MIN_SOI_HUM 50

//ideale Lichtintensitaet Bedingungen muessen zwischen 100-600umol/m2s oder PPFD sein
//Wie das zu gross fuer die Lichtwiderstaende ist und diese Wert von PPFD ist auch von der Lichtquelle abhaengig,
//wir messen nur welche Sensor am meisten Licht bekommt
#define N_LIGHT_SENSORS 8

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
  return roundf((100.0 / step_value(top, bottom)) * (value_read - mean_value(top, bottom)));
}


int calculate_rating_temp(int top, int bottom, float value_read)
{
  if(value_read >= (float) top)
    return 100;

  if(value_read <= (float) bottom)
    return -100;
  
  float rating = (100.0/(step_value(top, bottom) * step_value(top, bottom))) * (value_read - mean_value(top, bottom)) * (value_read - mean_value(top, bottom));
  if(value_read < (float) mean_value(top, bottom))
    return roundf(-1.0 * rating);
  return roundf(rating);
}

int find_max_light_sensor(int array[], int groesse)
{
  int hoechste_index = 0;

  for(int i = 1; i < groesse; i++)
  {
    if(array[i] > array[hoechste_index])
      hoechste_index = i;
  }
  return hoechste_index + 1;
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
  int Light_intense[] = {mySensorData.L1, mySensorData.L2,
                         mySensorData.L3, mySensorData.L4,
                         mySensorData.L5, mySensorData.L6,
                         mySensorData.L7, mySensorData.L8};

  
  //Array to determine which type of imbalance we have for opposing sensors
  //Meaning of values:
    //0 = no imbalance
    //1 = imbalance: higher-number sensor is receiving too much light in comparison to lower-number sensor
    //-1 = imbalance: higher-number sensor is receiving too much light in comparison to lower-number sensor
  
  //Calculate the individual rating for every variable
  int Rating_Temp = calculate_rating_temp(MAX_TEMP, MIN_TEMP, mySensorData.t);
  int Rating_Air_Humid = calculate_rating_humidity(MAX_AIR_HUM, MIN_AIR_HUM, mySensorData.h);
  int Rating_Soil_Humid = calculate_rating_humidity(MAX_SOI_HUM, MIN_SOI_HUM, mySensorData.B);

  int Brightest_Sensor_index = find_max_light_sensor(Light_intense, N_LIGHT_SENSORS);
  Serial.print("Light sensor No. ");
  Serial.print(Brightest_Sensor_index);
  Serial.println(" is receiving the most light");

  //Possible errors to be found according to each individual rating
  if(Rating_Temp >= 100)
  {
    Serial.println("Plant temperature conditions too hot");
  }
  if(Rating_Temp <= -100)
  {
    Serial.println("Plant temperature conditions too cold");
  }

  if(Rating_Air_Humid >= 100)
  {
    Serial.println("Plant air humidity conditions too humid");
  }

  if(Rating_Air_Humid <= -100)
  {
    Serial.println("Plant air humidity conditions too dry");
  }
  if(Rating_Soil_Humid >= 100)
  {
    Serial.println("Plant soil humidity conditions too humid");
  }

  if(Rating_Soil_Humid <= -100)
  {
    Serial.println("Plant soil humidity conditions too dry");
  }


  delay(5000);
}

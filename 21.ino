

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <M5Core2.h>
#include <ArduinoJson.h>

//解析json
String valor=""; 
String all_valor=""; 
int string_num=0; 
char s_str[60]; 
StaticJsonDocument<200> doc;


double lst_lat, lst_lng, f_lat, f_lng, f_speed; //本次经纬度、上次经纬度、速度
double f_dist=0; //距离


float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;

float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;

float temp = 0.0F;
  int bosu=0;
 int jump=0;
//BLE 定义

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// TinyGps++


double distanceBtw(double lat1, double long1, double lat2, double long2)
{
  // returns distance in meters between two positions, both specified
  // as signed decimal-degrees latitude and longitude. Uses great-circle
  // distance computation for hypothetical sphere of radius 6372795 meters.
  // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
  // Courtesy of Maarten Lamers
  double delta = radians(long1-long2);
  double sdlong = sin(delta);
  double cdlong = cos(delta);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double slat1 = sin(lat1);
  double clat1 = cos(lat1);
  double slat2 = sin(lat2);
  double clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = sq(delta);
  delta += sq(clat2 * sdlong);
  delta = sqrt(delta);
  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta*6372795;
  
}



void Cal(double lat_0,double long_0, double speed_num_0)   
{

    f_lat=lat_0;
    f_lng=long_0;
    if (lst_lat == 0) { //开始测距，lst_lat==0
        f_dist = f_dist;
    }
    else 
        {
          f_dist = f_dist + distanceBtw(f_lat, f_lng, lst_lat, lst_lng)/ 1000;
          f_speed= distanceBtw(f_lat, f_lng, lst_lat, lst_lng)*1.2;
        }
     lst_lat = f_lat;
     lst_lng = f_lng;
     
     M5.Lcd.setTextColor(TFT_YELLOW); 
     M5.Lcd.setCursor(3, 40, 2);
     M5.Lcd.setTextFont(4);
     M5.Lcd.print("V= ");
     M5.Lcd.print(speed_num_0*3.6); 
     M5.Lcd.setTextFont(4);
     M5.Lcd.print("  km/h");
     //M5.Lcd.setCursor(3, 33, 2);
     //M5.Lcd.print(f_speed);  
     M5.Lcd.setTextColor(TFT_WHITE);
     M5.Lcd.setCursor(3, 13, 2);
     M5.Lcd.setTextFont(4);
     M5.Lcd.print("Dist.= ");
     M5.Lcd.print(f_dist);  
     M5.Lcd.print(" km");
         M5.Lcd.setCursor(0, 65);
  M5.Lcd.printf(" %5.2f   %5.2f   %5.2f   ", accX, accY, accZ);
  M5.Lcd.setCursor(220, 65);
  M5.Lcd.print(" G");
  int a=abs(accX);
   int b=abs(accY);
    int c=abs(accZ);
  int d=a+b+c;
  int e=d*200;
 int f=e/0.8;
 //y[2] = {0};
 // y[1] = 0.8 * y[0] + 0.2 * (abs(accX) + abs(accY) + abs(accZ)) * 1000.0;
 // y[0] = y[1];
 
 // if(e>300){
 //   bosu = bosu+1; 
//  }

    if(d>1.4){
    jump = jump+1; 
  }
      M5.Lcd.setCursor(120, 100);
  M5.Lcd.print("bosu=");
    M5.Lcd.print(jump);
    
    //M5.Lcd.setCursor(120, 100);
  //M5.Lcd.print("bosu=");
    //M5.Lcd.print(bosu);
  

     
  
}

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      
      if (value.length() > 0) {
        valor = "";
        for (int i = 0; i < value.length(); i++){
          // Serial.print(value[i]); // Presenta value.
          valor = valor + value[i];
        }
      }
       if (string_num==3) {      
         
        M5.Lcd.fillScreen(BLACK);
        //Serial.print(all_valor.length());
        //Serial.println(all_valor); // Presenta valor.

      for(int i=0;i< all_valor.length();i++) 
          s_str[i]=all_valor.charAt(i);
            
        //Serial.print("s_str:  ");
        Serial.println(s_str);
       
       
        DeserializationError error = deserializeJson(doc, s_str);

       // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
      }

      // Fetch values.
      //
      // Most of the time, you can rely on the implicit casts.
      double latitude = doc["lat"];
      double longitude = doc["lon"];
      double speed_num = doc["speed"];

      Cal(latitude,longitude,speed_num);  



	 string_num=0; 
	 all_valor="";
      for(int i=0;i< 60;i++)   
           s_str[i]=all_valor.charAt(i);
      }
      if (string_num<3) { 
          all_valor= all_valor+valor.substring(0,valor.length());
                                                             
          string_num=string_num+1;
      }
      else 
      {

       string_num=0; 
       all_valor="";
       //for(int i=0;i< 60;i++)  
       //    s_str[i]=all_valor.charAt(i);
      }
               

    }
};

void setup() {

  Serial.begin(115200);
  M5.begin();
  M5.IMU.Init();

  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(TFT_RED);
  
  M5.Lcd.setCursor(100, 100);
  M5.Lcd.setTextFont(4);
  M5.Lcd.print("Starting action");
  BLEDevice::init("core2");
  
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  

}

void loop() {
     M5.IMU.getAccelData(&accX,&accY,&accZ);

  delay(2000);

}

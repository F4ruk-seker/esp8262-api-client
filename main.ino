#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
 


const String esp_id = "8f2166f2-676d-4c9c-9218-c49bdb115270";
const String API_HOST = "https://esp8266-api.up.railway.app";
//const String API_HOST = "http://192.168.0.111:8000";
LiquidCrystal_PCF8574 lcd(0x27);
int show;
// pins
const int read_gas_sensor_analog_pin = A0;
const int role_controle_pin = D0;
// lcd pins already usage D1 D2
const int read_gas_sensor_digital_pin = D3;
const int buzzer_pin = D5;
//led
const int led_power = D6;
const int led_data_wifi_status = D7;
const int led_alert = D8;
// pins END
bool isAlert_sending = false;
// wifi _
const String wifi_SSID = "pars+";
const String wifi_Password = "#6£1!&}[886@5=%3£8";
// wifi END
void setup ()
{
    //pin mods
    // sensor analog read
    pinMode(read_gas_sensor_analog_pin,INPUT);

    //pin buzzer
    pinMode(buzzer_pin,OUTPUT);
    digitalWrite(buzzer_pin,LOW);
    //pin power led mode
    pinMode(led_power,OUTPUT);
    digitalWrite(led_power,HIGH);
    // role controller
    pinMode(role_controle_pin,INPUT);

    // wifi data sending led
    pinMode(led_data_wifi_status,OUTPUT);
    digitalWrite(led_data_wifi_status,LOW);
    //led alert
    pinMode(led_alert,OUTPUT);
    digitalWrite(led_alert,LOW);

    // lcd setup with debug
    int error;

    Serial.begin (115200);
    Serial.println ("LCD..");

    while ( ! Serial);
    Serial.println ("Dose: check for LCD");
    Wire.begin ();
    Wire.beginTransmission (0x27);
    error = Wire.endTransmission();
    Serial.print ("Error: ");
    Serial.print (error);
    if (error == 0) {
      Serial.println ("LCD found");
    }else {
      Serial.println ("LCS not found");
    }
    lcd.begin (16, 2);
    show = 0;
    //lcd setup END
    lcd.setBacklight(255) ;

    lcd.clear();
    lcd.print("Buzzer TEST ");
    //digitalWrite(buzzer_pin,HIGH);
   
    //digitalWrite(buzzer_pin,LOW);

    // wifi initialize
    lcd.clear();
    lcd.print("Wait For WIFI ");
    lcd.blink();

    WiFi.begin(wifi_SSID, wifi_Password);

    while (WiFi.status() != WL_CONNECTED){
      digitalWrite(led_data_wifi_status,HIGH);
      delay(500);
      digitalWrite(led_data_wifi_status,LOW);
      delay(500);
    }
    // wifi initialize END
    Serial.println("eco wifi");
    lcd.noBlink();
    
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Wifi | connected");
 }
int last_gas_sensor_data = 0;
void loop (){
  lcd.clear();

  lcd.setCursor(0,1);
  if (WiFi.status() == WL_CONNECTED){
    lcd.print("Wifi | connected");
  }else{
    lcd.print("Wifi | broken off");
  }
//yield
  int gas_sensor_data = analogRead(read_gas_sensor_analog_pin);
  lcd.home();
  lcd.print("Sensor :"+String(gas_sensor_data));
  bool sensor_status = digitalRead(read_gas_sensor_digital_pin);

  if (sensor_status){
    Serial.println ("hava temiz");
    isAlert_sending = false;
    digitalWrite(buzzer_pin,LOW);
    pinMode(role_controle_pin,OUTPUT);
    digitalWrite(led_alert,LOW);
    digitalWrite(led_power,HIGH);

  }else{
    // alert 
    pinMode(role_controle_pin,INPUT);

    digitalWrite(buzzer_pin,HIGH);
    digitalWrite(led_alert,HIGH);
    
    digitalWrite(led_power,LOW);

    //tone(buzzer_pin, 523);

    if (! isAlert_sending){
      isAlert_sending = true;
      sendAlert();
      Serial.println ("bildirim gönderildi");
    }
  }
  if (gas_sensor_data > last_gas_sensor_data + 10 || gas_sensor_data < last_gas_sensor_data - 10 && ! isAlert_sending){
    last_gas_sensor_data = gas_sensor_data;
    //sendSensorData(gas_sensor_data);
    sendSensorData(gas_sensor_data);

  }
  delay(500);
}


bool sendSensorData(int status){
  if (WiFi.status() == WL_CONNECTED){
    
  digitalWrite(led_data_wifi_status,LOW);

  String _url = API_HOST + "/api/esp/"+ esp_id +"/sensor/update/";
  //String jsonData = "{\"value\":"+String(status)+"}";

  //WiFiClient  client;
  WiFiClientSecure client;
  //AsyncWebClient client;
  HTTPClient http;

  client.setInsecure();

  client.connect(_url, 443);



  Serial.print(_url);
//  request.POST(_url, "application/json", jsonData, [](AsyncHTTPResponse *response){
  //  // handle the response here
  //});
  
  http.begin(client, _url);

  http.addHeader("Content-Type", "application/json");

  String json = "{\"value\":"+String(status)+"}";

  //http.POST(json);

  int httpCode = http.POST(json);
  Serial.print(json);

  if (httpCode > 0) {
    Serial.printf("HTTP POST request code: %d\n", httpCode);
    //Serial.printf("HTTP POST response payload: %s\n", payload.c_str());
  } else {
    Serial.printf("Error_sensor status: %s\n", http.errorToString(httpCode).c_str());
    Serial.print(httpCode);
    Serial.print(http.getString());
  }
  http.end();
  digitalWrite(led_data_wifi_status,HIGH);
  return true;
  }else{
    digitalWrite(led_data_wifi_status,LOW);
    return false;
  }
}

bool sendAlert(){
  if (WiFi.status() == WL_CONNECTED){
    
  digitalWrite(led_data_wifi_status,LOW);
  String _url = API_HOST + "/api/esp/"+ esp_id +"/alert/";

  WiFiClientSecure client;
  HTTPClient http;
  
  client.setInsecure();
  client.connect(_url, 443);

  http.begin(client, _url);

  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST("");

  Serial.print(_url);

  if (httpCode > 0) {
    Serial.printf("HTTP POST request code: %d\n", httpCode);
    //Serial.printf("HTTP POST response payload: %s\n", payload.c_str());
  } else {
    Serial.printf("Error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  digitalWrite(led_data_wifi_status,HIGH);
  return true;
  }else{
    digitalWrite(led_data_wifi_status,LOW);
    return false;
  }
}

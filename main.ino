#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

const String api_host = "http://192.168.0.111/";

const int ldrPin = 0;
//const int ldrPin = D8;
const int wifi_wait_led = D1;
const int data_sending_led = D2;
//const int led = D4;
int old_ldr_data = 0;

const String SSID = "";
const String Password = "";

void setup() {
pinMode(ldrPin,INPUT);
pinMode(wifi_wait_led,OUTPUT);
pinMode(data_sending_led,OUTPUT);
digitalWrite(wifi_wait_led,LOW);
digitalWrite(data_sending_led,LOW);

delay(1000);


Serial.begin(9600);
Serial.println();
Serial.println("pars");


WiFi.begin(SSID, Password);

Serial.print("Connecting");
while (WiFi.status() != WL_CONNECTED)
{
  digitalWrite(wifi_wait_led,HIGH);
  delay(500);
  digitalWrite(wifi_wait_led,LOW);
  delay(500);
  
  Serial.print(".");
}
Serial.println();

Serial.print("Connected, IP address: ");
Serial.println(WiFi.localIP());

 
}

void loop() {
  int ldr_data = analogRead(ldrPin);
  Serial.println(ldr_data);
  Serial.println(old_ldr_data);
  Serial.println("**************");
  //delay(1000);
  if(ldr_data < 100) {
    digitalWrite(data_sending_led,LOW);  
  }else{
    digitalWrite(data_sending_led,HIGH);
  }
  if (ldr_data > old_ldr_data + 10 || ldr_data < old_ldr_data - 10){
    old_ldr_data = ldr_data; 
    Serial.println(old_ldr_data);
    Serial.println(ldr_data);

    _send_sensor_data(String(ldr_data));

  }
  
  delay(500);
  }

int _send_sensor_data(String data){
  if (WiFi.status() == WL_CONNECTED){
  WiFiClient  client;
  HTTPClient http; //Object of class HTTPClient
  String _url = api_host + "esp/data/" + data;
  Serial.println(_url);
  http.begin(client,_url);
  int httpCode = http.GET();


  if (httpCode > 0 ){
    String pyload = http.getString();
    Serial.println(pyload);
  }
  }
 
  return 0;
}

 /*
 The Last P4ARS : F4
 [-1]



 */
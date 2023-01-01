#include <Servo.h>
#include <DHT.h>
#define DHTPIN D5
#define DHTTYPE DHT11
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
DHT dht(DHTPIN, DHTTYPE);
Servo myservo;
int sensorLDR = A0;
int sensorHUJAN = D6;
int RLY;
int gelap = 500 ;

#define WIFI_SSID "xxx"/// isi dengan nama wifi
#define WIFI_PASSWORD "xxx"/// isi dengan pasword  wifi
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "xxx" ///isi dengan api token bot telegram
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime; //last time messages' scan has been done 
bool Start = false;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";
    if (text == "/monitoring") {
      int analog_LDR = analogRead(sensorLDR);
      int digital_HUJAN= digitalRead(sensorHUJAN);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      if ((analog_LDR < gelap) && (digital_HUJAN == 1)){
        bot.sendMessage(chat_id, " SELAMAT DATANG ", "");
        bot.sendMessage(chat_id, " Kondisi Sekarang : \t Cuaca Sangat Mendukung. Jemuran Aman!", "");
        delay(5);
      }
      else if ((analog_LDR > gelap) || (digital_HUJAN == 0)){
        bot.sendMessage(chat_id, " SELAMAT DATANG ", "");
        bot.sendMessage(chat_id, "Kondisi Sekarang : \t Cuaca Tidak Mendukukung. Pintu Tertutup Otimatisdan Jemuran Aman! ", "");
        delay (5);
      }
      String Data = "Data Monitoring Sensor :\n ";
      Data += " Kelembapan : ";
      Data += h ;
      Data += " %\n Suhu : ";
      Data += t;
      Data += " *C\n Insesnsitas Cahaya : ";
      Data += analog_LDR;
      Data += " Lux \n Kondisi Cuaca Hujan[0] ,Tidak Hujan[1] = ";
      Data += digital_HUJAN;
      Data += " \n Ketik [ /monitoring ] jika ingen mengetahui kondisi jemuran \n Terimakasih "; 
      bot.sendMessage(chat_id, Data);
    }
  }
}

void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.print("Connecting Wifi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(RLY,HIGH);
    delay(100);
    digitalWrite(RLY,LOW);
    delay (100);
  }
  Serial.println("");
  digitalWrite(RLY,HIGH);
  delay(500);
  digitalWrite(RLY,LOW);
  delay(500);

  myservo.attach(D7);
  myservo.write(0);
  dht.begin();
  delay(10); 
  pinMode(sensorLDR,INPUT); 
  pinMode(sensorHUJAN,INPUT);
}

void loop(){
  int analog_LDR = analogRead(sensorLDR);
  int digital_HUJAN = digitalRead(sensorHUJAN);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if ((analog_LDR < gelap) && (digital_HUJAN == 1)){
    // Serial.print (analog_LDR);
    // Serial.print (" | ");
    // Serial.print (digital_HUJAN);
    digitalWrite(RLY,HIGH);
    delay(2000);
    digitalWrite(RLY,LOW);
    delay(2000);
    Serial.println("Cuaca Sangat Mendukung. Jemuran Aman!");
    myservo.write(90);
    delay(50);
  }
  else if ((analog_LDR > gelap) || (digital_HUJAN == 0)){
    // Serial.print (analog_LDR);
    // Serial.print (" | ");
    // Serial.print (digital_HUJAN);
    digitalWrite(RLY,HIGH);
    delay(1000);
    digitalWrite(RLY,LOW);
    delay(1000);
    Serial.println("Cuaca Tidak Mendukukung. Pintu Tertutup Otimatisdan Jemuran Aman! ");
    myservo.write(0);
    delay(50);
  }
  if (millis() > Bot_lasttime + Bot_mtbs) {
    int numNewMessages = bot.getUpdates(bot.last_message_received +1);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}

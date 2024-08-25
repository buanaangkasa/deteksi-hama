#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

Servo servo1;
Servo servo2;
Servo servo3;

const int trigPin = D1;    // Pin trigger sensor ultrasonik
const int echoPin = D2;    // Pin echo sensor ultrasonik
const int servo1Pin = D3;  // Pin servo motor 1
const int servo2Pin = D5;  // Pin servo motor 2
const int servo3Pin = D6;  // Pin servo motor 3

const char* ssid = "SWAG";
const char* password = "Mangeak35";
const char* serverUrl = "http://deteksihama.komputasi.org/post-esp-data.php"; // Ganti dengan URL API PHP Anda
String apiKeyValue = "tPmAT5Ab3j7F9";

void setup() {
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo3.attach(servo3Pin);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);

  // Menghubungkan ke jaringan WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  long duration, distance;

  // Mengirim sinyal ultrasonik
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Menerima dan menghitung waktu perjalanan gelombang ultrasonik
  duration = pulseIn(echoPin, HIGH);

  // Menghitung jarak berdasarkan waktu perjalanan
  distance = (duration / 2) / 29.1;  // Faktor konversi 29.1 (kecepatan suara dalam udara) dapat disesuaikan

  // Mengendalikan servo motor berdasarkan jarak
  if (distance < 60) {
    servo1.write(90);  
    servo2.write(90);  
    servo3.write(90);  
  } else if (distance < 120) {
    servo1.write(180);  
    servo2.write(180);  
    servo3.write(160);  
  } else {
    servo1.write(0);  
    servo2.write(0);  
    servo3.write(0);  
  }

  // Determine the status of servo1 motion
  String servo1Status;
  if (distance < 60) {
    servo1.write(90);
    servo1Status = "Servo bergerak Normal, Hama Terdeteksi Dekat";
  } else if (distance < 120) {
    servo1.write(180);
    servo1Status = "Servo bergerak Kencang, Hama Terdeteksi Jauh";
  } else {
    servo1.write(0);
    servo1Status = "Servo tidak bergerak, Hama Tidak Terdeteksi";
  }

  // Mengirim data ke server menggunakan metode POST
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Membuat data yang akan dikirim ke server
    String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(distance) + "&statuss=" + servo1Status;

    // Mengirim data ke server
    int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error sending POST request");
    }

    http.end();
  }

  delay(1000);  // Jeda 1 detik sebelum mengulang
}
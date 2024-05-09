// cách lắp mạch:
// Động cơ bước: IN1234 lần lượt gắn với chân D1,D2,D3,D4
// DHT11: nâu: GND, đỏ: 3v3, vàng: chân D6

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

#define DHT11Pin D6
#define DHTType DHT11

int Pin1 = 5;
int Pin2 = 4;
int Pin3 = 0;
int Pin4 = 2;
DHT HT(DHT11Pin, DHTType); 
float humi; 
float tempC; 
float tempF;
ESP8266WebServer server(80);                                            
boolean direction = false;  // false=ngược chiều kim đồng hồ, true=cùng chiều kim đồng hồ
int motorspeed = 0;         //Tốc độ của moto
void setup() {
  pinMode(Pin1, OUTPUT);
  pinMode(Pin2, OUTPUT);
  pinMode(Pin3, OUTPUT);
  pinMode(Pin4, OUTPUT);
  HT.begin();
  Serial.begin(9600);
  delay(1000);
  WiFi.begin("_o3o_", "helloworld"); //SSID && Pasword
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println(".");
  }
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
  server.on("/", [] {
    server.send(200, "text/html",
                "<!DOCTYPE html>"
                "<html>"
                "<head>"
                "<title>Serial Monitor</title>"
                "<script>"
                "window.onload = function() {"
                "readSerial();"
                "};"
                "function readSerial() {"
                "var serialMonitor = document.getElementById('serialMonitor');"
                "var xhttp = new XMLHttpRequest();"
                "xhttp.onreadystatechange = function() {"
                "if (this.readyState == 4 && this.status == 200) {"
                "serialMonitor.value = this.responseText;"
                "serialMonitor.scrollTop = serialMonitor.scrollHeight;"
                "setTimeout(readSerial, 500);"
                "}"
                "};"
                "xhttp.open('GET', 'readSerial', true);"
                "xhttp.send();"
                "}"
                "function sendValue(){"
                "var degreesInput = document.getElementById('degrees');"
                "var xhttp2 =  new XMLHttpRequest();"
                "var degrees = degreesInput.value;"
                "xhttp2.open('POST', 'sendValue', true);"
                "xhttp2.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');"
                "xhttp2.send('degrees=' + degrees);"
                "}"
                "</script>"
                "</head>"
                "<body>"
                "<label>cam bien DHT11</label><br>"
                "<textarea id='serialMonitor' style='font-family: monospace; width: 100%; height: 300px; white-space: pre; '></textarea>"
                "<label>Goc cua Stepper: </label>"
                "<input type='text' name='degrees' id = 'degrees'>"
                "<input type='button' value='Send' onclick = sendValue()>"
                "</body>"
                "</html>");
  });
  setupRoutes();
  server.begin();  // Bắt đầu web server
}

void step_1() {
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, HIGH);
  delay(motorspeed);
}

void step_2() {
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, HIGH);
  digitalWrite(Pin4, HIGH);
  delay(motorspeed);
}

void step_3() {
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, HIGH);
  digitalWrite(Pin4, LOW);
  delay(motorspeed);
}

void step_4() {
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, HIGH);
  digitalWrite(Pin3, HIGH);
  digitalWrite(Pin4, LOW);
  delay(motorspeed);
}

void step_5() {
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, HIGH);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, LOW);
  delay(motorspeed);
}

void step_6() {
  digitalWrite(Pin1, HIGH);
  digitalWrite(Pin2, HIGH);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, LOW);
  delay(motorspeed);
}

void step_7() {
  digitalWrite(Pin1, HIGH);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, HIGH);
  delay(motorspeed);
}

void step_8() {
  digitalWrite(Pin1, HIGH);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, HIGH);
  delay(motorspeed);
}

void counter_clockwise_rotating()  //Xoay động cơ ngược chiều kim đồng hồ
{
  step_1();
  step_2();
  step_3();
  step_4();
  step_5();
  step_6();
  step_7();
  step_8();
}

void clockwise_rotating()  //Xoay động cơ cùng chiều kim đồng hồ
{
  step_8();
  step_7();
  step_6();
  step_5();
  step_4();
  step_3();
  step_2();
  step_1();
}


void handleReadSerial() {
  // Gửi giá trị cảm biến về client
  humi = HT.readHumidity(); 
  tempC = HT.readTemperature(); 
  server.send(200, "text/plain", "do am: " + String(humi) + "\n" + "nhiet do: " + String(tempC) + "\n");
}
int receivedValue = 0;
void handleSendValue() {
  if (server.hasArg("degrees")) {
    receivedValue = server.arg("degrees").toInt();
  }
}

void setupRoutes() {
  server.on("/readSerial", handleReadSerial);
  server.on("/sendValue", handleSendValue);
}

void speed_changing(int value)  //Thay đổi tốc độ của động cơ
{
  motorspeed = value / 120 + 1;
}

void direction_changing()  //Thay đổi hướng quay của động cơ
{
  if (direction == true) {
    direction = false;
  } else {
    direction = true;
  }
}
void loop() {
  // Xử lý các yêu cầu từ client
  server.handleClient();
  delay(500);  // Đợi để hệ thống xử lý các yêu cầu khác
  if (true) {
  if (receivedValue > 0) {
    //quay servo thuận theo góc INPUT
      Serial.print(receivedValue);
      direction_changing();
      for (int i = 1; i <= 512*receivedValue/360; i++) {
        speed_changing(0);
        clockwise_rotating();
      } 
  }
  else{
        //quay servo thuận theo góc INPUT
      Serial.print(receivedValue);
      direction_changing();
      for (int i = 1; i <= -512*receivedValue/360; i++) {
        speed_changing(0);
        counter_clockwise_rotating();
      } 
  }
  }
}
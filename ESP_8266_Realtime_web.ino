#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

// Khai báo thư viện cần thiết
#include <ArduinoJson.h>

#define RX_PIN D2 // Chân RX của cổng nối tiếp mềm
#define TX_PIN D3 // Chân TX của cổng nối tiếp mềm

float light_value = 0.0;

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Tạo một đối tượng cổng nối tiếp mềm

// Tạo một đối tượng JsonDocument có dung lượng 100 byte
StaticJsonDocument<100> doc;
StaticJsonDocument<100> docSend;

// Replace with your network credentials
const char* ssid = "your_wifi_name";
const char* password = "your_wifi_password";

String motor_status = "off";
ESP8266WebServer server(80);  

void setup() {
  
  WiFi.begin(ssid, password); //SSID && Pasword
  delay(1000);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println(".");
  }
  Serial.begin(9600);
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", [] {
    server.send(200,"text/html", 
    "<!DOCTYPE html><html><head><title>Serial Monitor</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style>html {font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;}.button {background-color: #4CAF50;border: none;color: white;padding: 16px 40px;text-decoration: none;font-size: 30px;margin: 2px;cursor: pointer;}.button2 {background-color: #555555;}</style></head><body><h1>ESP8266 Web Server</h1><p id='serialMonitor'>None</p><p id=\"motor_status\"> Motor - State off</p><br><a href=\"#\" id=\"btnChangeStatus\" class=\"button\">ON</a></body><script>function readSerial() {var serialMonitor = document.getElementById('serialMonitor');var xhttp = new XMLHttpRequest();xhttp.onreadystatechange = function() {if (this.readyState == 4 && this.status == 200) {serialMonitor.innerHTML = 'Light value: ' + this.responseText;serialMonitor.scrollTop = serialMonitor.scrollHeight;setTimeout(readSerial, 500);}};xhttp.open('GET', 'readSerial', true);xhttp.send();}function sendValue() {var degreesInput = document.getElementById('degrees');var xhttp2 = new XMLHttpRequest();var degrees = degreesInput.value;xhttp2.open('POST', 'sendValue', true);xhttp2.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');xhttp2.send('degrees=' + degrees);}function changeStatus() {var motor_status = document.getElementById('motor_status');var btnChangeStatus = document.getElementById('btnChangeStatus');var xhttp3 = new XMLHttpRequest();xhttp3.onreadystatechange = function() {if (this.readyState == 4 && this.status == 200) {if (this.responseText == '1') {motor_status.innerHTML = 'Motor - State on';btnChangeStatus.innerHTML = 'OFF';btnChangeStatus.className = 'button button2';} else {motor_status.innerHTML = 'Motor - State off';btnChangeStatus.innerHTML = 'ON';btnChangeStatus.className = 'button';}}};xhttp3.open('GET', 'changeStatus', true);xhttp3.send();}document.getElementById('btnChangeStatus').addEventListener('click', changeStatus);window.onload = function() {readSerial();};</script></html>"
    );
  });
  setupRoutes();
  server.begin();  // Bắt đầu web server


  // Khởi tạo cổng nối tiếp ảo
  mySerial.begin(9600);
}

int receivedValue = 0;
void handleSendValue() {
  if (server.hasArg("degrees")) {
    receivedValue = server.arg("degrees").toInt();
  }
}

void handleReadSerial() {
  server.send(200, "text/plain", String(light_value) + "\n");
}

void setupRoutes() {
  server.on("/readSerial", handleReadSerial);
  server.on("/sendValue", handleSendValue);
  server.on("/changeStatus", handlechangeStatus);
}

void handlechangeStatus(){
   if (motor_status == "off") {
    motor_status = "on";
    handleMotorOn();
    server.send(200, "text/plain", "1");
   } else {
    motor_status = "off";
    handleMotorOff();
    server.send(200, "text/plain", "0");
   }
}

void handleMotorOn(){
    docSend["motor"] = 1;
    docSend["light_value"] = light_value;
    // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp ảo
    serializeJson(docSend, mySerial);
    mySerial.println();
    // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp ảo
    serializeJson(docSend, Serial);
    Serial.println();
}

void handleMotorOff(){
    docSend["motor"] = 0;
    docSend["light_value"] = light_value;
    // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp ảo
    serializeJson(docSend, mySerial);
    mySerial.println();
    // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp ảo
    serializeJson(docSend, Serial);
    Serial.println();
}

void loop() {
  // Kiểm tra xem có dữ liệu nào sẵn sàng trên cổng nối tiếp ảo hay không
  if (mySerial.available()) {
    // Đọc một dòng từ cổng nối tiếp ảo
    String line = mySerial.readStringUntil('\n');

    // Chuyển đổi chuỗi JSON thành đối tượng JsonDocument
    DeserializationError error = deserializeJson(doc, line);

    // Kiểm tra xem có lỗi nào trong quá trình chuyển đổi hay không
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Lấy giá trị nhiệt độ và độ ẩm từ đối tượng JsonDocument
    light_value = doc["light_value"];
    Serial.println(light_value);
  }
  // Xử lý các yêu cầu từ client
  server.handleClient();
  delay(250);

}

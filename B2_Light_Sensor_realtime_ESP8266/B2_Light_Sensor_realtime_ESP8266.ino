#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
// Có thể dùng các giá trị đã define nếu không nhớ được giá trị chân GPIO
// Ở đây mình dùng chân built in có sẵn trên ESP8266 luôn nha! 
// (Lưu ý: trạng thái LOW, HIGH bị đảo ở trong trường hợp dùng chân built in)
const int LED = LED_BUILTIN; 

int led_status = 0;

String  WebPage = {
  "<!DOCTYPE html><html><head>"
    "<title>Web realtime - Light Sensor</title>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<style>"
    "html {font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;}"
    ".button {background-color: #4CAF50;border: none;color: white;padding: 16px 40px;text-decoration: none;font-size: 30px;margin: 2px;cursor: pointer;}"
    ".button2 {background-color: #555555;}</style>"
    "</head><body>"
    "<h1><p id=\"serialMonitor\"> None </p><h1><br>"
    "</body><script>"
    "window.onload = function() {"
       "readSerial();"
    "};"
    "function readSerial() {"
    "var serialMonitor = document.getElementById('serialMonitor');"
    "var xhttp = new XMLHttpRequest();"
    "xhttp.onreadystatechange = function() {"
    "if (this.readyState == 4 && this.status == 200) {"
    "serialMonitor.innerHTML = this.responseText;"
    "setTimeout(readSerial, 500);"
    "}"
    "};"
    "xhttp.open('GET', 'readSerial', true);"
    "xhttp.send();"
    "}"
    "</script></html>"
};

void setup() {
  Serial.begin(9600);

  // Thiết lập LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH); // Thiết lập trạng thái mức điện cao cho chân tín hiệu LED

  // ===========THIẾT LẬP KẾT NỐI WIFI====================================================
  delay(1000);
  WiFi.begin("your_wifi_name", "your_wifi_password"); //SSID && Pasword
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println(".");
  }
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
  // ===============================================================================
  // Giao diện trang chủ
  server.on("/", [] {server.send(200,"text/html", WebPage);});
  setupRoutes();
  server.begin();  // Bắt đầu web server
}

void handleReadSerial() {
  // Gửi giá trị cảm biến về client
  float light = analogRead(A0);
  server.send(200, "text/plain", "Giá trị cảm biến ánh sáng: " + String(light));
}

void setupRoutes() {
  server.on("/readSerial", handleReadSerial);
}

void loop() {
  // Xử lý các yêu cầu từ client
  server.handleClient();
}
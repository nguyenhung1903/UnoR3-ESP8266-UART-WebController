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
    "<title>LED light control website</title>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<style>"
    "html {font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;}"
    ".button {background-color: #4CAF50;border: none;color: white;padding: 16px 40px;text-decoration: none;font-size: 30px;margin: 2px;cursor: pointer;}"
    ".button2 {background-color: #555555;}</style>"
    "</head><body><h1>LED light control</h1>"
    "<p id=\"led_status\"> Light is off </p><br>"
    "<a href=\"#\" id=\"btnChangeStatus\" class=\"button\">ON</a>"
    "</body><script>"
    "function changeStatus() {"
    "    var led_status = document.getElementById('led_status');"
    "   var btnChangeStatus = document.getElementById('btnChangeStatus');"
    "    var xhttp3 = new XMLHttpRequest();"
    "    xhttp3.onreadystatechange = function() {"
    "        if (this.readyState == 4 && this.status == 200) {"
    "            if (this.responseText == '1') {"
    "                led_status.innerHTML = 'Light is on';"
    "                btnChangeStatus.innerHTML = 'OFF';"
    "                btnChangeStatus.className = 'button button2';"
    "            } else {"
    "                led_status.innerHTML = 'Light is off';"
    "                btnChangeStatus.innerHTML = 'ON';"
    "                btnChangeStatus.className = 'button';"
    "            }"
    "        }"
    "    };"
    "    xhttp3.open('GET', '/changeStatus', true);"
    "    xhttp3.send();"
    "}document.getElementById('btnChangeStatus').addEventListener('click', changeStatus);"
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

void handleLED(){
  if (led_status == 1){
    digitalWrite(LED, HIGH);
    led_status = 0;
    server.send(200, "text/plain", "0");
  } else if (led_status == 0){
    digitalWrite(LED, LOW);
    led_status = 1;
    server.send(200, "text/plain", "1");
  }
}

void setupRoutes() {
  server.on("/changeStatus", handleLED);
}

void loop() {
  // Xử lý các yêu cầu từ client
  server.handleClient();
}
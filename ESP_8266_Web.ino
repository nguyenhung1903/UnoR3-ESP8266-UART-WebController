// Load Wi-Fi library
#include <ESP8266WiFi.h>
// Khai báo thư viện cần thiết
#include <ArduinoJson.h>
// Khai báo thư viện SoftwareSerial
#include <SoftwareSerial.h>

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

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 1000;

String output26State = "off";

void setup() {
  // Khởi tạo giao tiếp nối tiếp với tốc độ baud là 9600
  Serial.begin(9600);
  // Khởi tạo cổng nối tiếp ảo
  mySerial.begin(9600);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected

      
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

      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              
              docSend["motor"] = 1;
              docSend["light_value"] = light_value;
              // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp ảo
              serializeJson(docSend, mySerial);
              mySerial.println();

              // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp ảo
              serializeJson(docSend, Serial);
              Serial.println();

            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
               
              docSend["motor"] = 0;
              docSend["light_value"] = light_value;
              // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp ảo
              serializeJson(docSend, mySerial);
              mySerial.println();

              // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp ảo
              serializeJson(docSend, Serial);
              Serial.println();
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            String myString = String(light_value, 2);

            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p> Light value: " + myString + "</p>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>Motor - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
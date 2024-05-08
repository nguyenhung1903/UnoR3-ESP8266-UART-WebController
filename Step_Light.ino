// Khai báo các thư viện cần thiết
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "Stepper.h"


const int STEPS = 1024;
Stepper stepper(STEPS, 4, 6, 5, 7);

int degreeToSteps(int degree, int steps = STEPS) {
  if (degree==0) return 0;
  return steps / (360/degree);
}

// Tạo một đối tượng JsonDocument có dung lượng 100 byte
StaticJsonDocument<100> doc;
StaticJsonDocument<100> docRecv;

#define RX_PIN 10 // Chân RX của EspSoftwareSerial
#define TX_PIN 11 // Chân TX của EspSoftwareSerial
SoftwareSerial mySerial(RX_PIN , TX_PIN ); // RX, TX

int motor_status = -1;
int led = 2;
int light_value = 0.0;

void setup() {
  // Khởi tạo giao tiếp nối tiếp với tốc độ baud là 9600
  Serial.begin(9600);
  // Khởi tạo cổng nối tiếp ảo
  mySerial.begin(9600);
  pinMode(led, OUTPUT);
  stepper.setSpeed(30);
}

void loop() {

  // Kiểm tra xem có dữ liệu nào sẵn sàng trên cổng nối tiếp ảo hay không
  if (mySerial.available()) {
    // Đọc một dòng từ cổng nối tiếp ảo
    String line = mySerial.readStringUntil('\n');

    // Chuyển đổi chuỗi JSON thành đối tượng JsonDocument
    DeserializationError error = deserializeJson(docRecv, line);

    // Kiểm tra xem có lỗi nào trong quá trình chuyển đổi hay không
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Lấy giá trị nhiệt độ và độ ẩm từ đối tượng JsonDocument
    motor_status = docRecv["motor"];
    light_value = docRecv["light_value"];
    Serial.println(motor_status);
    Serial.println(light_value);
    // In các giá trị ra màn hình nối tiếp
    digitalWrite(led, int(motor_status));
  }
  int value = analogRead(A0);
  if (motor_status==1) {
    if (value > 700){
      stepper.step(degreeToSteps(180/2));
    } else {
      stepper.step(-degreeToSteps(180/2));
    }
  }

  doc["light_value"] = value;
  // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp ảo
  serializeJson(doc, mySerial);
  mySerial.println();

  // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp thật
  serializeJson(doc, Serial);
  Serial.println();

  // Đợi một giây
  delay(1000);
}

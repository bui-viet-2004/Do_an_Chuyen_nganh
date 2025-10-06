#include <SoftwareSerial.h>

SoftwareSerial BT(10, 11); // RX = 10, TX = 11

void setup() {
  Serial.begin(9600);
  BT.begin(9600);
  Serial.println("Nhập lệnh AT cho HM10:");
}

void loop() {
  // Gửi dữ liệu từ Serial Monitor đến HM10
  if (Serial.available()) {
    char c = Serial.read();
    BT.write(c);
  }

  // Nhận phản hồi từ HM10 và in ra Serial Monitor
  if (BT.available()) {
    char c = BT.read();
    Serial.write(c);
  }
}

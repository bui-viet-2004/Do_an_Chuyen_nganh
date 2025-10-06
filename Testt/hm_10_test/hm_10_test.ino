#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11); // RX, TX

String inputString = "";   // Biến lưu chuỗi nhận được

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  Serial.println("Đang chờ chuỗi dữ liệu từ HM10...");
}

void loop() {
  // Kiểm tra có dữ liệu từ HM10 không
  while (BTSerial.available()) {
    char c = BTSerial.read();
    
    if (c == '\n' || c == '\r') {  // Nếu gặp ký tự kết thúc
      if (inputString.length() > 0) {
        Serial.print("Nhận được: ");
        Serial.println(inputString);  // In toàn bộ chuỗi
        inputString = "";             // Xóa chuỗi để chuẩn bị nhận mới
      }
    } 
    else {
      inputString += c;  // Ghép ký tự vào chuỗi
    }
  }
}

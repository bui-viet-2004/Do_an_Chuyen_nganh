HardwareSerial HM10(2); // UART2

void setup() {
  Serial.begin(115200);

  HM10.begin(9600, SERIAL_8N1, 26, 25); 
  // RX=16, TX=17 (tuỳ bạn)
}

void loop() {
  if (HM10.available()) {
    Serial.write(HM10.read());
  }

  if (Serial.available()) {
    HM10.write(Serial.read());
  }
}

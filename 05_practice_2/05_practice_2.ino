#define PIN_LED 7
unsigned int i=5;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, 0);
  delay(1000);
}

void loop() {
  while(i) {
    digitalWrite(PIN_LED, 1);
    delay(100);
    digitalWrite(PIN_LED, 0);
    delay(100);
    i -= 1;
  }
  digitalWrite(PIN_LED, 1);
}

#include <Hanuman.h>

void setup() {
  Serial.begin(9600);
}

void loop()  {
  int x = knob(180);
  servo(1, x);
  Serial.printf("Servo: %d\n", x);
  delay(100);
}
